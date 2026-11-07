#pragma once

#define SQUARE_SEEN_BEFORE 1
#define SQUARE_SEEN_IN_VIEW 2

typedef enum {
    READ_FREE, READ_HIDDEN, READ_UNIT, READ_COMMANDER, READ_OWN_UNIT, READ_OWN_COMMANDER, READ_VIEWER, READ_SQUARE_SHOWN,
    READ_SQUARE_TERRAIN, READ_SQUARE_KNOWN, READ_SQUARE_VIEW, READ_SQUARE_ARMY, READ_SQUARE_ARMY_COUNT, READ_SQUARE_ARMY_OWNER,
    READ_SQUARE_HAS_SEEN, READ_UNITS_OF, READ_SLOT_HOME, READ_ENCHANT, READ_UNIT_LIST, READ_SQUARE_LIST, READ_ARMIES
} ReadRule;
typedef enum { UNSEEN_NIL, UNSEEN_ZERO, UNSEEN_MINUS_ONE, UNSEEN_FALSE, UNSEEN_EMPTY_LIST } UnseenForm;
typedef struct {
    const char *name;
    ReadRule rule;
    int at;
    ReadRule rule2;
    int at2;
    UnseenForm unseen;
} ReadRow;

#include "read_filter_rows.h"

static const ReadRow *findReadRow(const ReadRow *rows, size_t count, const char *name) {
    size_t low = 0, high = count;
    while (low < high) {
        size_t middle = (low + high) / 2;
        int order = strcmp(name, rows[middle].name);
        if (!order) return &rows[middle];
        if (order < 0) high = middle;
        else low = middle + 1;
    }
    return NULL;
}

// ---- The game's answers -------------------------------------------------------------------------
static int (*readUnitVisibleStandIn)(int, int, int, int);
static int (*readIllusionStandIn)(int, int, int, int *);

static int readUnitVisible(int player, int unit, int x, int y) {
    if (readUnitVisibleStandIn) return readUnitVisibleStandIn(player, unit, x, y);
    return GAME_FN(int (*)(int, int, int, int), RVA_UNIT_VISIBLE_TO)(player, unit, x, y) > 0;
}

static int readIllusion(int player, int x, int y, int *shown) {
    if (!(G32(RVA_SQUARE_FLAGS + ((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE) & SQUARE_FLAG_ILLUSION)) return 0;
    if (readIllusionStandIn) return readIllusionStandIn(player, x, y, shown);
    return GAME_FN(int (*)(int, int, int, int *), RVA_ILLUSION_TERRAIN)(player, x, y, shown) != 0;
}

static int readSquareInArrays(lua_Integer x, lua_Integer y) { return x >= 0 && y >= 0 && x < MAP_COLUMNS_MAX && y < MAP_COLUMN_SQUARES; }

static signed char readSquareSeen(int player, int x, int y) {
    return (signed char)G8(RVA_SQUARE_RECORDS + ((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE + SQUARE_SEEN + (size_t)player);
}

static int readViewer(void) {
    int active = G32(RVA_ACTIVE_PLAYER);
    if (active >= 0 && active < PLAYER_SLOTS && PG16(RVA_SLOT_CONTROLLER + 2 * (size_t)active) == SLOT_CONTROLLER_MINE) return active;
    for (int slot = 0; slot < PLAYER_SLOTS; slot++)
        if (PG16(RVA_SLOT_CONTROLLER + 2 * (size_t)slot) == SLOT_CONTROLLER_MINE) return slot;
    return -1;
}

static int readFightShows(int x, int y) {
    if (!*(const unsigned char *)GAME_PTR(RVA_IN_BATTLE) || !*(const unsigned char *)GAME_PTR(RVA_BATTLE_IS_REPLAY)) return 0;
    return x == G32(RVA_BATTLE_X) && y == G32(RVA_BATTLE_Y);
}

static int readUnitSeen(int viewer, int unit) {
    size_t at = (size_t)unit * UNIT_SIZE;
    if (viewer < 0) return 0;
    if ((short)G16(RVA_UNIT_PLAYER + at) == viewer) return 1;
    if (!unitAliveInTable(unit)) return 0;
    int x = G32(RVA_UNIT_X + at), y = G32(RVA_UNIT_X + 4 + at);
    if (!readSquareInArrays(x, y)) return 0;
    if (readFightShows(x, y)) return 1;
    if (readSquareSeen(viewer, x, y) < SQUARE_SEEN_IN_VIEW || readIllusion(viewer, x, y, NULL)) return 0;
    return readUnitVisible(viewer, unit, x, y);
}

// ---- Armies: one walk of the unit table a frame ---------------------------------------------------
typedef struct { unsigned short all, seen; short firstSeenOwner; } ReadSquareUnits;
static struct {
    ReadSquareUnits *squares;
    size_t count;
    int frame, viewer, built;
} readArmies;

static int readArmiesBuild(int viewer) {
    int frame = G32(RVA_FRAME_COUNT);
    if (readArmies.built && readArmies.frame == frame && readArmies.viewer == viewer) return 1;
    size_t squares = (size_t)MAP_COLUMNS_MAX * MAP_COLUMN_SQUARES;
    if (!readArmies.squares) {
        readArmies.squares = malloc(squares * sizeof *readArmies.squares);
        if (!readArmies.squares) {
            writeLogError("read_filter_out_of_memory", "reason=out_of_memory");
            return 0;
        }
        readArmies.count = squares;
    }
    memset(readArmies.squares, 0, squares * sizeof *readArmies.squares);
    int last = G32(RVA_UNIT_COUNT);
    for (int u = 0; u <= last && u < UNIT_TABLE_MAX; u++) {
        size_t at = (size_t)u * UNIT_SIZE;
        if ((short)G16(RVA_UNIT_HP + at) <= 0) continue;
        int x = G32(RVA_UNIT_X + at), y = G32(RVA_UNIT_X + 4 + at);
        if (!readSquareInArrays(x, y)) continue;
        ReadSquareUnits *square = &readArmies.squares[(size_t)x * MAP_COLUMN_SQUARES + (size_t)y];
        square->all++;
        if (!readUnitSeen(viewer, u)) continue;
        if (!square->seen) square->firstSeenOwner = (short)G16(RVA_UNIT_PLAYER + at);
        square->seen++;
    }
    readArmies.frame = frame, readArmies.viewer = viewer, readArmies.built = 1;
    return 1;
}

static const ReadSquareUnits *readSquareUnits(int viewer, int x, int y) {
    return readArmiesBuild(viewer) ? &readArmies.squares[(size_t)x * MAP_COLUMN_SQUARES + (size_t)y] : NULL;
}

// ---- The rules ----------------------------------------------------------------------------------
typedef enum { READ_PASSES, READ_UNSEEN, READ_ANSWERED } ReadVerdict;

static int readArgument(lua_State *L, int at, lua_Integer *value) {
    int isNumber;
    *value = lua_tointegerx(L, at, &isNumber);
    return isNumber;
}

static int readPlayerArgument(lua_State *L, int at, lua_Integer *value) {
    if (lua_type(L, at) == LUA_TSTRING && !lua_isnumber(L, at)) {
        *value = playerOfFactionName(lua_tostring(L, at));
        return 1;
    }
    return readArgument(L, at, value);
}

static ReadVerdict readCheck(lua_State *L, ReadRule rule, int at, int viewer) {
    lua_Integer a, b;
    int shown = 0;
    switch (rule) {
    case READ_FREE: case READ_ARMIES: case READ_UNIT_LIST: return READ_PASSES;
    case READ_HIDDEN: return READ_UNSEEN;
    case READ_VIEWER:
        if (!readPlayerArgument(L, at, &a)) return READ_PASSES;
        return viewer >= 0 && a == viewer ? READ_PASSES : READ_UNSEEN;
    case READ_UNIT: case READ_OWN_UNIT:
        if (!readArgument(L, at, &a) || a < 0 || a >= UNIT_TABLE_MAX) return READ_PASSES;
        if (rule == READ_OWN_UNIT) return viewer >= 0 && (short)G16(RVA_UNIT_PLAYER + (size_t)a * UNIT_SIZE) == viewer ? READ_PASSES : READ_UNSEEN;
        return readUnitSeen(viewer, (int)a) ? READ_PASSES : READ_UNSEEN;
    case READ_COMMANDER: case READ_OWN_COMMANDER: {
        if (!readArgument(L, at, &a) || a < 0 || a >= COMMANDER_TABLE_MAX) return READ_PASSES;
        int unit = G32(RVA_COMMANDERS + (size_t)a * COMMANDER_SIZE);
        if (unit < 0 || unit >= UNIT_TABLE_MAX) return READ_UNSEEN;
        if (rule == READ_OWN_COMMANDER) return viewer >= 0 && (short)G16(RVA_UNIT_PLAYER + (size_t)unit * UNIT_SIZE) == viewer ? READ_PASSES : READ_UNSEEN;
        return readUnitSeen(viewer, unit) ? READ_PASSES : READ_UNSEEN;
    }
    case READ_SLOT_HOME: {
        if (!readPlayerArgument(L, at, &a) || a < 0 || a >= PLAYER_SLOTS) return READ_PASSES;
        if (a == viewer) return READ_PASSES;
        int x = PG16(RVA_PLAYER_HOME_X + 2 * (size_t)a), y = PG16(RVA_PLAYER_HOME_Y + 2 * (size_t)a);
        if (viewer < 0 || !readSquareInArrays(x, y)) return READ_UNSEEN;
        return readSquareSeen(viewer, x, y) >= SQUARE_SEEN_BEFORE && !readIllusion(viewer, x, y, NULL) ? READ_PASSES : READ_UNSEEN;
    }
    case READ_ENCHANT: {
        if (!readArgument(L, at, &a) || a < 0 || a >= ENCHANTS_MAX) return READ_PASSES;
        size_t record = RVA_ENCHANTS + (size_t)a * ENCHANT_SIZE;
        if (viewer >= 0 && G32(record + ENCHANT_OWNER) == viewer) return READ_PASSES;
        int x = G32(record + ENCHANT_X), y = G32(record + ENCHANT_Y);
        if (viewer < 0 || !readSquareInArrays(x, y)) return READ_UNSEEN;
        return readSquareSeen(viewer, x, y) >= SQUARE_SEEN_IN_VIEW && !readIllusion(viewer, x, y, NULL) ? READ_PASSES : READ_UNSEEN;
    }
    case READ_UNITS_OF:
        if (!readPlayerArgument(L, at, &a)) return READ_PASSES;
        if (viewer >= 0 && a == viewer) return READ_PASSES;
        at++;
        break;
    default:
        break;
    }
    if (!readArgument(L, at, &a) || !readArgument(L, at + 1, &b) || !readSquareInArrays(a, b)) return READ_PASSES;
    int x = (int)a, y = (int)b;
    if (viewer < 0) return READ_UNSEEN;
    signed char seen = readSquareSeen(viewer, x, y);
    if (seen < SQUARE_SEEN_BEFORE) return READ_UNSEEN;
    int illusion = readIllusion(viewer, x, y, &shown);
    if (rule == READ_SQUARE_SHOWN) return READ_PASSES;
    if (rule == READ_SQUARE_TERRAIN) {
        if (!illusion) return READ_PASSES;
        lua_pushinteger(L, shown);
        return READ_ANSWERED;
    }
    if (illusion) return READ_UNSEEN;
    if (rule == READ_SQUARE_KNOWN || rule == READ_SQUARE_LIST) return READ_PASSES;
    if (seen < SQUARE_SEEN_IN_VIEW) return READ_UNSEEN;
    if (rule == READ_SQUARE_VIEW) return READ_PASSES;
    const ReadSquareUnits *units = readSquareUnits(viewer, x, y);
    if (!units) return READ_UNSEEN;
    switch (rule) {
    case READ_SQUARE_HAS_SEEN: return units->seen ? READ_PASSES : READ_UNSEEN;
    case READ_SQUARE_ARMY_COUNT:
        if (units->seen == units->all) return READ_PASSES;
        lua_pushinteger(L, units->seen);
        return READ_ANSWERED;
    case READ_SQUARE_ARMY_OWNER:
        if (units->seen == units->all) return READ_PASSES;
        if (!units->seen) return READ_UNSEEN;
        lua_pushinteger(L, units->firstSeenOwner);
        return READ_ANSWERED;
    default:
        return units->seen == units->all ? READ_PASSES : READ_UNSEEN;
    }
}

static int pushReadUnseen(lua_State *L, UnseenForm form) {
    switch (form) {
    case UNSEEN_ZERO: lua_pushinteger(L, 0); break;
    case UNSEEN_MINUS_ONE: lua_pushinteger(L, -1); break;
    case UNSEEN_FALSE: lua_pushboolean(L, 0); break;
    case UNSEEN_EMPTY_LIST: lua_createtable(L, 0, 0); break;
    default: lua_pushnil(L); break;
    }
    return 1;
}

static void cutReadList(lua_State *L, ReadRule rule, int viewer) {
    lua_Integer count = (lua_Integer)lua_rawlen(L, -1), kept = 0;
    for (lua_Integer i = 1; i <= count; i++) {
        lua_rawgeti(L, -1, i);
        int keep = 0;
        if (rule == READ_UNIT_LIST) {
            lua_Integer unit = lua_tointeger(L, -1);
            keep = viewer >= 0 && unit >= 0 && unit < UNIT_TABLE_MAX && readUnitSeen(viewer, (int)unit);
        } else {
            lua_getfield(L, -1, "x");
            lua_getfield(L, -2, "y");
            lua_Integer x = lua_tointeger(L, -2), y = lua_tointeger(L, -1);
            lua_pop(L, 2);
            keep = viewer >= 0 && readSquareInArrays(x, y) && readSquareSeen(viewer, (int)x, (int)y) >= SQUARE_SEEN_BEFORE && !readIllusion(viewer, (int)x, (int)y, NULL);
        }
        if (keep) lua_rawseti(L, -2, ++kept);
        else lua_pop(L, 1);
    }
    for (lua_Integer i = kept + 1; i <= count; i++) {
        lua_pushnil(L);
        lua_rawseti(L, -2, i);
    }
}

static int lArmiesSeen(lua_State *L, int viewer) {
    lua_Integer asked = viewer;
    int askedOk = lua_isnoneornil(L, 1) || readPlayerArgument(L, 1, &asked);
    lua_createtable(L, 0, 0);
    if (viewer < 0 || !askedOk || asked != viewer) return 1;
    int depth = G32(RVA_MAP_DEPTH), height = G32(RVA_MAP_H);
    if (depth > MAP_COLUMNS_MAX) depth = MAP_COLUMNS_MAX;
    if (height > MAP_COLUMN_SQUARES) height = MAP_COLUMN_SQUARES;
    if (!readArmiesBuild(viewer)) return 1;
    int *entry = lua_newuserdatauv(L, (size_t)(depth > 0 ? depth : 0) * (size_t)(height > 0 ? height : 0) * sizeof(int) + sizeof(int), 0);
    lua_insert(L, -2);
    int n = 0;
    for (int x = 0; x < depth; x++)
        for (int y = 0; y < height; y++) {
            entry[(size_t)x * (size_t)height + (size_t)y] = 0;
            size_t square = (size_t)x * MAP_COLUMN_SQUARES + (size_t)y;
            const ReadSquareUnits *units = &readArmies.squares[square];
            if (!units->seen) continue;
            int whole = units->seen == units->all;
            lua_createtable(L, 0, 5);
            setIntField(L, "x", x);
            setIntField(L, "y", y);
            setIntField(L, "owner", whole ? armyOwnerAt(x, y) : units->firstSeenOwner);
            setIntField(L, "units", whole ? (short)G16(RVA_SQUARE_ARMY + square * 2) : units->seen);
            lua_createtable(L, 0, 0);
            lua_setfield(L, -2, "commanders");
            lua_rawseti(L, -2, ++n);
            entry[(size_t)x * (size_t)height + (size_t)y] = n;
        }
    int last = G32(RVA_LAST_COMMANDER);
    for (int c = 0; c <= last && c < COMMANDER_TABLE_MAX; c++) {
        int unit = livingCommanderUnit(c);
        if (unit < 0 || !readUnitSeen(viewer, unit)) continue;
        int x = G32(RVA_UNIT_X + (size_t)unit * UNIT_SIZE), y = G32(RVA_UNIT_X + (size_t)unit * UNIT_SIZE + 4);
        if (x < 0 || y < 0 || x >= depth || y >= height) continue;
        int at = entry[(size_t)x * (size_t)height + (size_t)y];
        if (!at) continue;
        lua_rawgeti(L, -1, at);
        lua_getfield(L, -1, "commanders");
        lua_pushinteger(L, c);
        lua_rawseti(L, -2, (lua_Integer)lua_rawlen(L, -2) + 1);
        lua_pop(L, 2);
    }
    lua_remove(L, -2);
    return 1;
}

// ---- The gate ---------------------------------------------------------------------------------------
static int runFilteredRead(lua_State *L, const luaL_Reg *fn, const char *name) {
    if (netClientGameOverSeen) return fn->func(L);
    int get = fn->func == lGet;
    if (get && lua_type(L, 1) != LUA_TSTRING) return fn->func(L);
    const ReadRow *row = get ? findReadRow(READ_GET_ROWS, sizeof READ_GET_ROWS / sizeof READ_GET_ROWS[0], lua_tostring(L, 1))
                             : findReadRow(READ_CALL_ROWS, sizeof READ_CALL_ROWS / sizeof READ_CALL_ROWS[0], name);
    if (!row) return get && !findGameVar(lua_tostring(L, 1)) ? fn->func(L) : pushReadUnseen(L, UNSEEN_NIL);
    if (row->rule == READ_FREE) return fn->func(L);
    int viewer = readViewer();
    if (row->rule == READ_ARMIES) return lArmiesSeen(L, viewer);
    int top = lua_gettop(L);
    ReadVerdict verdict = readCheck(L, row->rule, row->at, viewer);
    if (verdict == READ_PASSES && row->rule2 != READ_FREE) verdict = readCheck(L, row->rule2, row->at2, viewer);
    if (verdict == READ_UNSEEN) return pushReadUnseen(L, row->unseen);
    if (verdict == READ_ANSWERED) return lua_gettop(L) - top;
    int results = fn->func(L);
    if (results == 1 && (row->rule == READ_UNIT_LIST || row->rule == READ_SQUARE_LIST) && lua_istable(L, -1)) cutReadList(L, row->rule, viewer);
    return results;
}
