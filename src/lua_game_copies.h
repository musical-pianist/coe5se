#pragma once

#if defined(__APPLE__) || defined(COE5SE_COPY_SELFCHECK)
#define COPY_MONSTER_FLAGS (RVA_MONSTER_FLAGS - RVA_MONSTER_TABLE)
#define COPY_BATTLE_CELL 24
#define COPY_BATTLE_COLUMN (BATTLE_ROWS * COPY_BATTLE_CELL)

static int copyUnitMonster(int u) { return (short)G16(RVA_UNIT_MONSTER + (size_t)u * UNIT_SIZE); }
static unsigned long long copyMonsterWord(int monster, int at) {
    const unsigned char *m = monsterRecord(monster);
    unsigned long long v = 0;
    if (m) memcpy(&v, m + at, sizeof v);
    return v;
}
static int copySquareTerrain(int x, int y) {
    return (short)G16(RVA_SQUARE_RECORDS + ((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE + SQUARE_TERRAIN_TYPE);
}
static unsigned long long copyTerrainFlags(int terrain) {
    return *(const unsigned long long *)GAME_PTR(RVA_TERRAIN_TABLE + (size_t)terrain * TERRAIN_SIZE + TERRAIN_FLAGS);
}
static int copyNameMatches(const char *a, const char *b) {
    for (int i = 0; i < 500000; i++) {
        int ca = (unsigned char)(a[i] - 'A') < 26 ? a[i] + 32 : a[i], cb = (unsigned char)(b[i] - 'A') < 26 ? b[i] + 32 : b[i];
        if (!a[i] && !b[i]) return 1;
        if (ca != cb) return 0;
    }
    return 1;
}

// ---- Planes ---------------------------------------------------------------------------------------

static int planesAlikeCopy(int a, int b) {
    return a >= 0 && b >= 0 && (a == b || ((a & ~3) == 0 && a != 1 && (b & ~3) == 0 && b != 1));
}
static int planeOrderStepCopy(int plane, int dir) {
    int at = plane < 0 ? 0 : plane < 10 ? plane : 10, step = dir < 1 ? dir : 1;
    if (!step) return at;
    if (plane == 0) {
        if (step == -1) return 3;
        if (step == 1) return 2;
        return at;
    } else if (plane == 1) {
        if (step == 1) return 1;
    } else if (plane == 9) {
        if (step == -1) return 9;
    } else if (plane == 2) {
        if (step == -1) return 0;
    } else if (plane == 3 && step == 1) {
        return 0;
    }
    at -= step;
    return at < 0 ? 0 : at < 10 ? at : 10;
}
static int planeIsReachableCopy(int plane, int x, int y) {
    int last = G32(RVA_LAST_PLANE);
    for (int p = 0; p <= last && p < planeTableRecords(); p++) {
        int px = planeField(p, PLANE_X), py = planeField(p, PLANE_Y);
        if (px > x || x >= planeField(p, PLANE_W) + px || py > y || y >= py + planeField(p, PLANE_H)) continue;
        if (plane == p) return 1;
        int cx = planeField(plane, PLANE_W) / 2 + planeField(plane, PLANE_X), cy = planeField(plane, PLANE_H) / 2 + planeField(plane, PLANE_Y);
        GameMonsterPath path = GAME_FN(GameMonsterPath, RVA_MONSTER_PATH);
        void (*trace)(const char *, ...) = GAME_FN(void (*)(const char *, ...), RVA_TRACE);
        int r = path(SKY_PATH_MODE, x, y, cx, cy, SKY_PATH_AP_MAX, 0, 0);
        trace("planeisreachable %d,%d -> pln %d = %d\n", x, y, plane, r);
        if (r >= 0) return 1;
        r = path(SKY_PATH_MODE, x, y, cx - SKY_PATH_SHIFT, cy, SKY_PATH_AP_MAX, 0, 0);
        trace("planeisreachable %d,%d -> pln %d = %d\n", x - SKY_PATH_SHIFT, y, plane, r);
        if (r >= 0) return 1;
        r = path(SKY_PATH_MODE, x, y, cx + SKY_PATH_SHIFT, cy, SKY_PATH_AP_MAX, 0, 0);
        trace("planeisreachable %d,%d -> pln %d = %d\n", x + SKY_PATH_SHIFT, y, plane, r);
        return r >= 0;
    }
    return 0;
}
static int countTerrainCodeCopy(int plane, int code) {
    int (*match)(int, int, int) = GAME_FN(int (*)(int, int, int), RVA_TERRAIN_MATCH);
    int count = 0, x0 = planeField(plane, PLANE_X), y0 = planeField(plane, PLANE_Y);
    for (int x = x0; x < planeField(plane, PLANE_W) + x0; x++)
        for (int y = y0; y < planeField(plane, PLANE_H) + y0; y++)
            if (match(code, x, y)) count++;
    return count;
}

// ---- Units ----------------------------------------------------------------------------------------

static int unitPlaneCopy(int u) {
    if (u < 0) return -1;
    return GAME_FN(int (*)(int, int), RVA_PLANE_AT)(G32(RVA_UNIT_X + (size_t)u * UNIT_SIZE), G32(RVA_UNIT_X + 4 + (size_t)u * UNIT_SIZE));
}
#define COPY_ABILITY_ACIDRES 0x82
static int unitAcidResistanceCopy(int u) { return unitAbilityTotal(u, COPY_ABILITY_ACIDRES, 1); }
static int unitBattleSlownessCopy(int u) {
    if (u < 0 || copyUnitMonster(u) < 0) return 0;
    unsigned long long move = copyMonsterWord(copyUnitMonster(u), MONSTER_MOVE_FLAGS);
    return (move >> 54) & 1 ? 2 : (int)((move >> 53) & 1);
}
static int unitBattleFastCopy(int u) {
    if (u < 0 || copyUnitMonster(u) < 0) return 0;
    unsigned long long move = copyMonsterWord(copyUnitMonster(u), MONSTER_MOVE_FLAGS);
    if (move & (1ULL << 33) || move & 0x10) return 1;
    return (int)((GAME_FN(unsigned long long (*)(int), RVA_UNIT_BENEFITS)(u) >> 30) & 1);
}
#define COPY_INCAPACITATED_BITS (0x8ULL | 0x10 | 0x40 | 0x100 | 0x200 | 0x400 | 0x1000 | 0x80000 | 0x100000000ULL | 0x1000000000ULL)
static int unitIncapacitatedCopy(int u) {
    if (u < 0 || u > G32(RVA_UNIT_COUNT)) return 0;
    size_t at = (size_t)u * UNIT_SIZE;
    int x = G32(RVA_UNIT_X + at);
    unsigned long long state = *(const unsigned long long *)GAME_PTR(RVA_UNITS + at);
    if ((short)G16(RVA_UNIT_HP + at) > 0 && x > -9 && x + 6 > 4 && !(state & COPY_INCAPACITATED_BITS)) return (int)((state >> 40) & 1);
    return 1;
}
static int unitAfflictionCountCopy(int u) {
    if (u < 0) return 0;
    return __builtin_popcountll((GAME_FN(unsigned long long (*)(int), RVA_UNIT_STATE_FLAGS)(u) >> 20) & 0x7ff);
}
static int unitMonsterFlagCopy(int u, int bit) {
    if (u < 0 || copyUnitMonster(u) < 0) return 0;
    return (int)((copyMonsterWord(copyUnitMonster(u), COPY_MONSTER_FLAGS) >> bit) & 1);
}
static int unitIsUndeadCopy(int u) { return unitMonsterFlagCopy(u, 4); }
static int unitIsDemonCopy(int u) { return unitMonsterFlagCopy(u, 33); }
static int squareHasSeenUnitCopy(int player, int x, int y) {
    if (x < 0 || y < 0) return 0;
    int (*visible)(int, int, int, int) = GAME_FN(int (*)(int, int, int, int), RVA_UNIT_VISIBLE_TO);
    for (int u = 0; u <= G32(RVA_UNIT_COUNT) && u < UNIT_TABLE_MAX; u++) {
        size_t at = (size_t)u * UNIT_SIZE;
        int ux = G32(RVA_UNIT_X + at), uy = G32(RVA_UNIT_X + 4 + at);
        if (ux == x && uy == y && (short)G16(RVA_UNIT_HP + at) > 0 && visible(player, u, ux, uy) > 0) return 1;
    }
    return 0;
}

// ---- Spells and rituals ---------------------------------------------------------------------------

static int slotBlessingOfCopy(int player, int ability) {
    if (player < 0 || ability <= 0) return 0;
    for (int i = 0; i < 24; i++) {
        int id = PG32(RVA_PLAYER_BLESSINGS + (size_t)player * PLAYER_BLOCK_SIZE + (size_t)i * 8);
        if (!id) return 0;
        if (id == ability) return PG32(RVA_PLAYER_BLESSINGS + (size_t)player * PLAYER_BLOCK_SIZE + (size_t)i * 8 + 4);
    }
    return 0;
}
static int spellPathWeightCopy(int path) {
    unsigned p = (unsigned)path;
    if (path < 0) return 0;
    if (p == 0x19 || p - 0xc <= 3) return 75;
    if (p == 0x12) return 50;
    if (p == 0x14 || (p <= 0x3b && (0x800000580000000ULL >> (p & 0x3f)) & 1) || p == 0x21 || p == 0x3a) return 75;
    return p == 0x37 ? 75 : 100;
}
static int memorizeAllowanceCopy(int u, int path) {
    int level = GAME_FN(int (*)(int, int), RVA_UNIT_PATH_LEVEL)(u, path);
    return level < 1 ? 0 : level + 1;
}
static int countMemorizedOfTypeCopy(int u, int path, int type) {
    if (u < 0) return 0;
    int count = 0;
    for (int i = 0; i < 30; i++) {
        int spell = (short)G16(RVA_UNIT_MEMORIZED_SPELLS + (size_t)u * UNIT_SIZE + (size_t)i * 2);
        if (spell < 0) return count;
        const SpellRecord *r = spellSlot(spell);
        if ((path == -1 || r->path == path) &&
            (type == -1 || (r->weapon > 0 && *(const int *)(tableAddress(TABLE_WEAPON) + (size_t)r->weapon * WEAPON_SIZE + WEAPON_DMGTYPE) == type)))
            count++;
    }
    return count;
}
static int countSpellsCopy(int path, int level) {
    int count = 0;
    for (int s = 0; spellSlot(s)->path != SPELL_END_PATH; s++)
        if ((path == -1 || spellSlot(s)->path == path) && (level == -1 || spellSlot(s)->level == level)) count++;
    return count;
}
static int isSpellAttackCopy(int type) { return (unsigned)type <= 0x13 && (0x8c048u >> type) & 1; }
static int findSpellCopy(const char *name) {
    for (int s = 0; spellSlot(s)->path != SPELL_END_PATH; s++)
        if (spellSlot(s)->name && copyNameMatches(spellSlot(s)->name, name)) return s;
    GAME_FN(void (*)(const char *, ...), RVA_TRACE)("*** No spell called %s\n", name);
    return -1;
}
static int findRitualCopy(const char *name, int n) {
    int found = -1;
    for (int slot = 0; slot <= G32(RVA_RITUAL_COUNT) && slot < ritualSlotCount(); slot++)
        if (copyNameMatches((const char *)GAME_PTR(ritualTableRva() + (size_t)slot * RITUAL_SIZE), name) && ++found == n) return slot;
    return -1;
}
static int randomRitualCopy(int ritpow, int level) {
    if (ritualExpansionActive) return randomRitual(ritpow, level);
    static int matches[VANILLA_RITUAL_SLOTS];
    int count = 0;
    for (int slot = 1; slot < VANILLA_RITUAL_SLOTS; slot++) {
        const unsigned char *r = GAME_PTR(RVA_RITUAL_TABLE + (size_t)slot * RITUAL_SIZE);
        if ((*(const int *)(r + RITUAL_RITPOW) == ritpow || ritpow == -1) && (*(const int *)(r + RITUAL_LEVEL) == level || level == -1))
            matches[count++] = slot;
    }
    if (count < 1) return 0;
    return matches[count > 1 ? GAME_FN(int (*)(int), RVA_RANDOM)(count) : 0];
}
static int commanderKnowsRitualCopy(int c, int slot) {
    if (c < 0) return 0;
    for (int i = 0; i < 64; i++) {
        int known = (short)G16(RVA_COMMANDER_KNOWN_RITUALS + (size_t)c * COMMANDER_SIZE + (size_t)i * 2);
        if (known < 1) return 0;
        if (known == slot) return 1;
    }
    return 0;
}

static int commanderUseItemsCopy(int c) {
    if (c < 0 || c > G32(RVA_LAST_COMMANDER)) return 0;
    int u = G32(RVA_COMMANDERS + (size_t)c * COMMANDER_SIZE), used = 0;
    if (u < 0) return 0;
    int (*rate)(int, int) = GAME_FN(int (*)(int, int), RVA_AI_ITEM_USE_RATING);
    void (*use)(int, int) = GAME_FN(void (*)(int, int), RVA_ITEM_USE);
    for (int i = 0; i < ITEM_POSITION_MAX; i++) {
        int item = unitItemAt(u, i);
        if (item >= 0 && rate(u, item) >= 2) use(u, item), used++;
    }
    return used;
}

// ---- Factions -------------------------------------------------------------------------------------

static int playerHasCommanderCopy(int player) {
    for (int c = 0; c <= G32(RVA_LAST_COMMANDER) && c < COMMANDER_TABLE_MAX; c++) {
        int u = G32(RVA_COMMANDERS + (size_t)c * COMMANDER_SIZE);
        if (u < 0 || u > G32(RVA_UNIT_COUNT)) continue;
        if ((short)G16(RVA_UNIT_HP + (size_t)u * UNIT_SIZE) > 0 && (short)G16(RVA_UNIT_PLAYER + (size_t)u * UNIT_SIZE) == player) return 1;
    }
    return 0;
}
static int playerClassAbilityCopy(int player, int ability) {
    if (player < 0) return 0;
    int cls = PG16(RVA_SLOT_CLASS + (size_t)player * 2);
    if (cls < 0 || cls > G32(RVA_CLASS_COUNT) || cls >= tableRecords(TABLE_CLASS)) return 0;
    const int *pairs = (const int *)(tableAddress(TABLE_CLASS) + (size_t)cls * CLASS_SIZE + CLASS_ABILITIES);
    for (int i = 0; i <= 0x1e && pairs[i]; i += 2)
        if (pairs[i] == ability) return pairs[i + 1];
    return 0;
}
#define COPY_ABILITY_IRONBONUS 0xc9
#define COPY_RITUAL_FX_IRONBONUS 0x489
#define COPY_TERRAIN_IRON_BONUS (1ULL << 9)
static int ironBonusPercentCopy(int player) {
    if (player < 0) return 0;
    int total = GAME_FN(int (*)(int, int, int), RVA_PLAYER_ABILITY)(player, COPY_ABILITY_IRONBONUS, 0);
    int mapW = G32(RVA_MAP_W), mapH = G32(RVA_MAP_H);
    long long effects = 0;
    for (int e = 0; e <= G32(RVA_LAST_ENCHANT); e++) {
        const unsigned char *rec = GAME_PTR(RVA_ENCHANTS + (size_t)e * ENCHANT_SIZE);
        int ritual = *(const int *)(rec + ENCHANT_RITUAL), x = *(const int *)(rec + ENCHANT_X), y = *(const int *)(rec + ENCHANT_Y);
        if (!*(const int *)(rec + ENCHANT_KIND) || ritual <= 0 || *(const int *)(rec + ENCHANT_OWNER) != player) continue;
        int owner = x < 0 || y < 0 || x >= mapW || y >= mapH ? -1
                                                             : *(const signed char *)GAME_PTR(RVA_SQUARE_RECORDS + ((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE + SQUARE_OWNER);
        if (!playerSameTeam(owner, player)) continue;
        const unsigned char *fx = GAME_PTR(ritualTableRva() + (size_t)ritual * RITUAL_SIZE + RITUAL_FX);
        for (int i = 0; i < 20; i++) {
            if (*(const int *)(fx + (size_t)i * RITUAL_FX_SIZE) < 1) break;
            if (*(const int *)(fx + (size_t)i * RITUAL_FX_SIZE) == COPY_RITUAL_FX_IRONBONUS) {
                effects += *(const long long *)(fx + (size_t)i * RITUAL_FX_SIZE + 8);
                break;
            }
        }
    }
    int fromEffects = (int)effects < 100 ? (int)effects : 100;
    total += fromEffects;
    for (int x = 0; x < G32(RVA_MAP_DEPTH); x++)
        for (int y = 0; y < mapH; y++) {
            if (x >= mapW) continue;
            size_t square = ((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE;
            if (*(const signed char *)GAME_PTR(RVA_SQUARE_RECORDS + square + SQUARE_OWNER) != player) continue;
            int terrain = copySquareTerrain(x, y);
            if (terrain >= 0 && (unsigned)copyTerrainFlags(terrain) & COPY_TERRAIN_IRON_BONUS) total += 25;
        }
    return total;
}
static int countSlotsWithControllerCopy(int controller) {
    int count = 0;
    for (int p = 0; p < playerSlotCount(); p++)
        if (PG16(RVA_SLOT_CONTROLLER + (size_t)p * 2) == controller) count++;
    return count;
}

// ---- Squares --------------------------------------------------------------------------------------

static int terrainBurnableAtCopy(int x, int y) {
    if (x < 0 || y < 0 || x >= G32(RVA_MAP_W) || y >= G32(RVA_MAP_H) || copySquareTerrain(x, y) < 0) return 0;
    return (int)((copyTerrainFlags(copySquareTerrain(x, y)) >> 1) & 1);
}
static int squareIsNorthCopy(int x, int y) {
    if (x < 0 || y < 0 || x >= G32(RVA_MAP_W) || y >= G32(RVA_MAP_H)) return 0;
    return (short)G16(RVA_SQUARE_RECORDS + ((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE + SQUARE_TEMPERATURE) < 1;
}
static int squareIsWetCopy(int x, int y) {
    if (x < 0 || x >= G32(RVA_MAP_W) || y < 0 || y >= G32(RVA_MAP_H)) return 0;
    if (G32(RVA_SQUARE_FLAGS + ((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE) & SQUARE_FLAG_FLOODED) return 1;
    int terrain = copySquareTerrain(x, y);
    if (terrain == 10 || terrain == 11 || terrain == 0x127 || terrain == 0x158 || terrain < 0) return 0;
    return (copyTerrainFlags(terrain) & 4) != 0;
}
static int armyAtForPlayerCopy(int player, int x, int y) {
    if (x < 0 || y < 0 || x >= G32(RVA_MAP_DEPTH) || y >= G32(RVA_MAP_H)) return 0;
    size_t at = (size_t)x * MAP_COLUMN_SQUARES + (size_t)y;
    short army = G16(RVA_SQUARE_ARMY + at * 2);
    if (!army) return 0;
    if (player < 0 || army < 0) return 1;
    return playerSameTeam((short)G16(RVA_SQUARE_RECORDS + at * SQUARE_SIZE + SQUARE_ARMY_OWNER), player);
}
static int weaponExtraCopy(int weapon, int id) {
    if (weapon <= 0) return 0;
    const int *pairs = (const int *)(tableAddress(TABLE_WEAPON) + (size_t)weapon * WEAPON_SIZE + WEAPON_EXTRAS);
    for (int i = 0; i < 4 && pairs[i]; i += 2)
        if (pairs[i] == id) return pairs[i + 1];
    return 0;
}

// ---- The AI ---------------------------------------------------------------------------------------

static int aiCountAllyTargetsNearCopy(int player, int x, int y, int commander, int range) {
    int count = 0;
    for (int c = 0; c <= G32(RVA_LAST_COMMANDER) && c < COMMANDER_TABLE_MAX; c++) {
        int u = G32(RVA_COMMANDERS + (size_t)c * COMMANDER_SIZE);
        if (u < 0 || (short)G16(RVA_UNIT_HP + (size_t)u * UNIT_SIZE) <= 0) continue;
        int owner = (short)G16(RVA_UNIT_PLAYER + (size_t)u * UNIT_SIZE);
        int dx = (short)G16(RVA_COMMANDER_AI_TARGET_X + (size_t)c * 2) - x, dy = (short)G16(RVA_COMMANDER_AI_TARGET_Y + (size_t)c * 2) - y;
        if (playerSameTeam(owner, player) && abs(dx) <= range && abs(dy) <= range && c != commander) count++;
    }
    return count;
}

// ---- Battle ---------------------------------------------------------------------------------------

#define COPY_BATTLE_LIST_MAX 0x7800
#define COPY_UNIMPORTANT (1ULL << 59)
static int battleWinnerCopy(void) {
    static int list[COPY_BATTLE_LIST_MAX];
    int n = 0, width = G32(RVA_BATTLE_WIDTH);
    for (int row = 0; row < BATTLE_ROWS; row++)
        for (int col = 0; col < width; col++) {
            int u = G32(RVA_BATTLE_GRID + (size_t)col * COPY_BATTLE_COLUMN + (size_t)row * COPY_BATTLE_CELL);
            if (u >= 0 && n < COPY_BATTLE_LIST_MAX) list[n++] = u;
        }
    for (int u = 0; u <= G32(RVA_UNIT_COUNT) && u < UNIT_TABLE_MAX; u++) {
        if ((unsigned)(G32(RVA_UNIT_X + (size_t)u * UNIT_SIZE) + 8) >= 2) continue;
        int listed = 0;
        for (int i = 0; i < n && !listed; i++) listed = list[i] == u;
        if (!listed && n < COPY_BATTLE_LIST_MAX) list[n++] = u;
    }
    void (*trace)(const char *, ...) = GAME_FN(void (*)(const char *, ...), RVA_TRACE);
    int winner = -2;
    while (n > 0) {
        int u = list[--n];
        if ((short)G16(RVA_UNIT_HP + (size_t)u * UNIT_SIZE) <= 0 || GAME_FN(unsigned long long (*)(int), RVA_UNIT_RESISTANCES)(u) & COPY_UNIMPORTANT) continue;
        int owner = u > G32(RVA_UNIT_COUNT) ? -1 : (short)G16(RVA_UNIT_PLAYER + (size_t)u * UNIT_SIZE);
        if (winner == -2) {
            winner = owner;
        } else if (!playerSameTeam(winner, owner)) {
            trace("No fightwinner %d %d\n", winner, owner);
            return -1;
        }
    }
    trace("Fightwinner: %d\n", winner);
    return winner;
}
static void battleEnemyExtentCopy(int player, int *low, int *high) {
    int lo = 9999, hi = -1;
    int (*unitAt)(int, int) = GAME_FN(int (*)(int, int), RVA_BATTLE_UNIT_AT);
    for (int row = 0; row < BATTLE_ROWS; row++)
        for (int col = 0; col < G32(RVA_BATTLE_WIDTH); col++) {
            int u = unitAt(col, row);
            if (u < 0) continue;
            if (u <= G32(RVA_UNIT_COUNT) && playerSameTeam((short)G16(RVA_UNIT_PLAYER + (size_t)u * UNIT_SIZE), player)) continue;
            if (col < lo) lo = col;
            if (col > hi) hi = col;
        }
    *low = lo, *high = hi;
}
#endif
