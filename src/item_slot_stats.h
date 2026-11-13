#pragma once

// ---- The folds, both builds ---------------------------------------------------------------------------
#define ITEM_STAT_PAIRS     20
#define MONSTER_STAT_PAIRS  0x1ff
#define ITEM_PATHBOOST      0x11
#define ITEM_NAME_OF_SOUL   0x190
#define ITEM_STAT_BLESS_A   0x19b
#define ITEM_STAT_BLESS_B   0x4cb
#define UNIT_TEMP_IDS_AT    (RVA_UNIT_TEMP_ABILITY_IDS - RVA_UNITS)
#define UNIT_TEMP_VALUES_AT (RVA_UNIT_TEMP_ABILITY_VALUES - RVA_UNITS)

static const unsigned char *itemStatRecord(int item) {
    return item >= 0 && item < tableRecords(TABLE_ITEM) ? tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE : NULL;
}

static int itemStatPair(int item, int ability) {
    const unsigned char *r = itemStatRecord(item);
    if (!r || ability <= 0) return 0;
    const int *pairs = (const int *)(r + ITEM_ABILITIES);
    for (int i = 0; i < ITEM_STAT_PAIRS && pairs[2 * i]; i++)
        if (pairs[2 * i] == ability) return pairs[2 * i + 1];
    return 0;
}

static int itemStatBoostsPath(int item, int path) {
    const unsigned char *r = itemStatRecord(item);
    if (!r) return 0;
    const int *pairs = (const int *)(r + ITEM_ABILITIES);
    for (int i = 0; i < ITEM_STAT_PAIRS && pairs[2 * i]; i++)
        if (pairs[2 * i] == ITEM_PATHBOOST && pairs[2 * i + 1] == path) return 1;
    return 0;
}

static int itemStatLastHeld(int unit) {
    int last = -1;
    for (int p = 0; p < UNIT_ITEM_SLOTS; p++) {
        int item = unitItemAt(unit, p);
        if (item >= 0) last = item;
    }
    return last;
}

static int itemStatCombine(int total, int value, int sum) {
    if (sum) return total + value;
    return total == 0 || value > total ? value : total;
}

static int itemStatFold(int unit, int ability, int total, int sum, char *out, int size, int *first) {
    int last = itemStatLastHeld(unit);
    for (int p = UNIT_ITEM_SLOTS; p < ITEM_POSITION_MAX; p++) {
        int item = unitItemAt(unit, p);
        if (item < 0 || item == last) continue;
        last = item;
        int value = itemStatPair(item, ability);
        if (!value) continue;
        total = itemStatCombine(total, value, sum);
        if (out && !out[0] && size > 0) snprintf(out, (size_t)size, "%s", (const char *)itemStatRecord(item) + ITEM_NAME_OF_SOUL);
        if (first && *first < 0) *first = item;
    }
    return total;
}

static int itemStatPathboost(int unit, int path, int count) {
    int last = -1;
    for (int p = 0; p < ITEM_POSITION_MAX; p++) {
        int item = unitItemAt(unit, p);
        if (item < 0 || !itemStatBoostsPath(item, path) || item == last) continue;
        last = item;
        if (p >= UNIT_ITEM_SLOTS) count++;
    }
    return count;
}

static unsigned long long itemStatFlags(int unit, int field) {
    unsigned long long flags = 0;
    for (int p = UNIT_ITEM_SLOTS; p < ITEM_POSITION_MAX; p++) {
        const unsigned char *r = itemStatRecord(unitItemAt(unit, p));
        if (r) flags |= *(const unsigned long long *)(r + field);
    }
    return flags;
}

// ---- RVA_UNIT_ABILITY_TOTAL: the game's own, or the copy for a unit holding items past position 7 ---------------
typedef int (*UnitAbilityTotalFn)(int unit, int ability, int add, char *out, int size, int *first);
static UnitAbilityTotalFn gameUnitAbilityTotal;

static int itemStatMonsterPair(int monster, int ability) {
    const unsigned char *r = monsterRecord(monster);
    if (!r || ability <= 0) return 0;
    const int *pairs = (const int *)(r + MONSTER_ABILITIES);
    for (int i = 0; i < MONSTER_STAT_PAIRS && pairs[i]; i += 2)
        if (pairs[i] == ability) return pairs[i + 1];
    return 0;
}

static int itemStatBlessing(int player, int ability) {
#ifndef __APPLE__
    return GAME_FN(int (*)(int, int), RVA_PLAYER_BLESSING_VALUE)(player, ability);
#else
    // @platform-split item_stat_blessing: Mac inlines RVA_PLAYER_BLESSING_VALUE, so CoE5SE reads the 24 pairs itself
    if (player < 0 || ability <= 0) return 0;
    for (int i = 0; i < 24; i++) {
        int id = PG32(RVA_PLAYER_BLESSINGS + (size_t)player * PLAYER_BLOCK_SIZE + (size_t)i * 8);
        if (!id) return 0;
        if (id == ability) return PG32(RVA_PLAYER_BLESSINGS + (size_t)player * PLAYER_BLOCK_SIZE + (size_t)i * 8 + 4);
    }
    return 0;
#endif
}

static int itemStatsAbilityTotal(int unit, int ability, int add, char *out, int size, int *first) {
    if (out && size > 0) out[0] = 0;
    if (first) *first = -1;
    if (unit < 0 || unit > G32(RVA_UNIT_COUNT) || ability <= 0) return 0;
    size_t at = (size_t)unit * UNIT_SIZE;
    int monster = (short)G16(RVA_UNIT_MONSTER + at);
    if (monster < 0) return 0;
    int total = itemStatMonsterPair(monster, ability), last = -1;
    for (int p = 0; p < UNIT_ITEM_SLOTS; p++) {
        int item = unitItemAt(unit, p);
        if (item < 0 || item == last) continue;
        last = item;
        int value = itemStatPair(item, ability);
        if (!value) continue;
        total = itemStatCombine(total, value, add);
        if (out && !out[0] && size > 0) snprintf(out, (size_t)size, "%s", (const char *)itemStatRecord(item) + ITEM_NAME_OF_SOUL);
        if (first && *first < 0) *first = item;
    }
    total = itemStatFold(unit, ability, total, add, out, size, first);
    int owner = (short)G16(RVA_UNIT_PLAYER + at);
    if (owner >= 0) {
        typedef unsigned long long (*FlagsFn)(int);
        if ((GAME_FN(FlagsFn, RVA_UNIT_RESISTANCES)(unit) >> 14 & 1) || (GAME_FN(FlagsFn, RVA_UNIT_BENEFITS)(unit) >> 16 & 1) ||
            itemStatMonsterPair(monster, ITEM_STAT_BLESS_A) > 0 || itemStatMonsterPair(monster, ITEM_STAT_BLESS_B) > 0)
            total += itemStatBlessing(owner, ability);
    }
    int temporary = 0;
    for (int i = 0; i < UNIT_TEMP_ABILITIES; i++) {
        int id = (short)G16(RVA_UNITS + at + UNIT_TEMP_IDS_AT + (size_t)i * 2);
        if (!id) break;
        if (id == ability) {
            temporary = (int)G32(RVA_UNITS + at + UNIT_TEMP_VALUES_AT + (size_t)i * 4);
            break;
        }
    }
    if (add) return total + temporary;
    return temporary && total <= temporary ? temporary : total;
}

static int unitAbilityTotalHook(int unit, int ability, int add, char *out, int size, int *first) {
    if (!unitExtraItemCount(unit)) return gameUnitAbilityTotal(unit, ability, add, out, size, first);
    return itemStatsAbilityTotal(unit, ability, add, out, size, first);
}

// ---- Windows: the sites' stubs and their fold -----------------------------------------------------------
enum { ITEM_STAT_SUM, ITEM_STAT_MAX, ITEM_STAT_BY_MODE, ITEM_STAT_PATHBOOST, ITEM_STAT_FLAGS };
enum { ITEM_PLACE_NONE, ITEM_PLACE_REG, ITEM_PLACE_MEM, ITEM_PLACE_IMM };
typedef struct { unsigned char where, reg; int disp; } ItemPlace;
typedef struct {
    unsigned int rva;
    unsigned char kind;
    unsigned char unitReg, unitScale, unitAbsolute;
    int unitAdd;
    ItemPlace total, mode, ability;
    int abilityAdd;
    unsigned char movedSize;
    unsigned char movedCount;
    struct { unsigned char size, fieldAt, shortJump; } moved[6];
} ItemStatSite;
#include "item_slot_stats_sites.h"

static UNUSED int copyItemStatMoved(const ItemStatSite *s, const unsigned char *at, unsigned char *code, int room, MovedField fields[2],
                                    int *count) {
    int size = 0, fieldCount = 0;
    *count = 0;
    const unsigned char *from = at;
    for (int i = 0; i < s->movedCount && i < 6; i++) {
        int n = s->moved[i].size;
        if (s->moved[i].shortJump) {
            if (n != 2 || (from[0] & 0xf0) != 0x70 || size + 6 > room || fieldCount >= 2) return -1;
            code[size] = 0x0f;
            code[size + 1] = (unsigned char)(0x80 | (from[0] & 0x0f));
            memset(code + size + 2, 0, 4);
            fields[fieldCount++] = (MovedField){size + 2, size + 6, from + 2 + (signed char)from[1]};
            size += 6;
        } else {
            if (size + n > room) return -1;
            memcpy(code + size, from, (size_t)n);
            if (s->moved[i].fieldAt) {
                int at32 = s->moved[i].fieldAt, value;
                if (at32 + 4 > n || fieldCount >= 2) return -1;
                memcpy(&value, from + at32, 4);
                fields[fieldCount++] = (MovedField){size + at32, size + n, from + n + value};
            }
            size += n;
        }
        from += n;
    }
    *count = fieldCount;
    return from - at == s->movedSize ? size : -1;
}

static UNUSED int itemPlaceRead(const ItemPlace *p, const unsigned long long *r) {
    if (p->where == ITEM_PLACE_REG) return (int)(unsigned)r[p->reg & 15];
    if (p->where == ITEM_PLACE_MEM) return *(const int *)(uintptr_t)(r[p->reg & 15] + (long long)p->disp);
    return p->disp;
}

static UNUSED void itemPlaceWrite(const ItemPlace *p, unsigned long long *r, int value) {
    if (p->where == ITEM_PLACE_REG) r[p->reg & 15] = (unsigned)value;
    else if (p->where == ITEM_PLACE_MEM) *(int *)(uintptr_t)(r[p->reg & 15] + (long long)p->disp) = value;
}

static UNUSED int itemStatSiteUnit(const ItemStatSite *s, const unsigned long long *r) {
    long long offset = (long long)r[s->unitReg & 15] * s->unitScale + s->unitAdd - (s->unitAbsolute ? (long long)(uintptr_t)gameBase : 0);
    if (offset < 0 || offset % UNIT_SIZE || offset / UNIT_SIZE >= UNIT_TABLE_MAX) return -1;
    return (int)(offset / UNIT_SIZE);
}

static unsigned char itemStatSiteLogged[ITEM_STAT_SITE_COUNT];

static UNUSED void itemStatSiteRun(int index, unsigned long long *r) {
    const ItemStatSite *s = &ITEM_STAT_SITES[index];
    int unit = itemStatSiteUnit(s, r);
    if (unit < 0) {
        if (!itemStatSiteLogged[index]) writeLogWarning("item_slot_stats_no_unit", "site=0x%x reason=registers_name_no_unit", s->rva);
        itemStatSiteLogged[index] = 1;
        return;
    }
    if (!unitExtraItemCount(unit)) return;
    int argument = itemPlaceRead(&s->ability, r) + s->abilityAdd;
    if (s->kind == ITEM_STAT_FLAGS) {
        r[s->total.reg & 15] |= itemStatFlags(unit, argument);
        return;
    }
    int total = itemPlaceRead(&s->total, r);
    if (s->kind == ITEM_STAT_PATHBOOST) total = itemStatPathboost(unit, argument, total);
    else {
        int sum = s->kind == ITEM_STAT_SUM || (s->kind == ITEM_STAT_BY_MODE && itemPlaceRead(&s->mode, r) != 0);
        total = itemStatFold(unit, argument, total, sum, NULL, 0, NULL);
    }
    itemPlaceWrite(&s->total, r, total);
}

#ifndef __APPLE__
// ---- Windows: the stubs, their entry and their unwind info ---------------------------------------------
#define ITEM_STAT_ENTRY_SLOT   (ITEM_STAT_SITE_COUNT * ITEM_STAT_STUB_SIZE)
#define ITEM_STAT_FUNCTIONS    (ITEM_STAT_ENTRY_SLOT + 16)
#define ITEM_STAT_UNWIND       (ITEM_STAT_FUNCTIONS + ITEM_STAT_SITE_COUNT * (int)sizeof(RUNTIME_FUNCTION))
#define ITEM_STAT_UNWIND_SIZE  136
#define ITEM_STAT_PAGE_BYTES   (ITEM_STAT_UNWIND + ITEM_STAT_SITE_COUNT * ITEM_STAT_UNWIND_SIZE)
static unsigned char *itemStatPage;

__attribute__((used)) short **coe5seItemStatStore = &extraItems;
void coe5seItemStatHandle(unsigned long long *registers, const unsigned char *returnAddress);
__attribute__((used)) void coe5seItemStatHandle(unsigned long long *registers, const unsigned char *returnAddress) {
    long long index = (returnAddress - ITEM_STAT_CALL_SIZE - itemStatPage) / ITEM_STAT_STUB_SIZE;
    if (index < 0 || index >= ITEM_STAT_SITE_COUNT) return;
    itemStatSiteRun((int)index, registers);
}
__asm__(
    ".text\n"
    ".globl coe5seItemStatEntry\n"
    ".def coe5seItemStatEntry; .scl 2; .type 32; .endef\n"
    ".p2align 4\n"
    "coe5seItemStatEntry:\n"
    ".seh_proc coe5seItemStatEntry\n"
    "    pushq %rax\n"
    "    .seh_stackalloc 8\n"
    "    .seh_endprologue\n"
    "    movq coe5seItemStatStore(%rip), %rax\n"
    "    movq (%rax), %rax\n"
    "    testq %rax, %rax\n"
    "    popq %rax\n"
    "    jnz coe5seItemStatSlow\n"
    "    ret\n"
    ".seh_endproc\n"
    ".def coe5seItemStatSlow; .scl 3; .type 32; .endef\n"
    ".p2align 4\n"
    "coe5seItemStatSlow:\n"
    ".seh_proc coe5seItemStatSlow\n"
    "    pushq %r15\n    .seh_pushreg %r15\n"
    "    pushq %r14\n    .seh_pushreg %r14\n"
    "    pushq %r13\n    .seh_pushreg %r13\n"
    "    pushq %r12\n    .seh_pushreg %r12\n"
    "    pushq %r11\n    .seh_stackalloc 8\n"
    "    pushq %r10\n    .seh_stackalloc 8\n"
    "    pushq %r9\n    .seh_stackalloc 8\n"
    "    pushq %r8\n    .seh_stackalloc 8\n"
    "    pushq %rdi\n    .seh_pushreg %rdi\n"
    "    pushq %rsi\n    .seh_pushreg %rsi\n"
    "    pushq %rbp\n    .seh_pushreg %rbp\n"
    "    pushq $0\n    .seh_stackalloc 8\n"
    "    pushq %rbx\n    .seh_pushreg %rbx\n"
    "    pushq %rdx\n    .seh_stackalloc 8\n"
    "    pushq %rcx\n    .seh_stackalloc 8\n"
    "    pushq %rax\n    .seh_stackalloc 8\n"
    "    subq $0x88, %rsp\n    .seh_stackalloc 0x88\n"
    "    leaq 0x80(%rsp), %rbp\n    .seh_setframe %rbp, 0x80\n"
    "    .seh_endprologue\n"
    "    andq $-16, %rsp\n"
    "    movdqu %xmm0, 0x20(%rsp)\n"
    "    movdqu %xmm1, 0x30(%rsp)\n"
    "    movdqu %xmm2, 0x40(%rsp)\n"
    "    movdqu %xmm3, 0x50(%rsp)\n"
    "    movdqu %xmm4, 0x60(%rsp)\n"
    "    movdqu %xmm5, 0x70(%rsp)\n"
    "    leaq 0x90(%rbp), %rax\n"
    "    movq %rax, 0x28(%rbp)\n"
    "    leaq 8(%rbp), %rcx\n"
    "    movq 0x88(%rbp), %rdx\n"
    "    callq coe5seItemStatHandle\n"
    "    movdqu 0x20(%rsp), %xmm0\n"
    "    movdqu 0x30(%rsp), %xmm1\n"
    "    movdqu 0x40(%rsp), %xmm2\n"
    "    movdqu 0x50(%rsp), %xmm3\n"
    "    movdqu 0x60(%rsp), %xmm4\n"
    "    movdqu 0x70(%rsp), %xmm5\n"
    "    leaq -0x80(%rbp), %rsp\n"
    "    addq $0x88, %rsp\n"
    "    popq %rax\n"
    "    popq %rcx\n"
    "    popq %rdx\n"
    "    popq %rbx\n"
    "    addq $8, %rsp\n"
    "    popq %rbp\n"
    "    popq %rsi\n"
    "    popq %rdi\n"
    "    popq %r8\n"
    "    popq %r9\n"
    "    popq %r10\n"
    "    popq %r11\n"
    "    popq %r12\n"
    "    popq %r13\n"
    "    popq %r14\n"
    "    popq %r15\n"
    "    ret\n"
    ".seh_endproc\n");
void coe5seItemStatEntry(void);

static size_t copyItemStatUnwind(const unsigned char *at, unsigned char *out) {
    DWORD64 imageBase = 0;
    PRUNTIME_FUNCTION f = RtlLookupFunctionEntry((DWORD64)(uintptr_t)at, &imageBase, NULL);
    if (!f || imageBase != (DWORD64)(uintptr_t)gameBase) return 0;
    int codes = 0, maxCodes = (ITEM_STAT_UNWIND_SIZE - 4) / 2;
    for (int link = 0; link < 16; link++) {
        const unsigned char *info = gameBase + f->UnwindData;
        int flags = info[0] >> 3, count = info[2];
        if ((info[0] & 7) != 1 || codes + count > maxCodes) return 0;
        memcpy(out + 4 + codes * 2, info + 4, (size_t)count * 2);
        codes += count;
        if (!(flags & 4)) {
            out[0] = 1;
            out[1] = 0;
            out[2] = (unsigned char)codes;
            out[3] = info[3];
            return 4 + (size_t)((codes + 1) & ~1) * 2;
        }
        f = (PRUNTIME_FUNCTION)(info + 4 + ((count + 1) & ~1) * 2);
    }
    return 0;
}

static int installItemStatSites(int counts[5]) {
    if (!gameCodeMatches) {
        writeLogError("hook_skipped", "site=item_slot_stats reason=game_code_differs");
        return 0;
    }
    itemStatPage = allocNearBytes(gameBase + RVA_TEXT_SECTION, ITEM_STAT_PAGE_BYTES);
    if (!itemStatPage) {
        writeLogError("hook_skipped", "site=item_slot_stats reason=no_near_page");
        return 0;
    }
    void *entry = (void *)coe5seItemStatEntry;
    memcpy(itemStatPage + ITEM_STAT_ENTRY_SLOT, &entry, sizeof entry);
    static unsigned char ready[ITEM_STAT_SITE_COUNT];
    RUNTIME_FUNCTION *functions = (RUNTIME_FUNCTION *)(itemStatPage + ITEM_STAT_FUNCTIONS);
    int n = 0;
    for (int i = 0; i < ITEM_STAT_SITE_COUNT; i++) {
        const ItemStatSite *s = &ITEM_STAT_SITES[i];
        unsigned char *at = gameBase + s->rva, *stub = itemStatPage + (size_t)i * ITEM_STAT_STUB_SIZE;
        unsigned char *unwind = itemStatPage + ITEM_STAT_UNWIND + (size_t)i * ITEM_STAT_UNWIND_SIZE, code[32];
        MovedField fields[2];
        int fieldCount = 0, codeSize = copyItemStatMoved(s, at, code, sizeof code, fields, &fieldCount);
        if (codeSize < 0 || !copyItemStatUnwind(at, unwind) ||
            !buildItemStatStub(stub, stub, itemStatPage + ITEM_STAT_ENTRY_SLOT, code, codeSize, fields, fieldCount, at + s->movedSize)) {
            writeLog("hook_skipped", "site=item_stat_0x%x reason=stub_not_built", s->rva);
            continue;
        }
        functions[n].BeginAddress = (DWORD)(stub - itemStatPage);
        functions[n].EndAddress = (DWORD)(stub - itemStatPage + ITEM_STAT_STUB_SIZE);
        functions[n].UnwindData = (DWORD)(unwind - itemStatPage);
        n++;
        ready[i] = 1;
    }
    if (!RtlAddFunctionTable(functions, (DWORD)n, (DWORD64)(uintptr_t)itemStatPage)) {
        writeLogError("hook_skipped", "site=item_slot_stats reason=unwind_not_added");
        return 0;
    }
    makePagesReadExecute(itemStatPage, ITEM_STAT_PAGE_BYTES);
    int installed = 0;
    for (int i = 0; i < ITEM_STAT_SITE_COUNT; i++) {
        if (!ready[i]) continue;
        const ItemStatSite *s = &ITEM_STAT_SITES[i];
        unsigned char *at = gameBase + s->rva, code[24];
        int rel = (int)(itemStatPage + (size_t)i * ITEM_STAT_STUB_SIZE - (at + 5));
        code[0] = 0xe9;
        memcpy(code + 1, &rel, 4);
        memset(code + 5, 0x90, (size_t)s->movedSize - 5);
        if (!writeGameCode(at, code, s->movedSize)) {
            writeLog("hook_skipped", "site=item_stat_0x%x reason=code_unwritable", s->rva);
            continue;
        }
        counts[s->kind]++;
        installed++;
    }
    return installed;
}
#else
// ---- Mac: the flag sums and the pathboost count as step hooks --------------------------------------------
// @platform-split item_stat_sites: Windows patches about 390 generated sites, Mac five step hooks (the rest call RVA_UNIT_ABILITY_TOTAL)
typedef struct {
    StepHook hook;
    unsigned int rva;
    const char *name;
    unsigned char kind, unitReg, totalReg, argumentReg;
    int argument;
} ItemStatMacSite;

static void itemStatMacHandler(StepContext *ctx, const StepHook *hook) {
    const ItemStatMacSite *s = (const ItemStatMacSite *)hook;
    int unit = (int)(unsigned)ctx->x[s->unitReg];
    if (!unitExtraItemCount(unit)) return;
    if (s->kind == ITEM_STAT_FLAGS) ctx->x[s->totalReg] |= itemStatFlags(unit, s->argument);
    else ctx->x[s->totalReg] = (unsigned)itemStatPathboost(unit, (int)(unsigned)ctx->x[s->argumentReg], (int)(unsigned)ctx->x[s->totalReg]);
}

static ItemStatMacSite itemStatMacSites[] = {
    {{0}, RVA_UNIT_RESISTANCES_ITEMS_END, "item_stat_resistances", ITEM_STAT_FLAGS, 26, 24, 0xff, 0x40},
    {{0}, RVA_UNIT_STATE_FLAGS_ITEMS_END, "item_stat_state_flags", ITEM_STAT_FLAGS, 9, 0, 0xff, 0x48},
    {{0}, RVA_UNIT_BENEFITS_ITEMS_END, "item_stat_benefits", ITEM_STAT_FLAGS, 10, 19, 0xff, 0x50},
    {{0}, RVA_UNIT_FLAGS_ITEMS_END, "item_stat_flags", ITEM_STAT_FLAGS, 22, 9, 0xff, 0x58},
    {{0}, RVA_UNIT_PATH_BOOST_ITEMS_END, "item_stat_pathboost", ITEM_STAT_PATHBOOST, 25, 26, 20, 0},
};

static int installItemStatSites(int counts[5]) {
    if (!gameCodeMatches) {
        writeLogError("hook_skipped", "site=item_slot_stats reason=game_code_differs");
        return 0;
    }
    int installed = 0;
    for (size_t i = 0; i < sizeof itemStatMacSites / sizeof itemStatMacSites[0]; i++) {
        ItemStatMacSite *s = &itemStatMacSites[i];
        s->hook.handler = itemStatMacHandler;
        if (!putStepHookMac(s->rva, &s->hook, s->name, 0)) continue;
        counts[s->kind]++;
        installed++;
    }
    return installed;
}
#endif

static int itemSlotStatsState;

static int installItemSlotStats(void) {
    if (itemSlotStatsState) return itemSlotStatsState > 0;
    itemSlotStatsState = -1;
    void *target = gameBase + RVA_UNIT_ABILITY_TOTAL;
    int total = fanpatchHookCreate(target, (void *)unitAbilityTotalHook, (void **)&gameUnitAbilityTotal) && fanpatchHookEnable(target, 1);
    int counts[5] = {0};
    int sites = installItemStatSites(counts);
    writeResultLog(total && sites, "item_slot_stats_installed", "ability_total=%d loops=%d pathboost=%d flags=%d sites=%d",
                   total, counts[ITEM_STAT_SUM] + counts[ITEM_STAT_MAX] + counts[ITEM_STAT_BY_MODE], counts[ITEM_STAT_PATHBOOST],
                   counts[ITEM_STAT_FLAGS], sites);
    if (total && sites) itemSlotStatsState = 1;
    return itemSlotStatsState > 0;
}
