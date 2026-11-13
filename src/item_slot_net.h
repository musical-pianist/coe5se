// @platform-split item_slot_net: Windows hooks the record's writer and reader at their entries, redirects the writer's last check and jumps to a stub at the reader's index test; Mac inlines both into their callers, so step hooks there
#pragma once

// ---- The record's entries, both builds ------------------------------------------------------------------------
static void gameNetPutByte(int byte) { GAME_FN(void (*)(int), RVA_NET_PUT_BYTE)(byte); }
static int gameNetGetByte(void) { return GAME_FN(char (*)(void), RVA_NET_GET_BYTE)(); }
static void (*itemNetPutByte)(int byte) = gameNetPutByte;
static int (*itemNetGetByte)(void) = gameNetGetByte;

static void unitExtraItemsText(int unit, char *out, size_t size) {
    size_t at = 0;
    out[0] = 0;
    for (int p = UNIT_ITEM_SLOTS; p < ITEM_POSITION_MAX && at < size; p++) {
        int item = unitItemAt(unit, p);
        if (item >= 0) at += (size_t)snprintf(out + at, size - at, "%s%d:%d", at ? "," : "", p, item);
    }
}

static int putUnitExtraItems(int unit) {
    if (!unitExtraItemCount(unit)) return 0;
    for (int p = UNIT_ITEM_SLOTS; p < ITEM_POSITION_MAX; p++) {
        int item = unitItemAt(unit, p);
        itemNetPutByte(p);
        itemNetPutByte(item & 0xff);
        itemNetPutByte(item >> 8 & 0xff);
    }
    char held[ITEM_EXTRA_POSITIONS * 10 + 1];
    unitExtraItemsText(unit, held, sizeof held);
    writeLog("item_slots_net_sent", "unit=%d items=%s", unit, held);
    return ITEM_EXTRA_POSITIONS;
}

static void getUnitExtraItem(int unit, int position) {
    unsigned low = (unsigned)itemNetGetByte() & 0xff, high = (unsigned)itemNetGetByte() & 0xff;
    int item = (short)(unsigned short)(low | high << 8);
    if (unit < 0) return;
    setUnitItemAt(unit, position, item < 0 ? -1 : item);
    if (position < ITEM_POSITION_MAX - 1) return;
    char held[ITEM_EXTRA_POSITIONS * 10 + 1];
    unitExtraItemsText(unit, held, sizeof held);
    writeLog("item_slots_net_read", "unit=%d items=%s", unit, held);
}

static __attribute__((unused)) int netItemsUnit(const void *items) {
    long long offset = (const unsigned char *)items - GAME_PTR(RVA_UNIT_ITEMS);
    if (offset < 0 || offset % UNIT_SIZE || offset / UNIT_SIZE >= UNIT_TABLE_MAX) return -1;
    return (int)(offset / UNIT_SIZE);
}

static int itemSlotNetState;

#ifndef __APPLE__
// ---- Windows: two entry hooks, the writer's last check, the reader's index test --------------------------------
static int netPutUnit = -1;

typedef void (*NetPutUnitRecordFn)(int unit);
static NetPutUnitRecordFn gameNetPutUnitRecord;
static void netPutUnitRecordHook(int unit) {
    int outer = netPutUnit;
    netPutUnit = unit;
    gameNetPutUnitRecord(unit);
    netPutUnit = outer;
}

static void netPutItemsEndHook(int failed, const char *text, long long cap) {
    if (netPutUnit >= 0) putUnitExtraItems(netPutUnit);
    GAME_FN(void (*)(int, const char *, ...), RVA_FATAL_IF)(failed, text, cap);
}

typedef void (*NetGetUnitItemsFn)(short *items);
static NetGetUnitItemsFn gameNetGetUnitItems;
static void netGetUnitItemsHook(short *items) {
    clearUnitExtraItems(netItemsUnit(items));
    gameNetGetUnitItems(items);
}

static void netGetExtraItemHook(const short *items, int position) { getUnitExtraItem(netItemsUnit(items), position); }

#define ITEM_NET_MOVED       3
#define ITEM_NET_JUMP        5
#define ITEM_NET_FUNCTIONS   (ITEM_CARD_STUB_SIZE + 8)
#define ITEM_NET_UNWIND      (ITEM_NET_FUNCTIONS + (int)sizeof(RUNTIME_FUNCTION))
#define ITEM_NET_PAGE_BYTES  (ITEM_NET_UNWIND + ITEM_STAT_UNWIND_SIZE)
_Static_assert(ITEM_NET_PAGE_BYTES <= 0x1000, "the item net page");

static int installItemNetIndexStub(void) {
    unsigned char *at = GAME_PTR(RVA_NET_GET_UNIT_ITEMS_TEST), *page = allocNearBytes(gameBase + RVA_TEXT_SECTION, ITEM_NET_PAGE_BYTES);
    if (!page) {
        writeLogError("hook_skipped", "site=item_net_index reason=no_near_page");
        return 0;
    }
    void *handler = (void *)netGetExtraItemHook;
    memcpy(page + ITEM_CARD_STUB_SIZE, &handler, 8);
    RUNTIME_FUNCTION *function = (RUNTIME_FUNCTION *)(page + ITEM_NET_FUNCTIONS);
    size_t n = buildItemNetIndexStub(page, page, page + ITEM_CARD_STUB_SIZE, at, ITEM_NET_MOVED, ITEM_POSITION_MAX - 1,
                                     GAME_PTR(RVA_NET_GET_UNIT_ITEMS_STORE), GAME_PTR(RVA_NET_GET_UNIT_ITEMS_BAD),
                                     GAME_PTR(RVA_NET_GET_UNIT_ITEMS_NEXT));
    if (!n || !copyItemStatUnwind(at, page + ITEM_NET_UNWIND)) {
        writeLogError("hook_skipped", "site=item_net_index reason=stub_not_built");
        return 0;
    }
    function->BeginAddress = 0;
    function->EndAddress = (DWORD)n;
    function->UnwindData = ITEM_NET_UNWIND;
    if (!RtlAddFunctionTable(function, 1, (DWORD64)(uintptr_t)page)) {
        writeLogError("hook_skipped", "site=item_net_index reason=unwind_not_added");
        return 0;
    }
    makePagesReadExecute(page, ITEM_NET_PAGE_BYTES);
    unsigned char jump[ITEM_NET_JUMP];
    buildEntryHookJump(jump, at, page, ITEM_NET_JUMP);
    return writeGameCode(at, jump, ITEM_NET_JUMP);
}

static int installItemNetSites(void) {
    struct { unsigned int rva; void *hook; void **game; } hooks[] = {
        {RVA_NET_PUT_UNIT_RECORD, (void *)netPutUnitRecordHook, (void **)&gameNetPutUnitRecord},
        {RVA_NET_GET_UNIT_ITEMS, (void *)netGetUnitItemsHook, (void **)&gameNetGetUnitItems},
    };
    if (!gameCodeMatches) {
        writeLogError("hook_skipped", "site=item_slot_net reason=game_code_differs");
        return 0;
    }
    for (size_t i = 0; i < sizeof hooks / sizeof hooks[0]; i++) {
        void *target = gameBase + hooks[i].rva;
        if (!fanpatchHookCreate(target, hooks[i].hook, hooks[i].game) || !fanpatchHookEnable(target, 1)) return 0;
    }
    if (!installItemNetIndexStub()) return 0;
    unsigned char *stub = allocNear(gameBase + RVA_TEXT_SECTION);
    if (!stub) {
        writeLogError("hook_skipped", "site=item_net_put reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stub, 0x1000);
    return redirectCall(RVA_NET_PUT_UNIT_ITEMS_END, RVA_FATAL_IF, stub, (void *)netPutItemsEndHook, "item_net_put", NULL);
}
#else
// ---- Mac: step hooks in RVA_NET_PUT_FULL_UNIT and RVA_NET_GET_UNIT_RECORD -------------------------------------------
static StepHook itemNetNextHook;

static void itemNetPutEndMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    putUnitExtraItems((int)*(const long long *)(uintptr_t)(ctx->sp + NET_PUT_FRAME_UNIT));
}

static void itemNetGetClearMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    clearUnitExtraItems((int)(unsigned)ctx->x[25]);
}

static void itemNetGetBadMac(StepContext *ctx, const StepHook *hook) {
    (void)hook;
    int position = (int)(unsigned)ctx->x[21];
    if (position < UNIT_ITEM_SLOTS || position >= ITEM_POSITION_MAX) return;
    getUnitExtraItem((int)(unsigned)ctx->x[25], position);
    ctx->pc = itemNetNextHook.resume;
}

static void itemNetNextMac(StepContext *ctx, const StepHook *hook) {
    (void)ctx;
    (void)hook;
}

static int installItemNetSites(void) {
    static StepHook putEnd, getClear, getBad;
    static const struct { StepHook *hook; void (*handler)(StepContext *, const StepHook *); unsigned int rva; const char *name; } HOOKS[] = {
        {&itemNetNextHook, itemNetNextMac, RVA_NET_GET_UNIT_ITEMS_NEXT, "item_net_get_next"},
        {&getClear, itemNetGetClearMac, RVA_NET_GET_UNIT_ITEMS_CLEAR, "item_net_get_clear"},
        {&getBad, itemNetGetBadMac, RVA_NET_GET_UNIT_ITEMS_BAD, "item_net_get_bad"},
        {&putEnd, itemNetPutEndMac, RVA_NET_PUT_UNIT_ITEMS_END, "item_net_put_end"},
    };
    if (!gameCodeMatches) {
        writeLogError("hook_skipped", "site=item_slot_net reason=game_code_differs");
        return 0;
    }
    for (size_t i = 0; i < sizeof HOOKS / sizeof HOOKS[0]; i++) {
        HOOKS[i].hook->handler = HOOKS[i].handler;
        if (!putStepHookMac(HOOKS[i].rva, HOOKS[i].hook, HOOKS[i].name, 0)) return 0;
    }
    return 1;
}
#endif

static int installItemSlotNet(void) {
    if (itemSlotNetState) return itemSlotNetState > 0;
    itemSlotNetState = installItemNetSites() ? 1 : -1;
#ifdef __APPLE__
    writeResultLog(itemSlotNetState > 0, "item_slot_net_installed", "padding_left=%ld", paddingLeft());
#else
    writeResultLog(itemSlotNetState > 0, "item_slot_net_installed", "sites=4");
#endif
    return itemSlotNetState > 0;
}
