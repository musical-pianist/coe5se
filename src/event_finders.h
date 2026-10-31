#pragma once
#include "event_finder_shadow.h"

#define FINDER_MATCHES_MAX 10000
#define FINDER_ANY_PLAYER  (-1)
#define FINDER_NO_UNIT     (-1)
#define FINDER_KEPT_MAX    4
#define FINDER_NAME_MAX    128

typedef void (*FinderTraceFn)(const char *format, ...);

static struct {
    int lineDepth;
    int built;
    int width, height;
    int installed;
    int keptLists, keptCounts;
    EventFinderCounts counts;
} eventFinders;

static int finderHead[MAP_COLUMNS_MAX * MAP_COLUMN_SQUARES];
static int finderNext[UNIT_TABLE_MAX];

typedef struct {
    int kind, player, plane, n;
    char name[FINDER_NAME_MAX];
} FinderKept;
static FinderKept finderLists[FINDER_KEPT_MAX], finderCounts[FINDER_KEPT_MAX];
static int finderListUnits[FINDER_KEPT_MAX][FINDER_MATCHES_MAX];

static EventFinderFn finderGame[EVENT_FINDER_KINDS];
static int (*finderRandom)(int n);
static int (*finderArmyHere)(int player, int x, int y);
static unsigned long long (*finderUnitState)(int unit);
static unsigned long long (*finderUnitResistances)(int unit);
static FinderTraceFn finderTrace;
static const char *finderCountTrace;
static EventFinderShadowFn finderShadow;

static void eventFindersLineBegin(void) {
    eventFinders.lineDepth++;
    eventFinders.built = 0;
    eventFinders.keptLists = eventFinders.keptCounts = 0;
}

static void eventFindersLineEnd(void) {
    eventFinders.lineDepth--;
    eventFinders.built = 0;
    eventFinders.keptLists = eventFinders.keptCounts = 0;
}

static int buildFinderLists(void) {
    int width = G32(RVA_MAP_DEPTH), height = G32(RVA_MAP_H), last = G32(RVA_UNIT_COUNT);
    if (last >= UNIT_TABLE_MAX) return 0;
    if (width > MAP_COLUMNS_MAX) width = MAP_COLUMNS_MAX;
    if (height > MAP_COLUMN_SQUARES) height = MAP_COLUMN_SQUARES;
    if (width < 0) width = 0;
    if (height < 0) height = 0;
    memset(finderHead, 0xff, (size_t)width * MAP_COLUMN_SQUARES * sizeof finderHead[0]);
    for (int u = last; u >= 0; u--) {
        size_t at = (size_t)u * UNIT_SIZE;
        if ((short)G16(RVA_UNIT_HP + at) <= 0) continue;
        int x = (int)G32(RVA_UNIT_X + at), y = (int)G32(RVA_UNIT_X + at + 4);
        if (x < 0 || y < 0 || x >= width || y >= height) continue;
        size_t square = (size_t)x * MAP_COLUMN_SQUARES + (size_t)y;
        finderNext[u] = finderHead[square];
        finderHead[square] = u;
    }
    eventFinders.width = width;
    eventFinders.height = height;
    eventFinders.built = 1;
    eventFinders.counts.builds++;
    return 1;
}

static int finderSquareFirst(int x, int y, int *ok) {
    *ok = (eventFinders.built || buildFinderLists()) && x >= 0 && y >= 0 && x < eventFinders.width && y < eventFinders.height;
    return *ok ? finderHead[(size_t)x * MAP_COLUMN_SQUARES + (size_t)y] : FINDER_NO_UNIT;
}

static const char *finderMonsterName(int u) {
    long monster = (short)G16(RVA_UNIT_MONSTER + (size_t)u * UNIT_SIZE);
    return (const char *)(tableAddress(TABLE_MONSTER) + monster * MONSTER_SIZE);
}

static int finderUnitMatches(int kind, int u, int player, const char *name) {
    size_t at = (size_t)u * UNIT_SIZE;
    if (player != FINDER_ANY_PLAYER && (short)G16(RVA_UNIT_PLAYER + at) != player) return 0;
    if (kind == EVENT_FINDER_UNIT) return !name[0] || !strcmp(finderMonsterName(u), name);
    for (int i = 0; i < UNIT_ITEM_SLOTS; i++) {
        short item = (short)G16(RVA_UNIT_ITEMS + at + (size_t)i * 2);
        if (item >= 0 && !strcmp((const char *)(tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE), name)) return 1;
    }
    return 0;
}

static int finderAfflictedMatches(int u, const EventFinderQuery *q) {
    size_t at = (size_t)u * UNIT_SIZE;
    if (q->player != FINDER_ANY_PLAYER && (short)G16(RVA_UNIT_PLAYER + at) != q->player) return 0;
    if (q->monster != -1 && (short)G16(RVA_UNIT_MONSTER + at) != q->monster) return 0;
    if ((finderUnitState(u) & q->stateMask) != q->stateMask) return 0;
    return (finderUnitResistances(u) & q->resistanceMask) == q->resistanceMask;
}

static int finderCountMatches(int u, const EventFinderQuery *q) {
    size_t at = (size_t)u * UNIT_SIZE;
    if (q->player != FINDER_ANY_PLAYER && (short)G16(RVA_UNIT_PLAYER + at) != q->player) return 0;
    return (short)G16(RVA_UNIT_MONSTER + at) >= 0 && !strcmp(finderMonsterName(u), q->name);
}

static int finderMatches(int u, const EventFinderQuery *q) {
    return q->kind == EVENT_FINDER_AFFLICTED ? finderAfflictedMatches(u, q) : finderUnitMatches(q->kind, u, q->player, q->name);
}

static int finderPickOnSquare(int first, const EventFinderQuery *q) {
    int n = 0;
    for (int u = first; u != FINDER_NO_UNIT && n < FINDER_MATCHES_MAX; u = finderNext[u])
        if (finderMatches(u, q)) n++;
    if (!n) return FINDER_NO_UNIT;
    int pick = finderRandom(n);
    for (int u = first; u != FINDER_NO_UNIT; u = finderNext[u])
        if (finderMatches(u, q) && pick-- == 0) return u;
    return FINDER_NO_UNIT;
}

static FinderKept *finderKept(FinderKept *kept, int *count, int kind, int player, int plane, const char *name, int add) {
    for (int i = 0; i < *count && i < FINDER_KEPT_MAX; i++)
        if (kept[i].kind == kind && kept[i].player == player && kept[i].plane == plane && !strcmp(kept[i].name, name)) return &kept[i];
    if (!add) return NULL;
    FinderKept *k = &kept[*count % FINDER_KEPT_MAX];
    (*count)++;
    k->kind = kind, k->player = player, k->plane = plane, k->n = 0;
    snprintf(k->name, sizeof k->name, "%s", name);
    return k;
}

static int finderPickAnywhere(const EventFinderQuery *q) {
    FinderKept *k = finderKept(finderLists, &eventFinders.keptLists, q->kind, q->player, 0, q->name, 0);
    if (!k) {
        k = finderKept(finderLists, &eventFinders.keptLists, q->kind, q->player, 0, q->name, 1);
        int *units = finderListUnits[k - finderLists], last = G32(RVA_UNIT_COUNT);
        for (int u = 0; u <= last && k->n < FINDER_MATCHES_MAX; u++)
            if ((short)G16(RVA_UNIT_HP + (size_t)u * UNIT_SIZE) > 0 && finderUnitMatches(q->kind, u, q->player, q->name)) units[k->n++] = u;
        eventFinders.counts.walks++;
    }
    return k->n ? finderListUnits[k - finderLists][finderRandom(k->n)] : FINDER_NO_UNIT;
}

static int finderCountOnSquare(const EventFinderQuery *q, int *ok) {
    *ok = 1;
    G32(RVA_RULERSHIP_SLOT) = -1;
    if (q->x < 0 || q->y < 0 || q->x >= G32(RVA_MAP_DEPTH) || q->y >= G32(RVA_MAP_H)) return 0;
    if (!G16(RVA_SQUARE_ARMY + ((size_t)q->x * MAP_COLUMN_SQUARES + (size_t)q->y) * 2)) return 0;
    int first = finderSquareFirst(q->x, q->y, ok), n = 0;
    if (!*ok) return 0;
    for (int u = first; u != FINDER_NO_UNIT; u = finderNext[u]) {
        if (!finderCountMatches(u, q)) continue;
        if (!n++) G32(RVA_RULERSHIP_SLOT) = (short)G16(RVA_UNIT_PLAYER + (size_t)u * UNIT_SIZE);
    }
    finderTrace(finderCountTrace, q->player, q->name, n);
    return n;
}

static int answerEventFinder(const EventFinderQuery *q) {
    int ok = 1, answer = FINDER_NO_UNIT;
    if (eventFinders.lineDepth <= 0 || (q->name && strlen(q->name) >= FINDER_NAME_MAX)) {
        ok = 0;
    } else if (q->kind == EVENT_FINDER_COUNT) {
        FinderKept *k = finderKept(finderCounts, &eventFinders.keptCounts, q->kind, q->player, q->x, q->name, 0);
        if (!k) {
            int n = finderGame[q->kind](q);
            k = finderKept(finderCounts, &eventFinders.keptCounts, q->kind, q->player, q->x, q->name, 1);
            k->n = n;
            eventFinders.counts.walks++;
        }
        answer = k->n;
    } else if (q->kind == EVENT_FINDER_COUNT_AT) {
        if (q->x == -1 && q->y == -1) ok = 0;
        else answer = finderCountOnSquare(q, &ok);
    } else if (q->x == -1 && q->y == -1 && q->kind != EVENT_FINDER_AFFLICTED) {
        answer = finderPickAnywhere(q);
    } else if (q->x < 0) {
        ok = 0;
    } else if (!finderArmyHere(q->player, q->x, q->y)) {
        answer = FINDER_NO_UNIT;
    } else {
        int first = finderSquareFirst(q->x, q->y, &ok);
        if (ok) answer = finderPickOnSquare(first, q);
    }
    if (!ok) {
        eventFinders.counts.passed++;
        return finderGame[q->kind](q);
    }
    eventFinders.counts.answered++;
    return answer;
}

static int routeEventFinder(const EventFinderQuery *q) {
    EventFinderShadowFn shadow = finderShadow;
    return shadow ? shadow(q, finderGame[q->kind], answerEventFinder) : answerEventFinder(q);
}

static int findUnitAtHook(int player, int x, int y, const char *name) {
    EventFinderQuery q = {EVENT_FINDER_UNIT, player, x, y, name, -1, 0, 0};
    return routeEventFinder(&q);
}

static int findItemHolderAtHook(int player, int x, int y, const char *name) {
    EventFinderQuery q = {EVENT_FINDER_ITEM, player, x, y, name, -1, 0, 0};
    return routeEventFinder(&q);
}

static int findAfflictedUnitAtHook(int player, int x, int y, int monster, unsigned long long stateMask, unsigned long long resistanceMask) {
    EventFinderQuery q = {EVENT_FINDER_AFFLICTED, player, x, y, NULL, monster, stateMask, resistanceMask};
    return routeEventFinder(&q);
}

static int countUnitsAtHook(int player, int x, int y, const char *name) {
    EventFinderQuery q = {EVENT_FINDER_COUNT_AT, player, x, y, name, -1, 0, 0};
    return routeEventFinder(&q);
}

static int countUnitsHook(int player, int plane, const char *name) {
    EventFinderQuery q = {EVENT_FINDER_COUNT, player, plane, -1, name, -1, 0, 0};
    return routeEventFinder(&q);
}

static int gameFindUnitAt(const EventFinderQuery *q) {
    return GAME_FN(int (*)(int, int, int, const char *), RVA_FIND_UNIT_AT)(q->player, q->x, q->y, q->name);
}

static int gameFindItemHolderAt(const EventFinderQuery *q) {
    return GAME_FN(int (*)(int, int, int, const char *), RVA_FIND_ITEM_HOLDER_AT)(q->player, q->x, q->y, q->name);
}

static int gameFindAfflictedUnitAt(const EventFinderQuery *q) {
    return GAME_FN(int (*)(int, int, int, int, unsigned long long, unsigned long long), RVA_FIND_AFFLICTED_UNIT_AT)(
        q->player, q->x, q->y, q->monster, q->stateMask, q->resistanceMask);
}

static int gameCountUnitsAt(const EventFinderQuery *q) {
    return GAME_FN(int (*)(int, int, int, const char *), RVA_COUNT_UNITS_AT)(q->player, q->x, q->y, q->name);
}

static int gameCountUnits(const EventFinderQuery *q) {
    return GAME_FN(int (*)(int, int, const char *), RVA_COUNT_UNITS)(q->player, q->x, q->name);
}

__declspec(dllexport) const EventFinderCounts *coe5seEventFinderShadow(EventFinderShadowFn shadow) {
    if (!eventFinders.installed) return NULL;
    finderShadow = shadow;
    writeLog("event_finders_shadow", "on=%d", shadow != NULL);
    return &eventFinders.counts;
}

#ifdef __APPLE__
// @platform-split event_finder_army_test: Mac runs CoE5SE's copy of the army test its finders inline
static int armyHereForPlayer(int player, int x, int y) {
    if (x < 0 || y < 0 || x >= G32(RVA_MAP_DEPTH) || y >= G32(RVA_MAP_H)) return 0;
    size_t square = (size_t)x * MAP_COLUMN_SQUARES + (size_t)y;
    short army = (short)G16(RVA_SQUARE_ARMY + square * 2);
    if (!army) return 0;
    if (player < 0 || army < 0) return 1;
    int owner = *(const short *)(gameBase + RVA_SQUARE_RECORDS + square * SQUARE_SIZE + SQUARE_ARMY_OWNER);
    if (owner < 0 || owner >= playerSlotCount() || player >= playerSlotCount()) return 0;
    if (owner == player) return 1;
    short ownerTeam = (short)(*(short *)playerSlotAddress(RVA_SLOT_TEAM, owner, PLAYER_BLOCK_SIZE));
    short playerTeam = (short)(*(short *)playerSlotAddress(RVA_SLOT_TEAM, player, PLAYER_BLOCK_SIZE));
    return ownerTeam > 0 && playerTeam > 0 && ownerTeam == playerTeam;
}
#endif

typedef struct {
    unsigned int callRva, targetRva;
    void *hook;
    const char *name;
} EventFinderCall;

#ifndef __APPLE__
#define EVENT_FINDERS_STUB_SIZE 0x20
static int pointEventFinderCall(const EventFinderCall *c, int index) {
    static unsigned char *stubs;
    if (!stubs) {
        if (!(stubs = allocNear(gameBase + RVA_TEXT_SECTION))) {
            writeLog("hook_skipped", "site=%s reason=no_near_page", c->name);
            return 0;
        }
        makePagesReadExecute(stubs, 0x1000);
    }
    return redirectCall(c->callRva, c->targetRva, stubs + EVENT_FINDERS_STUB_SIZE * index, c->hook, c->name, NULL);
}
#else
static int pointEventFinderCall(const EventFinderCall *c, int index) {
    (void)index;
    if (blTarget(gameBase + c->callRva) != gameBase + c->targetRva) {
        writeLog("hook_skipped", "site=%s reason=not_the_expected_call va=0x%llx", c->name, c->callRva + 0x100000000ULL);
        return 0;
    }
    unsigned char *trampoline = newTrampoline(c->hook);
    uint32_t word = trampoline ? blTo(gameBase + c->callRva, trampoline) : 0;
    if (trampoline && writeGameCode(gameBase + c->callRva, &word, 4)) return 1;
    writeLog("hook_skipped", "site=%s reason=%s", c->name, trampoline ? "code_unwritable" : "no_padding");
    return 0;
}
#endif

static void eventFindersInit(void) {
    finderGame[EVENT_FINDER_UNIT] = gameFindUnitAt;
    finderGame[EVENT_FINDER_ITEM] = gameFindItemHolderAt;
    finderGame[EVENT_FINDER_AFFLICTED] = gameFindAfflictedUnitAt;
    finderGame[EVENT_FINDER_COUNT_AT] = gameCountUnitsAt;
    finderGame[EVENT_FINDER_COUNT] = gameCountUnits;
    finderRandom = GAME_FN(int (*)(int), RVA_RANDOM);
#ifdef __APPLE__
    finderArmyHere = armyHereForPlayer;
#else
    finderArmyHere = GAME_FN(int (*)(int, int, int), RVA_ARMY_AT_FOR_PLAYER);
#endif
    finderUnitState = GAME_FN(unsigned long long (*)(int), RVA_UNIT_STATE_FLAGS);
    finderUnitResistances = GAME_FN(unsigned long long (*)(int), RVA_UNIT_RESISTANCES);
    finderTrace = GAME_FN(FinderTraceFn, RVA_TRACE_C);
    finderCountTrace = (const char *)GAME_PTR(RVA_STR_COUNT_UNITS_AT_TRACE);
    static const EventFinderCall CALLS[] = {
        {RVA_EVENT_HASUNIT_CALL, RVA_FIND_UNIT_AT, (void *)findUnitAtHook, "event_hasunit"},
        {RVA_EVENT_HASUNITHERE_CALL, RVA_FIND_UNIT_AT, (void *)findUnitAtHook, "event_hasunithere"},
        {RVA_EVENT_HASITEM_CALL, RVA_FIND_ITEM_HOLDER_AT, (void *)findItemHolderAtHook, "event_hasitem"},
        {RVA_EVENT_HASITEMHERE_CALL, RVA_FIND_ITEM_HOLDER_AT, (void *)findItemHolderAtHook, "event_hasitemhere"},
        {RVA_EVENT_HASAFFUNITHERE_CALL, RVA_FIND_AFFLICTED_UNIT_AT, (void *)findAfflictedUnitAtHook, "event_hasaffunithere"},
        {RVA_EVENT_MINUNITSHERE_CALL, RVA_COUNT_UNITS_AT, (void *)countUnitsAtHook, "event_minunitshere"},
        {RVA_EVENT_MINUNITS_CALL, RVA_COUNT_UNITS, (void *)countUnitsHook, "event_minunits"},
    };
    enum { COUNT = sizeof CALLS / sizeof CALLS[0] };
    if (!gameBuild) {
        writeLog("event_finders_installed", "ok=0 of %d reason=unknown_game_build", COUNT);
        return;
    }
    if (!checkEventFinderCode()) {
        writeLog("event_finders_installed", "ok=0 of %d reason=game_code_changed", COUNT);
        return;
    }
    int ok = 0;
    for (int i = 0; i < COUNT; i++) ok += pointEventFinderCall(&CALLS[i], i);
    eventFinders.installed = ok > 0;
    writeLog("event_finders_installed", "ok=%d of %d", ok, COUNT);
}
