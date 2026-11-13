#pragma once

#define ITEM_POSITION_MAX    20
#define ITEM_EXTRA_POSITIONS (ITEM_POSITION_MAX - UNIT_ITEM_SLOTS)

static short *extraItems;
static unsigned char *extraItemCounts;

static int extraItemUnits;

static __attribute__((unused)) int anyExtraItems(void) { return extraItemUnits > 0; }

static __attribute__((unused)) int unitExtraItemCount(int unit) {
    if (!extraItems || unit < 0 || unit >= UNIT_TABLE_MAX) return 0;
    return extraItemCounts[unit];
}

static __attribute__((unused)) int unitItemAt(int unit, int position) {
    if (unit < 0 || unit >= UNIT_TABLE_MAX || position < 0 || position >= ITEM_POSITION_MAX) return -1;
    if (position < UNIT_ITEM_SLOTS) return (short)G16(RVA_UNIT_ITEMS + (size_t)unit * UNIT_SIZE + (size_t)position * 2);
    if (!extraItems || !extraItemCounts[unit]) return -1;
    return extraItems[(size_t)unit * ITEM_EXTRA_POSITIONS + (size_t)(position - UNIT_ITEM_SLOTS)];
}

static int ensureExtraItems(void) {
    if (extraItems) return 1;
    short *items = malloc((size_t)UNIT_TABLE_MAX * ITEM_EXTRA_POSITIONS * sizeof(short));
    unsigned char *counts = calloc(UNIT_TABLE_MAX, 1);
    if (!items || !counts) {
        free(items);
        free(counts);
        writeLogWarning("item_slots_store_failed", "reason=out_of_memory units=%d", UNIT_TABLE_MAX);
        return 0;
    }
    memset(items, 0xff, (size_t)UNIT_TABLE_MAX * ITEM_EXTRA_POSITIONS * sizeof(short));
    extraItems = items;
    extraItemCounts = counts;
    return 1;
}

static __attribute__((unused)) int setUnitItemAt(int unit, int position, int item) {
    if (unit < 0 || unit >= UNIT_TABLE_MAX || position < 0 || position >= ITEM_POSITION_MAX || item < -1 || item >= ITEM_RECORDS_MOVED) {
        writeLogWarning("item_slots_write_refused", "reason=bad_value unit=%d position=%d item=%d", unit, position, item);
        return 0;
    }
    if (position < UNIT_ITEM_SLOTS) {
        G16(RVA_UNIT_ITEMS + (size_t)unit * UNIT_SIZE + (size_t)position * 2) = (short)item;
        return 1;
    }
    if (item < 0 && !extraItems) return 1;
    if (!ensureExtraItems()) return 0;
    short *at = &extraItems[(size_t)unit * ITEM_EXTRA_POSITIONS + (size_t)(position - UNIT_ITEM_SLOTS)];
    if (*at < 0 && item >= 0 && !extraItemCounts[unit]++) extraItemUnits++;
    if (*at >= 0 && item < 0 && !--extraItemCounts[unit]) extraItemUnits--;
    *at = (short)item;
    return 1;
}

static __attribute__((unused)) void clearUnitExtraItems(int unit) {
    if (!extraItems || unit < 0 || unit >= UNIT_TABLE_MAX || !extraItemCounts[unit]) return;
    memset(&extraItems[(size_t)unit * ITEM_EXTRA_POSITIONS], 0xff, ITEM_EXTRA_POSITIONS * sizeof(short));
    extraItemCounts[unit] = 0;
    extraItemUnits--;
}

static __attribute__((unused)) void clearAllExtraItems(void) {
    if (!extraItems) return;
    memset(extraItems, 0xff, (size_t)UNIT_TABLE_MAX * ITEM_EXTRA_POSITIONS * sizeof(short));
    memset(extraItemCounts, 0, UNIT_TABLE_MAX);
    extraItemUnits = 0;
}

// ---- A slot lent to another unit: its positions 8..19 kept from the file the game parks it in ----------------
static short *lentItems;
static unsigned *lentRounds;
static unsigned lentRound = 1;
static int reloadingParkedUnits;

static int ensureLentItems(void) {
    if (lentItems) return 1;
    if (!ensureExtraItems()) return 0;
    short *items = malloc((size_t)UNIT_TABLE_MAX * ITEM_EXTRA_POSITIONS * sizeof(short));
    unsigned *rounds = calloc(UNIT_TABLE_MAX, sizeof(unsigned));
    if (!items || !rounds) {
        free(items);
        free(rounds);
        writeLogWarning("item_slots_store_failed", "reason=out_of_memory part=lent units=%d", UNIT_TABLE_MAX);
        return 0;
    }
    lentItems = items;
    lentRounds = rounds;
    return 1;
}

static void parkUnitItems(int unit) {
    if (!lentItems || unit < 0 || unit >= UNIT_TABLE_MAX) return;
    short *kept = &lentItems[(size_t)unit * ITEM_EXTRA_POSITIONS];
    if (extraItemCounts[unit]) memcpy(kept, &extraItems[(size_t)unit * ITEM_EXTRA_POSITIONS], ITEM_EXTRA_POSITIONS * sizeof(short));
    else memset(kept, 0xff, ITEM_EXTRA_POSITIONS * sizeof(short));
    lentRounds[unit] = lentRound;
}

static void lendUnitSlot(int unit) {
    if (!extraItems || unit < 0 || unit >= UNIT_TABLE_MAX || !extraItemCounts[unit]) return;
    writeLog("item_slots_slot_emptied", "unit=%d items=%d round=%u", unit, extraItemCounts[unit], lentRound);
    clearUnitExtraItems(unit);
}

static void returnUnitSlot(int unit) {
    if (!lentItems || unit < 0 || unit >= UNIT_TABLE_MAX || lentRounds[unit] != lentRound) return;
    clearUnitExtraItems(unit);
    for (int i = 0; i < ITEM_EXTRA_POSITIONS; i++) {
        short item = lentItems[(size_t)unit * ITEM_EXTRA_POSITIONS + (size_t)i];
        if (item >= 0) setUnitItemAt(unit, UNIT_ITEM_SLOTS + i, item);
    }
    lentRounds[unit] = 0;
}

static void endLentRound(void) {
    lentRound++;
    if (!lentRound) lentRound = 1;
}

// ---- The layout: the item type each position takes -------------------------------------------------
#define ITEMSLOTS_WORD        "itemslots"
#define ITEM_TYPE_WEAPON      1
#define ITEM_TYPE_MISC        7
#define ITEM_TYPE_COUNT       8

typedef struct {
    unsigned char set;
    unsigned char total;
    unsigned char counts[ITEM_TYPE_COUNT];
    unsigned char kinds[ITEM_POSITION_MAX];
} MonsterItemSlots;
static MonsterItemSlots *monsterItemSlots;
static int monsterItemSlotCount;
static int itemSlotsLines;

static MonsterItemSlots *monsterItemSlotsOf(int monster) {
    if (!monsterItemSlots || monster < 0 || monster >= monsterItemSlotCount || !monsterItemSlots[monster].set) return NULL;
    return &monsterItemSlots[monster];
}

static MonsterItemSlots *unitItemSlots(int unit) {
    if (!monsterItemSlots || unit < 0 || unit >= UNIT_TABLE_MAX) return NULL;
    return monsterItemSlotsOf((short)G16(RVA_UNIT_MONSTER + (size_t)unit * UNIT_SIZE));
}

static int unitNeedsItemSlots(int unit) { return unitItemSlots(unit) || unitExtraItemCount(unit); }

static void layOutItemSlots(MonsterItemSlots *s) {
    int at = 0;
    for (int type = 1; type < ITEM_TYPE_COUNT; type++)
        for (int n = 0; n < s->counts[type] && at < ITEM_POSITION_MAX; n++) s->kinds[at++] = (unsigned char)type;
    s->total = (unsigned char)at;
    for (int p = at; p < ITEM_POSITION_MAX; p++) s->kinds[p] = 0;
}

static int gameItemSlotKind(unsigned long long flags, int position) {
    static const unsigned char allSlots[UNIT_ITEM_SLOTS] = {1, 0, 3, 4, 5, 6, 7, 7};
    if (position < 0 || position >= UNIT_ITEM_SLOTS || (flags >> MONSTER_FLAG_NOSLOTS & 1)) return 0;
    int noBoots = (int)(flags >> MONSTER_FLAG_NOBOOTSLOTS & 1);
    if (flags >> MONSTER_FLAG_WEAPONSLOTS & 1) return position == 0 ? ITEM_TYPE_WEAPON : position >= 6 ? ITEM_TYPE_MISC : 0;
    if (!noBoots && !(flags >> MONSTER_FLAG_ALLITEMSLOTS & 1)) return position >= 6 ? ITEM_TYPE_MISC : 0;
    return position == 5 && noBoots ? 0 : allSlots[position];
}

static unsigned long long gameUnitResistances(int unit);
static unsigned long long (*itemSlotUnitFlags)(int unit) = gameUnitResistances;

static int itemSlotKind(int unit, int position) {
    if (unit < 0 || unit >= UNIT_TABLE_MAX || position < 0 || position >= ITEM_POSITION_MAX) return 0;
    const MonsterItemSlots *s = unitItemSlots(unit);
    if (s) return s->kinds[position];
    if (position >= UNIT_ITEM_SLOTS) return 0;
    return gameItemSlotKind(itemSlotUnitFlags(unit), position);
}

static int installItemSlotHooks(void);
static int installItemSlotStats(void);
static int installItemSlotCard(void);
static int installItemSlotBattle(void);
static int installItemSlotWeapons(void);
static int installItemSlotNet(void);
static int installItemSlotReplay(void);

static const char *const ITEM_TYPE_LABELS[ITEM_TYPE_COUNT] = {"no item type", "weapon", "missile weapon", "helmet",
                                                               "body armor", "gauntlets", "boots", "misc item"};

static int readItemSlotType(const char **p) {
    skipSpaces(p);
    int type = -1;
    if (**p == '"') {
        const char *end = strchr(*p + 1, '"');
        if (!end) return -1;
        size_t len = (size_t)(end - *p - 1);
        for (int t = 0; t < ITEM_TYPE_COUNT; t++)
            if (strlen(ITEM_TYPE_LABELS[t]) == len && !strncmp(ITEM_TYPE_LABELS[t], *p + 1, len)) type = t;
        *p = end + 1;
    } else {
        long long n;
        if (!readSpellNumber(p, &n) || n < 0 || n >= ITEM_TYPE_COUNT) return -1;
        type = (int)n;
    }
    return type < ITEM_TYPE_WEAPON || type == ITEM_TYPE_MISSILE_WEAPON ? -1 : type;
}

static void itemslotsLine(const char *args, int mod, int level) {
    if (level != 2) return;
    const char *p = args;
    long long count = -1;
    int type = readItemSlotType(&p);
    if (type < 0 || !readSpellNumber(&p, &count) || (skipSpaces(&p), !isLineEnd(p)) || count < 0 || count > ITEM_POSITION_MAX) {
        refuseC5mLine(ITEMSLOTS_WORD, spellModName(mod), "bad_value", "line=%d text=%.40s types=1,3-7 max=%d", G32(RVA_MOD_LINE), args,
                      ITEM_POSITION_MAX);
        return;
    }
    int monster = G32(RVA_MONSTER_CURRENT);
    if (!monsterRecord(monster)) {
        refuseC5mLine(ITEMSLOTS_WORD, spellModName(mod), "no_monster", "line=%d", G32(RVA_MOD_LINE));
        return;
    }
    if (!installItemSlotHooks() || !(featureOn(FEATURE_ITEM_SLOT_STATS) && installItemSlotStats()) || !installItemSlotCard() ||
        !installItemSlotBattle() || !installItemSlotWeapons() || !installItemSlotNet() || !installItemSlotReplay()) {
        refuseC5mLine(ITEMSLOTS_WORD, spellModName(mod), "hooks_failed", "line=%d", G32(RVA_MOD_LINE));
        return;
    }
    if (!monsterItemSlots) {
        int records = tableRecords(TABLE_MONSTER);
        if (!(monsterItemSlots = calloc((size_t)records, sizeof monsterItemSlots[0]))) {
            refuseC5mLine(ITEMSLOTS_WORD, spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
            return;
        }
        monsterItemSlotCount = records;
    }
    if (monster >= monsterItemSlotCount) {
        refuseC5mLine(ITEMSLOTS_WORD, spellModName(mod), "no_monster", "line=%d monster=%d", G32(RVA_MOD_LINE), monster);
        return;
    }
    MonsterItemSlots *s = &monsterItemSlots[monster];
    if (s->set && s->counts[type]) {
        refuseC5mLine(ITEMSLOTS_WORD, spellModName(mod), "already_set", "line=%d monster=%.48s type=%d kept=%d", G32(RVA_MOD_LINE),
                      (const char *)monsterRecord(monster), type, s->counts[type]);
        return;
    }
    if (s->total + count > ITEM_POSITION_MAX) {
        refuseC5mLine(ITEMSLOTS_WORD, spellModName(mod), "too_many_positions", "line=%d monster=%.48s had=%d max=%d", G32(RVA_MOD_LINE),
                      (const char *)monsterRecord(monster), s->set ? s->total : 0, ITEM_POSITION_MAX);
        return;
    }
    if (!s->set) memset(s, 0, sizeof *s);
    s->set = 1;
    s->counts[type] = (unsigned char)count;
    layOutItemSlots(s);
    itemSlotsLines++;
}

static void copyMonsterItemSlots(int source, int target) {
    if (!monsterItemSlots || target < 0 || target >= monsterItemSlotCount) return;
    if (source >= 0 && source < monsterItemSlotCount) monsterItemSlots[target] = monsterItemSlots[source];
    else memset(&monsterItemSlots[target], 0, sizeof monsterItemSlots[target]);
}

static void clearMonsterItemSlots(int monster) {
    if (!monsterItemSlots || monster < 0 || monster >= monsterItemSlotCount) return;
    memset(&monsterItemSlots[monster], 0, sizeof monsterItemSlots[monster]);
}

static void itemSlotsBeforePass(int level) {
    if (level != 2) return;
    if (monsterItemSlots) memset(monsterItemSlots, 0, (size_t)monsterItemSlotCount * sizeof monsterItemSlots[0]);
    itemSlotsLines = 0;
}

// ---- Saves and turn files: ITMS ------------------------------------------------------------------------------
#define ITEM_SLOTS_SAVE_VERSION 1

static int turnFileReadPlayer(void);

static int itemSlotsUnitPlayer(int unit) {
    return unit >= 0 && unit < UNIT_TABLE_MAX ? (short)G16(RVA_UNIT_PLAYER + (size_t)unit * UNIT_SIZE) : -1;
}

static int itemSlotsItemNamed(const char *name) {
    int last = G32(RVA_ITEM_COUNT);
    for (int i = 0; i <= last && i < tableRecords(TABLE_ITEM); i++)
        if (!strncmp((const char *)(tableAddress(TABLE_ITEM) + (size_t)i * ITEM_SIZE), name, ITEM_NAME_SIZE)) return i;
    return -1;
}

static int itemSlotsUnitWritten(int unit, int turnPlayer) {
    return unitExtraItemCount(unit) && (turnPlayer < 0 || itemSlotsUnitPlayer(unit) == turnPlayer);
}

static void addItemSlotsChunk(SaveList *list) {
    int turn = saveTailPlayer >= 0;
    if (!anyExtraItems() || (turn && netRoleNow() != NET_ROLE_CLIENT)) return;
    int entries = 0;
    for (int u = 0; u < UNIT_TABLE_MAX; u++)
        if (itemSlotsUnitWritten(u, saveTailPlayer)) entries += unitExtraItemCount(u);
    if (!entries && !turn) return;
    SaveBuffer chunk = {0};
    saveBufferInt(&chunk, ITEM_SLOTS_SAVE_VERSION);
    saveBufferInt(&chunk, entries);
    for (int u = 0; u < UNIT_TABLE_MAX; u++) {
        if (!itemSlotsUnitWritten(u, saveTailPlayer)) continue;
        for (int p = UNIT_ITEM_SLOTS; p < ITEM_POSITION_MAX; p++) {
            int item = unitItemAt(u, p);
            if (item < 0) continue;
            saveBufferInt(&chunk, u);
            saveBufferInt(&chunk, p);
            saveBufferString(&chunk, (const char *)(tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE));
        }
    }
    writeLog("item_slots_written", "file=%s entries=%d bytes=%zu", turn ? "turn" : "save", entries, chunk.size);
    saveListAdd(list, SAVE_TAG_ITMS, &chunk);
}

static void useItemSlotsChunk(const SaveChunk *chunk, SaveFile kind) {
    int turn = kind == SAVE_FILE_TURN, player = turn ? turnFileReadPlayer() : -1;
    endLentRound();
    if (turn && !chunk) return;
    if (turn && (netRoleNow() != NET_ROLE_HOST || player < 0)) {
        writeLogWarning("item_slots_refused", "reason=not_the_host_read");
        return;
    }
    if (!turn) clearAllExtraItems();
    if (!chunk) return;
    SaveCursor c = {chunk->payload, chunk->payload + chunk->size, 0};
    int version = saveCursorInt(&c), entries = saveCursorInt(&c);
    if (c.failed || version != ITEM_SLOTS_SAVE_VERSION || entries < 0 || entries > chunk->size / 9) {
        writeLogWarning("item_slots_read", "file=%s reason=%s version=%d", SAVE_FILE_NAMES[kind],
                        !c.failed && version != ITEM_SLOTS_SAVE_VERSION ? "other_version" : "bad_chunk", version);
        return;
    }
    if (turn)
        for (int u = 0; u < UNIT_TABLE_MAX; u++)
            if (unitExtraItemCount(u) && itemSlotsUnitPlayer(u) == player) clearUnitExtraItems(u);
    int read = 0, skipped = 0, missing = 0;
    for (int i = 0; i < entries; i++) {
        int unit = saveCursorInt(&c), position = saveCursorInt(&c);
        char name[SAVE_STRING_MAX + 1];
        saveCursorString(&c, name, sizeof name);
        if (c.failed) break;
        if (unit < 0 || unit >= UNIT_TABLE_MAX || position < UNIT_ITEM_SLOTS || position >= ITEM_POSITION_MAX ||
            (turn && itemSlotsUnitPlayer(unit) != player)) {
            skipped++;
            continue;
        }
        int item = itemSlotsItemNamed(name);
        if (item < 0) {
            missing++;
            continue;
        }
        if (setUnitItemAt(unit, position, item)) read++;
        else skipped++;
    }
    if (c.failed) writeLogWarning("item_slots_read", "file=%s reason=bad_chunk read=%d", SAVE_FILE_NAMES[kind], read);
    else if (skipped || missing)
        writeLogWarning("item_slots_read", "file=%s reason=bad_entries read=%d skipped=%d missing=%d player=%d", SAVE_FILE_NAMES[kind],
                        read, skipped, missing, player);
    else writeLog("item_slots_read", "file=%s read=%d player=%d", SAVE_FILE_NAMES[kind], read, player);
}
