#pragma once

#define GAME_DATA_VERSION    1
#define GAME_DATA_PLAYERS    PLAYER_SLOTS_MOVED
#define GAME_DATA_DEPTH_MAX  100
#define GAME_DATA_REFUSALS_LOGGED 8
#define GAME_DATA_PATH_MAX   160
enum { GAME_DATA_FALSE = 'f', GAME_DATA_TRUE = 't', GAME_DATA_INT = 'i', GAME_DATA_FLOAT = 'd', GAME_DATA_STRING = 's', GAME_DATA_TABLE = 'T' };
_Static_assert(SAVE_TAG_GDAT == SAVE_TAG('G', 'D', 'A', 'T'), "the chunk's tag");

// ---- The bytes ----------------------------------------------------------------------------------
typedef struct { unsigned char *bytes; int size; } GameDataPart;
typedef struct { char mod[MOD_NAME_MAX]; GameDataPart shared, players[GAME_DATA_PLAYERS]; } GameDataHeld;
static GameDataHeld *gameDataHeld;
static int gameDataHeldCount, gameDataHeldCapacity;

static void freeGameDataPart(GameDataPart *p) {
    free(p->bytes);
    *p = (GameDataPart){0};
}

static int setGameDataPart(GameDataPart *p, const unsigned char *bytes, int size, const char *mod) {
    freeGameDataPart(p);
    if (size <= 0) return 1;
    if (!(p->bytes = malloc((size_t)size))) {
        writeLogError("game_data_dropped", "mod=%s reason=out_of_memory bytes=%d", mod, size);
        return 0;
    }
    memcpy(p->bytes, bytes, (size_t)size);
    p->size = size;
    return 1;
}

static GameDataHeld *findGameDataHeld(const char *mod) {
    for (int i = 0; i < gameDataHeldCount; i++)
        if (!strcmp(gameDataHeld[i].mod, mod)) return &gameDataHeld[i];
    return NULL;
}

static GameDataHeld *heldGameDataFor(const char *mod) {
    GameDataHeld *h = findGameDataHeld(mod);
    if (h) return h;
    if (!growList((void **)&gameDataHeld, &gameDataHeldCapacity, gameDataHeldCount + 1, sizeof gameDataHeld[0])) {
        writeLogError("game_data_dropped", "mod=%s reason=out_of_memory", mod);
        return NULL;
    }
    h = &gameDataHeld[gameDataHeldCount++];
    memset(h, 0, sizeof *h);
    snprintf(h->mod, sizeof h->mod, "%s", mod);
    return h;
}

static void forgetHeldGameData(void) {
    for (int i = 0; i < gameDataHeldCount; i++) {
        freeGameDataPart(&gameDataHeld[i].shared);
        for (int p = 0; p < GAME_DATA_PLAYERS; p++) freeGameDataPart(&gameDataHeld[i].players[p]);
    }
    gameDataHeldCount = 0;
}

static void saveBufferInt64(SaveBuffer *b, long long value) {
    unsigned char le[8];
    for (int i = 0; i < 8; i++) le[i] = (unsigned char)((unsigned long long)value >> (8 * i));
    saveBufferAdd(b, le, 8);
}

static long long gameDataInt64(const unsigned char *at) {
    unsigned long long value = 0;
    for (int i = 7; i >= 0; i--) value = value << 8 | at[i];
    return (long long)value;
}

static int gameDataTableValid(const unsigned char **at, const unsigned char *end, int depth);
static int gameDataValueValid(const unsigned char **at, const unsigned char *end, int depth, int isKey) {
    if (*at >= end) return 0;
    unsigned char tag = *(*at)++;
    switch (tag) {
    case GAME_DATA_FALSE:
    case GAME_DATA_TRUE: return 1;
    case GAME_DATA_INT:
        if (end - *at < 8) return 0;
        *at += 8;
        return 1;
    case GAME_DATA_FLOAT: {
        if (end - *at < 8) return 0;
        long long bits = gameDataInt64(*at);
        double value;
        memcpy(&value, &bits, 8);
        *at += 8;
        return !isKey || value == value;
    }
    case GAME_DATA_STRING: {
        if (end - *at < 4) return 0;
        int length = (int)((unsigned)(*at)[0] | (unsigned)(*at)[1] << 8 | (unsigned)(*at)[2] << 16 | (unsigned)(*at)[3] << 24);
        *at += 4;
        if (length < 0 || end - *at < length) return 0;
        *at += length;
        return 1;
    }
    case GAME_DATA_TABLE: return !isKey && depth < GAME_DATA_DEPTH_MAX && gameDataTableValid(at, end, depth + 1);
    default: return 0;
    }
}
static int gameDataTableValid(const unsigned char **at, const unsigned char *end, int depth) {
    if (end - *at < 4) return 0;
    int pairs = (int)((unsigned)(*at)[0] | (unsigned)(*at)[1] << 8 | (unsigned)(*at)[2] << 16 | (unsigned)(*at)[3] << 24);
    *at += 4;
    if (pairs < 0 || pairs > (end - *at) / 2) return 0;
    for (int i = 0; i < pairs; i++)
        if (!gameDataValueValid(at, end, depth, 1) || !gameDataValueValid(at, end, depth, 0)) return 0;
    return 1;
}
static int gameDataPartValid(const unsigned char *bytes, int size) {
    if (size == 0) return 1;
    const unsigned char *at = bytes;
    return size > 0 && gameDataTableValid(&at, bytes + size, 0) && at == bytes + size;
}

// ---- Lua: writing a table's bytes ---------------------------------------------------------------
typedef struct {
    SaveBuffer *b;
    const LuaScript *s;
    int visited;
    int refused;
    char path[GAME_DATA_PATH_MAX];
} GameDataWriter;

static void refuseGameDataValue(GameDataWriter *w, const char *reason, const char *type) {
    if (w->refused++ < GAME_DATA_REFUSALS_LOGGED) refuseLuaCall(w->s->mod, "coe5.gameData", reason, "at=%s type=%s", w->path, type);
}

static size_t pushGameDataPath(lua_State *L, GameDataWriter *w, int index) {
    size_t used = strlen(w->path);
    if (lua_type(L, index) == LUA_TSTRING) snprintf(w->path + used, sizeof w->path - used, ".%.40s", lua_tostring(L, index));
    else if (lua_isinteger(L, index)) snprintf(w->path + used, sizeof w->path - used, "[%lld]", (long long)lua_tointeger(L, index));
    else if (lua_type(L, index) == LUA_TNUMBER) snprintf(w->path + used, sizeof w->path - used, "[%.14g]", (double)lua_tonumber(L, index));
    else snprintf(w->path + used, sizeof w->path - used, "[%s]", lua_typename(L, lua_type(L, index)));
    return used;
}

static int gameDataValueWritable(lua_State *L, GameDataWriter *w, int index, int depth, int isKey) {
    int type = lua_type(L, index);
    if (type == LUA_TBOOLEAN || type == LUA_TNUMBER || type == LUA_TSTRING) return 1;
    if (type != LUA_TTABLE || isKey) {
        refuseGameDataValue(w, isKey ? "bad_key" : "not_plain_value", lua_typename(L, type));
        return 0;
    }
    if (depth >= GAME_DATA_DEPTH_MAX) {
        refuseGameDataValue(w, "too_deep", "table");
        return 0;
    }
    if (lua_getmetatable(L, index)) {
        lua_pop(L, 1);
        refuseGameDataValue(w, "has_metatable", "table");
        return 0;
    }
    lua_pushvalue(L, index);
    int seen = lua_rawget(L, w->visited) != LUA_TNIL;
    lua_pop(L, 1);
    if (seen) refuseGameDataValue(w, "holds_itself", "table");
    return !seen;
}

static void writeGameDataTable(lua_State *L, GameDataWriter *w, int index, int depth);
static void writeGameDataValue(lua_State *L, GameDataWriter *w, int index, int depth) {
    unsigned char tag;
    switch (lua_type(L, index)) {
    case LUA_TBOOLEAN:
        tag = lua_toboolean(L, index) ? GAME_DATA_TRUE : GAME_DATA_FALSE;
        saveBufferAdd(w->b, &tag, 1);
        break;
    case LUA_TNUMBER:
        if (lua_isinteger(L, index)) {
            tag = GAME_DATA_INT;
            saveBufferAdd(w->b, &tag, 1);
            saveBufferInt64(w->b, (long long)lua_tointeger(L, index));
        } else {
            double value = (double)lua_tonumber(L, index);
            long long bits;
            memcpy(&bits, &value, 8);
            tag = GAME_DATA_FLOAT;
            saveBufferAdd(w->b, &tag, 1);
            saveBufferInt64(w->b, bits);
        }
        break;
    case LUA_TSTRING: {
        size_t length;
        const char *text = lua_tolstring(L, index, &length);
        tag = GAME_DATA_STRING;
        saveBufferAdd(w->b, &tag, 1);
        if (length > SAVE_CHUNK_MAX) {
            w->b->failed = 1;
            return;
        }
        saveBufferInt(w->b, (int)length);
        saveBufferAdd(w->b, text, length);
        break;
    }
    default:
        tag = GAME_DATA_TABLE;
        saveBufferAdd(w->b, &tag, 1);
        writeGameDataTable(L, w, index, depth + 1);
    }
}

static void writeGameDataTable(lua_State *L, GameDataWriter *w, int index, int depth) {
    if (w->b->failed) return;
    index = lua_absindex(L, index);
    luaL_checkstack(L, 6, "gameData");
    lua_pushvalue(L, index);
    lua_pushboolean(L, 1);
    lua_rawset(L, w->visited);
    size_t countAt = w->b->size;
    int pairs = 0;
    saveBufferInt(w->b, 0);
    lua_pushnil(L);
    while (lua_next(L, index)) {
        size_t pathUsed = pushGameDataPath(L, w, -2);
        if (gameDataValueWritable(L, w, -2, depth, 1) && gameDataValueWritable(L, w, -1, depth, 0)) {
            writeGameDataValue(L, w, -2, depth);
            writeGameDataValue(L, w, -1, depth);
            pairs++;
        }
        w->path[pathUsed] = '\0';
        lua_pop(L, 1);
    }
    if (!w->b->failed) {
        unsigned char le[4] = {(unsigned char)pairs, (unsigned char)(pairs >> 8), (unsigned char)(pairs >> 16), (unsigned char)(pairs >> 24)};
        memcpy(w->b->bytes + countAt, le, 4);
    }
    lua_pushvalue(L, index);
    lua_pushnil(L);
    lua_rawset(L, w->visited);
}

#define GAME_DATA_HASH_BUDGET 4000000
static unsigned long long mixGameDataHash(unsigned long long x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}
static unsigned long long hashGameDataValue(lua_State *L, int index, int visited, long *budget) {
    index = lua_absindex(L, index);
    if (--*budget < 0) return 0;
    switch (lua_type(L, index)) {
    case LUA_TBOOLEAN: return mixGameDataHash(lua_toboolean(L, index) ? 2 : 1);
    case LUA_TNUMBER: {
        if (lua_isinteger(L, index)) return mixGameDataHash((unsigned long long)lua_tointeger(L, index) ^ 0x1000);
        double value = (double)lua_tonumber(L, index);
        unsigned long long bits;
        memcpy(&bits, &value, 8);
        return mixGameDataHash(bits ^ 0x2000);
    }
    case LUA_TSTRING: {
        size_t length;
        const unsigned char *text = (const unsigned char *)lua_tolstring(L, index, &length);
        unsigned long long h = 1469598103934665603ULL;
        for (size_t i = 0; i < length; i++) h = (h ^ text[i]) * 1099511628211ULL;
        return mixGameDataHash(h ^ 0x3000);
    }
    case LUA_TTABLE: {
        luaL_checkstack(L, 4, "gameData");
        lua_pushvalue(L, index);
        if (lua_rawget(L, visited) != LUA_TNIL) {
            lua_pop(L, 1);
            return 0x4000;
        }
        lua_pop(L, 1);
        lua_pushvalue(L, index);
        lua_pushboolean(L, 1);
        lua_rawset(L, visited);
        unsigned long long sum = 0x5000;
        lua_pushnil(L);
        while (*budget >= 0 && lua_next(L, index)) {
            unsigned long long key = hashGameDataValue(L, -2, visited, budget);
            sum += mixGameDataHash(key * 31 + hashGameDataValue(L, -1, visited, budget));
            lua_pop(L, 1);
        }
        if (*budget < 0) lua_pop(L, 1);
        lua_pushvalue(L, index);
        lua_pushnil(L);
        lua_rawset(L, visited);
        return mixGameDataHash(sum);
    }
    default: return 0x6000;
    }
}
static unsigned long long hashGameDataTable(lua_State *L, int index, int *known) {
    index = lua_absindex(L, index);
    lua_newtable(L);
    long budget = GAME_DATA_HASH_BUDGET;
    unsigned long long h = hashGameDataValue(L, index, lua_gettop(L), &budget);
    lua_pop(L, 1);
    *known = budget >= 0;
    return h;
}

// ---- Lua: reading a table's bytes ---------------------------------------------------------------
static void readGameDataTableInto(lua_State *L, int index, const unsigned char **at);
static void pushGameDataValue(lua_State *L, const unsigned char **at) {
    unsigned char tag = *(*at)++;
    switch (tag) {
    case GAME_DATA_FALSE: lua_pushboolean(L, 0); break;
    case GAME_DATA_TRUE: lua_pushboolean(L, 1); break;
    case GAME_DATA_INT:
        lua_pushinteger(L, (lua_Integer)gameDataInt64(*at));
        *at += 8;
        break;
    case GAME_DATA_FLOAT: {
        long long bits = gameDataInt64(*at);
        double value;
        memcpy(&value, &bits, 8);
        lua_pushnumber(L, (lua_Number)value);
        *at += 8;
        break;
    }
    case GAME_DATA_STRING: {
        int length = (int)((unsigned)(*at)[0] | (unsigned)(*at)[1] << 8 | (unsigned)(*at)[2] << 16 | (unsigned)(*at)[3] << 24);
        lua_pushlstring(L, (const char *)*at + 4, (size_t)length);
        *at += 4 + length;
        break;
    }
    default:
        lua_newtable(L);
        readGameDataTableInto(L, -1, at);
    }
}
static void readGameDataTableInto(lua_State *L, int index, const unsigned char **at) {
    index = lua_absindex(L, index);
    luaL_checkstack(L, 4, "gameData");
    int pairs = (int)((unsigned)(*at)[0] | (unsigned)(*at)[1] << 8 | (unsigned)(*at)[2] << 16 | (unsigned)(*at)[3] << 24);
    *at += 4;
    for (int i = 0; i < pairs; i++) {
        pushGameDataValue(L, at);
        pushGameDataValue(L, at);
        lua_rawset(L, index);
    }
}

static void clearGameDataTable(lua_State *L, int index) {
    index = lua_absindex(L, index);
    luaL_checkstack(L, 4, "gameData");
    lua_newtable(L);
    int keys = 0;
    lua_pushnil(L);
    while (lua_next(L, index)) {
        lua_pop(L, 1);
        lua_pushvalue(L, -1);
        lua_rawseti(L, -3, ++keys);
    }
    for (int i = 1; i <= keys; i++) {
        lua_rawgeti(L, -1, i);
        lua_pushnil(L);
        lua_rawset(L, index);
    }
    lua_pop(L, 1);
}

static void pushGameDataRoot(lua_State *L, const char *field) {
    lua_rawgetp(L, LUA_REGISTRYINDEX, &luaCoe5Key);
    if (lua_getfield(L, -1, field) != LUA_TTABLE) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, field);
    }
    lua_remove(L, -2);
}
static void pushPlayerDataTable(lua_State *L, int playerData, int slot) {
    if (lua_rawgeti(L, playerData, slot) == LUA_TTABLE) return;
    lua_pop(L, 1);
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_rawseti(L, playerData, slot);
}

// ---- Lua: the jobs a save, a load and the rehearsal run in a script's state ------------------------
typedef enum { GAME_DATA_WRITE, GAME_DATA_FILL, GAME_DATA_HASH, GAME_DATA_HASH_BOTH } GameDataJobKind;
typedef struct {
    GameDataJobKind kind;
    LuaScript *s;
    int player;
    SaveBuffer *entry;
    int any;
    const GameDataHeld *from;
    unsigned long long hash;
    int hashKnown;
    int refused;
} GameDataJob;

static void writeGameDataPart(lua_State *L, GameDataJob *job, GameDataWriter *w, const char *path) {
    SaveBuffer part = {0};
    w->b = &part;
    snprintf(w->path, sizeof w->path, "%s", path);
    writeGameDataTable(L, w, -1, 0);
    if (part.failed) job->entry->failed = 1;
    if (part.size > 4) job->any = 1;
    saveBufferInt(job->entry, (int)part.size);
    saveBufferAdd(job->entry, part.bytes, part.size);
    free(part.bytes);
}

static void fillGameDataTable(lua_State *L, const GameDataPart *part) {
    clearGameDataTable(L, -1);
    if (!part || part->size <= 0) return;
    const unsigned char *at = part->bytes;
    readGameDataTableInto(L, -1, &at);
}

static int runGameDataJob(lua_State *L) {
    GameDataJob *job = lua_touserdata(L, 1);
    lua_settop(L, 0);
    pushGameDataRoot(L, "gameData");
    pushGameDataRoot(L, "playerData");
    int gameData = 1, playerData = 2;
    if (job->kind == GAME_DATA_HASH) {
        job->hash = hashGameDataTable(L, gameData, &job->hashKnown);
        return 0;
    }
    if (job->kind == GAME_DATA_HASH_BOTH) {
        int sharedKnown = 0, playersKnown = 0;
        unsigned long long shared = hashGameDataTable(L, gameData, &sharedKnown);
        job->hash = mixGameDataHash(shared * 31 + hashGameDataTable(L, playerData, &playersKnown));
        job->hashKnown = sharedKnown && playersKnown;
        return 0;
    }
    if (job->kind == GAME_DATA_FILL) {
        if (job->player < 0) {
            lua_pushvalue(L, gameData);
            fillGameDataTable(L, job->from ? &job->from->shared : NULL);
            lua_pop(L, 1);
        }
        for (int slot = 0; slot < GAME_DATA_PLAYERS; slot++) {
            if (job->player >= 0 && slot != job->player) continue;
            pushPlayerDataTable(L, playerData, slot);
            fillGameDataTable(L, job->from ? &job->from->players[slot] : NULL);
            lua_pop(L, 1);
        }
        if (job->player < 0) {
            lua_newtable(L);
            int extra = 0;
            lua_pushnil(L);
            while (lua_next(L, playerData)) {
                lua_pop(L, 1);
                if (lua_isinteger(L, -1) && lua_tointeger(L, -1) >= 0 && lua_tointeger(L, -1) < GAME_DATA_PLAYERS) continue;
                lua_pushvalue(L, -1);
                lua_rawseti(L, -3, ++extra);
            }
            for (int i = 1; i <= extra; i++) {
                lua_rawgeti(L, -1, i);
                lua_pushnil(L);
                lua_rawset(L, playerData);
            }
            lua_pop(L, 1);
        }
        job->hash = hashGameDataTable(L, gameData, &job->hashKnown);
        return 0;
    }
    GameDataWriter w = {.s = job->s};
    lua_newtable(L);
    w.visited = lua_gettop(L);
    if (job->player < 0) {
        lua_pushvalue(L, gameData);
        writeGameDataPart(L, job, &w, "gameData");
        lua_pop(L, 1);
    } else saveBufferInt(job->entry, -1);
    SaveBuffer parts = {0};
    int count = 0;
    SaveBuffer *entry = job->entry;
    job->entry = &parts;
    for (int slot = 0; slot < GAME_DATA_PLAYERS; slot++) {
        if (job->player >= 0 && slot != job->player) continue;
        pushPlayerDataTable(L, playerData, slot);
        int any = job->any;
        job->any = 0;
        char path[32];
        snprintf(path, sizeof path, "playerData[%d]", slot);
        size_t before = parts.size;
        saveBufferInt(&parts, slot);
        writeGameDataPart(L, job, &w, path);
        if (job->player < 0 && !job->any) parts.size = before;
        else count++;
        job->any = job->any || any;
        lua_pop(L, 1);
    }
    lua_pushnil(L);
    while (lua_next(L, playerData)) {
        if (!lua_isinteger(L, -2) || lua_tointeger(L, -2) < 0 || lua_tointeger(L, -2) >= GAME_DATA_PLAYERS) {
            snprintf(w.path, sizeof w.path, "playerData");
            pushGameDataPath(L, &w, -2);
            refuseGameDataValue(&w, "bad_player", lua_typename(L, lua_type(L, -2)));
        }
        lua_pop(L, 1);
    }
    job->entry = entry;
    if (parts.failed) entry->failed = 1;
    saveBufferInt(entry, count);
    saveBufferAdd(entry, parts.bytes, parts.size);
    free(parts.bytes);
    job->refused = w.refused;
    return 0;
}
static int pushGameDataJob(lua_State *L, void *job) {
    lua_pushcfunction(L, runGameDataJob);
    lua_pushlightuserdata(L, job);
    return 1;
}

static int gameDataCountsFor(const LuaScript *s) {
    NetRole role = netRoleNow();
    if (role == NET_ROLE_OFFICIAL) return 0;
    return role == NET_ROLE_SINGLE || modClassNow(s->mod) == LUA_CLASS_CONTENT;
}
static int gameDataStateLive(const LuaScript *s) { return s->L && !s->dead && !s->retired && !s->stopping; }

static void fillGameDataOf(LuaScript *s, int player) {
    if (!gameDataStateLive(s) || !gameDataCountsFor(s)) return;
    GameDataJob job = {.kind = GAME_DATA_FILL, .s = s, .player = player, .from = findGameDataHeld(s->mod)};
    if (!callLuaProtected(s, pushGameDataJob, &job, 0)) return;
    if (player < 0) s->gameDataHash = job.hash, s->gameDataHashKnown = job.hashKnown;
}
static void fillNewScriptGameData(LuaScript *s) { fillGameDataOf(s, -1); }

static int gameDataSharedChanged(LuaScript *s) {
    if (!s->gameDataHashKnown || !gameDataStateLive(s)) return 0;
    GameDataJob job = {.kind = GAME_DATA_HASH, .s = s};
    return callLuaProtected(s, pushGameDataJob, &job, 0) && job.hashKnown && job.hash != s->gameDataHash;
}

static unsigned long long hashBothGameDataOf(LuaScript *s, int *known) {
    *known = 0;
    if (!gameDataStateLive(s) || !gameDataCountsFor(s)) return 0;
    GameDataJob job = {.kind = GAME_DATA_HASH_BOTH, .s = s};
    if (!callLuaProtected(s, pushGameDataJob, &job, 0)) return 0;
    *known = job.hashKnown;
    return job.hash;
}
static void keepGameDataThroughLoad(LuaScript *s, unsigned long long before, int known, const char *file) {
    if (!known) return;
    int afterKnown = 0;
    unsigned long long after = hashBothGameDataOf(s, &afterKnown);
    if (!afterKnown || after == before) return;
    refuseLuaCall(s->mod, "coe5.gameData", "during_load", "file=%s", file);
    fillGameDataOf(s, -1);
}

// ---- The chunk ----------------------------------------------------------------------------------
static int writeHeldGameDataEntry(SaveBuffer *entry, const GameDataHeld *h, int player) {
    int any = h->shared.size > 4;
    if (player < 0) {
        saveBufferInt(entry, h->shared.size ? h->shared.size : 4);
        if (h->shared.size) saveBufferAdd(entry, h->shared.bytes, (size_t)h->shared.size);
        else saveBufferInt(entry, 0);
    } else saveBufferInt(entry, -1);
    int count = 0;
    SaveBuffer parts = {0};
    for (int slot = 0; slot < GAME_DATA_PLAYERS; slot++) {
        const GameDataPart *p = &h->players[slot];
        if (player >= 0 ? slot != player : p->size <= 4) continue;
        saveBufferInt(&parts, slot);
        saveBufferInt(&parts, p->size ? p->size : 4);
        if (p->size) saveBufferAdd(&parts, p->bytes, (size_t)p->size);
        else saveBufferInt(&parts, 0);
        any = any || p->size > 4;
        count++;
    }
    if (parts.failed) entry->failed = 1;
    saveBufferInt(entry, count);
    saveBufferAdd(entry, parts.bytes, parts.size);
    free(parts.bytes);
    return any;
}

static int heldGameDataPlayerPart(const char *mod, int player) {
    const GameDataHeld *h = player >= 0 && player < GAME_DATA_PLAYERS ? findGameDataHeld(mod) : NULL;
    return h && h->players[player].size > 4;
}

static void addGameDataEntry(SaveBuffer *chunk, int *mods, const char *mod, SaveBuffer *entry, int any, int player) {
    if (entry->failed || chunk->size + 1 + strlen(mod) + entry->size > SAVE_CHUNK_MAX) {
        writeLogError("game_data_dropped", "mod=%s reason=too_big bytes=%zu max=%d", mod, entry->size, SAVE_CHUNK_MAX);
    } else if (any || heldGameDataPlayerPart(mod, player)) {
        saveBufferString(chunk, mod);
        saveBufferAdd(chunk, entry->bytes, entry->size);
        (*mods)++;
    }
    free(entry->bytes);
    *entry = (SaveBuffer){0};
}

static int gameDataChunkMods;

static void addGameDataChunk(SaveList *list) {
    gameDataChunkMods = 0;
    if (netRoleNow() == NET_ROLE_OFFICIAL) return;
    int player = saveTailPlayer;
    SaveBuffer chunk = {0}, entry = {0};
    int mods = 0, refused = 0;
    saveBufferInt(&chunk, GAME_DATA_VERSION);
    saveBufferInt(&chunk, player);
    size_t modsAt = chunk.size;
    saveBufferInt(&chunk, 0);
    for (int i = 0; i < luaScriptCount; i++) {
        LuaScript *s = luaScripts[i];
        if (s->retired || !gameDataCountsFor(s)) continue;
        int any = 0;
        if (gameDataStateLive(s)) {
            if (player >= 0 && gameDataSharedChanged(s))
                writeLogWarning("game_data_not_sent", "mod=%s reason=client_change part=gameData player=%d", s->mod, player);
            GameDataJob job = {.kind = GAME_DATA_WRITE, .s = s, .player = player, .entry = &entry};
            if (!callLuaProtected(s, pushGameDataJob, &job, 0)) {
                free(entry.bytes);
                entry = (SaveBuffer){0};
                const GameDataHeld *h = findGameDataHeld(s->mod);
                if (h) any = writeHeldGameDataEntry(&entry, h, player);
                else continue;
            } else any = job.any;
            refused += job.refused;
        } else {
            const GameDataHeld *h = findGameDataHeld(s->mod);
            if (!h) continue;
            any = writeHeldGameDataEntry(&entry, h, player);
        }
        addGameDataEntry(&chunk, &mods, s->mod, &entry, any, player);
    }
    if (!mods) {
        free(chunk.bytes);
        return;
    }
    unsigned char le[4] = {(unsigned char)mods, (unsigned char)(mods >> 8), (unsigned char)(mods >> 16), (unsigned char)(mods >> 24)};
    if (!chunk.failed) memcpy(chunk.bytes + modsAt, le, 4);
    writeLog("game_data_written", "file=%s mods=%d bytes=%zu refused=%d", player >= 0 ? "turn" : "save", mods, chunk.size, refused);
    gameDataChunkMods = mods;
    saveListAdd(list, SAVE_TAG_GDAT, &chunk);
}

typedef struct { char mod[MOD_NAME_MAX]; int hasShared; GameDataPart shared; int parts; struct { int slot; GameDataPart part; } at[GAME_DATA_PLAYERS]; } GameDataEntryRead;

static int readGameDataPartAt(SaveCursor *c, GameDataPart *part) {
    int size = saveCursorInt(c);
    if (c->failed || size < 0 || c->end - c->at < size || !gameDataPartValid(c->at, size)) return c->failed = 1, 0;
    *part = (GameDataPart){(unsigned char *)c->at, size};
    c->at += size;
    return 1;
}

static int readGameDataEntryBody(SaveCursor *c, GameDataEntryRead *e) {
    int sharedSize = saveCursorInt(c);
    if (c->failed) return 0;
    if (sharedSize >= 0) {
        c->at -= 4;
        if (!readGameDataPartAt(c, &e->shared)) return 0;
        e->hasShared = 1;
    } else if (sharedSize != -1) return c->failed = 1, 0;
    int parts = saveCursorInt(c);
    if (c->failed || parts < 0 || parts > GAME_DATA_PLAYERS) return c->failed = 1, 0;
    for (int i = 0; i < parts; i++) {
        int slot = saveCursorInt(c);
        if (c->failed || slot < 0 || slot >= GAME_DATA_PLAYERS) return c->failed = 1, 0;
        if (!readGameDataPartAt(c, &e->at[i].part)) return 0;
        e->at[i].slot = slot;
    }
    e->parts = parts;
    return 1;
}
static int readGameDataEntry(SaveCursor *c, GameDataEntryRead *e) {
    memset(e, 0, sizeof *e);
    saveCursorString(c, e->mod, sizeof e->mod);
    return !c->failed && readGameDataEntryBody(c, e);
}

static LuaScript *gameDataScriptOf(const char *mod) {
    for (int i = 0; i < luaScriptCount; i++)
        if (!luaScripts[i]->retired && !strcmp(luaScripts[i]->mod, mod) && gameDataCountsFor(luaScripts[i])) return luaScripts[i];
    return NULL;
}

static int gameDataTurnPlayer = -1;
static int turnFileReadPlayer(void) { return gameDataTurnPlayer; }

static void useGameDataChunk(const SaveChunk *chunk, SaveFile kind) {
    int version = 0, player = -1, mods = 0;
    SaveCursor c = {0};
    if (chunk) {
        c = (SaveCursor){chunk->payload, chunk->payload + chunk->size, 0};
        version = saveCursorInt(&c), player = saveCursorInt(&c), mods = saveCursorInt(&c);
        if (c.failed || version != GAME_DATA_VERSION || mods < 0) {
            writeLogWarning("game_data_read", "file=%s reason=%s version=%d", SAVE_FILE_NAMES[kind], c.failed ? "bad_chunk" : "other_version", version);
            if (kind == SAVE_FILE_TURN) return;
            chunk = NULL, mods = 0;
        }
    }
    if (kind == SAVE_FILE_TURN) {
        if (!chunk) return;
        if (netRoleNow() != NET_ROLE_HOST || gameDataTurnPlayer < 0) {
            writeLogWarning("game_data_refused", "reason=not_the_host_read player=%d", player);
            return;
        }
    } else forgetHeldGameData();
    int taken = 0, bad = 0;
    GameDataEntryRead *e = malloc(sizeof *e);
    if (!e && mods) writeLogError("game_data_read", "file=%s reason=out_of_memory", SAVE_FILE_NAMES[kind]);
    for (int i = 0; e && i < mods; i++) {
        if (!readGameDataEntry(&c, e)) {
            bad = 1;
            writeLogWarning("game_data_read", "file=%s reason=bad_entry at=%d", SAVE_FILE_NAMES[kind], i);
            break;
        }
        if (kind == SAVE_FILE_TURN) {
            LuaScript *s = gameDataScriptOf(e->mod);
            if (!s) {
                writeLogWarning("game_data_refused", "mod=%s reason=not_running player=%d", e->mod, gameDataTurnPlayer);
                continue;
            }
            for (int k = 0; k < e->parts; k++) {
                if (e->at[k].slot != gameDataTurnPlayer) {
                    writeLogWarning("game_data_refused", "mod=%s reason=other_player slot=%d player=%d", e->mod, e->at[k].slot, gameDataTurnPlayer);
                    continue;
                }
                GameDataHeld *h = heldGameDataFor(e->mod);
                if (h && setGameDataPart(&h->players[gameDataTurnPlayer], e->at[k].part.bytes, e->at[k].part.size, e->mod)) {
                    fillGameDataOf(s, gameDataTurnPlayer);
                    taken++;
                }
            }
            continue;
        }
        GameDataHeld *h = heldGameDataFor(e->mod);
        if (!h) continue;
        int ok = !e->hasShared || setGameDataPart(&h->shared, e->shared.bytes, e->shared.size, e->mod);
        for (int k = 0; ok && k < e->parts; k++) ok = setGameDataPart(&h->players[e->at[k].slot], e->at[k].part.bytes, e->at[k].part.size, e->mod);
        taken++;
    }
    free(e);
    if (kind == SAVE_FILE_SAVE)
        for (int i = 0; i < luaScriptCount; i++) fillGameDataOf(luaScripts[i], -1);
    if (chunk || kind == SAVE_FILE_SAVE)
        writeLog("game_data_read", "file=%s result=%s mods=%d taken=%d player=%d", SAVE_FILE_NAMES[kind], !chunk ? "none" : bad ? "bad_entry" : "read",
                 mods, taken, kind == SAVE_FILE_TURN ? gameDataTurnPlayer : player);
}

static u64 serverLoad2hHook(u64 player, u64 path) {
    gameDataTurnPlayer = (int)player;
    u64 result = GAME_FN(u64 (*)(u64, u64), RVA_LOAD_PLAYER_2H)(player, path);
    gameDataTurnPlayer = -1;
    return result;
}

// ---- A new game -------------------------------------------------------------------------------------
static u64 (*nextGameDataNewWorld)(u64);
static u64 gameDataNewWorldHook(u64 mode) {
    forgetHeldGameData();
    for (int i = 0; i < luaScriptCount; i++) fillGameDataOf(luaScripts[i], -1);
    return nextGameDataNewWorld(mode);
}

// ---- The single player rehearsal ------------------------------------------------------------------
#define LAN_REHEARSAL_SETTING "lan_rehearsal"
#define LAN_REHEARSAL_DEFAULT 0
static int lanRehearsalOn(void) {
    finishReading();
    const ModSetting *set = findDeclaredSetting(COE5SE_MOD, LAN_REHEARSAL_SETTING);
    return set ? settingValueNow(set) != 0 : LAN_REHEARSAL_DEFAULT;
}

static int restartContentScriptsForRehearsal(const char *when, int player) {
    int restarted = 0;
    for (int i = 0, count = luaScriptCount; i < count; i++) {
        LuaScript *s = luaScripts[i];
        if (s->retired || s->dead || modClassNow(s->mod) != LUA_CLASS_CONTENT) continue;
        if (s->depth) {
            writeLog("lan_rehearsal_skipped", "mod=%s reason=call_open when=%s", s->mod, when);
            continue;
        }
        restartLuaScript(s, "lan_rehearsal");
        restarted++;
    }
    writeLog("lan_rehearsal_restarted", "when=%s player=%d scripts=%d", when, player, restarted);
    return restarted;
}

static void holdContentGameData(int player) {
    GameDataEntryRead *e = malloc(sizeof *e);
    if (!e) {
        writeLogError("game_data_dropped", "reason=out_of_memory when=rehearsal");
        return;
    }
    for (int i = 0; i < luaScriptCount; i++) {
        LuaScript *s = luaScripts[i];
        if (s->retired || !gameDataStateLive(s) || modClassNow(s->mod) != LUA_CLASS_CONTENT) continue;
        if (player >= 0 && gameDataSharedChanged(s)) writeLogWarning("game_data_not_sent", "mod=%s reason=client_change part=gameData player=%d rehearsal=1", s->mod, player);
        SaveBuffer entry = {0};
        GameDataJob job = {.kind = GAME_DATA_WRITE, .s = s, .player = player, .entry = &entry};
        SaveCursor c = {0};
        GameDataHeld *h = NULL;
        if (callLuaProtected(s, pushGameDataJob, &job, 0) && !entry.failed) {
            memset(e, 0, sizeof *e);
            c = (SaveCursor){entry.bytes, entry.bytes + entry.size, 0};
            if (readGameDataEntryBody(&c, e)) h = heldGameDataFor(s->mod);
        }
        if (h && player < 0) {
            setGameDataPart(&h->shared, e->shared.bytes, e->shared.size, s->mod);
            for (int slot = 0; slot < GAME_DATA_PLAYERS; slot++) freeGameDataPart(&h->players[slot]);
        }
        for (int k = 0; h && k < e->parts; k++) setGameDataPart(&h->players[e->at[k].slot], e->at[k].part.bytes, e->at[k].part.size, s->mod);
        free(entry.bytes);
    }
    free(e);
}

static u64 (*nextRehearsalHumanTurn)(u64);
static u64 rehearsalHumanTurnHook(u64 player) {
    int on = netRoleNow() == NET_ROLE_SINGLE && lanRehearsalOn();
    if (on) {
        holdContentGameData(-1);
        restartContentScriptsForRehearsal("turn_start", (int)player);
    }
    int wasInTurn = lanRehearsalInTurn;
    lanRehearsalInTurn = on;
    u64 result = nextRehearsalHumanTurn(player);
    lanRehearsalInTurn = wasInTurn;
    if (on && netRoleNow() == NET_ROLE_SINGLE) {
        holdContentGameData((int)player);
        restartContentScriptsForRehearsal("turn_end", (int)player);
    }
    return result;
}

// ---- Install --------------------------------------------------------------------------------------------
#ifndef __APPLE__
#define GAME_DATA_STUB_SIZE 0x20
static int installServerLoad2hHook(void) {
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION);
    if (!stubs) {
        writeLog("hook_skipped", "site=server_load_2h reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    return redirectCall(RVA_SERVER_LOAD_2H_CALL, RVA_LOAD_PLAYER_2H, stubs, (void *)serverLoad2hHook, "server_load_2h", NULL);
}
#else
static int installServerLoad2hHook(void) {
    if (blTarget(gameBase + RVA_SERVER_LOAD_2H_CALL) != gameBase + RVA_LOAD_PLAYER_2H) {
        writeLog("hook_skipped", "site=server_load_2h reason=not_the_expected_call va=0x%llx", RVA_SERVER_LOAD_2H_CALL + 0x100000000ULL);
        return 0;
    }
    unsigned char *trampoline = newTrampoline((void *)serverLoad2hHook);
    uint32_t word = trampoline ? blTo(gameBase + RVA_SERVER_LOAD_2H_CALL, trampoline) : 0;
    if (trampoline && writeGameCode(gameBase + RVA_SERVER_LOAD_2H_CALL, &word, 4)) return 1;
    writeLog("hook_skipped", "site=server_load_2h reason=%s", trampoline ? "code_unwritable" : "no_padding");
    return 0;
}
#endif

static void gameDataInit(void) {
    char owner[64];
    moduleFileName((void *)gameDataNewWorldHook, owner, sizeof owner);
    WrapPlace outermost = {.layer = WRAP_LAYER_OUTERMOST};
    int ok = installServerLoad2hHook() +
             insertWrap(SITE_PLAY_GAME_NEW_WORLD, (void *)gameDataNewWorldHook, (void **)&nextGameDataNewWorld, outermost, owner, NULL) +
             insertWrap(SITE_HUMAN_TURN, (void *)rehearsalHumanTurnHook, (void **)&nextRehearsalHumanTurn, outermost, owner, NULL);
    writeInstallLog("game_data_hooks_installed", ok, 3);
}
