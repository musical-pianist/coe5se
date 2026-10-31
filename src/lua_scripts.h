#pragma once
#include <float.h>
#include <math.h>
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
#include "lua_random.h"
#include "mod_storage.h"
#include "ritual_effect_names.h"
#include "record_words.h"

#define LUA_STEP_LIMIT      20000000
#define LUA_STEP_LIMIT_TEXT "20000000"
#define LUA_HOOK_STEPS      10000

static const PluginApi *const luaApi = &API;

static size_t luaMemoryScriptMax = (size_t)1536 << 20;
static size_t luaMemoryTotalMax = (size_t)6 << 30;
static size_t luaMemoryTotal;

enum { LUA_STOP_NONE, LUA_STOP_STEPS, LUA_STOP_MEMORY, LUA_STOP_MEMORY_TOTAL, LUA_STOP_CLOSED, LUA_STOP_KINDS };
static const struct { const char *reason, *error; } LUA_STOPS[LUA_STOP_KINDS] = {
    [LUA_STOP_STEPS] = {"step_limit", "script ran too long (more than " LUA_STEP_LIMIT_TEXT " steps in one call)"},
    [LUA_STOP_MEMORY] = {"memory_cap", "script used more than its 1.5 GB of memory"},
    [LUA_STOP_MEMORY_TOTAL] = {"memory_total", "scripts used more than 6 GB of memory together, and this one the most"},
    [LUA_STOP_CLOSED] = {"script_closed", "script was turned off"},
};
enum { LUA_NOTICE_STOPPED = 1, LUA_NOTICE_STORAGE = 2 };
static int luaCodeRunning;
static unsigned unitsChangeEpoch = 1;

typedef struct {
    char mod[MOD_NAME_MAX], path[MAX_PATH];
    char commandsName[MOD_NAME_MAX + sizeof "_commands"];
    char folder[MAX_PATH];
    char dataFolder[STORAGE_NAME_MAX + 1];
    long long dataBytes;
    int dataBytesKnown;
    lua_State *L;
    int dead;
    int retired;
    int commandsTried;
    int mainTried;
    int stopLogged;
    long long stepsLeft;
    unsigned long stepsFrame;
    int depth;
    int stopping;
    size_t memory;
    int noticesDue, noticesShown;
    int warnOff, warnContinued, warningLength, warningCapacity;
    char *warning;
} LuaScript;
static LuaScript **luaScripts;
static int luaScriptCount, luaScriptCapacity;

// ---- Protected calls --------------------------------------------------------------------------
static LuaScript *luaScriptOf(lua_State *L) { return L ? *(LuaScript **)lua_getextraspace(L) : NULL; }

static LuaScript *luaRunning;
static const char *runningLuaMod(void) { return luaRunning ? luaRunning->mod : NULL; }

static PRINTF_FORMAT(4, 5) void refuseLuaCall(const char *mod, const char *call, const char *reason, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    logRefusal("lua_refused", call, "mod", mod ? mod : "?", reason, fmt, ap);
    va_end(ap);
}

static int playerOfFactionName(const char *name);

static lua_Integer luaCheckPlayer(lua_State *L, int arg) {
    if (lua_type(L, arg) == LUA_TSTRING && !lua_isnumber(L, arg)) {
        const char *name = lua_tostring(L, arg);
        int slot = playerOfFactionName(name);
        if (slot < 0) luaL_error(L, "no faction called '%s'", name);
        lua_pushinteger(L, slot);
        lua_replace(L, arg);
    }
    return luaL_checkinteger(L, arg);
}

static char luaStopKeys[LUA_STOP_KINDS];

static void luaStepHook(lua_State *L, lua_Debug *ar);
static void stopLuaScript(LuaScript *s, int why, const char *detail) {
    if (s->stopping) return;
    s->stopping = why;
    s->dead = 1;
    if (why != LUA_STOP_CLOSED) s->noticesDue |= LUA_NOTICE_STOPPED;
    refuseLuaCall(s->mod, "script", LUA_STOPS[why].reason, "%s", detail);
    if (s->L) lua_sethook(s->L, luaStepHook, LUA_MASKCOUNT, 1);
}
static int raiseLuaStop(lua_State *L, LuaScript *s) {
    lua_sethook(L, luaStepHook, LUA_MASKCOUNT, 1);
    lua_rawgetp(L, LUA_REGISTRYINDEX, &luaStopKeys[s->stopping]);
    return lua_error(L);
}

static void stopLuaScriptForSteps(lua_State *L, LuaScript *s) {
    lua_Debug ar;
    char detail[LUA_IDSIZE + 64] = "steps=" LUA_STEP_LIMIT_TEXT;
    for (int level = 0; level < 3 && lua_getstack(L, level, &ar); level++)
        if (lua_getinfo(L, "Sl", &ar) && ar.currentline > 0) {
            snprintf(detail, sizeof detail, "steps=" LUA_STEP_LIMIT_TEXT " at=%s:%d", ar.short_src, ar.currentline);
            break;
        }
    stopLuaScript(s, LUA_STOP_STEPS, detail);
}

static void luaStepHook(lua_State *L, lua_Debug *ar) {
    (void)ar;
    LuaScript *s = luaScriptOf(L);
    if (!s) return;
    if (!s->stopping) {
        s->stepsLeft -= LUA_HOOK_STEPS;
        if (s->stepsLeft > 0) return;
        stopLuaScriptForSteps(L, s);
    }
    raiseLuaStop(L, s);
}
void coe5seChargeLuaSteps(lua_State *L, size_t steps) {
    if (lua_gethook(L) != luaStepHook) return;
    LuaScript *s = luaScriptOf(L);
    if (!s) return;
    if (!s->stopping) {
        s->stepsLeft -= steps < LUA_STEP_LIMIT ? (long long)steps : LUA_STEP_LIMIT;
        if (s->stepsLeft > 0) return;
        stopLuaScriptForSteps(L, s);
    }
    raiseLuaStop(L, s);
}
static void startLuaSteps(LuaScript *s) {
    s->stepsLeft = LUA_STEP_LIMIT;
    s->stepsFrame = gameFrames;
}
static void resetLuaSteps(lua_State *L) {
    LuaScript *s = luaScriptOf(L);
    if (s) startLuaSteps(s);
}

static LuaScript *luaSetupAutomatedBy;

static int refuseFromDeadLuaScript(lua_State *L, const char *call) {
    LuaScript *s = luaScriptOf(L);
    if (!s || !s->dead) return 0;
    refuseLuaCall(s->mod, call, "script_stopped", "depth=%d", s->depth);
    return 1;
}

// ---- Memory: each script's state allocates through luaAlloc ------------------------------------
static void closeLuaScriptState(LuaScript *s) {
    if (!s->L || s->depth) return;
    if (luaSetupAutomatedBy == s) {
        luaSetupAutomatedBy = NULL;
        if (luaApi->setupAutomated()) {
            luaApi->setSetupAutomated(0);
            writeLog("lua_setup_automated_cleared", "mod=%s", s->mod);
        }
    }
    if (!s->stopping) s->stopping = LUA_STOP_CLOSED;
    s->warnOff = 1;
    lua_sethook(s->L, luaStepHook, LUA_MASKCOUNT, 1);
    size_t held = s->memory;
    lua_close(s->L);
    s->L = NULL;
    writeLog("lua_script_closed", "mod=%s memory=%zu", s->mod, held);
}
static void closeDeadLuaScript(LuaScript *s) {
    if (s->dead && !s->depth) closeLuaScriptState(s);
}

static int luaMemoryFits(LuaScript *s, size_t grow) {
    char detail[128 + MOD_NAME_MAX];
    if (s->memory + grow > luaMemoryScriptMax) {
        snprintf(detail, sizeof detail, "bytes=%zu max=%zu", s->memory + grow, luaMemoryScriptMax);
        stopLuaScript(s, LUA_STOP_MEMORY, detail);
        return 0;
    }
    while (luaMemoryTotal + grow > luaMemoryTotalMax) {
        LuaScript *big = s;
        for (int i = 0; i < luaScriptCount; i++)
            if (luaScripts[i]->L && luaScripts[i]->memory > big->memory) big = luaScripts[i];
        snprintf(detail, sizeof detail, "bytes=%zu total=%zu max=%zu asked_by=%s", big->memory, luaMemoryTotal + grow,
                 luaMemoryTotalMax, s->mod);
        stopLuaScript(big, LUA_STOP_MEMORY_TOTAL, detail);
        if (big == s) return 0;
        if (!big->stopLogged) writeLog("lua_script_stopped", "mod=%s", big->mod);
        big->stopLogged = 1;
        if (big->depth) return 1;
        closeLuaScriptState(big);
    }
    return 1;
}

static void *luaAlloc(void *ud, void *ptr, size_t osize, size_t nsize) {
    LuaScript *s = ud;
    size_t old = ptr ? osize : 0;
    if (nsize == 0) {
        free(ptr);
        s->memory -= old;
        luaMemoryTotal -= old;
        return NULL;
    }
    if (nsize > old && !luaMemoryFits(s, nsize - old)) return NULL;
    void *p = realloc(ptr, nsize);
    if (!p) return NULL;
    s->memory = s->memory - old + nsize;
    luaMemoryTotal = luaMemoryTotal - old + nsize;
    return p;
}
static int luaPanic(lua_State *L) {
    LuaScript *s = luaScriptOf(L);
    writeLog("lua_panic", "mod=%s error=%s", s ? s->mod : "?", lua_type(L, -1) == LUA_TSTRING ? lua_tostring(L, -1) : "(not a string)");
    return 0;
}

typedef int (*LuaPushFn)(lua_State *L, void *ctx);
typedef struct { LuaPushFn push; void *ctx; int nresults; } LuaProtectedCall;
static int runLuaProtectedCall(lua_State *L) {
    const LuaProtectedCall *c = lua_touserdata(L, 1);
    lua_settop(L, 0);
    int nargs = c->push(L, c->ctx);
    lua_call(L, nargs, c->nresults);
    return c->nresults;
}

static int callLuaProtected(LuaScript *s, LuaPushFn push, void *ctx, int nresults) {
    if (!lua_checkstack(s->L, 2)) {
        refuseLuaCall(s->mod, "script", "out_of_memory", "stack=full");
        return 0;
    }
    LuaProtectedCall call = {push, ctx, nresults};
    if (!s->depth++ || s->stepsFrame != gameFrames) startLuaSteps(s);
    LuaScript *outer = luaRunning;
    luaRunning = s;
    luaCodeRunning++;
    lua_pushcfunction(s->L, runLuaProtectedCall);
    lua_pushlightuserdata(s->L, &call);
    unitsChangeEpoch++;
    int rc = lua_pcall(s->L, 1, nresults, 0);
    unitsChangeEpoch++;
    luaCodeRunning--;
    luaRunning = outer;
    s->depth--;
    if (rc == LUA_OK) {
        if (!nresults) closeDeadLuaScript(s);
        return 1;
    }
    char line[600];
    int type = lua_type(s->L, -1);
    if (type == LUA_TSTRING) snprintf(line, sizeof line, "%s", lua_tostring(s->L, -1));
    else if (type == LUA_TNUMBER && lua_isinteger(s->L, -1)) snprintf(line, sizeof line, "%lld", (long long)lua_tointeger(s->L, -1));
    else if (type == LUA_TNUMBER) snprintf(line, sizeof line, "%.14g", (double)lua_tonumber(s->L, -1));
    else snprintf(line, sizeof line, "(error object is a %s value)", lua_typename(s->L, type));
    writeLog("lua_error", "mod=%s error=%s", s->mod, line);
    lua_pop(s->L, 1);
    s->dead = 1;
    if (!s->stopLogged) writeLog("lua_script_stopped", "mod=%s", s->mod);
    s->stopLogged = 1;
    closeDeadLuaScript(s);
    return 0;
}

// ---- Callbacks: a script's main menu buttons, Game Menu lines, screen lines, and Read Tip hiders --
typedef struct { LuaScript *s; int ref; int refVisible; } LuaCallback;
static LuaCallback *luaCallbacks;
static int luaCallbackCount, luaCallbackCapacity;

static int pushLuaRef(lua_State *L, void *ref) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, *(const int *)ref);
    return 0;
}

static int runLuaCallback(int i, int wantResult, int fallback) {
    if (i < 0 || i >= luaCallbackCount) return fallback;
    LuaCallback c = luaCallbacks[i];
    if (!c.s || c.s->dead || isNetworkGame()) return fallback;
    if (!callLuaProtected(c.s, pushLuaRef, &c.ref, 1)) return fallback;
    int r = wantResult ? (lua_isnumber(c.s->L, -1) ? (int)lua_tointeger(c.s->L, -1) : lua_toboolean(c.s->L, -1)) : fallback;
    lua_pop(c.s->L, 1);
    closeDeadLuaScript(c.s);
    return r;
}
static int runLuaVisible(int i) {
    if (i < 0 || i >= luaCallbackCount) return 0;
    LuaCallback c = luaCallbacks[i];
    if (!c.s || c.s->dead || isNetworkGame()) return 0;
    if (c.refVisible == LUA_NOREF) return 1;
    if (!callLuaProtected(c.s, pushLuaRef, &c.refVisible, 1)) return 0;
    int r = lua_toboolean(c.s->L, -1);
    lua_pop(c.s->L, 1);
    closeDeadLuaScript(c.s);
    return r;
}

static int newLuaCallback(lua_State *L, int index, int visibleIndex) {
    luaL_checktype(L, index, LUA_TFUNCTION);
    if (!growList((void **)&luaCallbacks, &luaCallbackCapacity, luaCallbackCount + 1, sizeof luaCallbacks[0])) return -1;
    LuaCallback *c = &luaCallbacks[luaCallbackCount];
    c->s = luaScriptOf(L);
    lua_pushvalue(L, index);
    c->ref = luaL_ref(L, LUA_REGISTRYINDEX);
    c->refVisible = LUA_NOREF;
    if (visibleIndex && lua_isfunction(L, visibleIndex)) {
        lua_pushvalue(L, visibleIndex);
        c->refVisible = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    return luaCallbackCount++;
}

static void checkLuaMainThread(lua_State *L, const char *what) {
    int isMain = lua_pushthread(L);
    lua_pop(L, 1);
    if (!isMain) luaL_error(L, "%s can't be called from a coroutine", what);
}
