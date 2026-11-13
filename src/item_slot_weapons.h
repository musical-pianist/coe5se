#pragma once

// ---- Monsters that keep their own weapons --------------------------------------------------------------------------
static unsigned char *monsterKeepWeapons;
static int monsterKeepWeaponCount;

static int monsterKeepsWeapons(int monster) {
    return monsterKeepWeapons && monster >= 0 && monster < monsterKeepWeaponCount && monsterKeepWeapons[monster];
}

static int setMonsterKeepsWeapons(int monster, int on) {
    if (!monsterKeepWeapons) {
        if (!on) return 1;
        int records = tableRecords(TABLE_MONSTER);
        if (!(monsterKeepWeapons = calloc((size_t)records, 1))) {
            writeLogWarning("item_slots_keep_weapons_failed", "reason=out_of_memory monster=%d", monster);
            return 0;
        }
        monsterKeepWeaponCount = records;
    }
    if (monster < 0 || monster >= monsterKeepWeaponCount) return 0;
    monsterKeepWeapons[monster] = (unsigned char)(on != 0);
    return 1;
}

static void copyMonsterKeepsWeapons(int source, int target) {
    if (!monsterKeepWeapons || target < 0 || target >= monsterKeepWeaponCount) return;
    monsterKeepWeapons[target] = (unsigned char)monsterKeepsWeapons(source);
}

static void clearMonsterKeepsWeapons(int monster) {
    if (!monsterKeepWeapons || monster < 0 || monster >= monsterKeepWeaponCount) return;
    monsterKeepWeapons[monster] = 0;
}

static void keepWeaponsBeforePass(int level) {
    if (level != 2 || !monsterKeepWeapons) return;
    memset(monsterKeepWeapons, 0, (size_t)monsterKeepWeaponCount);
}

static void keepweaponsLine(const char *args, int mod, int level) {
    if (level != 2) return;
    const char *p = args;
    long long on = 1;
    skipSpaces(&p);
    if (!isLineEnd(p) && (!readSpellNumber(&p, &on) || (skipSpaces(&p), !isLineEnd(p)) || on < 0 || on > 1)) {
        refuseC5mLine("keepweapons", spellModName(mod), "bad_value", "line=%d text=%.40s min=0 max=1", G32(RVA_MOD_LINE), args);
        return;
    }
    int monster = G32(RVA_MONSTER_CURRENT);
    if (!monsterRecord(monster)) {
        refuseC5mLine("keepweapons", spellModName(mod), "no_monster", "line=%d", G32(RVA_MOD_LINE));
        return;
    }
    if (!setMonsterKeepsWeapons(monster, (int)on))
        refuseC5mLine("keepweapons", spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
}

// ---- The rows ------------------------------------------------------------------------------------------------------
#define ATTACK_TYPE_MELEE 1

typedef struct {
    int type, weapon;
    long long value;
} UnitAttackRow;

static int weaponDealsDamage(int weapon) {
    if (weapon < 0 || weapon >= tableRecords(TABLE_WEAPON)) return 0;
    return *(const long long *)(tableAddress(TABLE_WEAPON) + (size_t)weapon * WEAPON_SIZE + WEAPON_DMG) != 0;
}

static int weaponUnitMonster(int unit) { return (short)G16(RVA_UNIT_MONSTER + (size_t)unit * UNIT_SIZE); }

static int unitWeaponRowsDiffer(int unit) {
    if (unit < 0 || unit >= UNIT_TABLE_MAX) return 0;
    return unitNeedsItemSlots(unit) || monsterKeepsWeapons(weaponUnitMonster(unit));
}

static unsigned char weaponsDroppedLogged[UNIT_TABLE_MAX / 8 + 1];

static int buildUnitAttackRows(int unit, UnitAttackRow rows[MONSTER_ATTACK_MAX], int *dropped) {
    *dropped = 0;
    const unsigned char *record = monsterRecord(weaponUnitMonster(unit));
    if (!record) return -1;
    int count = 0;
    for (; count < MONSTER_ATTACK_MAX; count++) {
        const unsigned char *row = record + MONSTER_ATTACKS + (size_t)count * MONSTER_ATTACK_SIZE;
        if (*(const int *)row < 1) break;
        rows[count].type = *(const int *)row;
        rows[count].weapon = *(const int *)(row + MONSTER_ATTACK_WEAPON);
        rows[count].value = *(const long long *)(row + MONSTER_ATTACK_VALUE);
    }
    int own[MONSTER_ATTACK_MAX], damaging[MONSTER_ATTACK_MAX], damagingCount = 0;
    long long firstValue = 0;
    for (int r = 0; r < count; r++) {
        own[r] = rows[r].weapon;
        if (rows[r].type != ATTACK_TYPE_MELEE || !weaponDealsDamage(rows[r].weapon)) continue;
        int seen = 0;
        for (int d = 0; d < damagingCount; d++) seen |= damaging[d] == rows[r].weapon;
        if (seen) continue;
        if (!damagingCount) firstValue = rows[r].value;
        damaging[damagingCount++] = rows[r].weapon;
    }
    int keep = monsterKeepsWeapons(weaponUnitMonster(unit)), n = 0;
    for (int p = 0; p < ITEM_POSITION_MAX; p++) {
        int item = unitItemAt(unit, p);
        if (item < 0 || itemSlotKind(unit, p) != ITEM_TYPE_WEAPON) continue;
        const unsigned char *itemRecord = itemStatRecord(item);
        int weapon = itemRecord ? *(const int *)(itemRecord + ITEM_WEAPON) : -1;
        int nth = n++;
        if (weapon < 0) continue;
        if (nth == 0 && !damagingCount) continue;
        if ((nth == 0 || !keep) && nth < damagingCount) {
            for (int r = 0; r < count; r++)
                if (rows[r].type == ATTACK_TYPE_MELEE && own[r] == damaging[nth]) rows[r].weapon = weapon;
            continue;
        }
        if (count == MONSTER_ATTACK_MAX) {
            (*dropped)++;
            continue;
        }
        rows[count].type = ATTACK_TYPE_MELEE;
        rows[count].weapon = weapon;
        rows[count].value = firstValue;
        own[count++] = -1;
    }
    return count;
}

typedef int (*UnitAttackRowFn)(int unit, int slot, int *type, int *weapon, long long *value);
static UnitAttackRowFn gameUnitAttackRow;
static int unitAttackRowHook(int unit, int slot, int *type, int *weapon, long long *value) {
    if (!unitWeaponRowsDiffer(unit)) return gameUnitAttackRow(unit, slot, type, weapon, value);
#ifdef __APPLE__
    *type = 0, *weapon = -1, *value = 0;
#endif
    UnitAttackRow rows[MONSTER_ATTACK_MAX];
    int dropped, count = buildUnitAttackRows(unit, rows, &dropped);
    if (dropped && !(weaponsDroppedLogged[unit / 8] >> (unit % 8) & 1)) {
        weaponsDroppedLogged[unit / 8] |= (unsigned char)(1 << (unit % 8));
        writeLogWarning("item_slots_weapons_dropped", "reason=no_row unit=%d monster=%d rows=%d dropped=%d", unit, weaponUnitMonster(unit), count, dropped);
    }
    if (slot < 0 || slot >= count) return -1;
    *type = rows[slot].type, *weapon = rows[slot].weapon, *value = rows[slot].value;
    return rows[slot].weapon;
}

static int itemSlotWeaponsState;

static int installItemSlotWeapons(void) {
    if (itemSlotWeaponsState) return itemSlotWeaponsState > 0;
    void *target = gameBase + RVA_UNIT_ATTACK_ROW;
    int ok = fanpatchHookCreate(target, (void *)unitAttackRowHook, (void **)&gameUnitAttackRow) && fanpatchHookEnable(target, 1);
    itemSlotWeaponsState = ok ? 1 : -1;
    writeResultLog(ok, "item_slot_weapons_installed", "hooks=%d", ok);
    return ok;
}
