#pragma once

#define CODE_TOGGLE_BYTES_MAX   8
#define RANDOM_EVENT_SITES_MAX  2
#define RANDOM_EVENTS_VERSION   1
#define RANDOM_EVENT_NAMES_TEXT 512

typedef enum {
    CODE_TOGGLE_JCC8_ALWAYS, CODE_TOGGLE_JCC32_ALWAYS, CODE_TOGGLE_JCC8_RETARGET, CODE_TOGGLE_MOV_IMM32_ZERO, CODE_TOGGLE_CMP_IMM8_ZERO,
    CODE_TOGGLE_BCOND_ALWAYS, CODE_TOGGLE_BCOND_NOP, CODE_TOGGLE_BCOND_RETARGET, CODE_TOGGLE_MOVZ_ZERO, CODE_TOGGLE_CMP_ZERO_TO_CMN_ONE,
} CodeToggleKind;

static size_t codeToggleSize(CodeToggleKind kind) {
    switch (kind) {
    case CODE_TOGGLE_JCC8_ALWAYS:
    case CODE_TOGGLE_JCC8_RETARGET: return 2;
    case CODE_TOGGLE_CMP_IMM8_ZERO: return 3;
    case CODE_TOGGLE_JCC32_ALWAYS:
    case CODE_TOGGLE_MOV_IMM32_ZERO: return 6;
    case CODE_TOGGLE_BCOND_ALWAYS:
    case CODE_TOGGLE_BCOND_NOP:
    case CODE_TOGGLE_BCOND_RETARGET:
    case CODE_TOGGLE_MOVZ_ZERO:
    case CODE_TOGGLE_CMP_ZERO_TO_CMN_ONE: return 4;
    }
    return 0;
}

static uint32_t codeWordAt(const unsigned char *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }
static void putCodeWord(unsigned char *p, uint32_t w) {
    for (int i = 0; i < 4; i++) p[i] = (unsigned char)(w >> (8 * i));
}

static int buildCodeToggle(CodeToggleKind kind, const unsigned char *live, uint64_t at, uint64_t target, unsigned char out[CODE_TOGGLE_BYTES_MAX]) {
    size_t size = codeToggleSize(kind);
    if (!size) return 0;
    memcpy(out, live, size);
    uint32_t w = size == 4 ? codeWordAt(live) : 0;
    int isBcond = size == 4 && (w & 0xff000010u) == 0x54000000u;
    switch (kind) {
    case CODE_TOGGLE_JCC8_ALWAYS:
        if ((live[0] & 0xf0) != 0x70) return 0;
        out[0] = 0xeb;
        return 1;
    case CODE_TOGGLE_JCC32_ALWAYS:
        if (live[0] != 0x0f || (live[1] & 0xf0) != 0x80) return 0;
        out[0] = 0x90, out[1] = 0xe9;
        return 1;
    case CODE_TOGGLE_JCC8_RETARGET: {
        if ((live[0] & 0xf0) != 0x70) return 0;
        long long rel = (long long)target - (long long)(at + 2);
        if (rel < -128 || rel > 127) return 0;
        out[1] = (unsigned char)(signed char)rel;
        return 1;
    }
    case CODE_TOGGLE_MOV_IMM32_ZERO:
        if (live[0] != 0x41 || (live[1] & 0xf8) != 0xb8) return 0;
        memset(out + 2, 0, 4);
        return 1;
    case CODE_TOGGLE_CMP_IMM8_ZERO:
        if (live[0] != 0x83 || (live[1] & 0xf8) != 0xf8) return 0;
        out[2] = 0;
        return 1;
    case CODE_TOGGLE_BCOND_ALWAYS: {
        if (!isBcond) return 0;
        int32_t imm19 = (int32_t)(w << 8) >> 13;
        putCodeWord(out, 0x14000000u | ((uint32_t)imm19 & 0x3ffffffu));
        return 1;
    }
    case CODE_TOGGLE_BCOND_NOP:
        if (!isBcond) return 0;
        putCodeWord(out, 0xd503201fu);
        return 1;
    case CODE_TOGGLE_BCOND_RETARGET: {
        long long words = ((long long)target - (long long)at) / 4;
        if (!isBcond || ((long long)target - (long long)at) % 4 || words < -(1 << 18) || words >= (1 << 18)) return 0;
        putCodeWord(out, (w & 0xff00001fu) | (((uint32_t)words & 0x7ffffu) << 5));
        return 1;
    }
    case CODE_TOGGLE_MOVZ_ZERO:
        if ((w & 0xffe00000u) != 0x52800000u) return 0;
        putCodeWord(out, w & ~(0xffffu << 5));
        return 1;
    case CODE_TOGGLE_CMP_ZERO_TO_CMN_ONE:
        if ((w & 0xfffffc1fu) != 0x7100001fu) return 0;
        putCodeWord(out, 0x3100001fu | 1u << 10 | (w & 0x3e0u));
        return 1;
    }
    return 0;
}

// ---- Toggles: one instruction each ----------------------------------------------------------------
typedef struct {
    const char *owner;
    unsigned rva, target;
    CodeToggleKind kind;
    unsigned char game[CODE_TOGGLE_BYTES_MAX], ours[CODE_TOGGLE_BYTES_MAX];
    int available, read, applied;
} CodeToggle;

static int (*codeToggleWrite)(void *address, const void *bytes, size_t size) = writeGameCode;

static int prepareCodeToggle(CodeToggle *t) {
    t->available = t->read = t->applied = 0;
    if (!gameCodeMatches || !codeToggleSize(t->kind)) {
        writeLogError("code_toggle_refused", "owner=%s rva=0x%x reason=%s", t->owner, t->rva, !gameCodeMatches ? "game_code_differs" : "bad_kind");
        return 0;
    }
    if (!claimGameCode(t->rva, (unsigned)codeToggleSize(t->kind), t->owner)) return 0;
    t->available = 1;
    return 1;
}

static const char *switchCodeToggle(CodeToggle *t, int want) {
    const char *reason = NULL;
    size_t size = codeToggleSize(t->kind);
    unsigned char *at = GAME_PTR(t->rva);
    if (!t->available) {
        if (!want) return NULL;
        reason = "unavailable";
    } else if (want && !t->applied) {
        if (!t->read) memcpy(t->game, at, size), t->read = 1;
        else if (memcmp(at, t->game, size)) reason = "changed_by_other";
        if (!reason && !buildCodeToggle(t->kind, at, (uint64_t)(uintptr_t)at, (uint64_t)(uintptr_t)GAME_PTR(t->target), t->ours))
            reason = "unexpected_instruction";
        if (!reason && !codeToggleWrite(at, t->ours, size)) reason = "code_unwritable";
        if (!reason) t->applied = 1;
    } else if (!want && t->applied) {
        if (memcmp(at, t->ours, size)) reason = "changed_by_other";
        else if (!codeToggleWrite(at, t->game, size)) reason = "code_unwritable";
        if (!reason) t->applied = 0;
    }
    if (reason)
        writeLogError("code_toggle_refused", "owner=%s rva=0x%x want=%d reason=%s", t->owner, t->rva, want, reason);
    return reason;
}

// ---- The events ----------------------------------------------------------------------------------
typedef struct {
    const char *name;
    int sites;
    struct {
        unsigned rva, target;
        CodeToggleKind kind;
    } site[RANDOM_EVENT_SITES_MAX];
} RandomEvent;

#ifdef __APPLE__
#define GATE_SKIP     CODE_TOGGLE_BCOND_ALWAYS
#define GATE_INTO     CODE_TOGGLE_BCOND_NOP
#define GATE_SKIP_W   CODE_TOGGLE_BCOND_ALWAYS
#define GATE_RETARGET CODE_TOGGLE_BCOND_RETARGET
#define HERO_TYPE     CODE_TOGGLE_MOVZ_ZERO
#define HERO_COMPARE  CODE_TOGGLE_CMP_ZERO_TO_CMN_ONE
#else
#define GATE_SKIP     CODE_TOGGLE_JCC8_ALWAYS
#define GATE_INTO     CODE_TOGGLE_JCC8_ALWAYS
#define GATE_SKIP_W   CODE_TOGGLE_JCC32_ALWAYS
#define GATE_RETARGET CODE_TOGGLE_JCC8_RETARGET
#define HERO_TYPE     CODE_TOGGLE_MOV_IMM32_ZERO
#define HERO_COMPARE  CODE_TOGGLE_CMP_IMM8_ZERO
#endif
#define GATE1(rva, kind) 1, {{rva, 0, kind}}

static const RandomEvent RANDOM_EVENTS[] = {
    {"hadesshift", GATE1(RVA_EVENT_GATE_HADES_SHIFT, GATE_INTO)},
    {"monument", GATE1(RVA_EVENT_GATE_MONUMENT, GATE_SKIP_W)},
    {"foretelling", GATE1(RVA_EVENT_GATE_FORETELLING, GATE_SKIP_W)},
    {"taintedmagic", GATE1(RVA_EVENT_GATE_TAINTED_MAGIC, GATE_SKIP)},
    {"elementalanomaly", GATE1(RVA_EVENT_GATE_ELEMENTAL_ANOMALY, GATE_SKIP)},
#ifdef __APPLE__
    {"heatwave", GATE1(RVA_EVENT_GATE_HEAT_WAVE, GATE_INTO)},
#else
    {"heatwave", GATE1(RVA_EVENT_GATE_HEAT_WAVE, GATE_SKIP_W)},
#endif
    {"bumperharvest", GATE1(RVA_EVENT_GATE_BUMPER_HARVEST, GATE_SKIP)},
    {"fungusyear", GATE1(RVA_EVENT_GATE_FUNGUS_YEAR, GATE_SKIP)},
    {"meteor", GATE1(RVA_EVENT_GATE_METEOR, GATE_SKIP)},
    {"insanefisherman", GATE1(RVA_EVENT_GATE_INSANE_FISHERMAN, GATE_SKIP_W)},
    {"drought", GATE1(RVA_EVENT_GATE_DROUGHT, GATE_SKIP)},
    {"strangemist", GATE1(RVA_EVENT_GATE_STRANGE_MIST, GATE_SKIP_W)},
    {"eviltimes", GATE1(RVA_EVENT_GATE_EVIL_TIMES, GATE_SKIP)},
    {"infernogate", GATE1(RVA_EVENT_GATE_INFERNO_GATE, GATE_SKIP_W)},
    {"primalgate", GATE1(RVA_EVENT_GATE_PRIMAL_GATE, GATE_SKIP_W)},
    {"goldmine", GATE1(RVA_EVENT_GATE_GOLD_MINE, GATE_SKIP)},
    {"youngdvala", GATE1(RVA_EVENT_GATE_YOUNG_DVALA, GATE_SKIP_W)},
    {"merchant", 2, {{RVA_EVENT_GATE_MERCHANT, RVA_EVENT_GATE_MERCHANT_SKIP, GATE_RETARGET}, {RVA_EVENT_GATE_MERCHANT_SOCIETY, 0, GATE_SKIP_W}}},
    {"hero", GATE1(RVA_HERO_TYPE_CLASS_HERO, HERO_TYPE)},
#ifdef __APPLE__
    {"unexpectedhero", 2, {{RVA_HERO_TYPE_UNEXPECTED, 0, HERO_TYPE}, {RVA_HERO_TYPE_UNEXPECTED_FLAG, 0, HERO_TYPE}}},
#else
    {"unexpectedhero", GATE1(RVA_HERO_TYPE_UNEXPECTED, HERO_TYPE)},
#endif
    {"weaponmaster", GATE1(RVA_HERO_TYPE_WEAPON_MASTER, HERO_COMPARE)},
    {"antspawn", GATE1(RVA_SITEPOP_GATE_ANT_HILLS, GATE_SKIP_W)},
};
#undef GATE1
#define RANDOM_EVENT_COUNT ((int)(sizeof RANDOM_EVENTS / sizeof RANDOM_EVENTS[0]))
_Static_assert(RANDOM_EVENT_COUNT == 22 && RANDOM_EVENT_COUNT <= 32, "the 22 events of Jazzy Game Presets' list, one bit each");

static CodeToggle randomEventGates[RANDOM_EVENT_COUNT][RANDOM_EVENT_SITES_MAX];
static unsigned randomEventsOff;
static int randomEventsInstalled;

static int randomEventIndex(const char *name) {
    for (int e = 0; e < RANDOM_EVENT_COUNT; e++)
        if (!strcmp(RANDOM_EVENTS[e].name, name)) return e;
    return -1;
}

static void randomEventNames(unsigned mask, char *out, size_t size) {
    size_t used = 0;
    out[0] = '\0';
    for (int e = 0; e < RANDOM_EVENT_COUNT; e++)
        if (mask & 1u << e) appendText(out, size, &used, "%s%s", used ? "," : "", RANDOM_EVENTS[e].name);
    if (!used) snprintf(out, size, "-");
}

static int syncRandomEventGates(void) {
    int on = 0, changed = 0, refused = 0;
    int official = netRoleNow() == NET_ROLE_OFFICIAL;
    for (int e = 0; e < RANDOM_EVENT_COUNT; e++) {
        int want = !official && (randomEventsOff >> e & 1), in = randomEventsInstalled;
        for (int s = 0; s < RANDOM_EVENTS[e].sites; s++) {
            CodeToggle *t = &randomEventGates[e][s];
            int was = t->applied;
            if (switchCodeToggle(t, want)) refused++, in = 0;
            else changed += was != t->applied;
            in = in && t->applied == want;
        }
        on += want && in;
    }
    if (changed || refused) {
        char names[RANDOM_EVENT_NAMES_TEXT];
        randomEventNames(official ? 0 : randomEventsOff, names, sizeof names);
        writeResultLog(!refused, "random_events_synced", "off=%s in_force=%d gates_changed=%d refused=%d", names, on, changed, refused);
    }
    return on;
}

static int setRandomEventsOff(unsigned mask, const char *by) {
    randomEventsOff = mask & ((1u << RANDOM_EVENT_COUNT) - 1);
    int on = syncRandomEventGates(), want = 0;
    for (int e = 0; e < RANDOM_EVENT_COUNT; e++) want += randomEventsOff >> e & 1;
    char names[RANDOM_EVENT_NAMES_TEXT];
    randomEventNames(randomEventsOff, names, sizeof names);
    writeResultLog(on == want, "random_events_set", "by=%s off=%s in_force=%d of %d", by, names, on, want);
    return on == want;
}

// ---- Saves: EVOF --------------------------------------------------------------------------------
static void addRandomEventsChunk(SaveList *list) {
    if (!randomEventsOff || saveTailPlayer >= 0 || netRoleNow() == NET_ROLE_OFFICIAL) return;
    SaveBuffer chunk = {0};
    int count = 0;
    for (int e = 0; e < RANDOM_EVENT_COUNT; e++) count += randomEventsOff >> e & 1;
    saveBufferInt(&chunk, RANDOM_EVENTS_VERSION);
    saveBufferInt(&chunk, count);
    for (int e = 0; e < RANDOM_EVENT_COUNT; e++)
        if (randomEventsOff >> e & 1) saveBufferString(&chunk, RANDOM_EVENTS[e].name);
    writeLog("random_events_written", "events=%d bytes=%zu", count, chunk.size);
    saveListAdd(list, SAVE_TAG_EVOF, &chunk);
}

static void useRandomEventsChunk(const SaveChunk *chunk, SaveFile kind) {
    if (kind != SAVE_FILE_SAVE || netRoleNow() == NET_ROLE_OFFICIAL) return;
    unsigned mask = 0;
    int dropped = 0, count = 0;
    const char *result = chunk ? "read" : "none";
    if (chunk) {
        SaveCursor c = {chunk->payload, chunk->payload + chunk->size, 0};
        int version = saveCursorInt(&c);
        count = saveCursorInt(&c);
        if (c.failed || version != RANDOM_EVENTS_VERSION || count < 0 || count > 1000) {
            result = c.failed || count < 0 || count > 1000 ? "bad_chunk" : "other_version";
        } else {
            for (int i = 0; i < count && !c.failed; i++) {
                char name[SAVE_STRING_MAX + 1];
                saveCursorString(&c, name, sizeof name);
                int e = c.failed ? -1 : randomEventIndex(name);
                if (e >= 0) mask |= 1u << e;
                else if (!c.failed) {
                    dropped++;
                    writeLogWarning("random_event_dropped", "name=%.63s reason=unknown_event", name);
                }
            }
            if (c.failed) mask = 0, result = "bad_chunk";
        }
        if (strcmp(result, "read")) writeLogWarning("random_events_read", "result=dropped reason=%s", result);
    }
    char names[RANDOM_EVENT_NAMES_TEXT];
    randomEventNames(mask, names, sizeof names);
    if (chunk || randomEventsOff) writeLog("random_events_read", "result=%s off=%s dropped=%d", result, names, dropped);
    randomEventsOff = mask;
    syncRandomEventGates();
}

// ---- Lua ----------------------------------------------------------------------------------------
static int lSetRandomEventsOff(lua_State *L) {
    const char *mod = runningLuaMod();
    if (!lua_istable(L, 1)) {
        refuseLuaCall(mod, "coe5.setRandomEventsOff", "not_a_table", "names=%.40s", luaL_tolstring(L, 1, NULL));
        lua_pushboolean(L, 0);
        return 1;
    }
    unsigned mask = 0;
    int refused = 0;
    lua_Integer n = (lua_Integer)lua_rawlen(L, 1);
    for (lua_Integer i = 1; i <= n; i++) {
        lua_rawgeti(L, 1, i);
        int e = lua_type(L, -1) == LUA_TSTRING ? randomEventIndex(lua_tostring(L, -1)) : -1;
        if (e < 0) {
            refused++;
            refuseLuaCall(mod, "coe5.setRandomEventsOff", lua_type(L, -1) == LUA_TSTRING ? "unknown_event" : "not_a_string", "at=%lld name=%.40s",
                          (long long)i, luaL_tolstring(L, -1, NULL));
            lua_pop(L, 1);
        } else mask |= 1u << e;
        lua_pop(L, 1);
    }
    int ok = setRandomEventsOff(mask, mod ? mod : "?");
    lua_pushboolean(L, ok && !refused);
    return 1;
}

static int lRandomEventsOff(lua_State *L) {
    lua_newtable(L);
    int k = 0;
    for (int e = 0; e < RANDOM_EVENT_COUNT; e++)
        if (randomEventsOff >> e & 1) {
            lua_pushstring(L, RANDOM_EVENTS[e].name);
            lua_rawseti(L, -2, ++k);
        }
    return 1;
}

// ---- Install ------------------------------------------------------------------------------------
static u64 (*nextRandomEventsNewWorld)(u64);
static u64 randomEventsNewWorldHook(u64 mode) {
    if (randomEventsOff) setRandomEventsOff(0, "new_world");
    return nextRandomEventsNewWorld(mode);
}

static int prepareRandomEventGates(int *sites) {
    int available = 0;
    *sites = 0;
    for (int e = 0; e < RANDOM_EVENT_COUNT; e++) {
        int all = 1;
        for (int s = 0; s < RANDOM_EVENTS[e].sites; s++) {
            CodeToggle *t = &randomEventGates[e][s];
            *t = (CodeToggle){.owner = RANDOM_EVENTS[e].name, .rva = RANDOM_EVENTS[e].site[s].rva, .target = RANDOM_EVENTS[e].site[s].target,
                              .kind = RANDOM_EVENTS[e].site[s].kind};
            all = prepareCodeToggle(t) && all;
            (*sites)++;
        }
        available += all;
    }
    randomEventsInstalled = 1;
    return available;
}

static void randomEventsInit(void) {
    int sites, available = prepareRandomEventGates(&sites);
    int newWorld = wrapSite(SITE_PLAY_GAME_NEW_WORLD, (void *)randomEventsNewWorldHook, (void **)&nextRandomEventsNewWorld);
    writeResultLog(available == RANDOM_EVENT_COUNT && newWorld, "random_events_installed", "events=%d of %d gates=%d new_world=%d", available,
                   RANDOM_EVENT_COUNT, sites, newWorld);
}
