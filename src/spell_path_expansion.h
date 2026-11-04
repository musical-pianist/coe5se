#define SPELL_PATH_COUNT       470
#define GAME_SPELL_PATH_COUNT  100
#define EXPANDED_PATH_COUNT    (SPELL_PATH_COUNT - GAME_SPELL_PATH_COUNT)
#define SPELL_PATH_LAST        (SPELL_PATH_COUNT - 1)
#define SPELL_PATH_LAST_LESS1  (SPELL_PATH_COUNT - 2)
_Static_assert(SPELL_PATH_COUNT < 0x1000, "a Mac cmp's immediate holds 12 bits");

static int spellPathsExpanded;
static const char **spellPathCopy;

static int spellPathTableSize(void) { return spellPathsExpanded ? SPELL_PATH_COUNT : GAME_SPELL_PATH_COUNT; }

static int spellPathCount(void) { return spellPathTableSize(); }

static void startSpellPathCopy(const char **copy) {
    memcpy(copy, gameBase + RVA_SPELL_PATH_NAMES, GAME_SPELL_PATH_COUNT * sizeof *copy);
    for (int p = GAME_SPELL_PATH_COUNT; p < SPELL_PATH_COUNT; p++) copy[p] = NULL;
    spellPathCopy = copy;
}

// ---- Network games -------------------------------------------------------------------------------
static uint32_t spellPathNamesHash(const char *const *names, int count, int *named) {
    uint32_t h = 2166136261u;
    *named = 0;
    for (int p = 0; p < count; p++) {
        const char *name = names[p] ? names[p] : "-";
        for (const char *c = name; *c; c++) h = (h ^ (unsigned char)*c) * 16777619u;
        h = (h ^ 0) * 16777619u;
        char placeholder[16];
        snprintf(placeholder, sizeof placeholder, "Path %d", p);
        if (p >= 70 && names[p] && strcmp(names[p], placeholder)) (*named)++;
    }
    return h;
}

static int spellPathsLastNetwork;

static void noteSpellPathsNetworkGame(const char *from) {
    int network = isNetworkGame();
    if (network == spellPathsLastNetwork) return;
    spellPathsLastNetwork = network;
    int named = 0;
    uint32_t hash = spellPathsExpanded ? spellPathNamesHash(spellPathCopy, SPELL_PATH_COUNT, &named) : 0;
    writeLog("spell_paths_network_game", "network=%d from=%s paths=%d named=%d names_hash=0x%08x", network, from, spellPathCount(), named,
             (unsigned)hash);
}

// ---- Transforms ----------------------------------------------------------------------------------
static int (*unitSpellWeaponLevel)(int unit, int path);
static void (*learnPromotionSpells)(int unit, int path, int level);
static void (*chooseMemorized)(int unit);

static struct {
    int unit;
    int levels[EXPANDED_PATH_COUNT];
} transformLevels = {.unit = -1};

static int transformSnapshotHook(int unit, int path) {
    if (path == 0) {
        transformLevels.unit = unit;
        for (int i = 0; i < EXPANDED_PATH_COUNT; i++) transformLevels.levels[i] = unitSpellWeaponLevel(unit, GAME_SPELL_PATH_COUNT + i);
    }
    return unitSpellWeaponLevel(unit, path);
}

static void transformMemorizeHook(int unit) {
    noteSpellPathsNetworkGame("transform");
    if (transformLevels.unit == unit) {
        for (int i = 0; i < EXPANDED_PATH_COUNT; i++) {
            int path = GAME_SPELL_PATH_COUNT + i, before = transformLevels.levels[i], now = unitSpellWeaponLevel(unit, path);
            for (int level = before + 1; level <= now; level++) learnPromotionSpells(unit, path, level);
            if (now > before) writeLog("spell_path_promotions", "unit=%d path=%d from=%d to=%d", unit, path, before, now);
        }
    }
    transformLevels.unit = -1;
    chooseMemorized(unit);
}

// ---- Windows path bounds -------------------------------------------------------------------------
typedef struct {
    unsigned int rva;
    int limit;
    const char *name;
} PathBoundSite;

static UNUSED int writeBoundStub(const PathBoundSite *s, unsigned char *stub, unsigned char site[10]) {
    unsigned char *at = gameBase + s->rva;
    int rex = (at[0] & 0xf0) == 0x40, jumpAt = rex + 3, size, rel;
    if (at[rex] != 0x83 || (at[rex + 1] & 0x38) != 0x38) return 0;
    unsigned char condition, *target;
    if (at[jumpAt] == 0x0f && (at[jumpAt + 1] & 0xf0) == 0x80) {
        size = jumpAt + 6;
        condition = at[jumpAt + 1];
        memcpy(&rel, at + jumpAt + 2, 4);
        target = at + size + rel;
    } else if ((at[jumpAt] & 0xf0) == 0x70) {
        size = jumpAt + 2;
        condition = (unsigned char)(at[jumpAt] + 0x10);
        target = at + size + (signed char)at[jumpAt + 1];
    } else {
        return 0;
    }
    int c = jumpAt - 1;
    unsigned char *back = at + size;
    long long toTarget = target - (stub + c + 10), toBack = back - (stub + c + 15), toStub = stub - (at + 5);
    if (toTarget < INT32_MIN || toTarget > INT32_MAX || toBack < INT32_MIN || toBack > INT32_MAX || toStub < INT32_MIN || toStub > INT32_MAX)
        return 0;
    if (rex) stub[0] = at[0];
    stub[rex] = 0x81;
    stub[rex + 1] = at[rex + 1];
    memcpy(stub + c, &s->limit, 4);
    stub[c + 4] = 0x0f;
    stub[c + 5] = condition;
    rel = (int)toTarget;
    memcpy(stub + c + 6, &rel, 4);
    stub[c + 10] = 0xe9;
    rel = (int)toBack;
    memcpy(stub + c + 11, &rel, 4);
    site[0] = 0xe9;
    rel = (int)toStub;
    memcpy(site + 1, &rel, 4);
    memset(site + 5, 0x90, (size_t)(size - 5));
    return size;
}

#ifndef __APPLE__
// ---- Windows: the patch --------------------------------------------------------------------------
static struct { unsigned char *at; int size; unsigned char bytes[10], old[10]; } pathWrites[64];
static int pathWriteCount;

static void queuePathWrite(unsigned char *at, const void *bytes, int size) {
    if (pathWriteCount >= (int)(sizeof pathWrites / sizeof pathWrites[0])) return;
    pathWrites[pathWriteCount].at = at;
    pathWrites[pathWriteCount].size = size;
    memcpy(pathWrites[pathWriteCount].old, at, (size_t)size);
    memcpy(pathWrites[pathWriteCount++].bytes, bytes, (size_t)size);
}

#include "spell_path_loops.h"

typedef struct {
    unsigned int rva;
    unsigned char immAt, imm, jumpAt, jump;
    const char *name;
} PathBytePatch;

static const PathBytePatch PATH_BYTE_PATCHES[] = {
    {RVA_STATS_KNOWN_SPELLS_PATH_LOOP,  3, 0x7f, 5, 0x8e, "stats_known_spells"},
    {RVA_LIBRARY_PATH_LOOP,             2, 0x7f, 4, 0x8e, "library"},
    {RVA_LEARN_START_SPELLS_PATH_LOOP,  3, 0x7f, 5, 0x8e, "learn_start_spells"},
    {RVA_CHOOSE_MEMORIZED_PATH_LOOP,    2, 0x7f, 4, 0x8f, "choose_memorized"},
};
#define PATH_BYTE_PATCH_COUNT ((int)(sizeof PATH_BYTE_PATCHES / sizeof PATH_BYTE_PATCHES[0]))
#define PATH_BUG_CHECK_SKIP_SIZE 6

static const PathBoundSite PATH_BOUND_SITES[] = {
    {RVA_PARSER_SPELLWEAPON50S_PATH,    SPELL_PATH_LAST_LESS1, "parser_spellweapon50s"},
    {RVA_PARSER_SPELLWEAPON_PATH,       SPELL_PATH_LAST_LESS1, "parser_spellweapon"},
    {RVA_PARSER_SPELLWEAPONBONUS_PATH,  SPELL_PATH_LAST_LESS1, "parser_spellweaponbonus"},
    {RVA_PARSER_SPELLWEAPON50_PATH,     SPELL_PATH_LAST_LESS1, "parser_spellweapon50"},
    {RVA_PARSER_SPELLWEAPONSINGLE_PATH, SPELL_PATH_LAST_LESS1, "parser_spellweaponsingle"},
    {RVA_ITEM_PATH_BOOST_BOUND,         SPELL_PATH_LAST_LESS1, "item_path_boost"},
    {RVA_STATS_SPELL_ATTACK_PATH_BOUND, SPELL_PATH_LAST,       "stats_path_level"},
    {RVA_UNIT_DEBUG_PATH_BOUND,         SPELL_PATH_COUNT,      "debug_unit_screen"},
};
#define PATH_BOUND_COUNT ((int)(sizeof PATH_BOUND_SITES / sizeof PATH_BOUND_SITES[0]))
#define PATH_BOUND_STUB_AT   0x100
#define PATH_BOUND_STUB_SIZE 32
_Static_assert(PATH_BOUND_STUB_AT + PATH_BOUND_COUNT * PATH_BOUND_STUB_SIZE <= 0x1000, "the stubs fit the one page allocNear gives");

typedef struct {
    unsigned int rva;
    int fieldOffset;
    const char *name;
} PathReaderSite;

static const PathReaderSite PATH_READER_SITES[] = {
    {RVA_PATH_NAMES_READ_STATS_LEVEL, 4, "stats_path_level"},
    {RVA_PATH_NAMES_READ_STATS_KNOWN, 4, "stats_known_spells"},
    {RVA_PATH_NAMES_READ_LIBRARY,     4, "library"},
    {RVA_PATH_NAMES_READ_LIBRARY_2,   4, "library_2"},
    {RVA_PATH_NAMES_READ_ITEM_BOOST,  4, "item_path_boost"},
    {RVA_PATH_NAMES_READ_DEBUG_UNIT,  4, "debug_unit_screen"},
};
#define PATH_READER_COUNT ((int)(sizeof PATH_READER_SITES / sizeof PATH_READER_SITES[0]))

typedef struct { unsigned int callRva, targetRva; int snapshot; const char *name; } PathTransformCall;

static const PathTransformCall PATH_TRANSFORM_CALLS[] = {
    {RVA_MASTERY_SNAPSHOT_CALL,          RVA_UNIT_SPELL_WEAPON_LEVEL, 1, "mastery_snapshot"},
    {RVA_MASTERY_CHOOSE_MEMORIZED_CALL,  RVA_CHOOSE_MEMORIZED,        0, "mastery_memorize"},
    {RVA_RITUAL_TRANSFORM_SNAPSHOT_CALL, RVA_UNIT_SPELL_WEAPON_LEVEL, 1, "ritual_transform_snapshot"},
    {RVA_RITUAL_TRANSFORM_MEMORIZE_CALL, RVA_CHOOSE_MEMORIZED,        0, "ritual_transform_memorize"},
};
#define PATH_TRANSFORM_CALL_COUNT ((int)(sizeof PATH_TRANSFORM_CALLS / sizeof PATH_TRANSFORM_CALLS[0]))
#define PATH_SITE_COUNT (PATH_BYTE_PATCH_COUNT + 1 + PATH_BOUND_COUNT + PATH_READER_COUNT + PATH_TRANSFORM_CALL_COUNT + PATH_LOOP_SITE_COUNT)
_Static_assert(PATH_SITE_COUNT <= (int)(sizeof pathWrites / sizeof pathWrites[0]), "every site's write fits pathWrites");

static int queueSpellPathWrites(const char **copy, unsigned char *stubs, unsigned char *loopStubs) {
    int matched = 0;
    pathWriteCount = 0;
    for (int i = 0; i < PATH_BYTE_PATCH_COUNT; i++) {
        const PathBytePatch *s = &PATH_BYTE_PATCHES[i];
        unsigned char *at = gameBase + s->rva, bytes[8];
        int size = s->jumpAt + 1;
        memcpy(bytes, at, (size_t)size);
        bytes[s->immAt] = s->imm;
        bytes[s->jumpAt] = s->jump;
        queuePathWrite(at, bytes, size);
        matched++;
    }
    unsigned char *skip = gameBase + RVA_SPELL_BUG_CHECK_SPELL_LOOP_SKIP, jump[PATH_BUG_CHECK_SKIP_SIZE] = {0xe9};
    int rel;
    memcpy(&rel, skip + 2, 4);
    rel += 1;
    memcpy(jump + 1, &rel, 4);
    jump[5] = 0x90;
    queuePathWrite(skip, jump, PATH_BUG_CHECK_SKIP_SIZE);
    matched++;
    for (int i = 0; i < PATH_BOUND_COUNT; i++) {
        const PathBoundSite *s = &PATH_BOUND_SITES[i];
        unsigned char site[10];
        int size = writeBoundStub(s, stubs + PATH_BOUND_STUB_AT + PATH_BOUND_STUB_SIZE * i, site);
        if (!size) {
            writeLogError("spell_paths_site_mismatch", "reason=bytes_differ group=bound name=%s rva=0x%x", s->name, s->rva);
            continue;
        }
        queuePathWrite(gameBase + s->rva, site, size);
        matched++;
    }
    long long delta = (const unsigned char *)copy - (gameBase + RVA_SPELL_PATH_NAMES);
    for (int i = 0; i < PATH_READER_COUNT; i++) {
        const PathReaderSite *s = &PATH_READER_SITES[i];
        unsigned char *at = gameBase + s->rva;
        int field;
        memcpy(&field, at + s->fieldOffset, 4);
        long long moved = field + delta;
        if (moved < INT32_MIN || moved > INT32_MAX) {
            writeLogError("spell_paths_site_mismatch", "reason=out_of_reach group=reader name=%s rva=0x%x", s->name, s->rva);
            continue;
        }
        int out = (int)moved;
        queuePathWrite(at + s->fieldOffset, &out, 4);
        matched++;
    }
    void *hooks[2] = {(void *)transformMemorizeHook, (void *)transformSnapshotHook};
    for (int h = 0; h < 2; h++) {
        unsigned char *stub = stubs + 16 * h;
        stub[0] = 0xff; stub[1] = 0x25; memset(stub + 2, 0, 4);
        memcpy(stub + 6, &hooks[h], 8);
    }
    for (int i = 0; i < PATH_TRANSFORM_CALL_COUNT; i++) {
        const PathTransformCall *c = &PATH_TRANSFORM_CALLS[i];
        unsigned char *call = gameBase + c->callRva, *stub = stubs + 16 * c->snapshot;
        memcpy(&rel, call + 1, 4);
        long long toStub = stub - (call + 5);
        if (call[0] != 0xe8 || call + 5 + rel != gameBase + c->targetRva || toStub < INT32_MIN || toStub > INT32_MAX) {
            writeLogError("spell_paths_site_mismatch", "reason=bytes_differ group=call name=%s rva=0x%x", c->name, c->callRva);
            continue;
        }
        int out = (int)toStub;
        queuePathWrite(call + 1, &out, 4);
        matched++;
    }
    return matched + queueSpellPathLoopWrites(loopStubs);
}

static void chooseMemorizedPastInPlace(int unit) { chooseMemorizedHook(unit); }

static void freeSpellPathMemory(const char **copy, unsigned char *stubs, unsigned char *loopStubs) {
    if (copy) VirtualFree((void *)copy, 0, MEM_RELEASE);
    if (stubs) VirtualFree(stubs, 0, MEM_RELEASE);
    if (loopStubs) VirtualFree(loopStubs, 0, MEM_RELEASE);
}

static int installSpellPathExpansion(void) {
    if (!gameCodeMatches) {
        writeLogError("spell_paths_not_expanded", "reason=game_code_differs sites=0 of %d", PATH_SITE_COUNT);
        return 0;
    }
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION), *loopStubs = allocNear(gameBase + RVA_TEXT_SECTION);
    const char **copy = (const char **)allocNearImage(SPELL_PATH_COUNT * sizeof(const char *));
    if (!copy || !stubs || !loopStubs) {
        writeLogError("spell_paths_not_expanded", "reason=no_memory_near_game names=%p stubs=%p loop_stubs=%p sites=0 of %d", (void *)copy,
                      (void *)stubs, (void *)loopStubs, PATH_SITE_COUNT);
        freeSpellPathMemory(copy, stubs, loopStubs);
        return 0;
    }
    int matched = queueSpellPathWrites(copy, stubs, loopStubs);
    const char *reason = matched != PATH_SITE_COUNT ? "game_build_differs"
                         : !openGameCode(gameBase + RVA_TEXT_SECTION, (size_t)(RVA_RDATA_SECTION - RVA_TEXT_SECTION)) ? "code_unwritable" : NULL;
    if (reason) {
        writeLogError("spell_paths_not_expanded", "reason=%s sites=%d of %d error=%lu", reason, matched, PATH_SITE_COUNT,
                 matched == PATH_SITE_COUNT ? GetLastError() : 0UL);
        freeSpellPathMemory(copy, stubs, loopStubs);
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    unitSpellWeaponLevel = GAME_FN(int (*)(int, int), RVA_UNIT_SPELL_WEAPON_LEVEL);
    learnPromotionSpells = GAME_FN(void (*)(int, int, int), RVA_LEARN_PROMOTION_SPELLS);
    chooseMemorized = chooseMemorizedPastInPlace;
    setPathGameFunctions(copy);
    startSpellPathCopy(copy);
    for (int i = 0; i < pathWriteCount; i++) {
        if (writeGameCode(pathWrites[i].at, pathWrites[i].bytes, (size_t)pathWrites[i].size)) continue;
        DWORD error = GetLastError();
        for (int j = i; j >= 0; j--) writeGameCode(pathWrites[j].at, pathWrites[j].old, (size_t)pathWrites[j].size);
        closeGameCode();
        writeLogError("spell_paths_not_expanded", "reason=code_unwritable sites=%d of %d error=%lu", matched, PATH_SITE_COUNT, error);
        spellPathCopy = NULL;
        freeSpellPathMemory(copy, stubs, loopStubs);
        return 0;
    }
    closeGameCode();
    spellPathsExpanded = 1;
    writeLog("spell_paths_expanded", "paths=%d sites=%d of %d names=%p", SPELL_PATH_COUNT, matched, PATH_SITE_COUNT, (void *)copy);
    return 1;
}
#else
// ---- Mac: the patch ------------------------------------------------------------------------------
typedef struct { unsigned int rva, value; const char *name; } PathCompareSite;

static const PathCompareSite PATH_COMPARE_SITES[] = {
    {RVA_PARSER_SPELLWEAPON50S_PATH,    SPELL_PATH_LAST_LESS1, "parser_spellweapon50s"},
    {RVA_PARSER_SPELLWEAPON_PATH,       SPELL_PATH_LAST_LESS1, "parser_spellweapon"},
    {RVA_PARSER_SPELLWEAPONBONUS_PATH,  SPELL_PATH_LAST_LESS1, "parser_spellweaponbonus"},
    {RVA_PARSER_SPELLWEAPON50_PATH,     SPELL_PATH_LAST_LESS1, "parser_spellweapon50"},
    {RVA_PARSER_SPELLWEAPONSINGLE_PATH, SPELL_PATH_LAST_LESS1, "parser_spellweaponsingle"},
    {RVA_ITEM_PATH_BOOST_BOUND,         SPELL_PATH_LAST_LESS1, "item_path_boost"},
    {RVA_ITEM_PATH_BOOST_BOUND_2,       SPELL_PATH_LAST_LESS1, "item_path_boost_2"},
    {RVA_ITEM_PATH_BOOST_BOUND_3,       SPELL_PATH_LAST_LESS1, "item_path_boost_3"},
    {RVA_ITEM_PATH_BOOST_BOUND_4,       SPELL_PATH_LAST_LESS1, "item_path_boost_4"},
    {RVA_STATS_SPELL_ATTACK_PATH_BOUND, SPELL_PATH_LAST,       "stats_path_level"},
    {RVA_UNIT_DEBUG_PATH_BOUND,         SPELL_PATH_LAST,       "debug_unit_screen"},
    {RVA_STATS_KNOWN_SPELLS_PATH_LOOP,  SPELL_PATH_COUNT,      "stats_known_spells"},
    {RVA_LIBRARY_PATH_LOOP,             SPELL_PATH_COUNT,      "library"},
    {RVA_LEARN_START_SPELLS_PATH_LOOP,  SPELL_PATH_COUNT,      "learn_start_spells"},
    {RVA_CHOOSE_MEMORIZED_PATH_LOOP,    SPELL_PATH_COUNT,      "choose_memorized"},
};
#define PATH_COMPARE_COUNT ((int)(sizeof PATH_COMPARE_SITES / sizeof PATH_COMPARE_SITES[0]))

typedef struct { unsigned int rva; const char *name; } PathReaderPair;

static const PathReaderPair PATH_READER_PAIRS[] = {
    {RVA_PATH_NAMES_READ_LIBRARY,      "library"},
    {RVA_PATH_NAMES_READ_STATS_LEVEL,  "stats_path_level"},
    {RVA_PATH_NAMES_READ_STATS_KNOWN,  "stats_known_spells"},
    {RVA_PATH_NAMES_READ_ITEM_BOOST,   "item_path_boost"},
    {RVA_PATH_NAMES_READ_ITEM_BOOST_2, "item_path_boost_2"},
    {RVA_PATH_NAMES_READ_ITEM_BOOST_3, "item_path_boost_3"},
    {RVA_PATH_NAMES_READ_ITEM_BOOST_4, "item_path_boost_4"},
    {RVA_PATH_NAMES_READ_DEBUG_UNIT,   "debug_unit_screen"},
};
#define PATH_READER_COUNT ((int)(sizeof PATH_READER_PAIRS / sizeof PATH_READER_PAIRS[0]))

typedef struct { unsigned int callRva, targetRva; int snapshot; const char *name; } PathTransformCall;

static const PathTransformCall PATH_TRANSFORM_CALLS[] = {
    {RVA_MASTERY_SNAPSHOT_CALL,          RVA_UNIT_SPELL_WEAPON_LEVEL, 1, "mastery_snapshot"},
    {RVA_MASTERY_CHOOSE_MEMORIZED_CALL,  RVA_CHOOSE_MEMORIZED,        0, "mastery_memorize"},
    {RVA_RITUAL_TRANSFORM_SNAPSHOT_CALL, RVA_UNIT_SPELL_WEAPON_LEVEL, 1, "ritual_transform_snapshot"},
    {RVA_RITUAL_TRANSFORM_MEMORIZE_CALL, RVA_CHOOSE_MEMORIZED,        0, "ritual_transform_memorize"},
};
#define PATH_TRANSFORM_CALL_COUNT ((int)(sizeof PATH_TRANSFORM_CALLS / sizeof PATH_TRANSFORM_CALLS[0]))
#define PATH_SITE_COUNT (PATH_COMPARE_COUNT + 1 + PATH_READER_COUNT + PATH_TRANSFORM_CALL_COUNT)

static uint32_t compareWithImmediate(uint32_t word, unsigned int value) { return (word & ~(0xfffu << 10)) | (value & 0xfffu) << 10; }

static int adrpMovedBy(uint32_t word, long long pages, uint32_t *out) {
    long long imm = (long long)((word >> 5) & 0x7ffff) << 2 | (word >> 29 & 3);
    if (imm & 0x100000) imm -= 0x200000;
    imm += pages;
    if (imm < -0x100000 || imm > 0xfffff) return 0;
    *out = (word & 0x9f00001fu) | (uint32_t)(imm & 3) << 29 | (uint32_t)((imm >> 2) & 0x7ffff) << 5;
    return 1;
}

static struct { unsigned char *at; uint32_t word, old; } pathWrites[PATH_SITE_COUNT];
static int pathWriteCount;
static unsigned char *pathMemory;
static size_t pathMemorySize;

static void queuePathWord(unsigned int rva, uint32_t word) {
    pathWrites[pathWriteCount].at = gameBase + rva;
    pathWrites[pathWriteCount].word = word;
    memcpy(&pathWrites[pathWriteCount++].old, gameBase + rva, 4);
}

static uint32_t pathGameWord(unsigned int rva) {
    uint32_t word;
    memcpy(&word, gameBase + rva, 4);
    return word;
}

static int queueSpellPathWrites(const char **copy) {
    int matched = 0;
    pathWriteCount = 0;
    for (int i = 0; i < PATH_COMPARE_COUNT; i++) {
        const PathCompareSite *s = &PATH_COMPARE_SITES[i];
        queuePathWord(s->rva, compareWithImmediate(pathGameWord(s->rva), s->value));
        matched++;
    }
    queuePathWord(RVA_SPELL_BUG_CHECK_SPELL_LOOP_SKIP,
                  0x14000000u | ((uint32_t)((RVA_SPELL_BUG_CHECK_SPELL_LOOP_END - RVA_SPELL_BUG_CHECK_SPELL_LOOP_SKIP) >> 2) & 0x3ffffffu));
    matched++;
    long long pages = ((long long)(uintptr_t)copy >> ADRP_PAGE_SHIFT) - ((long long)(uintptr_t)(gameBase + RVA_SPELL_PATH_NAMES) >> ADRP_PAGE_SHIFT);
    for (int i = 0; i < PATH_READER_COUNT; i++) {
        const PathReaderPair *s = &PATH_READER_PAIRS[i];
        uint32_t moved = 0;
        if (!adrpMovedBy(pathGameWord(s->rva), pages, &moved)) {
            writeLogError("spell_paths_site_mismatch", "reason=bytes_differ group=reader name=%s va=0x%llx", s->name, s->rva + 0x100000000ULL);
            continue;
        }
        queuePathWord(s->rva, moved);
        matched++;
    }
    unsigned char *trampolines[2] = {NULL, NULL};
    for (int i = 0; i < PATH_TRANSFORM_CALL_COUNT; i++) {
        const PathTransformCall *c = &PATH_TRANSFORM_CALLS[i];
        if (blTarget(gameBase + c->callRva) != gameBase + c->targetRva) {
            writeLogError("spell_paths_site_mismatch", "reason=bytes_differ group=call name=%s va=0x%llx", c->name, c->callRva + 0x100000000ULL);
            continue;
        }
        if (!trampolines[c->snapshot])
            trampolines[c->snapshot] = newTrampoline(c->snapshot ? (void *)transformSnapshotHook : (void *)transformMemorizeHook);
        if (!trampolines[c->snapshot]) {
            writeLogError("spell_paths_site_mismatch", "group=call name=%s reason=no_padding", c->name);
            continue;
        }
        queuePathWord(c->callRva, blTo(gameBase + c->callRva, trampolines[c->snapshot]));
        matched++;
    }
    return matched;
}

static int macRedirectCalls(unsigned int rva, void *hook, const char *name);
static void (*macLearnStartSpells)(int unit);
static void macLearnStartSpellsHook(int unit) {
    noteSpellPathsNetworkGame("learn_start_spells");
    macLearnStartSpells(unit);
}
static int (*macChooseMemorized)(int unit);
static int macChooseMemorizedHook(int unit) {
    noteSpellPathsNetworkGame("choose_memorized");
    return macChooseMemorized(unit);
}

static int installSpellPathExpansion(void) {
    if (!gameCodeMatches) {
        writeLogError("spell_paths_not_expanded", "reason=game_code_differs sites=0 of %d", PATH_SITE_COUNT);
        return 0;
    }
    size_t at = (uintptr_t)(gameBase + RVA_SPELL_PATH_NAMES) & ADRP_PAGE_MASK;
    size_t total = (at + SPELL_PATH_COUNT * sizeof(const char *) + MAC_PAGE_MASK) & ~(size_t)MAC_PAGE_MASK;
    unsigned char *memory = allocAfterImage(total);
    if (!memory) {
        writeLogError("spell_paths_not_expanded", "reason=no_memory_near_game bytes=%zu sites=0 of %d", total, PATH_SITE_COUNT);
        return 0;
    }
    const char **copy = (const char **)(memory + at);
    int matched = queueSpellPathWrites(copy);
    if (matched != PATH_SITE_COUNT) {
        writeLogError("spell_paths_not_expanded", "reason=game_build_differs sites=%d of %d", matched, PATH_SITE_COUNT);
        mach_vm_deallocate(mach_task_self(), (mach_vm_address_t)memory, total);
        return 0;
    }
    unitSpellWeaponLevel = GAME_FN(int (*)(int, int), RVA_UNIT_SPELL_WEAPON_LEVEL);
    learnPromotionSpells = GAME_FN(void (*)(int, int, int), RVA_LEARN_PROMOTION_SPELLS);
    chooseMemorized = GAME_FN(void (*)(int), RVA_CHOOSE_MEMORIZED);
    startSpellPathCopy(copy);
    for (int i = 0; i < pathWriteCount; i++) {
        if (writeGameCode(pathWrites[i].at, &pathWrites[i].word, 4)) continue;
        for (int j = i; j >= 0; j--) writeGameCode(pathWrites[j].at, &pathWrites[j].old, 4);
        writeLogError("spell_paths_not_expanded", "reason=code_unwritable va=0x%llx sites=%d of %d",
                 (unsigned long long)(pathWrites[i].at - gameBase) + 0x100000000ULL, matched, PATH_SITE_COUNT);
        spellPathCopy = NULL;
        return 0;
    }
    pathMemory = memory;
    pathMemorySize = total;
    spellPathsExpanded = 1;
    writeLog("spell_paths_expanded", "paths=%d sites=%d of %d names=%p", SPELL_PATH_COUNT, matched, PATH_SITE_COUNT, (void *)copy);
    macLearnStartSpells = GAME_FN(void (*)(int), RVA_LEARN_START_SPELLS);
    macChooseMemorized = GAME_FN(int (*)(int), RVA_CHOOSE_MEMORIZED);
    macRedirectCalls(RVA_LEARN_START_SPELLS, (void *)macLearnStartSpellsHook, "learn_start_spells");
    macRedirectCalls(RVA_CHOOSE_MEMORIZED, (void *)macChooseMemorizedHook, "choose_memorized");
    return 1;
}
#endif

static void spellPathExpansionInit(void) {
    installSpellPathExpansion();
}

__declspec(dllexport) int coe5seSpellPathCount(void) {
    return spellPathCount();
}

static int runsInNetworkGames(const void *address) { return isCoe5seCode(address); }
