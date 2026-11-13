// @platform-split item_slot_battle: Windows takes over the out-of-line functions at their entry and jumps to stubs inside the rest; Mac, whose compiler copied most of them into their callers, uses step hooks there
#pragma once

// ---- The game's calls, both builds ---------------------------------------------------------------------------------
static const char *gameUnitTraceName(int unit) { return GAME_FN(const char *(*)(int), RVA_UNIT_TRACE_NAME)(unit); }
static int gameAiItemValue(int unit, int item) { return GAME_FN(int (*)(int, int), RVA_AI_ITEM_VALUE)(unit, item); }
static int gameAiItemUseRating(int unit, int item) { return GAME_FN(int (*)(int, int), RVA_AI_ITEM_USE_RATING)(unit, item); }
static void gameUseItem(int unit, int item) { GAME_FN(void (*)(int, int), RVA_ITEM_USE)(unit, item); }
static int gameCanEquip(int unit, int item, int player) { return GAME_FN(CanEquipItemFn, RVA_CAN_EQUIP_ITEM)(unit, item, player); }
static int gameRemoveItem(int unit, int item) { return GAME_FN(UnitRemoveItemFn, RVA_UNIT_REMOVE_ITEM)(unit, item); }
static int gameEnforceItemsNow(int unit) { return GAME_FN(EnforceItemsFn, RVA_ENFORCE_ITEMS)(unit); }
#ifndef __APPLE__
static void gameGiveItemTo(int unit, int item) { GAME_FN(GiveItemFn, RVA_GIVE_ITEM)(unit, item, 0, NULL); }
// @platform-split find_named_unit: Windows passes RVA_FIND_NAMED_UNIT six arguments, Mac five, as each build's soul contract walk does
static int gameFindContractMaster(const char *name) {
    return GAME_FN(int (*)(int, const char *, int, int, int, int), RVA_FIND_NAMED_UNIT)(-1, name, -1, 0, 0, 0);
}
static int itemSpellNamed(const char *name) { return GAME_FN(int (*)(const char *), RVA_FIND_SPELL)(name); }
#else
static void gameGiveItemTo(int unit, int item) { GAME_FN(GiveItemFn, RVA_GIVE_ITEM)(unit, item, NULL); }
static int gameFindContractMaster(const char *name) {
    return GAME_FN(int (*)(int, const char *, int, int, int), RVA_FIND_NAMED_UNIT)(-1, name, 0, 0, 0);
}
static int itemSpellNamed(const char *name) { return findSpellCopy(name); }
#endif
static void gameAddMessage(int player, int x, int y, int unit, const char *title, const char *text, const char *name) {
    GAME_FN(void (*)(int, int, int, int, int, int, const char *, const char *, ...), RVA_ADD_MESSAGE)(-1, player, x, y, 2, unit, title, text,
                                                                                                      name);
}
#define ITEM_TRACE_SIZE 256
static void gameItemTrace(const char *line) { GAME_FN(void (*)(const char *, ...), RVA_TRACE)("%s", line); }
static void gameItemTraceError(const char *line) { GAME_FN(void (*)(const char *, ...), RVA_TRACE_ERROR)("%s", line); }
static void (*itemSlotTrace)(const char *line) = gameItemTrace;
static void (*itemSlotTraceError)(const char *line) = gameItemTraceError;
static const char *(*itemSlotTraceName)(int unit) = gameUnitTraceName;
static int (*itemSlotAiItemValue)(int unit, int item) = gameAiItemValue;
static int (*itemSlotAiUseRating)(int unit, int item) = gameAiItemUseRating;
static void (*itemSlotUseItem)(int unit, int item) = gameUseItem;
static int (*itemSlotCanEquip)(int unit, int item, int player) = gameCanEquip;
static int (*itemSlotRemoveItem)(int unit, int item) = gameRemoveItem;
static void (*itemSlotGiveItem)(int unit, int item) = gameGiveItemTo;
static int (*itemSlotEnforceItems)(int unit) = gameEnforceItemsNow;
static int (*itemSlotContractMaster)(const char *name) = gameFindContractMaster;
static void (*itemSlotAddMessage)(int player, int x, int y, int unit, const char *title, const char *text, const char *name) = gameAddMessage;
static int (*itemSlotSpellNamed)(const char *name) = itemSpellNamed;

static int itemUnitX(int unit) { return G32(RVA_UNIT_X + (size_t)unit * UNIT_SIZE); }
static int itemUnitY(int unit) { return G32(RVA_UNIT_X + 4 + (size_t)unit * UNIT_SIZE); }
static int itemUnitHp(int unit) { return (short)G16(RVA_UNIT_HP + (size_t)unit * UNIT_SIZE); }

// ---- After a fight: the loot of positions 8..19 ---------------------------------------------------------------------
static void lootItemsPast7(int x, int y, int foundX, int foundY, int winner, int message) {
    if (!anyExtraItems()) return;
    int looted = 0, lost = 0;
    for (int u = 0; u <= G32(RVA_UNIT_COUNT) && u < UNIT_TABLE_MAX; u++) {
        if (!unitExtraItemCount(u)) continue;
        int ux = itemUnitX(u);
        if (ux != ITEM_LOOT_IN_FIGHT_X && (ux != x || itemUnitY(u) != y || itemUnitHp(u) > 0)) continue;
        for (int p = UNIT_ITEM_SLOTS; p < ITEM_POSITION_MAX; p++) {
            int item = unitItemAt(u, p);
            if (item < 0) continue;
            setUnitItemAt(u, p, -1);
            if (winner < 0 || itemStatPair(item, ITEM_ABILITY_NODROP) > 0) {
                lost++;
                continue;
            }
            itemSlotFoundItem(foundX, foundY, item, FOUND_ITEM_LOOT, message);
            looted++;
        }
    }
    if (looted || lost) writeLog("item_slots_looted", "x=%d y=%d winner=%d looted=%d lost=%d", x, y, winner, looted, lost);
}

// ---- The extra life: positions 8..19 when none of the 8 holds one -----------------------------------------------
static void removeExtraLifePast7(int unit) {
    if (!unitExtraItemCount(unit)) return;
    for (int p = UNIT_ITEM_SLOTS; p < ITEM_POSITION_MAX; p++) {
        int item = unitItemAt(unit, p);
        if (item <= 0 || itemStatPair(item, ITEM_ABILITY_EXTRA_LIFE) < 1) continue;
        setUnitItemAt(unit, p, -1);
        flushUnitItemCaches(unit);
        writeLog("item_slots_extra_life", "unit=%d position=%d item=%d", unit, p, item);
        return;
    }
}

// ---- Upkeep: soul contracts in positions 8..19 ---------------------------------------------------------------
#define SOUL_CONTRACT_TITLE "Soul Contract Terminated"
#define SOUL_CONTRACT_TEXT  "%s's soul contract was terminated due to its master no longer being alive."

static void soulContractsPast7(int unit, int player, int x, int y) {
    if (!unitExtraItemCount(unit)) return;
    for (int p = UNIT_ITEM_SLOTS; p < ITEM_POSITION_MAX; p++) {
        int item = unitItemAt(unit, p);
        const unsigned char *record = itemStatRecord(item);
        if (item <= 0 || !record || itemStatPair(item, ITEM_ABILITY_SOUL_CONTRACT) < 1) continue;
        if (itemSlotContractMaster((const char *)(record + ITEM_SOUL_CONTRACT_MASTER)) >= 0) continue;
        itemSlotRemoveItem(unit, item);
        itemSlotAddMessage(player, x, y, unit, SOUL_CONTRACT_TITLE, SOUL_CONTRACT_TEXT, itemSlotTraceName(unit));
        writeLog("item_slots_contract_ended", "unit=%d position=%d item=%d", unit, p, item);
    }
}

// ---- The AI's use of items ----------------------------------------------------------------------------------------
static int useRatedItems(int unit) {
    int used = 0;
    for (int p = 0; p < ITEM_POSITION_MAX; p++) {
        int item = unitItemAt(unit, p);
        if (item < 0 || itemSlotAiUseRating(unit, item) < 2) continue;
        itemSlotUseItem(unit, item);
        used++;
    }
    return used;
}

static int aiUseItemsHook(int unit) {
    if (!unitExtraItemCount(unit)) return 0;
    useRatedItems(unit);
    return 1;
}

// ---- The AI's redistribution of items on a square -------------------------------------------------------------------
static void redistributeItemsAt(int player, int x, int y) {
    for (int u = 0; u <= G32(RVA_UNIT_COUNT) && u < UNIT_TABLE_MAX; u++) {
        if (itemSlotsUnitPlayer(u) != player || itemUnitX(u) != x || itemUnitY(u) != y) continue;
        for (int p = 0; p < ITEM_POSITION_MAX; p++) {
            int item = unitItemAt(u, p);
            if (item < 0 || (itemFlagsOf(item) & ITEM_FLAG_CURSED)) continue;
            int best = u, value = p > 0 && unitItemAt(u, p - 1) == item ? 1 : itemSlotAiItemValue(u, item);
            for (int b = 0; b <= G32(RVA_UNIT_COUNT) && b < UNIT_TABLE_MAX; b++) {
                if (b == u || itemSlotsUnitPlayer(b) != player || itemUnitX(b) != x || itemUnitY(b) != y) continue;
                if (itemSlotCanEquip(b, item, player) < 0) continue;
                int v = itemSlotAiItemValue(b, item);
                if (v > value) value = v, best = b;
            }
            if (best == u) continue;
            char line[ITEM_TRACE_SIZE];
            snprintf(line, sizeof line, "ai: redistributing %.47s from %.80s to %.80s\n", (const char *)itemStatRecord(item), itemSlotTraceName(u),
                     itemSlotTraceName(best));
            itemSlotTrace(line);
            itemSlotRemoveItem(u, item);
            itemSlotGiveItem(best, item);
            itemSlotEnforceItems(best);
            break;
        }
    }
}

// ---- Wands: RVA_ITEM_CASTABLE_SPELL --------------------------------------------------------------------------------
typedef int (*ItemCastableSpellFn)(int unit);
static ItemCastableSpellFn gameItemCastableSpell;

static int itemCastableSpellHook(int unit) {
    if (!unitExtraItemCount(unit)) return gameItemCastableSpell(unit);
    if (itemUnitHp(unit) <= 0) return -1;
    for (int p = 0; p < ITEM_POSITION_MAX; p++) {
        const unsigned char *record = itemStatRecord(unitItemAt(unit, p));
        if (!record || *(const int *)(record + ITEM_COMBATSPELL_MODE) != 1) continue;
        const char *name = (const char *)(record + ITEM_COMBATSPELL_NAME);
        int spell = itemSlotSpellNamed(name);
        if (spell >= 0) return spell;
        char line[ITEM_TRACE_SIZE];
        snprintf(line, sizeof line, "*** guis: no spell called %.63s (%.47s)\n", name, (const char *)record);
        itemSlotTraceError(line);
    }
    return -1;
}

// ---- The commander panel's Use rows ------------------------------------------------------------------------------
static int panelWalksOn(int unit, int position) { return position < ITEM_POSITION_MAX && unitExtraItemCount(unit) > 0; }

// ---- Mac: what a step hook's read past position 7 points at -------------------------------------------------------
#define ITEM_READ_SCRATCH 8
static short itemReadScratch[ITEM_READ_SCRATCH];
static int itemReadScratchNext;
static UNUSED uintptr_t itemReadScratchFor(int unit, int position) {
    short *at = &itemReadScratch[itemReadScratchNext++ % ITEM_READ_SCRATCH];
    *at = (short)unitItemAt(unit, position);
    return (uintptr_t)at;
}

static int itemSlotBattleState;

#ifndef __APPLE__
// ---- Windows: RVA_BATTLE_ITEM_EFFECTS's copy -------------------------------------------------------------------
typedef int (*BattleItemEffectsFn)(int unit);
static BattleItemEffectsFn gameBattleItemEffects;
#define BATTLE_GRID_SECOND_UNIT 4
#define BATTLE_GRID_CELL        0x18
#define BATTLE_GRID_ROWS        20
#define BATTLE_ITEM_SAMPLE      0x39

static void battleItemEffectsCell(int unit, int *column, int *row) {
    int c = -1, r = -1;
    if (GAME_FN(int (*)(int, int *, int *), RVA_BATTLE_UNIT_POSITION)(unit, &c, &r) > 0) {
        *column = c, *row = r;
        return;
    }
    *column = -1, *row = -1;
    for (int y = 0; y < BATTLE_GRID_ROWS; y++)
        for (int x = 0; x < G32(RVA_BATTLE_WIDTH); x++)
            if (G32(RVA_BATTLE_GRID + BATTLE_GRID_SECOND_UNIT + (size_t)x * BATTLE_GRID_ROWS * BATTLE_GRID_CELL + (size_t)y * BATTLE_GRID_CELL) ==
                unit) {
                *column = x, *row = y;
                return;
            }
}

static int battleItemEffectsHook(int unit) {
    if (!unitExtraItemCount(unit)) return gameBattleItemEffects(unit);
    if (unit < 0 || unit > G32(RVA_UNIT_COUNT) || itemUnitHp(unit) <= 0) return 0;
    int player = itemSlotsUnitPlayer(unit), column, row, ran = 0;
    battleItemEffectsCell(unit, &column, &row);
    for (int p = 0; p < ITEM_POSITION_MAX; p++) {
        int item = unitItemAt(unit, p);
        const unsigned char *record = itemStatRecord(item);
        if (!record) continue;
        int mode = *(const int *)(record + ITEM_COMBATSUM_MODE), round = G32(RVA_BATTLE_ROUND);
        if (mode < 0 ? round == 0 : mode > 0 && round >= 0) {
            const char *monster = (const char *)(record + ITEM_COMBATSUM_MONSTER);
            GAME_FN(void (*)(const char *, ...), RVA_TRACE)("itemautoeffect for %s (%d*%s)\n", GAME_FN(const char *(*)(int), RVA_UNIT_SHOWN_NAME)(unit),
                                                            mode, monster);
            ran++;
            int made = GAME_FN(int (*)(int, int, int, const char *), RVA_BATTLE_SUMMON_STRING)(player, column, row, monster);
            GAME_FN(void (*)(int, int, int, const char *, ...), RVA_BATTLE_LOG)(unit, -1, 1, "%s summons %d units", (const char *)record, made);
            GAME_FN(void (*)(int, int), RVA_PLAY_SAMPLE)(BATTLE_ITEM_SAMPLE, 0);
            GAME_FN(void (*)(int), RVA_BATTLE_SCREEN)(G32(RVA_BATTLE_DELAY));
            if (p >= UNIT_ITEM_SLOTS) writeLog("item_slots_combat_item", "unit=%d position=%d item=%d word=combatsum made=%d", unit, p, item, made);
        }
        int spellMode = *(const int *)(record + ITEM_COMBATSPELL_MODE);
        round = G32(RVA_BATTLE_ROUND);
        if (spellMode == -1 ? round == 0 : spellMode == 2 && round >= 0) {
            ran++;
            const char *name = (const char *)(record + ITEM_COMBATSPELL_NAME);
            int spell = itemSpellNamed(name);
            if (spell < 0) {
                GAME_FN(void (*)(const char *, ...), RVA_TRACE_ERROR)("*** iae: no spell called %s (%s)\n", name, (const char *)record);
                continue;
            }
            GAME_FN(void (*)(const char *, ...), RVA_TRACE)("itemautospell spl %s\n", spellSlot(spell)->name);
            GAME_FN(void (*)(int, int, int), RVA_DOATTACK)(unit, -1, spell);
            if (p >= UNIT_ITEM_SLOTS) writeLog("item_slots_combat_item", "unit=%d position=%d item=%d word=combatspell spell=%d", unit, p, item, spell);
        }
    }
    return ran;
}

typedef void (*AiRedistributeItemsFn)(int player, int x, int y);
static AiRedistributeItemsFn gameAiRedistributeItems;
static void aiRedistributeItemsHook(int player, int x, int y) {
    if (!anyExtraItems()) {
        gameAiRedistributeItems(player, x, y);
        return;
    }
    GAME_FN(void (*)(const char *, ...), RVA_TRACE_C)("ai: redistributing at %d,%d\n", x, y);
    redistributeItemsAt(player, x, y);
}

typedef int (*CommanderUseItemsFn)(int commander);
static CommanderUseItemsFn gameCommanderUseItems;
static int commanderUseItemsHook(int commander) {
    int unit = commander >= 0 && commander <= G32(RVA_LAST_COMMANDER) ? G32(RVA_COMMANDERS + (size_t)commander * COMMANDER_SIZE) : -1;
    if (!unitExtraItemCount(unit)) return gameCommanderUseItems(commander);
    return useRatedItems(unit);
}

// ---- Windows: the redirected call and the stubs -----------------------------------------------------------------
static uintptr_t *itemLootRspSlot;
static int lootConquerHook(int x, int y, int zero) {
    const unsigned char *frame = (const unsigned char *)(*itemLootRspSlot + 8);
    lootItemsPast7(x, y, *(const int *)(frame + LOOT_FRAME_X), *(const int *)(frame + LOOT_FRAME_Y), *(const int *)(frame + LOOT_FRAME_WINNER),
                   *(const int *)(frame + LOOT_FRAME_MESSAGE));
    return GAME_FN(int (*)(int, int, int), RVA_CONQUER_CHECK)(x, y, zero);
}

static void extraLifeHook(int unit) { removeExtraLifePast7(unit); }
static void soulContractsHook(int unit, int player, const int *x, const int *y) { soulContractsPast7(unit, player, *x, *y); }
static int panelUnitOf(const unsigned char *items) { return (int)((items - GAME_PTR(RVA_UNIT_ITEMS)) / UNIT_SIZE); }
static int panelItemHook(const unsigned char *items, int position) { return unitItemAt(panelUnitOf(items), position); }
static int panelNextHook(const unsigned char *items, int position) { return panelWalksOn(panelUnitOf(items), position); }

enum { ITEM_BATTLE_LIFE, ITEM_BATTLE_SOUL, ITEM_BATTLE_USE, ITEM_BATTLE_PANEL_READ, ITEM_BATTLE_PANEL_NEXT, ITEM_BATTLE_SITES };
#define ITEM_BATTLE_SLOTS      (ITEM_BATTLE_SITES * ITEM_CARD_STUB_SIZE)
#define ITEM_BATTLE_FUNCTIONS  (ITEM_BATTLE_SLOTS + ITEM_BATTLE_SITES * 8)
#define ITEM_BATTLE_UNWIND     (ITEM_BATTLE_FUNCTIONS + ITEM_BATTLE_SITES * (int)sizeof(RUNTIME_FUNCTION))
#define ITEM_BATTLE_PAGE_BYTES (ITEM_BATTLE_UNWIND + ITEM_BATTLE_SITES * ITEM_STAT_UNWIND_SIZE)
_Static_assert(ITEM_BATTLE_PAGE_BYTES <= 0x1000, "the item battle page");

static size_t buildItemBattleStub(int site, unsigned char *stub, const unsigned char *slot, const unsigned char *at, int moved) {
    static const unsigned char LIFE_ARGS[] = {0x44, 0x89, 0xf1};
    static const unsigned char SOUL_ARGS[] = {0x44, 0x89, 0xe1, 0x8b, 0x54, 0x24, SOUL_CONTRACT_FRAME_PLAYER, 0x4d, 0x89, 0xf0, 0x4d, 0x89, 0xf9};
    static const unsigned char USE_ARGS[] = {0x89, 0xf1};
    static const unsigned char PANEL_ARGS[] = {0x48, 0x89, 0xf1, 0x48, 0x29, 0xe9, 0x48, 0x29, 0xe9, 0x89, 0xea};
    static const unsigned char PANEL_GUARD[] = {0x48, 0x83, 0xfd, UNIT_ITEM_SLOTS};
    static const unsigned char PANEL_RESULT[] = {0x48, 0x63, 0xd8, 0x85, 0xdb};
    switch (site) {
    case ITEM_BATTLE_LIFE:
        return buildItemLoopEndStub(stub, stub, slot, at, moved, NULL, 0, 0x8c, GAME_PTR(RVA_EXTRA_LIFE_LOOP_TOP), LIFE_ARGS, sizeof LIFE_ARGS, 0,
                                    GAME_PTR(RVA_EXTRA_LIFE_LIVES_AGAIN));
    case ITEM_BATTLE_SOUL:
        return buildItemLoopEndStub(stub, stub, slot, at, moved, NULL, 0, 0x85, GAME_PTR(RVA_UPKEEP_SOUL_CONTRACTS_TOP), SOUL_ARGS, sizeof SOUL_ARGS,
                                    0, GAME_PTR(RVA_UPKEEP_SOUL_CONTRACTS_AFTER));
    case ITEM_BATTLE_USE:
        return buildItemTakeoverStub(stub, stub, slot, at, moved, USE_ARGS, sizeof USE_ARGS, at + moved, GAME_PTR(RVA_AI_USE_ITEMS_DONE));
    case ITEM_BATTLE_PANEL_READ:
        return buildItemReadStub(stub, stub, slot, at, moved, PANEL_GUARD, sizeof PANEL_GUARD, PANEL_ARGS, sizeof PANEL_ARGS, PANEL_RESULT,
                                 sizeof PANEL_RESULT, at + moved);
    case ITEM_BATTLE_PANEL_NEXT:
        return buildItemLoopEndStub(stub, stub, slot, at, moved, PANEL_GUARD, sizeof PANEL_GUARD, 0x8c, GAME_PTR(RVA_PANEL_USE_ITEM_READ), PANEL_ARGS,
                                    sizeof PANEL_ARGS, 1, GAME_PTR(RVA_PANEL_USE_ITEMS_DONE));
    }
    return 0;
}

static int installItemBattleSites(void) {
    static const struct { unsigned int rva; unsigned char moved; } SITES[ITEM_BATTLE_SITES] = {
        {RVA_EXTRA_LIFE_LOOP_END, 8}, {RVA_UPKEEP_SOUL_CONTRACTS_END, 8}, {RVA_AI_USE_ITEMS_LOOP, 9}, {RVA_PANEL_USE_ITEM_READ, 6},
        {RVA_PANEL_USE_ITEMS_NEXT, 7},
    };
    static const unsigned char JUMP_SIZE[ITEM_BATTLE_SITES] = {12, 14, 9, 6, 7};
    void *handlers[ITEM_BATTLE_SITES] = {(void *)extraLifeHook, (void *)soulContractsHook, (void *)aiUseItemsHook, (void *)panelItemHook,
                                         (void *)panelNextHook};
    if (!gameCodeMatches) {
        writeLogError("hook_skipped", "site=item_slot_battle reason=game_code_differs");
        return 0;
    }
    unsigned char *page = allocNearBytes(gameBase + RVA_TEXT_SECTION, ITEM_BATTLE_PAGE_BYTES);
    if (!page) {
        writeLogError("hook_skipped", "site=item_slot_battle reason=no_near_page");
        return 0;
    }
    RUNTIME_FUNCTION *functions = (RUNTIME_FUNCTION *)(page + ITEM_BATTLE_FUNCTIONS);
    for (int i = 0; i < ITEM_BATTLE_SITES; i++) {
        unsigned char *at = GAME_PTR(SITES[i].rva), *stub = page + (size_t)i * ITEM_CARD_STUB_SIZE, *slot = page + ITEM_BATTLE_SLOTS + (size_t)i * 8;
        unsigned char *unwind = page + ITEM_BATTLE_UNWIND + (size_t)i * ITEM_STAT_UNWIND_SIZE;
        memcpy(slot, &handlers[i], 8);
        size_t n = buildItemBattleStub(i, stub, slot, at, SITES[i].moved);
        if (!n || !copyItemStatUnwind(at, unwind)) {
            writeLogError("hook_skipped", "site=item_battle_0x%x reason=stub_not_built", SITES[i].rva);
            return 0;
        }
        functions[i].BeginAddress = (DWORD)(stub - page);
        functions[i].EndAddress = (DWORD)(stub - page + n);
        functions[i].UnwindData = (DWORD)(unwind - page);
    }
    if (!RtlAddFunctionTable(functions, ITEM_BATTLE_SITES, (DWORD64)(uintptr_t)page)) {
        writeLogError("hook_skipped", "site=item_slot_battle reason=unwind_not_added");
        return 0;
    }
    makePagesReadExecute(page, ITEM_BATTLE_PAGE_BYTES);
    for (int i = 0; i < ITEM_BATTLE_SITES; i++) {
        unsigned char *at = GAME_PTR(SITES[i].rva), jump[16];
        buildEntryHookJump(jump, at, page + (size_t)i * ITEM_CARD_STUB_SIZE, JUMP_SIZE[i]);
        if (!writeGameCode(at, jump, JUMP_SIZE[i])) return 0;
    }
    return 1;
}
#else
// ---- Mac: step hooks -----------------------------------------------------------------------------------------------
static StepHook itemEffectsReadHook, itemUseDoneHook, itemRedistributeDoneHook, itemPanelReadHook;

static void itemLootMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    const unsigned char *sp = (const unsigned char *)(uintptr_t)ctx->sp;
    lootItemsPast7(*(const int *)(sp + LOOT_FRAME_MATCH_X), *(const int *)(sp + LOOT_FRAME_MATCH_Y), *(const int *)(sp + LOOT_FRAME_X),
                   *(const int *)(sp + LOOT_FRAME_Y), *(const int *)(sp + LOOT_FRAME_WINNER), (int)ctx->x[22]);
}

static void itemEffectsNextMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    int unit = (int)(unsigned)ctx->x[20];
    if (ctx->x[28] == UNIT_ITEM_SLOTS - 1 && unitExtraItemCount(unit)) {
        ctx->x[28] = UNIT_ITEM_SLOTS;
        ctx->x[8] = itemReadScratchFor(unit, UNIT_ITEM_SLOTS) - ITEM_CARD_ITEMS_AT - 2 * UNIT_ITEM_SLOTS;
        ctx->pc = itemEffectsReadHook.resume;
    } else if (ctx->x[28] == ITEM_POSITION_MAX - 1) {
        ctx->x[28] = UNIT_ITEM_SLOTS - 1;
    }
}

static void itemEffectsReadMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    int position = (int)ctx->x[28];
    if (position < UNIT_ITEM_SLOTS) return;
    ctx->x[8] = itemReadScratchFor((int)(unsigned)ctx->x[20], position) - ITEM_CARD_ITEMS_AT - 2 * (uint64_t)position;
}

static void itemExtraLifeEmptyMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    if ((int)(unsigned)ctx->x[8] < 1) removeExtraLifePast7((int)(unsigned)ctx->x[19]);
}
static void itemExtraLifeNoneMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    if ((int)(unsigned)ctx->x[0] <= 0) removeExtraLifePast7((int)(unsigned)ctx->x[19]);
}

static void itemSoulContractsMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    const unsigned char *sp = (const unsigned char *)(uintptr_t)ctx->sp;
    const int *x = *(const int *const *)(sp + SOUL_CONTRACT_FRAME_X), *y = *(const int *const *)(sp + SOUL_CONTRACT_FRAME_Y);
    soulContractsPast7((int)(unsigned)ctx->x[24], (int)(unsigned)ctx->x[27], *x, *y);
}

static void itemAiUseMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    if (!aiUseItemsHook((int)(unsigned)ctx->x[19])) return;
    ctx->pc = itemUseDoneHook.resume;
}

static void itemRedistributeMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    if (!anyExtraItems()) return;
    int player = *(const int *)(uintptr_t)(ctx->x[29] + (uint64_t)(int64_t)AI_REDISTRIBUTE_FRAME_PLAYER);
    redistributeItemsAt(player, (int)(unsigned)ctx->x[19], (int)(unsigned)ctx->x[27]);
    ctx->x[8] = (uint32_t)G32(RVA_UNIT_COUNT);
    ctx->x[11] = (uint64_t)(uintptr_t)GAME_PTR(RVA_UNIT_COUNT) & ~(uint64_t)0xfff;
    ctx->x[13] = (uint64_t)(int64_t)player;
    ctx->pc = itemRedistributeDoneHook.resume;
}

static int itemPanelUnitMac(const StepContext *ctx) { return (int)*(const int64_t *)(uintptr_t)(ctx->sp + PANEL_FRAME_UNIT); }

static void itemPanelNextMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    int unit = itemPanelUnitMac(ctx);
    if (ctx->x[21] == 2 * (UNIT_ITEM_SLOTS - 1) && panelWalksOn(unit, UNIT_ITEM_SLOTS)) {
        ctx->x[21] = 2 * UNIT_ITEM_SLOTS;
        ctx->x[23] = itemReadScratchFor(unit, UNIT_ITEM_SLOTS) - 2 * UNIT_ITEM_SLOTS;
        ctx->pc = itemPanelReadHook.resume;
    } else if (ctx->x[21] == 2 * (ITEM_POSITION_MAX - 1)) {
        ctx->x[21] = 2 * (UNIT_ITEM_SLOTS - 1);
        ctx->x[23] = (uint64_t)(uintptr_t)GAME_PTR(RVA_UNIT_ITEMS) + (uint64_t)unit * UNIT_SIZE;
    }
}

static void itemPanelReadMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    if (ctx->x[21] < 2 * UNIT_ITEM_SLOTS) return;
    ctx->x[23] = itemReadScratchFor(itemPanelUnitMac(ctx), (int)(ctx->x[21] / 2)) - ctx->x[21];
}

static void itemStepResume(StepContext *ctx, const StepHook *hook) {
    (void)ctx;
    (void)hook;
}

static int installItemBattleSites(void) {
    static StepHook loot, effectsNext, life8, lifeLast, soul, aiUse, redistribute, panelNext;
    static const struct { StepHook *hook; void (*handler)(StepContext *, const StepHook *); unsigned int rva; const char *name; } HOOKS[] = {
        {&itemEffectsReadHook, itemEffectsReadMac, RVA_BATTLE_ITEM_EFFECTS_READ, "item_effects_read"},
        {&itemUseDoneHook, itemStepResume, RVA_AI_USE_ITEMS_DONE, "item_ai_use_done"},
        {&itemRedistributeDoneHook, itemStepResume, RVA_AI_REDISTRIBUTE_NEXT_SQUARE, "item_redistribute_done"},
        {&itemPanelReadHook, itemPanelReadMac, RVA_PANEL_USE_ITEM_READ, "item_panel_read"},
        {&loot, itemLootMac, RVA_LOOT_PAST_7_AT, "item_loot"},
        {&life8, itemExtraLifeEmptyMac, RVA_EXTRA_LIFE_LOOP_END, "item_extra_life_empty"},
        {&lifeLast, itemExtraLifeNoneMac, RVA_EXTRA_LIFE_LAST_TEST, "item_extra_life_none"},
        {&soul, itemSoulContractsMac, RVA_UPKEEP_SOUL_CONTRACTS_END, "item_soul_contracts"},
        {&aiUse, itemAiUseMac, RVA_AI_USE_ITEMS_LOOP, "item_ai_use"},
        {&redistribute, itemRedistributeMac, RVA_AI_REDISTRIBUTE_UNITS, "item_redistribute"},
        {&effectsNext, itemEffectsNextMac, RVA_BATTLE_ITEM_EFFECTS_NEXT, "item_effects_next"},
        {&panelNext, itemPanelNextMac, RVA_PANEL_USE_ITEMS_NEXT, "item_panel_next"},
    };
    if (!gameCodeMatches) {
        writeLogError("hook_skipped", "site=item_slot_battle reason=game_code_differs");
        return 0;
    }
    for (size_t i = 0; i < sizeof HOOKS / sizeof HOOKS[0]; i++) {
        HOOKS[i].hook->handler = HOOKS[i].handler;
        if (!putStepHookMac(HOOKS[i].rva, HOOKS[i].hook, HOOKS[i].name, 0)) return 0;
    }
    return 1;
}
#endif

static int installItemSlotBattle(void) {
    if (itemSlotBattleState) return itemSlotBattleState > 0;
    struct { unsigned int rva; void *hook; void **game; } hooks[] = {
        {RVA_ITEM_CASTABLE_SPELL, (void *)itemCastableSpellHook, (void **)&gameItemCastableSpell},
#ifndef __APPLE__
        {RVA_BATTLE_ITEM_EFFECTS, (void *)battleItemEffectsHook, (void **)&gameBattleItemEffects},
        {RVA_AI_REDISTRIBUTE_ITEMS, (void *)aiRedistributeItemsHook, (void **)&gameAiRedistributeItems},
        {RVA_COMMANDER_USE_ITEMS, (void *)commanderUseItemsHook, (void **)&gameCommanderUseItems},
#endif
    };
    int count = (int)(sizeof hooks / sizeof hooks[0]), ok = 0;
    for (int i = 0; i < count; i++) {
        void *target = gameBase + hooks[i].rva;
        ok += fanpatchHookCreate(target, hooks[i].hook, hooks[i].game) && fanpatchHookEnable(target, 1);
    }
    int sites = ok == count && installItemBattleSites();
#ifndef __APPLE__
    if (sites) {
        unsigned char *stub = allocNear(gameBase + RVA_TEXT_SECTION), *slots = stub ? allocNear(stub) : NULL;
        if (slots) makePagesReadExecute(stub, 0x1000);
        else writeLogError("hook_skipped", "site=item_loot reason=no_near_page");
        itemLootRspSlot = slots ? (uintptr_t *)slots : NULL;
        sites = slots && redirectCall(RVA_LOOT_PAST_7_AT, RVA_CONQUER_CHECK, stub, (void *)lootConquerHook, "item_loot", itemLootRspSlot);
    }
#endif
    itemSlotBattleState = ok == count && sites ? 1 : -1;
#ifdef __APPLE__
    writeResultLog(itemSlotBattleState > 0, "item_slot_battle_installed", "hooks=%d sites=%d padding_left=%ld", ok, sites, paddingLeft());
#else
    writeResultLog(itemSlotBattleState > 0, "item_slot_battle_installed", "hooks=%d sites=%d", ok, sites);
#endif
    return itemSlotBattleState > 0;
}
