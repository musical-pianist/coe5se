#ifndef X86_CODE_H
#define X86_CODE_H

#define STUB_SIZE         0x40
#define GATE_SIZE         0x50
#define ENTRY_HOOK_SLOT   64
#define ENTRY_HOOK_STUB   40
#define TRACE_STUB_SIZE   0x40
#define TRACE_RESUME_AT   0x20
#define TRACE_MOVED_BYTES 5

static UNUSED void emitRipStore(unsigned char **p, const unsigned char *opcode, int opcodeLen, const void *slot, const unsigned char *runsAt) {
    memcpy(*p, opcode, opcodeLen);
    int disp = (int)((const unsigned char *)slot - (runsAt + opcodeLen + 4));
    memcpy(*p + opcodeLen, &disp, 4);
    *p += opcodeLen + 4;
}

typedef struct {
    const void *xmm12, *xmm13;
    const void *ecxFrom;
    int zeroEcxEdx;
    const void *rsp, *head;
} SiteStubSlots;

static UNUSED size_t buildSiteStub(unsigned char *code, const unsigned char *stub, const SiteStubSlots *s) {
    static const unsigned char MOVSS_XMM12[] = {0xf3, 0x44, 0x0f, 0x11, 0x25};
    static const unsigned char MOVSS_XMM13[] = {0xf3, 0x44, 0x0f, 0x11, 0x2d};
    static const unsigned char MOV_ECX[] = {0x8b, 0x0d};
    static const unsigned char ZERO_ECX_EDX[] = {0x31, 0xc9, 0x31, 0xd2};
    static const unsigned char MOV_RSP[] = {0x48, 0x89, 0x25};
    static const unsigned char JMP_INDIRECT[] = {0xff, 0x25};
    unsigned char *p = code;
#define STUB_RUNS_AT (stub + (p - code))
    if (s->xmm12) emitRipStore(&p, MOVSS_XMM12, 5, s->xmm12, STUB_RUNS_AT);
    if (s->xmm13) emitRipStore(&p, MOVSS_XMM13, 5, s->xmm13, STUB_RUNS_AT);
    if (s->ecxFrom) emitRipStore(&p, MOV_ECX, 2, s->ecxFrom, STUB_RUNS_AT);
    if (s->zeroEcxEdx) {
        memcpy(p, ZERO_ECX_EDX, sizeof ZERO_ECX_EDX);
        p += sizeof ZERO_ECX_EDX;
    }
    emitRipStore(&p, MOV_RSP, 3, s->rsp, STUB_RUNS_AT);
    emitRipStore(&p, JMP_INDIRECT, 2, s->head, STUB_RUNS_AT);
#undef STUB_RUNS_AT
    return (size_t)(p - code);
}

static UNUSED void buildSiteCall(unsigned char code[6], const unsigned char *call, const unsigned char *stub, int tailJump) {
    int rel = (int)(stub - (call + 5));
    code[0] = tailJump ? 0xe9 : 0xe8;
    memcpy(code + 1, &rel, 4);
    code[5] = 0x90;
}

static UNUSED size_t buildGateCode(unsigned char *code, const unsigned char *gate, const void *const flags[3],
                                   const void *entered, const void *hookSlot, const void *nextSlot) {
    static const unsigned char JCC[3] = {0x7f, 0x75, 0x75};
    static const unsigned char JMP_INDIRECT[] = {0xff, 0x25};
    unsigned char *p = code, *fixups[3];
    for (int i = 0; i < 3; i++) {
        uintptr_t flag = (uintptr_t)flags[i];
        *p++ = 0x48; *p++ = 0xb8; memcpy(p, &flag, 8); p += 8;
        *p++ = 0x83; *p++ = 0x38; *p++ = 0x00;
        *p++ = JCC[i]; fixups[i] = p++;
    }
    uintptr_t self = (uintptr_t)gate, enteredAt = (uintptr_t)entered;
    *p++ = 0x48; *p++ = 0xb8; memcpy(p, &self, 8); p += 8;
    *p++ = 0x48; *p++ = 0xa3; memcpy(p, &enteredAt, 8); p += 8;
    emitRipStore(&p, JMP_INDIRECT, 2, hookSlot, gate + (p - code));
    unsigned char *skip = p;
    emitRipStore(&p, JMP_INDIRECT, 2, nextSlot, gate + (p - code));
    for (int i = 0; i < 3; i++) *fixups[i] = (unsigned char)(skip - (fixups[i] + 1));
    return (size_t)(p - code);
}

typedef struct { int at; const unsigned char *target; } CodeFixup;

static UNUSED int buildEntryHookSlot(unsigned char *code, const unsigned char *t, const unsigned char *fn, const unsigned char *saved,
                                     int size, const CodeFixup *fixups, int fixupCount, const void *detour) {
    static const unsigned char JMP_RIP[6] = {0xff, 0x25, 0, 0, 0, 0};
    memset(code, 0, ENTRY_HOOK_SLOT);
    memcpy(code, saved, (size_t)size);
    for (int k = 0; k < fixupCount; k++) {
        long long rel = fixups[k].target - (t + fixups[k].at + 4);
        int rel32 = (int)rel;
        if (rel != rel32) return 0;
        memcpy(code + fixups[k].at, &rel32, 4);
    }
    code[size] = 0xe9;
    int back = (int)((fn + size) - (t + size + 5));
    memcpy(code + size + 1, &back, 4);
    memcpy(code + ENTRY_HOOK_STUB, JMP_RIP, sizeof JMP_RIP);
    memcpy(code + ENTRY_HOOK_STUB + 6, &detour, 8);
    return 1;
}

static UNUSED void buildEntryHookJump(unsigned char *code, const unsigned char *fn, const unsigned char *stub, int size) {
    code[0] = 0xe9;
    int rel = (int)(stub - (fn + 5));
    memcpy(code + 1, &rel, 4);
    memset(code + 5, 0x90, (size_t)size - 5);
}

static UNUSED void buildTraceStubs(unsigned char *code, const unsigned char *entry, const unsigned char *fn,
                                   const unsigned char moved[TRACE_MOVED_BYTES], const void *hook) {
    memset(code, 0, TRACE_STUB_SIZE);
    unsigned char *resume = code + TRACE_RESUME_AT;
    memcpy(resume, moved, TRACE_MOVED_BYTES);
    resume[TRACE_MOVED_BYTES] = 0xe9;
    int back = (int)(fn + TRACE_MOVED_BYTES - (entry + TRACE_RESUME_AT + TRACE_MOVED_BYTES + 5));
    memcpy(resume + TRACE_MOVED_BYTES + 1, &back, 4);
    unsigned char *p = code;
    uintptr_t resumeAddress = (uintptr_t)(entry + TRACE_RESUME_AT);
    *p++ = 0x48; *p++ = 0xb8; memcpy(p, &resumeAddress, 8); p += 8;
    *p++ = 0xff; *p++ = 0x25; memset(p, 0, 4); p += 4;
    memcpy(p, &hook, 8);
}

static UNUSED unsigned long long buildTraceJumpWord(unsigned long long word, const unsigned char *fn, const unsigned char *entry) {
    unsigned int rel = (unsigned int)(int)(entry - (fn + 5));
    return (word & 0xffffff0000000000ULL) | (unsigned long long)rel << 8 | 0xe9;
}

#define ITEM_STAT_STUB_SIZE 64
#define ITEM_STAT_CALL_SIZE 6

typedef struct { int at, end; const unsigned char *target; } MovedField;

static UNUSED int buildItemStatStub(unsigned char *code, const unsigned char *stub, const void *entrySlot, const unsigned char *moved,
                                    int movedSize, const MovedField *fields, int fieldCount, const unsigned char *back) {
    if (movedSize < 0 || ITEM_STAT_CALL_SIZE + movedSize + 5 > ITEM_STAT_STUB_SIZE) return 0;
    memset(code, 0xcc, ITEM_STAT_STUB_SIZE);
    static const unsigned char CALL_INDIRECT[] = {0xff, 0x15};
    unsigned char *p = code;
    emitRipStore(&p, CALL_INDIRECT, 2, entrySlot, stub);
    memcpy(p, moved, (size_t)movedSize);
    for (int k = 0; k < fieldCount; k++) {
        if (fields[k].at < 0 || fields[k].at + 4 > fields[k].end || fields[k].end > movedSize) return 0;
        long long rel = fields[k].target - (stub + ITEM_STAT_CALL_SIZE + fields[k].end);
        if (rel != (int)rel) return 0;
        int rel32 = (int)rel;
        memcpy(p + fields[k].at, &rel32, 4);
    }
    p += movedSize;
    long long rel = back - (stub + (p - code) + 5);
    if (rel != (int)rel) return 0;
    int rel32 = (int)rel;
    *p++ = 0xe9;
    memcpy(p, &rel32, 4);
    return 1;
}

// ---- The unit card's item positions (item_slot_card.h) ----------------------------------------------------
#define ITEM_CARD_STUB_SIZE 96
#define ITEM_CARD_MOVED_MAX 16

typedef struct { unsigned char *p; const unsigned char *code, *runsAt; int ok; } ItemCardEmit;

static UNUSED void itemCardBytes(ItemCardEmit *e, const unsigned char *bytes, int n) {
    memcpy(e->p, bytes, (size_t)n);
    e->p += n;
}
static UNUSED void itemCardRel32(ItemCardEmit *e, const unsigned char *opcode, int opcodeLen, const unsigned char *target) {
    itemCardBytes(e, opcode, opcodeLen);
    long long rel = target - (e->runsAt + (e->p - e->code) + 4);
    if (rel != (int)rel) e->ok = 0;
    int rel32 = (int)rel;
    memcpy(e->p, &rel32, 4);
    e->p += 4;
}
static UNUSED void itemCardCall(ItemCardEmit *e, const void *slot) {
    static const unsigned char CALL_INDIRECT[] = {0xff, 0x15};
    unsigned char *at = e->p;
    emitRipStore(&e->p, CALL_INDIRECT, 2, slot, e->runsAt + (at - e->code));
}
static UNUSED unsigned char *itemCardShortJump(ItemCardEmit *e, unsigned char opcode) {
    *e->p++ = opcode;
    *e->p++ = 0;
    return e->p - 1;
}
static UNUSED void itemCardLand(ItemCardEmit *e, unsigned char *disp) { *disp = (unsigned char)(e->p - (disp + 1)); }
static UNUSED void itemCardGamePath(ItemCardEmit *e, const unsigned char *moved, int movedSize, const unsigned char *back) {
    static const unsigned char JMP[] = {0xe9};
    itemCardBytes(e, moved, movedSize);
    itemCardRel32(e, JMP, 1, back);
}
static UNUSED ItemCardEmit itemCardStart(unsigned char *code, const unsigned char *runsAt, int movedSize) {
    memset(code, 0xcc, ITEM_CARD_STUB_SIZE);
    return (ItemCardEmit){code, code, runsAt, movedSize > 0 && movedSize <= ITEM_CARD_MOVED_MAX};
}
static UNUSED size_t itemCardEnd(const ItemCardEmit *e) {
    return e->ok && e->p - e->code <= ITEM_CARD_STUB_SIZE ? (size_t)(e->p - e->code) : 0;
}

static UNUSED size_t buildItemCardRowStub(unsigned char *code, const unsigned char *stub, const void *slot, const unsigned char *moved,
                                          int movedSize, const unsigned char *loopTop, const unsigned char *rowEnd) {
    static const unsigned char UNIT[] = {0x44, 0x89, 0xe1, 0x44, 0x89, 0xf2};
    static const unsigned char TEST[] = {0x85, 0xc0};
    static const unsigned char LEFT[] = {0xbf, 8, 0, 0, 0, 0x48, 0x83, 0xc6, 16};
    ItemCardEmit e = itemCardStart(code, stub, movedSize);
    if (!e.ok) return 0;
    itemCardBytes(&e, UNIT, sizeof UNIT);
    itemCardCall(&e, slot);
    itemCardBytes(&e, TEST, sizeof TEST);
    unsigned char *game = itemCardShortJump(&e, 0x74);
    itemCardBytes(&e, moved, movedSize);
    itemCardBytes(&e, LEFT, sizeof LEFT);
    static const unsigned char JMP[] = {0xe9};
    itemCardRel32(&e, JMP, 1, rowEnd);
    itemCardLand(&e, game);
    itemCardGamePath(&e, moved, movedSize, loopTop);
    return itemCardEnd(&e);
}

static UNUSED size_t buildItemCardReadStub(unsigned char *code, const unsigned char *stub, const void *slot, const unsigned char *moved,
                                           int movedSize, const unsigned char *back) {
    static const unsigned char GUARD[] = {0x48, 0x83, 0xfb, 8};
    static const unsigned char ARGS[] = {0x44, 0x89, 0xe1, 0x89, 0xda};
    static const unsigned char RESULT[] = {0x4c, 0x63, 0xf0};
    static const unsigned char JMP[] = {0xe9};
    ItemCardEmit e = itemCardStart(code, stub, movedSize);
    if (!e.ok) return 0;
    itemCardBytes(&e, GUARD, sizeof GUARD);
    unsigned char *game = itemCardShortJump(&e, 0x72);
    itemCardBytes(&e, ARGS, sizeof ARGS);
    itemCardCall(&e, slot);
    itemCardBytes(&e, RESULT, sizeof RESULT);
    itemCardRel32(&e, JMP, 1, back);
    itemCardLand(&e, game);
    itemCardGamePath(&e, moved, movedSize, back);
    return itemCardEnd(&e);
}

static UNUSED size_t buildItemCardWriteStub(unsigned char *code, const unsigned char *stub, const void *slot, const unsigned char *moved,
                                            int movedSize, int positionAt, int spillAt, const unsigned char *back) {
    static const unsigned char CMP_RSP[] = {0x48, 0x83, 0xbc, 0x24};
    static const unsigned char UNIT[] = {0x44, 0x89, 0xe1};
    static const unsigned char EDX_RSP[] = {0x8b, 0x94, 0x24};
    static const unsigned char ITEM[] = {0x45, 0x0f, 0xbf, 0xc7};
    static const unsigned char JMP[] = {0xe9};
    ItemCardEmit e = itemCardStart(code, stub, movedSize);
    if (!e.ok) return 0;
    itemCardBytes(&e, CMP_RSP, sizeof CMP_RSP);
    itemCardBytes(&e, (const unsigned char *)&positionAt, 4);
    *e.p++ = 8;
    unsigned char *game = itemCardShortJump(&e, 0x72);
    itemCardBytes(&e, UNIT, sizeof UNIT);
    itemCardBytes(&e, EDX_RSP, sizeof EDX_RSP);
    itemCardBytes(&e, (const unsigned char *)&positionAt, 4);
    itemCardBytes(&e, ITEM, sizeof ITEM);
    itemCardCall(&e, slot);
    itemCardBytes(&e, EDX_RSP, sizeof EDX_RSP);
    itemCardBytes(&e, (const unsigned char *)&spillAt, 4);
    itemCardRel32(&e, JMP, 1, back);
    itemCardLand(&e, game);
    itemCardGamePath(&e, moved, movedSize, back);
    return itemCardEnd(&e);
}

static UNUSED size_t buildItemCardInfoStub(unsigned char *code, const unsigned char *stub, const void *slot, const unsigned char *moved,
                                           int movedSize, int idBase, int firstExtraId, const unsigned char *back, const unsigned char *skip) {
    if (idBase < 0 || idBase > 127 || firstExtraId < 0 || firstExtraId > 127) return 0;
    const unsigned char guard[] = {0x83, 0xfb, (unsigned char)firstExtraId};
    const unsigned char args[] = {0x44, 0x89, 0xe1, 0x8d, 0x53, (unsigned char)-idBase};
    static const unsigned char TEST[] = {0x85, 0xc0};
    static const unsigned char JL[] = {0x0f, 0x8c};
    static const unsigned char RESULT[] = {0x89, 0xc1};
    static const unsigned char JMP[] = {0xe9};
    ItemCardEmit e = itemCardStart(code, stub, movedSize);
    if (!e.ok) return 0;
    itemCardBytes(&e, guard, sizeof guard);
    unsigned char *game = itemCardShortJump(&e, 0x72);
    itemCardBytes(&e, args, sizeof args);
    itemCardCall(&e, slot);
    itemCardBytes(&e, TEST, sizeof TEST);
    itemCardRel32(&e, JL, 2, skip);
    itemCardBytes(&e, RESULT, sizeof RESULT);
    itemCardRel32(&e, JMP, 1, back);
    itemCardLand(&e, game);
    itemCardGamePath(&e, moved, movedSize, back);
    return itemCardEnd(&e);
}

// ---- Item positions in battle and item use (item_slot_battle.h) ----------------------------------------------
#define ITEM_ARGS_MAX 16

static UNUSED size_t buildItemLoopEndStub(unsigned char *code, const unsigned char *stub, const void *slot, const unsigned char *moved,
                                          int movedSize, const unsigned char *check, int checkSize, unsigned char jcc,
                                          const unsigned char *loopTop, const unsigned char *args, int argsSize, int goOn,
                                          const unsigned char *after) {
    static const unsigned char TEST[] = {0x85, 0xc0};
    static const unsigned char JNZ[] = {0x0f, 0x85};
    static const unsigned char JMP[] = {0xe9};
    const unsigned char jccOp[] = {0x0f, jcc};
    ItemCardEmit e = itemCardStart(code, stub, movedSize);
    if (!e.ok || checkSize < 0 || checkSize > ITEM_ARGS_MAX || argsSize < 0 || argsSize > ITEM_ARGS_MAX) return 0;
    itemCardBytes(&e, moved, movedSize);
    itemCardBytes(&e, check, checkSize);
    itemCardRel32(&e, jccOp, 2, loopTop);
    itemCardBytes(&e, args, argsSize);
    itemCardCall(&e, slot);
    if (goOn) {
        itemCardBytes(&e, TEST, sizeof TEST);
        itemCardRel32(&e, JNZ, 2, loopTop);
    }
    itemCardRel32(&e, JMP, 1, after);
    return itemCardEnd(&e);
}

static UNUSED size_t buildItemTakeoverStub(unsigned char *code, const unsigned char *stub, const void *slot, const unsigned char *moved,
                                           int movedSize, const unsigned char *args, int argsSize, const unsigned char *back,
                                           const unsigned char *done) {
    static const unsigned char TEST[] = {0x85, 0xc0};
    static const unsigned char JZ[] = {0x0f, 0x84};
    static const unsigned char JMP[] = {0xe9};
    ItemCardEmit e = itemCardStart(code, stub, movedSize);
    if (!e.ok || argsSize < 0 || argsSize > ITEM_ARGS_MAX) return 0;
    itemCardBytes(&e, moved, movedSize);
    itemCardBytes(&e, args, argsSize);
    itemCardCall(&e, slot);
    itemCardBytes(&e, TEST, sizeof TEST);
    itemCardRel32(&e, JZ, 2, back);
    itemCardRel32(&e, JMP, 1, done);
    return itemCardEnd(&e);
}

static UNUSED size_t buildItemReadStub(unsigned char *code, const unsigned char *stub, const void *slot, const unsigned char *moved,
                                       int movedSize, const unsigned char *guard, int guardSize, const unsigned char *args, int argsSize,
                                       const unsigned char *result, int resultSize, const unsigned char *back) {
    static const unsigned char JMP[] = {0xe9};
    ItemCardEmit e = itemCardStart(code, stub, movedSize);
    if (!e.ok || guardSize < 1 || guardSize > ITEM_ARGS_MAX || argsSize < 0 || argsSize > ITEM_ARGS_MAX || resultSize < 0 ||
        resultSize > ITEM_ARGS_MAX)
        return 0;
    itemCardBytes(&e, guard, guardSize);
    unsigned char *game = itemCardShortJump(&e, 0x72);
    itemCardBytes(&e, args, argsSize);
    itemCardCall(&e, slot);
    itemCardBytes(&e, result, resultSize);
    itemCardRel32(&e, JMP, 1, back);
    itemCardLand(&e, game);
    itemCardGamePath(&e, moved, movedSize, back);
    return itemCardEnd(&e);
}

// ---- Item positions in a unit record's packet (item_slot_net.h) ------------------------------------------------
static UNUSED size_t buildItemNetIndexStub(unsigned char *code, const unsigned char *stub, const void *slot, const unsigned char *moved,
                                           int movedSize, int last, const unsigned char *store, const unsigned char *bad,
                                           const unsigned char *next) {
    static const unsigned char JBE[] = {0x0f, 0x86}, JA[] = {0x0f, 0x87}, JMP[] = {0xe9};
    static const unsigned char ARGS[] = {0x48, 0x89, 0xf9, 0x89, 0xda};
    const unsigned char compare[] = {0x83, 0xfb, (unsigned char)last};
    ItemCardEmit e = itemCardStart(code, stub, movedSize);
    if (!e.ok || last < 0 || last > 0x7f) return 0;
    itemCardBytes(&e, moved, movedSize);
    itemCardRel32(&e, JBE, 2, store);
    itemCardBytes(&e, compare, sizeof compare);
    itemCardRel32(&e, JA, 2, bad);
    itemCardBytes(&e, ARGS, sizeof ARGS);
    itemCardCall(&e, slot);
    itemCardRel32(&e, JMP, 1, next);
    return itemCardEnd(&e);
}

#endif
