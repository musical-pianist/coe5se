#pragma once

// @platform-split lua_game_calls: Windows calls the game's functions; Mac calls the BOTH_BUILDS_CALL rows (a hooked function through its hook, PER_BUILD letters where the functions differ, a CoE5SE copy where Mac's takes other arguments), runs CoE5SE's copy for the MAC_COPY_CALL rows (lua_game_copies.h), and refuses the rest (windows_only)
#ifdef __APPLE__
#define WINDOWS_ONLY_CALL(rva) 0
#else
#define WINDOWS_ONLY_CALL(rva) (rva)
#endif
#define BOTH_BUILDS_CALL(rva) (rva)
#ifdef __APPLE__
#define GAME_CALL_COPY ((uintptr_t)1 << 63)
#define MAC_COPY_CALL(rva, copy) ((uintptr_t)(void *)(copy) | GAME_CALL_COPY)
#else
#define MAC_COPY_CALL(rva, copy) (rva)
#endif
#ifdef __APPLE__
#define PER_BUILD(win, mac) mac
#else
#define PER_BUILD(win, mac) win
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

#ifdef __APPLE__
static long long macUnitMayBeOnLand(long long unit, long long b, long long c, long long d, long long e, long long f, long long g, long long h) {
    (void)b, (void)c, (void)d, (void)e, (void)f, (void)g, (void)h;
    if (!(GAME_FN(unsigned long long (*)(int), RVA_UNIT_FLAGS)((int)unit) & UNIT_FLAG_MT_NOLAND)) return 1;
    return GAME_FN(int (*)(int, int, int, char *, int, int *), RVA_UNIT_ABILITY_TOTAL)((int)unit, ABILITY_AIRBREATHER, 1, NULL, 0, NULL) > 0;
}

static long long macNearTerrainSquare(long long plane, long long code, long long x, long long y, long long outX, long long outY, long long g, long long h) {
    (void)g, (void)h;
    GAME_FN(void (*)(int, int, int, int, int *, int *), RVA_NEAR_TERRAIN_SQUARE)((int)plane, (int)code, (int)x, (int)y, (int *)(intptr_t)outX, (int *)(intptr_t)outY);
    return *(int *)(intptr_t)outX != -1;
}

static GameCallFn macGameCallCopy(uintptr_t rva) {
    return rva == RVA_UNIT_MAY_BE_ON_LAND ? macUnitMayBeOnLand : rva == RVA_NEAR_TERRAIN_SQUARE ? macNearTerrainSquare : NULL;
}
#endif

#ifdef COE5SE_COPY_SELFCHECK
static void selfcheckGameCopy(const char *call);
#endif
static int runGameCall(lua_State *L, const char *call, uintptr_t rva, const char *args, char result) {
    long long a[GAME_CALL_MAX_ARGS] = {0};
    int written[GAME_CALL_MAX_ARGS], writtenCount = 0, next = 1, hasFloat = 0, keepRandom = 0;
    float number = 0;
#ifdef __APPLE__
    int variadicFrom = -1;
#endif
    char buffer[GAME_CALL_NAME_MAX] = {0};
    GameCallFn fn = gameCallStandIn ? gameCallStandIn : rva ? (GameCallFn)(void *)(gameBase + rva) : NULL;
#ifdef __APPLE__
    if (rva & GAME_CALL_COPY) fn = gameCallStandIn ? gameCallStandIn : (GameCallFn)(void *)(rva & ~GAME_CALL_COPY);
    else if (!gameCallStandIn && rva && macRedirectedHook((unsigned int)rva)) fn = (GameCallFn)macRedirectedHook((unsigned int)rva);
    if (!(rva & GAME_CALL_COPY) && !gameCallStandIn && rva && macGameCallCopy(rva)) fn = macGameCallCopy(rva);
#endif
    const char *reason = !fn ? "windows_only" : G32(RVA_ACTIVE_PLAYER) < 0 && !strchr(args, '@') ? "no_game" : NULL;
    lua_Integer bad = 0;
    int badArg = 0;
    int i = 0;
    for (const char *at = args; *at && !reason; at++) {
        char letter = *at;
        if (letter == '~') { keepRandom = 1; continue; }
        if (letter == '@') continue;
#ifdef __APPLE__
        if (letter == '.') { variadicFrom = i; continue; }
        if (letter == '^') { a[i++] = G32(RVA_SELECTED_COMMANDER); continue; }
#endif
        if (letter == '!') {
            if (!*(const unsigned char *)GAME_PTR(RVA_IN_BATTLE) && !testField.open) reason = "no_battle";
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
#ifdef COE5SE_COPY_SELFCHECK
    selfcheckGameCopy(call);
#endif
    int randomIndex = G32(RVA_RANDOM_INDEX), randomShift = G32(RVA_RANDOM_SHIFT), randomXor = G32(RVA_RANDOM_XOR);
#ifdef __APPLE__
    long long r = variadicFrom == 1   ? ((long long (*)(long long, ...))(void *)fn)(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7])
                  : variadicFrom == 2 ? ((long long (*)(long long, long long, ...))(void *)fn)(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7])
                  : hasFloat          ? ((GameCallFloatFn)(void *)fn)(a[0], a[1], a[2], number, a[4], a[5])
                                      : fn(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
#else
    long long r = hasFloat ? ((GameCallFloatFn)(void *)fn)(a[0], a[1], a[2], number, a[4], a[5]) : fn(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
#endif
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
#include "lua_game_copies.h"
#include "lua_game_call_rows.h"

#define GAME_CALL_FN(name, rva, args, result) \
    static int lGameCall_##name(lua_State *L) { return runGameCall(L, "coe5." #name, rva, args, result); }
LUA_GAME_CALLS(GAME_CALL_FN)
#undef GAME_CALL_FN
#define GAME_CALL_REG(name, rva, args, result) {#name, lGameCall_##name},

#ifdef COE5SE_COPY_SELFCHECK
// ---- The Windows self-check of Mac's copies -------------------------------------------------------
#undef MAC_COPY_CALL
#define MAC_COPY_CALL(rva, copy) (rva), (GameCallFn)(void *)(copy)
typedef struct {
    const char *name;
    struct { uintptr_t rva; GameCallFn copy; } target;
    const char *args;
    char result;
} CopyCheckRow;
#define COPY_CHECK_ROW(name, target, args, result) {"coe5." #name, {target}, args, result},
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
static const CopyCheckRow COPY_CHECK_ROWS[] = {LUA_GAME_CALLS(COPY_CHECK_ROW)};
#pragma clang diagnostic pop
#undef COPY_CHECK_ROW
#undef MAC_COPY_CALL
#define MAC_COPY_CALL(rva, copy) (rva)
#define COPY_CHECK_ROW_COUNT ((int)(sizeof COPY_CHECK_ROWS / sizeof COPY_CHECK_ROWS[0]))
#define COPY_CHECK_MAX 50000
#define COPY_CHECK_TEXT 300
#define COPY_CHECK_NAMES 4000
static unsigned char copyCheckDone[COPY_CHECK_ROW_COUNT];
static long long copyCheckValues[GAME_CALL_MAX_ARGS][UNIT_TABLE_MAX];
static char copyCheckNames[COPY_CHECK_NAMES][COPY_CHECK_TEXT];

static int copyCheckLetterValues(char letter, long long *out) {
    int n = 0;
    switch (letter) {
    case 'u':
        for (int u = 0; u <= G32(RVA_UNIT_COUNT) && u < UNIT_TABLE_MAX; u++)
            if (!gameCallArgProblem('u', u)) out[n++] = u;
        return n;
    case 'c':
        for (int c = 0; c <= G32(RVA_LAST_COMMANDER) && c < COMMANDER_TABLE_MAX; c++)
            if (!gameCallArgProblem('c', c)) out[n++] = c;
        return n;
    case 'P': out[n++] = -1;
    case 'p': for (int p = 0; p < playerSlotCount(); p++) out[n++] = p; return n;
    case 'x': for (int x = 0; !gameCallArgProblem('x', x); x++) out[n++] = x; return n;
    case 'y': for (int y = 0; !gameCallArgProblem('y', y); y++) out[n++] = y; return n;
    case 'l': for (int l = 0; !gameCallArgProblem('l', l); l++) out[n++] = l; return n;
    case 'a': for (int a = 0; a < RITUAL_EFFECT_ID_COUNT; a++) out[n++] = a; return n;
    case 'r': for (int r = 1; r <= G32(RVA_RITUAL_COUNT) && r < ritualSlotCount(); r++) out[n++] = r; return n;
    case 'w': for (int w = 0; w < tableRecords(TABLE_WEAPON); w++) out[n++] = w; return n;
    case 'i': for (int i = -2; i <= 64; i++) out[n++] = i; return n;
    case 's': {
        int k = 0;
        for (int s = 0; spellSlot(s)->path != SPELL_END_PATH && k < COPY_CHECK_NAMES - 2; s++)
            if (spellSlot(s)->name) snprintf(copyCheckNames[k++], COPY_CHECK_TEXT, "%.200s", spellSlot(s)->name);
        for (int r = 0; r <= G32(RVA_RITUAL_COUNT) && r < ritualSlotCount() && k < COPY_CHECK_NAMES - 2; r++)
            snprintf(copyCheckNames[k++], COPY_CHECK_TEXT, "%.48s", (const char *)GAME_PTR(ritualTableRva() + (size_t)r * RITUAL_SIZE));
        snprintf(copyCheckNames[k++], COPY_CHECK_TEXT, "xyzzy");
        copyCheckNames[k++][0] = 0;
        for (int i = 0; i < k; i++) out[n++] = (long long)(intptr_t)copyCheckNames[i];
        return n;
    }
    default: return -1;
    }
}

static void selfcheckGameCopy(const char *call) {
    int row = -1;
    for (int i = 0; i < COPY_CHECK_ROW_COUNT && row < 0; i++)
        if (COPY_CHECK_ROWS[i].target.copy && !strcmp(COPY_CHECK_ROWS[i].name, call)) row = i;
    if (row < 0 || copyCheckDone[row]) return;
    copyCheckDone[row] = 1;
    const CopyCheckRow *c = &COPY_CHECK_ROWS[row];
    int count[GAME_CALL_MAX_ARGS], stride[GAME_CALL_MAX_ARGS], slot[GAME_CALL_MAX_ARGS], letters = 0, outs = 0;
    char kind[GAME_CALL_MAX_ARGS];
    for (const char *at = c->args; *at; at++) {
        if (*at == '!') continue;
        if (*at == 'O') {
            kind[letters] = 'O', count[letters] = 1, stride[letters++] = 1, outs++;
            continue;
        }
        int n = copyCheckLetterValues(*at, copyCheckValues[letters]);
        if (n <= 0) {
            writeLog("selfcheck_copy_skipped", "call=%s letter=%c values=%d", call, *at, n);
            return;
        }
        kind[letters] = *at, count[letters] = n, stride[letters++] = 1;
    }
    for (;;) {
        double product = 1;
        int longest = -1, longestTaken = 0;
        for (int i = 0; i < letters; i++) {
            int taken = (count[i] + stride[i] - 1) / stride[i];
            product *= taken;
            if (taken > 1 && taken > longestTaken) longest = i, longestTaken = taken;
        }
        if (product <= COPY_CHECK_MAX || longest < 0) break;
        stride[longest] *= 2;
    }
    long long checks = 0, mismatches = 0;
    memset(slot, 0, sizeof slot);
    for (;;) {
        long long a[GAME_CALL_MAX_ARGS] = {0};
        int gameOut[GAME_CALL_MAX_ARGS], copyOut[GAME_CALL_MAX_ARGS], k = 0;
        for (int i = 0; i < letters; i++) a[i] = kind[i] == 'O' ? (long long)(intptr_t)&gameOut[k++] : copyCheckValues[i][slot[i]];
        for (int i = 0; i < outs; i++) gameOut[i] = copyOut[i] = -1;
        int index = G32(RVA_RANDOM_INDEX), shift = G32(RVA_RANDOM_SHIFT), xorValue = G32(RVA_RANDOM_XOR);
        long long rg = ((GameCallFn)(void *)(gameBase + c->target.rva))(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
        char gameText[COPY_CHECK_TEXT] = {0};
        if (c->result == 'z' && (uintptr_t)rg >= GAME_CALL_LOWEST_TEXT) snprintf(gameText, sizeof gameText, "%.250s", (const char *)(intptr_t)rg);
        int gameIndex = G32(RVA_RANDOM_INDEX), gameShift = G32(RVA_RANDOM_SHIFT), gameXor = G32(RVA_RANDOM_XOR);
        G32(RVA_RANDOM_INDEX) = index, G32(RVA_RANDOM_SHIFT) = shift, G32(RVA_RANDOM_XOR) = xorValue;
        k = 0;
        for (int i = 0; i < letters; i++)
            if (kind[i] == 'O') a[i] = (long long)(intptr_t)&copyOut[k++];
        long long rc = c->target.copy(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
        int same = G32(RVA_RANDOM_INDEX) == gameIndex && G32(RVA_RANDOM_SHIFT) == gameShift && G32(RVA_RANDOM_XOR) == gameXor;
        switch (c->result) {
        case 'i': same &= (int)rg == (int)rc; break;
        case 'k':
        case 'B': same &= (unsigned char)rg == (unsigned char)rc; break;
        case 'b': same &= ((int)rg > 0) == ((int)rc > 0); break;
        case 'z': same &= (uintptr_t)rc >= GAME_CALL_LOWEST_TEXT ? !strcmp(gameText, (const char *)(intptr_t)rc) : !gameText[0]; break;
        default: break;
        }
        for (int i = 0; i < outs; i++) same &= gameOut[i] == copyOut[i];
        checks++;
        if (!same && ++mismatches <= 20)
            writeLogWarning("selfcheck_copy_mismatch", "call=%s reason=copy_differs a0=%lld a1=%lld a2=%lld a3=%lld game=%d copy=%d out0=%d/%d out1=%d/%d", call, a[0], a[1], a[2],
                     a[3], (int)rg, (int)rc, outs > 0 ? gameOut[0] : 0, outs > 0 ? copyOut[0] : 0, outs > 1 ? gameOut[1] : 0, outs > 1 ? copyOut[1] : 0);
        int i = 0;
        for (; i < letters; i++) {
            slot[i] += stride[i];
            if (slot[i] < count[i]) break;
            slot[i] = 0;
        }
        if (i == letters) break;
    }
    writeLog("selfcheck_copy", "call=%s checks=%lld mismatches=%lld", call, checks, mismatches);
}
#endif
