#pragma once

#define TERRAIN_FIX_SITE_WIN       6
#define TERRAIN_FIX_SITE_MAC       4
#define TERRAIN_FIX_STUB_WIN       79
#define TERRAIN_FIX_STUB_MAC_WORDS 30
#define TERRAIN_FIX_SITE_MAX       6
#define TERRAIN_FIX_STUB_MAX       120
#define TERRAIN_FIX_MAC_SLOT_LOAD  8

typedef struct {
    uint32_t saves;
    int32_t lastKind, lastSprite;
} TerrainFixUse;
static volatile TerrainFixUse terrainFixUse = {0, -1, -1};
_Static_assert(sizeof(TerrainFixUse) == 12, "the stubs store at offsets 0, 4 and 8");

static UNUSED int writeTerrainFixRel32(unsigned char *out, const unsigned char *next, const unsigned char *target) {
    intptr_t rel = target - next;
    if (rel < INT32_MIN || rel > INT32_MAX) return 0;
    int32_t rel32 = (int32_t)rel;
    memcpy(out, &rel32, 4);
    return 1;
}

static UNUSED int buildTerrainFixStubWin(unsigned char code[TERRAIN_FIX_STUB_WIN], const unsigned char *stub, const unsigned char *trsRead,
                                  const unsigned char *fsaPath, const unsigned char *slotReturn, const unsigned char *modLine,
                                  const unsigned char *widths, const unsigned char *heights, uint64_t use) {
    int32_t perKind = SPRITES_PER_KIND;
    code[0] = 0x0f; code[1] = 0x8e;
    code[6] = 0x83; code[7] = 0x3d; code[12] = 0x00;
    code[13] = 0x0f; code[14] = 0x8c;
    code[19] = 0x48; code[20] = 0x69; code[21] = 0xc3;
    memcpy(code + 22, &perKind, 4);
    code[26] = 0x49; code[27] = 0x03; code[28] = 0xc7;
    for (int i = 0; i < 2; i++) {
        unsigned char *p = code + 29 + 13 * i;
        p[0] = 0x48; p[1] = 0x8d; p[2] = 0x0d;
        p[7] = 0x66; p[8] = 0xc7; p[9] = 0x04; p[10] = 0x41; p[11] = 0x01; p[12] = 0x00;
    }
    code[55] = 0x48; code[56] = 0xb8;
    memcpy(code + 57, &use, 8);
    code[65] = 0xff; code[66] = 0x00;
    code[67] = 0x89; code[68] = 0x58; code[69] = 0x04;
    code[70] = 0x44; code[71] = 0x89; code[72] = 0x78; code[73] = 0x08;
    code[74] = 0xe9;
    return writeTerrainFixRel32(code + 2, stub + 6, trsRead) && writeTerrainFixRel32(code + 8, stub + 13, modLine) &&
           writeTerrainFixRel32(code + 15, stub + 19, fsaPath) && writeTerrainFixRel32(code + 32, stub + 36, widths) &&
           writeTerrainFixRel32(code + 45, stub + 49, heights) && writeTerrainFixRel32(code + 75, stub + 79, slotReturn);
}

static UNUSED int buildTerrainFixSiteWin(unsigned char code[TERRAIN_FIX_SITE_WIN], const unsigned char *site, const unsigned char *stub) {
    code[0] = 0xe9;
    code[5] = 0x90;
    return writeTerrainFixRel32(code + 1, site + 5, stub);
}

static UNUSED uint32_t terrainFixBranchMac(const unsigned char *at, const unsigned char *target) {
    intptr_t rel = target - at;
    if (rel < -(1 << 27) || rel >= (1 << 27) || (rel & 3)) return 0;
    return 0x14000000u | ((uint32_t)(rel >> 2) & 0x3ffffffu);
}

static UNUSED int buildTerrainFixStubMac(uint32_t code[TERRAIN_FIX_STUB_MAC_WORDS], const unsigned char *stub, const unsigned char *trsRead,
                                  const unsigned char *fsaPath, const unsigned char *slotReturn, uint32_t slotLoad, uint64_t modLine,
                                  uint64_t widths, uint64_t heights, uint64_t use) {
    code[0] = 0x5400004bu;
    code[1] = terrainFixBranchMac(stub + 4, trsRead);
    code[2] = 0x58000288u;
    code[3] = 0xb9400108u;
    code[4] = 0x36f80048u;
    code[5] = terrainFixBranchMac(stub + 20, fsaPath);
    code[6] = 0x52800009u | (uint32_t)(SPRITES_PER_KIND * 2) << 5;
    code[7] = 0x9b097f29u;
    code[8] = 0x8b170529u;
    code[9] = 0x580001eau;
    code[10] = 0x5280002bu;
    code[11] = 0x7829694bu;
    code[12] = 0x580001cau;
    code[13] = 0x7829694bu;
    code[14] = 0x580001cau;
    code[15] = 0xb940014bu;
    code[16] = 0x1100056bu;
    code[17] = 0xb900014bu;
    code[18] = 0xb9000559u;
    code[19] = 0xb9000957u;
    code[20] = slotLoad;
    code[21] = terrainFixBranchMac(stub + 84, slotReturn);
    memcpy(&code[22], &modLine, 8);
    memcpy(&code[24], &widths, 8);
    memcpy(&code[26], &heights, 8);
    memcpy(&code[28], &use, 8);
    return code[1] && code[5] && code[21];
}

typedef struct {
    unsigned char *site, *stub;
    size_t siteSize, stubSize;
    unsigned char original[TERRAIN_FIX_SITE_MAX], patched[TERRAIN_FIX_SITE_MAX], stubCode[TERRAIN_FIX_STUB_MAX];
    int installed;
} TerrainFix;
static TerrainFix terrainFix;

static int terrainFixInPlace(void) {
    return terrainFix.installed && !memcmp(terrainFix.site, terrainFix.patched, terrainFix.siteSize) &&
           !memcmp(terrainFix.stub, terrainFix.stubCode, terrainFix.stubSize);
}

static void logTerrainFixUse(void) {
    writeLog("mod_terrain_fix_used", "saves=%u last_kind=%d last_sprite=%d installed=%d", (unsigned)terrainFixUse.saves,
             (int)terrainFixUse.lastKind, (int)terrainFixUse.lastSprite, terrainFix.installed);
}

// @platform-split mod_terrain_fix: Windows jumps to a stub in a near page, Mac branches to one in the header padding
static int installModTerrainFix(void) {
    if (terrainFix.installed) return 1;
    static int exitLineRegistered;
    if (!exitLineRegistered) exitLineRegistered = !atexit(logTerrainFixUse);
    if (!gameCodeMatches) {
        writeLog("hook_skipped", "site=mod_terrain_fix reason=%s", gameCodeReason());
        return 0;
    }
    TerrainFix f = {.site = GAME_PTR(RVA_LOAD_SPRITE_COUNT_BRANCH)};
#ifdef __APPLE__
    f.siteSize = TERRAIN_FIX_SITE_MAC;
    f.stubSize = sizeof(uint32_t) * TERRAIN_FIX_STUB_MAC_WORDS;
    f.stub = takePadding(f.stubSize);
    if (!f.stub) {
        writeLog("hook_skipped", "site=mod_terrain_fix reason=no_padding");
        return 0;
    }
    uint32_t stubWords[TERRAIN_FIX_STUB_MAC_WORDS], slotLoad, branch;
    memcpy(&slotLoad, GAME_PTR(RVA_LOAD_SPRITE_SLOT_RETURN) - TERRAIN_FIX_MAC_SLOT_LOAD, 4);
    int built = buildTerrainFixStubMac(stubWords, f.stub, GAME_PTR(RVA_LOAD_SPRITE_TRS_READ), GAME_PTR(RVA_LOAD_SPRITE_FSA_PATH),
                                       GAME_PTR(RVA_LOAD_SPRITE_SLOT_RETURN), slotLoad, (uint64_t)(uintptr_t)GAME_PTR(RVA_MOD_LINE),
                                       (uint64_t)(uintptr_t)GAME_PTR(RVA_SPRITE_WIDTHS), (uint64_t)(uintptr_t)GAME_PTR(RVA_SPRITE_HEIGHTS),
                                       (uint64_t)(uintptr_t)&terrainFixUse);
    branch = terrainFixBranchMac(f.site, f.stub);
    memcpy(f.stubCode, stubWords, f.stubSize);
    memcpy(f.patched, &branch, 4);
    built = built && branch;
#else
    f.siteSize = TERRAIN_FIX_SITE_WIN;
    f.stubSize = TERRAIN_FIX_STUB_WIN;
    f.stub = allocNear(f.site);
    if (!f.stub) {
        writeLog("hook_skipped", "site=mod_terrain_fix reason=no_near_page");
        return 0;
    }
    int built = buildTerrainFixStubWin(f.stubCode, f.stub, GAME_PTR(RVA_LOAD_SPRITE_TRS_READ), GAME_PTR(RVA_LOAD_SPRITE_FSA_PATH),
                                       GAME_PTR(RVA_LOAD_SPRITE_SLOT_RETURN), GAME_PTR(RVA_MOD_LINE),
                                       GAME_PTR(RVA_SPRITE_WIDTHS), GAME_PTR(RVA_SPRITE_HEIGHTS), (uint64_t)(uintptr_t)&terrainFixUse) &&
                buildTerrainFixSiteWin(f.patched, f.site, f.stub);
#endif
    if (!built) {
        writeLog("hook_skipped", "site=mod_terrain_fix reason=out_of_reach");
        return 0;
    }
    memcpy(f.original, f.site, f.siteSize);
    if (!writeGameCode(f.stub, f.stubCode, f.stubSize)) {
        writeLog("hook_skipped", "site=mod_terrain_fix reason=code_unwritable");
        return 0;
    }
#ifndef __APPLE__
    makePagesReadExecute(f.stub, 0x1000);
#endif
    if (!writeGameCode(f.site, f.patched, f.siteSize)) {
        writeLog("hook_skipped", "site=mod_terrain_fix reason=code_unwritable");
        return 0;
    }
    f.installed = 1;
    terrainFix = f;
    writeLog("mod_terrain_fix_installed", "ok=1 site=%p stub=%p", (void *)f.site, (void *)f.stub);
    return 1;
}

static int removeModTerrainFix(const char *reason) {
    if (!terrainFix.installed) return 1;
    int ok = writeGameCode(terrainFix.site, terrainFix.original, terrainFix.siteSize);
    if (ok) terrainFix.installed = 0;
    writeResultLog(ok, "mod_terrain_fix_removed", "ok=%d reason=%s", ok, reason);
    return ok;
}
