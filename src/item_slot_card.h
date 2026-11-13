// @platform-split item_slot_card: Windows jumps to stubs that call CoE5SE at the game's rsp; Mac uses step hooks, and Mac's transfer inlines an ally's item removal, which a hook takes over for positions 8..19
#pragma once

// ---- The row's shape, both builds ---------------------------------------------------------------------------
static int itemCardPositionCount(int layoutTotal, int lastHeld) {
    int count = layoutTotal < 0 ? 0 : layoutTotal;
    if (lastHeld + 1 > count) count = lastHeld + 1;
    return count > ITEM_POSITION_MAX ? ITEM_POSITION_MAX : count;
}

static int itemCardWraps(float lineWidth, float step, float cardWidth, int onLine) {
    return onLine > 0 && lineWidth + step > cardWidth;
}

static int itemCardPositions(int unit) {
    const MonsterItemSlots *s = unitItemSlots(unit);
    int last = -1;
    for (int p = 0; p < ITEM_POSITION_MAX; p++)
        if (unitItemAt(unit, p) >= 0) last = p;
    return itemCardPositionCount(s ? s->total : UNIT_ITEM_SLOTS, last);
}

// ---- The handlers the patches call, both builds ------------------------------------------------------------
static int itemCardItemAt(int unit, int position) { return unitItemAt(unit, position); }

static void itemCardTransferWrite(int unit, int position, int item) {
    if (position < UNIT_ITEM_SLOTS || position >= ITEM_POSITION_MAX) return;
    setUnitItemAt(unit, position, item);
}

// ---- Drawing the row, both builds ---------------------------------------------------------------------------
static void itemCardButton(int id, int kind, int sprite, int flags) {
    GAME_FN(void (*)(int, int, int, int), RVA_ICON_BUTTON)(id, kind, sprite, flags);
}

static void itemCardGap(void) {
    int i = GAME_FN(int (*)(void), RVA_LAYOUT_NEW)();
    if (i < 0) return;
    unsigned char *e = GAME_PTR(RVA_LAYOUT_TABLE) + (size_t)i * LAYOUT_STRIDE;
    *(float *)(e + LAYOUT_ELEMENT_WIDTH) = ITEM_CARD_GAP_WIDTH;
    *(short *)(e + LAYOUT_ELEMENT_TYPE) = LAYOUT_TYPE_ITEM_GAP;
#ifdef __APPLE__
    layoutAdvance();
#else
    GAME_FN(void (*)(void), RVA_LINE_ADVANCE)();
#endif
}

static void endItemCardLine(void) {
    int last = G32(RVA_LAYOUT_LAST);
    if (last < 0) return;
    unsigned char *table = GAME_PTR(RVA_LAYOUT_TABLE);
    while (last > 0 && *(short *)(table + (size_t)last * LAYOUT_STRIDE + LAYOUT_ELEMENT_TYPE) >= LAYOUT_TYPE_ROW_BEGIN_LIMIT) last--;
    table[(size_t)last * LAYOUT_STRIDE + LAYOUT_ELEMENT_LINE_END] = 1;
    if (GF(RVA_WIDEST_LINE) <= GF(RVA_LINE_WIDTH)) GF(RVA_WIDEST_LINE) = GF(RVA_LINE_WIDTH);
    GF(RVA_LINE_WIDTH) = 0.0f;
}

static int drawItemCardRow(int unit, int monster) {
    const MonsterItemSlots *preview = unit < 0 ? monsterItemSlotsOf(monster) : NULL;
    if (unit < 0 ? !preview : !unitNeedsItemSlots(unit)) return 0;
    int positions = preview ? preview->total : itemCardPositions(unit), onLine = 0;
    float step = 0.0f;
    for (int p = 0; p < positions; p++) {
        int item = preview ? -1 : unitItemAt(unit, p), kind = item >= 0 ? 0 : preview ? preview->kinds[p] : itemSlotKind(unit, p);
        const unsigned char *record = item >= 0 ? itemStatRecord(item) : NULL;
        if (item >= 0 ? !record : kind <= 0 || kind >= ITEM_TYPE_COUNT) continue;
        if (itemCardWraps(GF(RVA_LINE_WIDTH), step, GF(RVA_SCREEN_WIDTH_A), onLine)) {
            endItemCardLine();
            onLine = 0;
        }
        float before = GF(RVA_LINE_WIDTH);
        if (record) {
            itemCardButton(UNIT_CARD_ITEM_IDS + p, SPRITE_KIND_ITEM, *(const short *)(record + ITEM_SPRITE), ITEM_CARD_HELD_FLAGS);
            tooltip((const char *)record);
        } else {
            itemCardButton(0, SPRITE_KIND_ICON, ITEM_TYPE_ICON_BASE + kind, ITEM_CARD_EMPTY_FLAGS);
            GAME_FN(void (*)(FMT_ARGS), RVA_TOOLTIP)((const char *)GAME_PTR(RVA_ITEM_SLOT_TOOLTIP), ITEM_TYPE_LABELS[kind]);
        }
        itemCardGap();
        if (GF(RVA_LINE_WIDTH) - before > step) step = GF(RVA_LINE_WIDTH) - before;
        onLine++;
    }
    return 1;
}

static int itemSlotCardState;

#ifndef __APPLE__
// ---- Windows: four stubs in a page near the game ----------------------------------------------------------
enum { ITEM_CARD_ROW, ITEM_CARD_READ, ITEM_CARD_WRITE, ITEM_CARD_INFO, ITEM_CARD_SITES };
#define ITEM_CARD_SLOTS      (ITEM_CARD_SITES * ITEM_CARD_STUB_SIZE)
#define ITEM_CARD_FUNCTIONS  (ITEM_CARD_SLOTS + ITEM_CARD_SITES * 8)
#define ITEM_CARD_UNWIND     (ITEM_CARD_FUNCTIONS + ITEM_CARD_SITES * (int)sizeof(RUNTIME_FUNCTION))
#define ITEM_CARD_PAGE_BYTES (ITEM_CARD_UNWIND + ITEM_CARD_SITES * ITEM_STAT_UNWIND_SIZE)
#define ITEM_CARD_POSITION_AT 0x98
#define ITEM_CARD_SPILL_AT    0x80
_Static_assert(ITEM_CARD_PAGE_BYTES <= 0x1000, "the item card's page");

static int itemCardRowHook(int unit, int monster) { return drawItemCardRow(unit, monster); }

static int installItemCardSites(void) {
    static const struct { unsigned int rva; unsigned char moved; } SITES[ITEM_CARD_SITES] = {
        {RVA_UNIT_CARD_ITEM_ROW_START, 8}, {RVA_ITEM_TRANSFER_ITEM_READ, 9}, {RVA_ITEM_TRANSFER_ITEM_WRITE, 9},
        {RVA_UNIT_CARD_ITEM_INFO_READ, 8},
    };
    void *handlers[ITEM_CARD_SITES] = {(void *)itemCardRowHook, (void *)itemCardItemAt, (void *)itemCardTransferWrite,
                                       (void *)itemCardItemAt};
    if (!gameCodeMatches) {
        writeLogError("hook_skipped", "site=item_slot_card reason=game_code_differs");
        return 0;
    }
    unsigned char *page = allocNearBytes(gameBase + RVA_TEXT_SECTION, ITEM_CARD_PAGE_BYTES);
    if (!page) {
        writeLogError("hook_skipped", "site=item_slot_card reason=no_near_page");
        return 0;
    }
    RUNTIME_FUNCTION *functions = (RUNTIME_FUNCTION *)(page + ITEM_CARD_FUNCTIONS);
    for (int i = 0; i < ITEM_CARD_SITES; i++) {
        unsigned char *at = GAME_PTR(SITES[i].rva), *stub = page + (size_t)i * ITEM_CARD_STUB_SIZE, *slot = page + ITEM_CARD_SLOTS + (size_t)i * 8;
        unsigned char *unwind = page + ITEM_CARD_UNWIND + (size_t)i * ITEM_STAT_UNWIND_SIZE;
        memcpy(slot, &handlers[i], 8);
        size_t n = 0;
        if (i == ITEM_CARD_ROW)
            n = buildItemCardRowStub(stub, stub, slot, at, SITES[i].moved, GAME_PTR(RVA_UNIT_CARD_ITEM_LOOP_TOP), GAME_PTR(RVA_UNIT_CARD_ITEM_ROW_END));
        else if (i == ITEM_CARD_READ)
            n = buildItemCardReadStub(stub, stub, slot, at, SITES[i].moved, at + SITES[i].moved);
        else if (i == ITEM_CARD_WRITE)
            n = buildItemCardWriteStub(stub, stub, slot, at, SITES[i].moved, ITEM_CARD_POSITION_AT, ITEM_CARD_SPILL_AT, at + SITES[i].moved);
        else
            n = buildItemCardInfoStub(stub, stub, slot, at, SITES[i].moved, UNIT_CARD_ITEM_IDS, UNIT_CARD_ITEM_IDS + UNIT_ITEM_SLOTS,
                                      at + SITES[i].moved, GAME_PTR(RVA_UNIT_CARD_ITEM_INFO_SKIP));
        if (!n || !copyItemStatUnwind(at, unwind)) {
            writeLogError("hook_skipped", "site=item_card_0x%x reason=stub_not_built", SITES[i].rva);
            return 0;
        }
        functions[i].BeginAddress = (DWORD)(stub - page);
        functions[i].EndAddress = (DWORD)(stub - page + n);
        functions[i].UnwindData = (DWORD)(unwind - page);
    }
    if (!RtlAddFunctionTable(functions, ITEM_CARD_SITES, (DWORD64)(uintptr_t)page)) {
        writeLogError("hook_skipped", "site=item_slot_card reason=unwind_not_added");
        return 0;
    }
    makePagesReadExecute(page, ITEM_CARD_PAGE_BYTES);
    static const int ORDER[ITEM_CARD_SITES] = {ITEM_CARD_INFO, ITEM_CARD_WRITE, ITEM_CARD_READ, ITEM_CARD_ROW};
    int written = 0;
    for (int i = 0; i < ITEM_CARD_SITES; i++) {
        int k = ORDER[i];
        unsigned char *at = GAME_PTR(SITES[k].rva), jump[ITEM_CARD_MOVED_MAX];
        buildEntryHookJump(jump, at, page + (size_t)k * ITEM_CARD_STUB_SIZE, SITES[k].moved);
        if (!writeGameCode(at, jump, SITES[k].moved)) return 0;
        written++;
    }
    return written == ITEM_CARD_SITES;
}
#else
// ---- Mac: step hooks ------------------------------------------------------------------------------------------
#define ITEM_CARD_SCRATCH 16
static struct { short item; short pad; int unit, position; } itemCardScratch[ITEM_CARD_SCRATCH];
static int itemCardScratchNext;

static short *itemCardTakeScratch(int unit, int position) {
    int i = itemCardScratchNext++ % ITEM_CARD_SCRATCH;
    itemCardScratch[i].item = (short)itemCardItemAt(unit, position);
    itemCardScratch[i].unit = unit;
    itemCardScratch[i].position = position;
    return &itemCardScratch[i].item;
}

#define ITEM_CARD_ITEMS_AT (RVA_UNIT_ITEMS - RVA_UNITS)

static StepHook itemCardRowEndHook, itemCardAllyRemovedHook;

static void itemCardRowStart(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    int unit = (int)(unsigned)ctx->x[25], monster = *(const int *)(uintptr_t)(ctx->sp + UNIT_CARD_FRAME_MONSTER);
    if (unit < 0 ? !monsterItemSlotsOf(monster) : !unitNeedsItemSlots(unit)) return;
    G8(RVA_ROW_OPEN) = 1;
    drawItemCardRow(unit, monster);
    ctx->x[19] = LAYOUT_STRIDE;
    ctx->x[21] = (uint64_t)(uintptr_t)GAME_PTR(RVA_COLUMN_BASE) & ~(uint64_t)0xfff;
    ctx->x[22] = 1;
    ctx->pc = itemCardRowEndHook.resume;
}

static void itemCardResume(StepContext *ctx, const StepHook *hook) {
    (void)ctx;
    (void)hook;
}

static void itemCardTransferReadMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    int position = (int)(unsigned)ctx->x[20];
    if (position < UNIT_ITEM_SLOTS) return;
    ctx->x[28] = (uint64_t)(uintptr_t)itemCardTakeScratch((int)(unsigned)ctx->x[19], position) - ITEM_CARD_ITEMS_AT;
}

static void itemCardTransferWriteMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    for (int i = 0; i < ITEM_CARD_SCRATCH; i++)
        if (ctx->x[28] == (uint64_t)(uintptr_t)&itemCardScratch[i].item) {
            itemCardTransferWrite(itemCardScratch[i].unit, itemCardScratch[i].position, (short)ctx->x[8]);
            return;
        }
}

static void itemCardAllyRemove(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    int giver = (int)(unsigned)ctx->x[19], item = (short)ctx->x[20];
    if (!unitNeedsItemSlots(giver)) return;
    for (int p = 0; p < ITEM_POSITION_MAX; p++) {
        if (unitItemAt(giver, p) != item) continue;
        if (p < UNIT_ITEM_SLOTS) return;
        setUnitItemAt(giver, p, -1);
        ctx->pc = itemCardAllyRemovedHook.resume;
        return;
    }
}

static void itemCardInfoReadMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    int id = (int)(unsigned)ctx->x[20], position = (id < 0 ? -id : id) - UNIT_CARD_ITEM_IDS;
    if (position < UNIT_ITEM_SLOTS) return;
    ctx->x[8] = (uint64_t)(uintptr_t)itemCardTakeScratch((int)(unsigned)ctx->x[12], position) -
                (ITEM_CARD_ITEMS_AT - 2 * UNIT_CARD_ITEM_IDS);
}

static int installItemCardSites(void) {
    static StepHook rowStart, read, write, allyRemove, info;
    static const struct { StepHook *hook; void (*handler)(StepContext *, const StepHook *); unsigned int rva; const char *name; } HOOKS[] = {
        {&itemCardRowEndHook, itemCardResume, RVA_UNIT_CARD_ITEM_ROW_END, "item_card_row_end"},
        {&itemCardAllyRemovedHook, itemCardResume, RVA_ITEM_TRANSFER_ALLY_REMOVED, "item_card_ally_removed"},
        {&read, itemCardTransferReadMac, RVA_ITEM_TRANSFER_ITEM_READ, "item_card_transfer_read"},
        {&write, itemCardTransferWriteMac, RVA_ITEM_TRANSFER_ITEM_WRITE, "item_card_transfer_write"},
        {&allyRemove, itemCardAllyRemove, RVA_ITEM_TRANSFER_ALLY_REMOVE, "item_card_ally_remove"},
        {&info, itemCardInfoReadMac, RVA_UNIT_CARD_ITEM_INFO_READ, "item_card_info_read"},
        {&rowStart, itemCardRowStart, RVA_UNIT_CARD_ITEM_ROW_START, "item_card_row_start"},
    };
    if (!gameCodeMatches) {
        writeLogError("hook_skipped", "site=item_slot_card reason=game_code_differs");
        return 0;
    }
    for (size_t i = 0; i < sizeof HOOKS / sizeof HOOKS[0]; i++) {
        HOOKS[i].hook->handler = HOOKS[i].handler;
        if (!putStepHookMac(HOOKS[i].rva, HOOKS[i].hook, HOOKS[i].name, 0)) return 0;
    }
    return 1;
}
#endif

static int installItemSlotCard(void) {
    if (itemSlotCardState) return itemSlotCardState > 0;
    itemSlotCardState = installItemCardSites() ? 1 : -1;
#ifdef __APPLE__
    writeResultLog(itemSlotCardState > 0, "item_slot_card_installed", "padding_left=%ld", paddingLeft());
#else
    writeResultLog(itemSlotCardState > 0, "item_slot_card_installed", "sites=%d", ITEM_CARD_SITES);
#endif
    return itemSlotCardState > 0;
}
