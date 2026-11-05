#pragma once

// @platform-split lua_game_calls: Windows calls the game's functions; Mac refuses each call (windows_only) until its address is looked up
#ifdef __APPLE__
#define WINDOWS_ONLY_CALL(rva) 0
#else
#define WINDOWS_ONLY_CALL(rva) (rva)
#endif

#define GAME_CALL_MAX_ARGS 8
#define GAME_CALL_TEXT_MAX 1000
typedef long long (*GameCallFn)(long long, long long, long long, long long, long long, long long, long long, long long);
typedef long long (*GameCallFloatFn)(long long, long long, long long, float, long long, long long);
#define GAME_CALL_LOWEST_TEXT 0x10000
#define GAME_CALL_NAME_MAX 256

static GameCallFn gameCallStandIn;

static int gameCallPlaneOk(lua_Integer plane) { return plane >= 0 && plane <= 10 && plane <= G32(RVA_LAST_PLANE); }

static const char *gameCallArgProblem(char letter, lua_Integer v) {
    switch (letter) {
    case 'p': return v < 0 || v >= playerSlotCount() ? "bad_player" : NULL;
    case 'P': return v < -1 || v >= playerSlotCount() ? "bad_player" : NULL;
    case 'u': return v < 0 || v > G32(RVA_UNIT_COUNT) || v >= UNIT_TABLE_MAX || G16(RVA_UNIT_HP + (size_t)v * UNIT_SIZE) <= 0 ? "dead_unit" : NULL;
    case 'c': return v < 0 || v >= COMMANDER_TABLE_MAX || livingCommanderUnit((int)v) < 0 ? "dead_commander" : NULL;
    case 'm': return !isUsedMonster(v) ? "bad_monster" : NULL;
    case 'x': return v < 0 || v >= G32(RVA_MAP_DEPTH) || v >= MAP_COLUMNS_MAX ? "off_map" : NULL;
    case 'y': return v < 0 || v >= G32(RVA_MAP_H) || v >= MAP_COLUMN_SQUARES ? "off_map" : NULL;
    case 'l': return !gameCallPlaneOk(v) ? "bad_plane" : NULL;
    case 'E': return v != -1 && !gameCallPlaneOk(v) ? "bad_plane" : NULL;
    case 'L': return v != -1 && v != -2 && !gameCallPlaneOk(v) ? "bad_plane" : NULL;
    case 'T': return v < 0 || v >= tableRecords(TABLE_ITEM) || !*(const char *)(tableAddress(TABLE_ITEM) + (size_t)v * ITEM_SIZE) ? "bad_item" : NULL;
    case 'w': return v < 0 || v >= tableRecords(TABLE_WEAPON) ? "bad_weapon" : NULL;
    case 'r': return v < 1 || v >= ritualSlotCount() ? "bad_ritual" : NULL;
    case 'S': return !battleSpellInTable(v) ? "bad_spell" : NULL;
    case 'R': return v < 0 || v > 18 ? "bad_resource" : NULL;
    case 'C': return v < 0 || v >= tableRecords(TABLE_CLASS) ? "bad_class" : NULL;
    case 'N': return v < 0 || v >= AUTO_NAME_TYPES ? "bad_name_list" : NULL;
    case 'U': return NULL;
    case 'X': return v < 0 || v >= G32(RVA_BATTLE_WIDTH) ? "off_field" : NULL;
    case 'Y': return v < 0 || v >= BATTLE_ROWS ? "off_field" : NULL;
    case 't': return v < 0 || v >= TERRAIN_TABLE_MAX ? "bad_terrain" : NULL;
    case 'a': return v < 0 || v >= RITUAL_EFFECT_ID_COUNT ? "bad_ability" : NULL;
    default: return v < INT_MIN || v > INT_MAX ? "out_of_range" : NULL;
    }
}

static int runGameCall(lua_State *L, const char *call, uintptr_t rva, const char *args, char result) {
    long long a[GAME_CALL_MAX_ARGS] = {0};
    int written[GAME_CALL_MAX_ARGS], writtenCount = 0, next = 1, hasFloat = 0, keepRandom = 0;
    float number = 0;
    char buffer[GAME_CALL_NAME_MAX] = {0};
    GameCallFn fn = rva ? (GameCallFn)(void *)(gameBase + rva) : gameCallStandIn;
    const char *reason = !fn ? "windows_only" : G32(RVA_ACTIVE_PLAYER) < 0 ? "no_game" : NULL;
    lua_Integer bad = 0;
    int badArg = 0;
    int i = 0;
    for (const char *at = args; *at && !reason; at++) {
        char letter = *at;
        if (letter == '~') { keepRandom = 1; continue; }
        if (letter == '!') {
            if (!*(const unsigned char *)GAME_PTR(RVA_IN_BATTLE)) reason = "no_battle";
            continue;
        }
        if (letter == '-') { a[i++] = -1; continue; }
        if (letter == '0') { a[i++] = 0; continue; }
        if (letter == '%') { a[i++] = (long long)(intptr_t)"%s"; continue; }
        if (letter == 'Z') { a[i++] = (long long)(intptr_t)buffer; continue; }
        if (letter == 'f') { number = (float)luaL_checknumber(L, next++); hasFloat = 1; i++; continue; }
        if (letter == 'O') { written[writtenCount] = -1; a[i++] = (long long)(intptr_t)&written[writtenCount++]; continue; }
        if (letter == 's' || letter == 'n') {
            const char *text = luaL_checkstring(L, next);
            badArg = next++;
            if (strlen(text) >= GAME_CALL_TEXT_MAX) reason = "too_long";
            else if (letter == 'n' && monsterByName(text) < 0) reason = "bad_monster";
            a[i++] = (long long)(intptr_t)text;
            continue;
        }
        lua_Integer v;
        if (letter == 'a' && lua_type(L, next) == LUA_TSTRING && !lua_isnumber(L, next)) {
            v = abilityIdOfWord(lua_tostring(L, next));
            if (v < 0) return luaL_error(L, "%s: unknown ability word '%s'", call, lua_tostring(L, next));
        }
        else v = luaL_checkinteger(L, next);
        bad = v, badArg = next++;
        reason = gameCallArgProblem(letter == '&' ? 'i' : letter, v);
        if (letter == '&') {
            written[writtenCount] = (int)v;
            a[i++] = (long long)(intptr_t)&written[writtenCount++];
        } else {
            a[i++] = v;
        }
    }
    if (reason) {
        refuseLuaCall(runningLuaMod(), call, reason, "arg=%d value=%lld", badArg, (long long)bad);
        return 0;
    }
    int randomIndex = G32(RVA_RANDOM_INDEX), randomShift = G32(RVA_RANDOM_SHIFT), randomXor = G32(RVA_RANDOM_XOR);
    long long r = hasFloat ? ((GameCallFloatFn)(void *)fn)(a[0], a[1], a[2], number, a[4], a[5]) : fn(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
    if (keepRandom) G32(RVA_RANDOM_INDEX) = randomIndex, G32(RVA_RANDOM_SHIFT) = randomShift, G32(RVA_RANDOM_XOR) = randomXor;
    int pushed = 1;
    switch (result) {
    case 'k': lua_pushinteger(L, (unsigned char)r); break;
    case 'b': lua_pushboolean(L, (int)r > 0); break;
    case 'B': lua_pushboolean(L, (unsigned char)r != 0); break;
    case 'v': lua_pushboolean(L, 1); break;
    case 'z':
        if (buffer[0]) lua_pushstring(L, buffer);
        else if (!strchr(args, 'Z') && (uintptr_t)r >= GAME_CALL_LOWEST_TEXT) lua_pushstring(L, (const char *)(intptr_t)r);
        else lua_pushnil(L);
        break;
    case 'o':
        if (writtenCount && written[0] == -1) return 0;
        pushed = 0;
        break;
    case 'w': pushed = 0; break;
    default: lua_pushinteger(L, (int)r); break;
    }
    for (int i = 0; i < writtenCount; i++) lua_pushinteger(L, written[i]);
    return pushed + writtenCount;
}

static int lChooseLocation(lua_State *L) {
    const char *title = luaL_checkstring(L, 1);
    lua_Integer player = luaL_checkinteger(L, 2), x = luaL_checkinteger(L, 3), y = luaL_checkinteger(L, 4), multiplane = luaL_checkinteger(L, 5);
    lua_Integer range = luaL_optinteger(L, 6, 9999);
    uintptr_t rva = WINDOWS_ONLY_CALL(RVA_PLANE_SCREEN);
    const char *reason = !rva ? "windows_only" : G32(RVA_ACTIVE_PLAYER) < 0 ? "no_game" : strlen(title) >= GAME_CALL_TEXT_MAX ? "too_long"
                         : gameCallArgProblem('p', player) ? gameCallArgProblem('p', player) : gameCallArgProblem('x', x) ? gameCallArgProblem('x', x)
                         : gameCallArgProblem('y', y) ? gameCallArgProblem('y', y) : gameCallArgProblem('i', multiplane) ? gameCallArgProblem('i', multiplane)
                         : range < 0 || range > 9999 ? "out_of_range" : NULL;
    if (reason) {
        refuseLuaCall(runningLuaMod(), "coe5.chooseLocation", reason, "player=%lld x=%lld y=%lld", (long long)player, (long long)x, (long long)y);
        return 0;
    }
    int picked[2] = {(int)x, (int)y};
    int randomIndex = G32(RVA_RANDOM_INDEX), randomShift = G32(RVA_RANDOM_SHIFT), randomXor = G32(RVA_RANDOM_XOR);
    long long r = ((long long (*)(const char *, int, int *, int *, int, int, int, int, int))(void *)(gameBase + rva))(
        title, (int)player, &picked[0], &picked[1], (int)multiplane, (int)x, (int)y, (int)range, 1);
    G32(RVA_RANDOM_INDEX) = randomIndex, G32(RVA_RANDOM_SHIFT) = randomShift, G32(RVA_RANDOM_XOR) = randomXor;
    if ((int)r <= 0) return 0;
    lua_pushinteger(L, picked[0]);
    lua_pushinteger(L, picked[1]);
    return 2;
}

// ---- The rows: lua_game_call_rows.h ------------------------------------------------------------------
#include "lua_game_call_rows.h"

#define GAME_CALL_FN(name, rva, args, result) \
    static int lGameCall_##name(lua_State *L) { return runGameCall(L, "coe5." #name, rva, args, result); }
LUA_GAME_CALLS(GAME_CALL_FN)
#undef GAME_CALL_FN
#define GAME_CALL_REG(name, rva, args, result) {#name, lGameCall_##name},
