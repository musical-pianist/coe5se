// @platform-split drown_search: Windows patches its one search loop, Mac the two copies clang made of it
#pragma once

#ifndef __APPLE__
#define DROWN_SEARCH_STUB_SIZE 0x80
#define DROWN_SEARCH_MOVED     7

static void emitDrownRel32(unsigned char **p, const unsigned char *target) {
    int rel = (int)(target - (*p + 4));
    memcpy(*p, &rel, 4);
    *p += 4;
}

static int installDrownSearch(void) {
    unsigned char *at = GAME_PTR(RVA_DROWN_COMMANDER_SEARCH), *page = allocNearBytes(at, DROWN_SEARCH_STUB_SIZE);
    if (!page) {
        writeLog("drown_search_installed", "ok=0 of 1 reason=no_near_page");
        return 0;
    }
    unsigned long long lastCommander = (unsigned long long)(uintptr_t)GAME_PTR(RVA_LAST_COMMANDER);
    unsigned char *skip = GAME_PTR(RVA_DROWN_UNIT_SKIP), *resume = at + DROWN_SEARCH_MOVED, *p = page;
    *p++ = 0x41; *p++ = 0x0f; *p++ = 0xbf; *p++ = 0x46; *p++ = 0xfe;
    *p++ = 0x85; *p++ = 0xc0;
    *p++ = 0x0f; *p++ = 0x8e; emitDrownRel32(&p, skip);
    *p++ = 0x41; *p++ = 0x8b; *p++ = 0x86; *p++ = 0x2c; *p++ = 0xff; *p++ = 0xff; *p++ = 0xff;
    *p++ = 0x44; *p++ = 0x39; *p++ = 0xe0;
    *p++ = 0x0f; *p++ = 0x85; emitDrownRel32(&p, skip);
    *p++ = 0x41; *p++ = 0x8b; *p++ = 0x86; *p++ = 0x30; *p++ = 0xff; *p++ = 0xff; *p++ = 0xff;
    *p++ = 0x3b; *p++ = 0x44; *p++ = 0x24; *p++ = 0x58;
    *p++ = 0x0f; *p++ = 0x85; emitDrownRel32(&p, skip);
    *p++ = 0x48; *p++ = 0xb8; memcpy(p, &lastCommander, 8); p += 8;
    *p++ = 0x83; *p++ = 0x38; *p++ = 0x00;
    *p++ = 0xe9; emitDrownRel32(&p, resume);
    makePagesReadExecute(page, DROWN_SEARCH_STUB_SIZE);
    unsigned char jump[DROWN_SEARCH_MOVED] = {0xe9, 0, 0, 0, 0, 0x90, 0x90};
    int rel = (int)(page - (at + 5));
    memcpy(jump + 1, &rel, 4);
    if (!writeGameCode(at, jump, sizeof jump)) {
        writeLog("drown_search_installed", "ok=0 of 1 reason=code_unwritable");
        return 0;
    }
    writeLog("drown_search_installed", "ok=1 of 1 address=%p stub=%p bytes=%d", (void *)at, (void *)page, (int)(p - page));
    return 1;
}
#else
__attribute__((used)) void *coe5seDrownUnits;
__attribute__((used)) void *coe5seDrownCommanderPage;
__attribute__((used)) void *coe5seDrownResume[2], *coe5seDrownSkip[2];
#define DROWN_SEARCH_STUB(n, slot)                                    \
    ".text\n.p2align 2\n_coe5seDrownSearchStub" #n ":\n"             \
    "  adrp x16, _coe5seDrownUnits@PAGE\n"                            \
    "  ldr x16, [x16, _coe5seDrownUnits@PAGEOFF]\n"                   \
    "  mov x17, #0x158\n"                                             \
    "  madd x16, x27, x17, x16\n"                                     \
    "  ldrsh w17, [x16, #0xe6]\n"                                     \
    "  cmp w17, #1\n"                                                 \
    "  b.lt 1f\n"                                                     \
    "  ldr w17, [x16, #0x14]\n"                                       \
    "  cmp x25, x17\n"                                                \
    "  b.ne 1f\n"                                                     \
    "  ldr w17, [x16, #0x18]\n"                                       \
    "  cmp x26, x17\n"                                                \
    "  b.ne 1f\n"                                                     \
    "  adrp x8, _coe5seDrownCommanderPage@PAGE\n"                     \
    "  ldr x8, [x8, _coe5seDrownCommanderPage@PAGEOFF]\n"             \
    "  adrp x16, _coe5seDrownResume@PAGE\n"                           \
    "  add x16, x16, _coe5seDrownResume@PAGEOFF\n"                    \
    "  ldr x16, [x16, #" #slot "]\n"                                  \
    "  br x16\n"                                                      \
    "1:\n"                                                            \
    "  adrp x16, _coe5seDrownSkip@PAGE\n"                             \
    "  add x16, x16, _coe5seDrownSkip@PAGEOFF\n"                      \
    "  ldr x16, [x16, #" #slot "]\n"                                  \
    "  br x16\n"
__asm__(DROWN_SEARCH_STUB(0, 0) DROWN_SEARCH_STUB(1, 8));
extern const unsigned char coe5seDrownSearchStub0[] __asm__("_coe5seDrownSearchStub0");
extern const unsigned char coe5seDrownSearchStub1[] __asm__("_coe5seDrownSearchStub1");

#define DROWN_ARM64_B 0x14000000u

static int installDrownSearch(void) {
    static const unsigned int SEARCH[2] = {RVA_DROWN_COMMANDER_SEARCH, RVA_DROWN_COMMANDER_SEARCH_2};
    static const unsigned int SKIP[2] = {RVA_DROWN_UNIT_SKIP, RVA_DROWN_UNIT_SKIP_2};
    const unsigned char *stubs[2] = {coe5seDrownSearchStub0, coe5seDrownSearchStub1};
    coe5seDrownUnits = GAME_PTR(RVA_UNITS);
    coe5seDrownCommanderPage = (void *)((uintptr_t)GAME_PTR(RVA_LAST_COMMANDER) & ~(uintptr_t)0xfff);
    int ok = 0;
    const char *reason = NULL;
    for (int i = 0; i < 2; i++) {
        unsigned char *at = gameBase + SEARCH[i];
        coe5seDrownResume[i] = at + 4;
        coe5seDrownSkip[i] = gameBase + SKIP[i];
        unsigned char *trampoline = newTrampoline((void *)stubs[i]);
        uint32_t word = trampoline ? DROWN_ARM64_B | ((uint32_t)((trampoline - at) >> 2) & 0x3ffffffu) : 0;
        if (trampoline && writeGameCode(at, &word, 4)) ok++;
        else reason = trampoline ? "code_unwritable" : "no_padding";
    }
    if (reason) writeLog("drown_search_installed", "ok=%d of 2 reason=%s", ok, reason);
    else writeLog("drown_search_installed", "ok=%d of 2", ok);
    return ok;
}
#endif

static void drownSearchInit(void) {
    if (!checkGameFunction("drown_search_check", "")) {
#ifdef __APPLE__
        writeLog("drown_search_installed", "ok=0 of 2 reason=game_code_changed");
#else
        writeLog("drown_search_installed", "ok=0 of 1 reason=game_code_changed");
#endif
        return;
    }
    installDrownSearch();
}
