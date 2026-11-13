#pragma once

#define ITEM_ABILITY_NOEYES     0x1b3
#define ITEM_ABILITY_EXTRAEYES  0x42
#define ITEM_ABILITY_FALSELIFE  0x1e9

typedef int (*CanEquipItemFn)(int unit, int item, int any);
typedef int (*EnforceItemsFn)(int unit);
typedef int (*UnitRemoveItemFn)(int unit, int item);
typedef int (*TakeItemByNameFn)(int unit, const char *name);
typedef int (*UnitNthItemOfKindFn)(int unit, int kind, int n);
#ifndef __APPLE__
typedef int (*GiveItemFn)(int unit, int item, int unused, int *spilled);
typedef int (*ItemSlotKindFn)(int unit, int monster, int position);
#else
typedef int (*GiveItemFn)(int unit, int item, int *spilled);
#endif

static GiveItemFn gameGiveItem;
static CanEquipItemFn gameCanEquipItem;
static EnforceItemsFn gameEnforceItems;
static UnitRemoveItemFn gameUnitRemoveItem;
static TakeItemByNameFn gameTakeItemByName;
static UnitNthItemOfKindFn gameUnitNthItemOfKind;
#ifndef __APPLE__
static ItemSlotKindFn gameItemSlotKindFn;
#endif
static int itemSlotHooksInstalled;

static int itemTypeOf(int item) {
    return *(const short *)(tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE + (RVA_ITEM_SLOT_TYPE - RVA_ITEM_TABLE));
}

static unsigned long long itemFlagsOf(int item) {
    unsigned long long v;
    memcpy(&v, tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE + (RVA_ITEM_FLAGS - RVA_ITEM_TABLE), sizeof v);
    return v;
}

static int itemFirstAbility(int item, int ability) {
    const int *pairs = (const int *)(tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE + ITEM_ABILITIES);
    for (int i = 0; i < ITEM_ABILITY_PAIRS && pairs[2 * i]; i++)
        if (pairs[2 * i] == ability) return pairs[2 * i + 1];
    return 0;
}

static void gameFlushUnitCaches(int unit) { GAME_FN(void (*)(int), RVA_UNIT_FLUSH_CACHES)(unit); }
static void gameItemEquipEffects(int unit, int item) { GAME_FN(void (*)(int, int), RVA_ITEM_EQUIP_EFFECTS)(unit, item); }
static void (*flushUnitItemCaches)(int unit) = gameFlushUnitCaches;
static void (*itemEquipEffects)(int unit, int item) = gameItemEquipEffects;
static int (*itemSlotAbilityTotal)(int unit, int ability, int add) = unitAbilityTotal;

static int unitEyeCount(int unit) {
    if (itemSlotAbilityTotal(unit, ITEM_ABILITY_NOEYES, 1) > 0) return 0;
    return 2 + itemSlotAbilityTotal(unit, ITEM_ABILITY_EXTRAEYES, 1);
}

static int unitLifeless(int unit) {
    int monster = (short)G16(RVA_UNIT_MONSTER + (size_t)unit * UNIT_SIZE);
    const unsigned char *m = monsterRecord(monster);
    if (!m) return 0;
    unsigned long long flags;
    memcpy(&flags, m + (RVA_MONSTER_FLAGS - RVA_MONSTER_TABLE), sizeof flags);
    return (flags >> MONSTER_FLAG_INANIMATE & 1) && itemSlotAbilityTotal(unit, ITEM_ABILITY_FALSELIFE, 1) <= 0;
}

static void equipItemAt(int unit, int position, int item) {
    setUnitItemAt(unit, position, item);
    itemEquipEffects(unit, item);
    flushUnitItemCaches(unit);
}

static int giveItemToPositions(int unit, int item, int *spilled) {
    if (spilled) *spilled = -1;
    if (unit < 0 || item < 0 || item > G32(RVA_ITEM_COUNT)) return -1;
    int type = itemTypeOf(item);
    for (int p = 0; p < ITEM_POSITION_MAX; p++)
        if (unitItemAt(unit, p) < 0 && itemSlotKind(unit, p) == type) {
            equipItemAt(unit, p, item);
            return p;
        }
    if (!spilled) return -1;
    for (int p = 0; p < ITEM_POSITION_MAX; p++) {
        int held = unitItemAt(unit, p);
        if (held < 0 || itemSlotKind(unit, p) != type || (itemFlagsOf(held) & ITEM_FLAG_CURSED)) continue;
        equipItemAt(unit, p, item);
        *spilled = held;
        return p;
    }
    return -1;
}

static int newUnitWatch, newUnitLent = -1;
static void watchNewUnitGive(int unit) {
    if (!newUnitWatch || newUnitLent >= 0) return;
    newUnitLent = unit;
    lendUnitSlot(unit);
}

#ifndef __APPLE__
static int giveItemHook(int unit, int item, int unused, int *spilled) {
    watchNewUnitGive(unit);
    if (!unitNeedsItemSlots(unit)) return gameGiveItem(unit, item, unused, spilled);
    return giveItemToPositions(unit, item, spilled);
}
#else
static int giveItemHook(int unit, int item, int *spilled) {
    watchNewUnitGive(unit);
    if (!unitNeedsItemSlots(unit)) return gameGiveItem(unit, item, spilled);
    return giveItemToPositions(unit, item, spilled);
}
#endif

static int canEquipItemHook(int unit, int item, int any) {
    if (!unitNeedsItemSlots(unit)) return gameCanEquipItem(unit, item, any);
    if (unit < 0 || item < 0) return -1;
    if (item >= tableRecords(TABLE_ITEM)) return -1;
    if (itemFirstAbility(item, ITEM_ABILITY_EYE) > 0 && unitEyeCount(unit) <= 0) return -1;
    if (itemFirstAbility(item, ITEM_ABILITY_HEART) == 1 && unitLifeless(unit)) return -1;
    int type = itemTypeOf(item);
    for (int p = 0; p < ITEM_POSITION_MAX; p++)
        if (unitItemAt(unit, p) < 0 && itemSlotKind(unit, p) == type) return p;
    return -1;
}

static int enforceItemsHook(int unit) {
    if (!unitNeedsItemSlots(unit)) return gameEnforceItems(unit);
    if (unit < 0) return 0;
    int removed = 0;
    for (int p = 0; p < ITEM_POSITION_MAX; p++) {
        int item = unitItemAt(unit, p);
        if (item < 0) continue;
        int kind = itemSlotKind(unit, p);
        if (kind > 0 && kind == itemTypeOf(item)) continue;
        setUnitItemAt(unit, p, -1);
        removed++;
        flushUnitItemCaches(unit);
        writeLog("item_slots_enforced", "unit=%d position=%d item=%d", unit, p, item);
    }
    return removed;
}

static int unitRemoveItemHook(int unit, int item) {
    if (!unitNeedsItemSlots(unit)) return gameUnitRemoveItem(unit, item);
    if (unit < 0 || item < 0) return -1;
    for (int p = 0; p < ITEM_POSITION_MAX; p++)
        if (unitItemAt(unit, p) == item) {
            setUnitItemAt(unit, p, -1);
            flushUnitItemCaches(unit);
            return p;
        }
    return -1;
}

static int takeItemByNameHook(int unit, const char *name) {
    if (!unitNeedsItemSlots(unit)) return gameTakeItemByName(unit, name);
    for (int p = 0; p < ITEM_POSITION_MAX; p++) {
        int item = unitItemAt(unit, p);
        if (item <= 0) continue;
        if (strncasecmp(name, (const char *)(tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE), ITEM_NAME_SIZE)) continue;
        setUnitItemAt(unit, p, -1);
        return item;
    }
    return -1;
}

static int unitNthItemOfKindHook(int unit, int kind, int n) {
    if (!unitNeedsItemSlots(unit)) return gameUnitNthItemOfKind(unit, kind, n);
    if (unit < 0) return -1;
    int seen = 0;
    for (int p = 0; p < ITEM_POSITION_MAX; p++) {
        int item = unitItemAt(unit, p);
        if (item < 0 || (kind != -1 && itemSlotKind(unit, p) != kind)) continue;
        if (seen == n) return item;
        seen++;
    }
    return -1;
}

#ifndef __APPLE__
// @platform-split item_slot_kind: Windows hooks RVA_ITEM_SLOT_KIND; Mac inlines it into the functions taken over here and the unit card
static int itemSlotKindHook(int unit, int monster, int position) {
    const MonsterItemSlots *s = monster >= 0 ? monsterItemSlotsOf(monster) : unitItemSlots(unit);
    if (unit < 0 || !s) return gameItemSlotKindFn(unit, monster, position);
    return position >= 0 && position < ITEM_POSITION_MAX ? s->kinds[position] : 0;
}
#endif

// ---- Units across new slots, fights, replays and loads ------------------------------------------------------
typedef int (*AllocUnitFn)(int player);
typedef void (*LoadUnitFn)(int unit, void *file);
typedef void (*SaveUnitFn)(int unit, void *file);
static SaveUnitFn gameSaveUnit;
typedef void (*BattleLoadOthersFn)(void);
static AllocUnitFn gameAllocUnit;
static LoadUnitFn gameLoadUnit;
static BattleLoadOthersFn gameBattleLoadOthers;

static int allocUnitHook(int player) {
    int unit = gameAllocUnit(player);
    lendUnitSlot(unit);
    return unit;
}

static void saveUnitHook(int unit, void *file) {
    parkUnitItems(unit);
    gameSaveUnit(unit, file);
}

static int replayLoadingUnits(void);
static void giveReplayUnitItems(int unit);
static void endReplayItems(void);

static void loadUnitHook(int unit, void *file) {
    int replay = !reloadingParkedUnits && replayLoadingUnits();
    if (!reloadingParkedUnits) lendUnitSlot(unit);
    gameLoadUnit(unit, file);
    if (reloadingParkedUnits) returnUnitSlot(unit);
    else if (replay) giveReplayUnitItems(unit);
}

static void battleLoadOthersHook(void) {
    endReplayItems();
    reloadingParkedUnits = 1;
    gameBattleLoadOthers();
    reloadingParkedUnits = 0;
    endLentRound();
}

static void itemSlotsNewUnitBefore(const CallInfo *call) {
    (void)call;
    newUnitWatch = 1, newUnitLent = -1;
}
static void itemSlotsNewUnitAfter(const CallInfo *call) {
    int unit = (int)call->result;
    if (unit >= 0 && newUnitLent != unit) lendUnitSlot(unit);
    newUnitWatch = 0, newUnitLent = -1;
}

static u64 (*nextItemSlotsNewWorld)(u64);
static void clearItemSlotsForNewWorld(void) {
    clearAllExtraItems();
    endLentRound();
}
static u64 itemSlotsNewWorld(u64 mode) {
    clearItemSlotsForNewWorld();
    return nextItemSlotsNewWorld(mode);
}

// ---- Dead units' items dropped on their square ---------------------------------------------------------------
typedef int (*DropUnitItemsAtFn)(int x, int y, int message);
#ifndef __APPLE__
typedef int (*DeadUnitsPassFn)(int unused);
#else
typedef void (*DeadUnitsPassFn)(void);
#endif
static DropUnitItemsAtFn gameDropUnitItemsAt;
static DeadUnitsPassFn gameDeadUnitsPass;

static int gameFoundItem(int x, int y, int item, int kind, int message) {
    return GAME_FN(int (*)(int, int, int, int, int), RVA_FOUND_ITEM)(x, y, item, kind, message);
}
static int (*itemSlotFoundItem)(int x, int y, int item, int kind, int message) = gameFoundItem;

static int dropUnitItemsAtPositions(int x, int y, int message) {
    if (*(const short *)(gameBase + RVA_SQUARE_RECORDS + ((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE + SQUARE_ARMY_OWNER) < 0) return 0;
    int placed = 0, last = G32(RVA_UNIT_COUNT);
    for (int u = 0; u <= last && u < UNIT_TABLE_MAX; u++) {
        size_t at = (size_t)u * UNIT_SIZE;
        if (G32(RVA_UNIT_X + at) != x || G32(RVA_UNIT_X + 4 + at) != y || (short)G16(RVA_UNIT_HP + at) > 0) continue;
        for (int p = 0; p < ITEM_POSITION_MAX; p++) {
            int item = unitItemAt(u, p);
            if (item < 0) continue;
            setUnitItemAt(u, p, -1);
            if (itemSlotFoundItem(x, y, item, 1, message) > 0) placed++;
        }
    }
    return placed;
}

static int dropUnitItemsAtHook(int x, int y, int message) {
    if (!anyExtraItems()) return gameDropUnitItemsAt(x, y, message);
    return dropUnitItemsAtPositions(x, y, message);
}

static int deadUnitsPassPositions(void) {
    int placed = 0;
    for (int u = 0; u <= G32(RVA_UNIT_COUNT) && u < UNIT_TABLE_MAX; u++) {
        size_t at = (size_t)u * UNIT_SIZE;
        int x = G32(RVA_UNIT_X + at), y = G32(RVA_UNIT_X + 4 + at), holds = 0;
        if ((short)G16(RVA_UNIT_HP + at) > 0 || x < 0 || y < 0) continue;
        for (int p = 0; p < ITEM_POSITION_MAX && !holds; p++) holds = unitItemAt(u, p) >= 0;
        if (holds) placed += dropUnitItemsAtPositions(x, y, 1);
    }
    return placed;
}

#ifndef __APPLE__
static int deadUnitsPassHook(int unused) {
    if (!anyExtraItems()) return gameDeadUnitsPass(unused);
    return deadUnitsPassPositions();
}
#else
static void deadUnitsPassHook(void) {
    if (!anyExtraItems()) {
        gameDeadUnitsPass();
        return;
    }
    deadUnitsPassPositions();
}
#endif

// ---- Using an item past position 7: RVA_ITEM_USE ---------------------------------------------------------------
typedef void (*ItemUseFn)(int unit, int item);
static ItemUseFn gameItemUse;

static void itemUseHook(int unit, int item) {
    if (!unitExtraItemCount(unit)) {
        gameItemUse(unit, item);
        return;
    }
    int held = -1;
    for (int p = 0; p < ITEM_POSITION_MAX && held < 0; p++)
        if (unitItemAt(unit, p) == item) held = p;
    if (held < UNIT_ITEM_SLOTS) {
        gameItemUse(unit, item);
        return;
    }
    int front = unitItemAt(unit, 0);
    setUnitItemAt(unit, 0, item);
    setUnitItemAt(unit, held, front);
    gameItemUse(unit, item);
    int left = unitItemAt(unit, 0);
    setUnitItemAt(unit, 0, front);
    setUnitItemAt(unit, held, left);
}

// ---- +hasitem and +hasitemhere: RVA_FIND_ITEM_HOLDER_AT ---------------------------------------------------
typedef int (*ItemHolderAtFn)(int player, int x, int y, const char *name);
static ItemHolderAtFn gameItemHolderAt;
#define ITEM_HOLDERS_MAX 10000
static int gameRandomBelow(int n) { return GAME_FN(int (*)(int), RVA_RANDOM)(n); }
static int (*itemSlotRandom)(int n) = gameRandomBelow;

// @platform-split event_finder_army_test: Mac runs CoE5SE's copy of the army test its finders inline
static int itemHolderArmyHere(int player, int x, int y) {
#ifdef __APPLE__
    return armyHereForPlayer(player, x, y);
#else
    return GAME_FN(int (*)(int, int, int), RVA_ARMY_AT_FOR_PLAYER)(player, x, y);
#endif
}

static int itemHolderAtHook(int player, int x, int y, const char *name) {
    if (!anyExtraItems()) return gameItemHolderAt(player, x, y, name);
    if (x >= 0 && !itemHolderArmyHere(player, x, y)) return -1;
    static int holders[ITEM_HOLDERS_MAX];
    int count = 0, last = G32(RVA_UNIT_COUNT);
    for (int u = 0; u <= last && u < UNIT_TABLE_MAX; u++) {
        size_t at = (size_t)u * UNIT_SIZE;
        if ((short)G16(RVA_UNIT_HP + at) <= 0) continue;
        if (x != -1 && (G32(RVA_UNIT_X + at) != x || G32(RVA_UNIT_X + 4 + at) != y)) continue;
        if (player != -1 && (short)G16(RVA_UNIT_PLAYER + at) != player) continue;
        for (int p = 0; p < ITEM_POSITION_MAX; p++) {
            int item = unitItemAt(u, p);
            if (item < 0 || strcmp(name, (const char *)(tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE))) continue;
            if (count < ITEM_HOLDERS_MAX) holders[count++] = u;
            break;
        }
    }
    if (!count) return -1;
    return holders[itemSlotRandom(count)];
}

static int installItemSlotHooks(void) {
    if (itemSlotHooksInstalled) return itemSlotHooksInstalled > 0;
    struct { unsigned int rva; void *hook; void **game; } hooks[] = {
        {RVA_GIVE_ITEM, (void *)giveItemHook, (void **)&gameGiveItem},
        {RVA_CAN_EQUIP_ITEM, (void *)canEquipItemHook, (void **)&gameCanEquipItem},
        {RVA_ENFORCE_ITEMS, (void *)enforceItemsHook, (void **)&gameEnforceItems},
        {RVA_UNIT_REMOVE_ITEM, (void *)unitRemoveItemHook, (void **)&gameUnitRemoveItem},
        {RVA_TAKE_ITEM_BY_NAME, (void *)takeItemByNameHook, (void **)&gameTakeItemByName},
        {RVA_UNIT_NTH_ITEM_OF_KIND, (void *)unitNthItemOfKindHook, (void **)&gameUnitNthItemOfKind},
        {RVA_ALLOC_UNIT, (void *)allocUnitHook, (void **)&gameAllocUnit},
        {RVA_SAVE_UNIT, (void *)saveUnitHook, (void **)&gameSaveUnit},
        {RVA_LOAD_UNIT, (void *)loadUnitHook, (void **)&gameLoadUnit},
        {RVA_BATTLE_LOAD_OTHERS, (void *)battleLoadOthersHook, (void **)&gameBattleLoadOthers},
        {RVA_DROP_UNIT_ITEMS_AT, (void *)dropUnitItemsAtHook, (void **)&gameDropUnitItemsAt},
        {RVA_DEAD_UNITS_PASS, (void *)deadUnitsPassHook, (void **)&gameDeadUnitsPass},
        {RVA_FIND_ITEM_HOLDER_AT, (void *)itemHolderAtHook, (void **)&gameItemHolderAt},
        {RVA_ITEM_USE, (void *)itemUseHook, (void **)&gameItemUse},
#ifndef __APPLE__
        {RVA_ITEM_SLOT_KIND, (void *)itemSlotKindHook, (void **)&gameItemSlotKindFn},
#endif
    };
    int count = (int)(sizeof hooks / sizeof hooks[0]), ok = 0;
    int store = ensureLentItems();
    for (int i = 0; store && i < count; i++) {
        void *target = gameBase + hooks[i].rva;
        ok += fanpatchHookCreate(target, hooks[i].hook, hooks[i].game) && fanpatchHookEnable(target, 1);
    }
    int sites = store && apiObserve(SITE_NEW_UNIT, itemSlotsNewUnitBefore, itemSlotsNewUnitAfter) &&
                wrapSite(SITE_PLAY_GAME_NEW_WORLD, (void *)itemSlotsNewWorld, (void **)&nextItemSlotsNewWorld);
    itemSlotHooksInstalled = ok == count && sites ? 1 : -1;
    writeInstallLogFields("item_slot_hooks_installed", ok + sites, count + 1, "hooks=%d sites=%d", ok, sites);
    return itemSlotHooksInstalled > 0;
}

static void keepweaponsLine(const char *args, int mod, int level);

static void itemSlotsInit(void) {
    int word = apiAddModCommand("itemslots", itemslotsLine), keep = apiAddModCommand("keepweapons", keepweaponsLine);
    writeLog("item_slots_ready", "word=%d keepweapons=%d", word, keep);
}
