#pragma once

#define TRAIT_LINE_LABEL_MAX   63
#define TRAIT_LINE_TEXT_MAX    511
#define TRAIT_LINE_PICTURES    64
#define TRAIT_LINE_DEFAULT_ICON 211

typedef struct {
    char label[TRAIT_LINE_LABEL_MAX + 1], text[TRAIT_LINE_TEXT_MAX + 1];
    int icon;
    int picture;
    int resBit;
    int lua;
    const char *file;
} TraitLine;
static TraitLine *traitLines;
static int traitLineCount, traitLineCapacity;

#define TRAIT_DESCRIPTION_SLOTS  512
static const char **traitDescriptions;

typedef struct {
    char path[MAX_PATH];
    int registered;
} TraitPicture;
static TraitPicture traitPictures[TRAIT_LINE_PICTURES];
static int traitPictureCount;

static int addTraitPicture(const char *path) {
    for (int i = 0; i < traitPictureCount; i++)
        if (!strcmp(traitPictures[i].path, path)) return i;
    if (traitPictureCount >= TRAIT_LINE_PICTURES) return -1;
    snprintf(traitPictures[traitPictureCount].path, sizeof traitPictures[0].path, "%s", path);
    traitPictures[traitPictureCount].registered = 0;
    return traitPictureCount++;
}

static int traitPictureSprite(int pic) {
    TraitPicture *t = &traitPictures[pic];
    int sprite = RES_TRS_SPRITES + pic;
    if (t->registered < 0) return -1;
    int *const *replaced = (int *const *)GAME_PTR(RVA_SPRITE_REPLACED);
    const int *list = replaced[SPRITE_KIND_RES];
    if (t->registered > 0 && list && list[sprite] >= 0) return sprite;
#ifndef __APPLE__
    int image = GAME_FN(int (*)(int, int, int, const char *), RVA_REPLACE_SPRITE)(SPRITE_KIND_RES, sprite, 0, t->path);
#else
    int image = GAME_FN(int (*)(int, int, const char *), RVA_REPLACE_SPRITE)(SPRITE_KIND_RES, sprite, t->path);
#endif
    list = replaced[SPRITE_KIND_RES];
    if (image < 0 || !list || list[sprite] < 0) {
        t->registered = -1;
        writeLogWarning("trait_icon_refused", "reason=picture_not_loaded sprite=%d path=%s", sprite, t->path);
        return -1;
    }
    t->registered = 1;
    writeLog("trait_icon_registered", "sprite=%d path=%s", sprite, t->path);
    return sprite;
}

static const char **allocTraitDescriptions(void) {
    uintptr_t table = (uintptr_t)GAME_PTR(RVA_ABILITY_DESCRIPTIONS), bytes = TRAIT_DESCRIPTION_SLOTS * sizeof(char *);
#ifndef __APPLE__
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    uintptr_t gran = si.dwAllocationGranularity, start = (table + gran) & ~(gran - 1);
    for (uintptr_t dist = 0; dist < 0x100000000ULL; dist += gran) {
        void *p = VirtualAlloc((void *)(start + dist), bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (p && (uintptr_t)p > table && (uintptr_t)p - table < 0x380000000ULL) return p;
        if (p) VirtualFree(p, 0, MEM_RELEASE);
    }
#else
    uintptr_t page = 0x4000, start = (table + page) & ~(page - 1);
    for (uintptr_t dist = 0; dist < 0x100000000ULL; dist += 0x1000000) {
        void *p = mmap((void *)(start + dist), bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (p == MAP_FAILED) continue;
        if ((uintptr_t)p > table && (uintptr_t)p - table < 0x380000000ULL) return p;
        munmap(p, bytes);
    }
#endif
    return NULL;
}

static int traitLineDescriptionId(int i) {
    if (!traitDescriptions || i < 0 || i >= TRAIT_DESCRIPTION_SLOTS) return 0;
    return (int)(((uintptr_t)&traitDescriptions[i] - (uintptr_t)GAME_PTR(RVA_ABILITY_DESCRIPTIONS)) / sizeof(char *));
}

typedef int (*AbilityLookupFn)(const char *text);

static int traitLookupHook(const char *text) {
    for (int i = 0; text && i < traitLineCount; i++)
        if (!strcmp(text, traitLines[i].label)) {
            int id = traitLineDescriptionId(i);
            if (id > 0) return id;
            break;
        }
    return GAME_FN(AbilityLookupFn, RVA_ABILITY_LOOKUP)(text);
}

static int traitLineIcon(const TraitLine *line) {
    if (line->picture < 0) return line->icon;
    int sprite = traitPictureSprite(line->picture);
    return sprite < 0 ? TRAIT_LINE_DEFAULT_ICON : sprite;
}

static int addTraitLine(const char *label, const char *text, int icon, int picture, int resBit, int lua) {
    if (!growList((void **)&traitLines, &traitLineCapacity, traitLineCount + 1, sizeof traitLines[0])) return -1;
    TraitLine *line = &traitLines[traitLineCount];
    snprintf(line->label, sizeof line->label, "%s", label);
    snprintf(line->text, sizeof line->text, "%s", text ? text : "");
    line->icon = icon;
    line->picture = picture;
    line->resBit = resBit;
    line->lua = lua;
    line->file = NULL;
    if (traitDescriptions && traitLineCount < TRAIT_DESCRIPTION_SLOTS) traitDescriptions[traitLineCount] = strdup(line->text);
    return traitLineCount++;
}

typedef struct { int ref, unit, monster; } TraitVisibleCall;
static int pushTraitVisible(lua_State *L, void *arg) {
    const TraitVisibleCall *c = arg;
    lua_rawgeti(L, LUA_REGISTRYINDEX, c->ref);
    lua_pushinteger(L, c->unit);
    lua_pushinteger(L, c->monster);
    return 2;
}

static int traitLineVisible(int lua, int unit, int monster) {
    if (lua < 0 || lua >= luaCallbackCount) return 0;
    LuaCallback c = luaCallbacks[lua];
    if (!c.s || c.s->dead || isNetworkGame()) return 0;
    TraitVisibleCall call = {c.ref, unit, monster};
    if (!callLuaProtected(c.s, pushTraitVisible, &call, 1)) return 0;
    int r = lua_toboolean(c.s->L, -1);
    lua_pop(c.s->L, 1);
    closeDeadLuaScript(c.s);
    return r;
}

static int traitLineShown(const TraitLine *line, int unit, int monster) {
    if (line->resBit) return (unitResBits(unit) & line->resBit) != 0;
    return traitLineVisible(line->lua, unit, monster);
}

static void findOwnTraitPictures(void);
typedef unsigned (*TraitListFn)(int unit, int monster, unsigned long long flags, unsigned long long more);
typedef int (*AbilityLineFn)(int icon, const char *fmt, ...);

static unsigned traitListHook(int unit, int monster, unsigned long long flags, unsigned long long more) {
    unsigned count = GAME_FN(TraitListFn, RVA_UNIT_TRAIT_LIST)(unit, monster, flags, more);
    int m = unitMonsterNumber(unit);
    if (m < 0) return count;
    findOwnTraitPictures();
    for (int i = 0; i < traitLineCount; i++)
        if (traitLineShown(&traitLines[i], unit, m) &&
            GAME_FN(AbilityLineFn, RVA_ABILITY_LIST)(traitLineIcon(&traitLines[i]), "%s", traitLines[i].label))
            count++;
    return count;
}

static unsigned itemTraitListHook(int unit, int item, unsigned long long flags, unsigned long long more) {
    unsigned count = GAME_FN(TraitListFn, RVA_UNIT_TRAIT_LIST)(unit, item, flags, more);
    int bits = itemResBits(item);
    if (!bits) return count;
    findOwnTraitPictures();
    for (int i = 0; i < traitLineCount; i++)
        if ((traitLines[i].resBit & bits) && GAME_FN(AbilityLineFn, RVA_ABILITY_LIST)(traitLineIcon(&traitLines[i]), "%s", traitLines[i].label))
            count++;
    return count;
}

static void findOwnTraitPictures(void) {
    static int done;
    if (done) return;
    done = 1;
    int mod = modIndexByName("CoE5SE");
    for (int i = 0; i < traitLineCount; i++) {
        TraitLine *line = &traitLines[i];
        if (!line->file) continue;
        char path[MAX_PATH];
        const char *reason = mod < 0 ? "no_coe5se_folder" : findModImage(modFolder(mod), line->file, path, sizeof path);
        if (!reason && (line->picture = addTraitPicture(path)) < 0) reason = "too_many_pictures";
        if (reason) writeLogWarning("trait_icon_missing", "line=%s reason=%s", line->label, reason);
    }
}

static void addOwnTraitLines(void) {
    static const struct { const char *label, *text, *file; int bit; } OWN[] = {
        {"Polymorph Immunity", "A unit with Polymorph Immunity holds its true shape and cannot be affected by polymorph spells.",
         "./icons/polymorph_immunity.tga", MONSTER_RES_POLYMORPH},
        {"Petrification Immunity", "A unit with Petrification Immunity cannot be turned to stone by petrifying gazes or spells.",
         "./icons/petrify_immunity.tga", MONSTER_RES_PETRIFY},
        {"Instant Death Immunity",
         "A unit with Instant Death Immunity cannot be slain outright by magic that kills instantly.  Other magic and weapons harm it as usual.",
         "./icons/kill_immunity.tga", MONSTER_RES_KILL},
        {"Feeblemind Immunity", "A unit with Feeblemind Immunity keeps its wits and cannot become feebleminded in battle.",
         "./icons/feeblemind_immunity.tga", MONSTER_RES_FEEBLE},
    };
    for (size_t i = 0; i < sizeof OWN / sizeof OWN[0]; i++) {
        int id = addTraitLine(OWN[i].label, OWN[i].text, TRAIT_LINE_DEFAULT_ICON, -1, OWN[i].bit, -1);
        if (id >= 0) traitLines[id].file = OWN[i].file;
    }
}

static int installTraitListHook(void) {
#ifndef __aarch64__
    unsigned char *stub = allocNear(GAME_PTR(RVA_UNIT_TRAIT_LIST_UNIT_CALL));
    if (!stub) {
        writeLogError("trait_lines_installed", "ok=0 reason=no_near_page");
        return 0;
    }
    return redirectCall(RVA_UNIT_TRAIT_LIST_UNIT_CALL, RVA_UNIT_TRAIT_LIST, stub, (void *)traitListHook, "unit_trait_list", NULL) +
           redirectCall(RVA_UNIT_TRAIT_LIST_ITEM_CALL, RVA_UNIT_TRAIT_LIST, stub + 0x40, (void *)itemTraitListHook, "item_trait_list", NULL) +
           redirectCall(RVA_ABILITY_LOOKUP_CALL, RVA_ABILITY_LOOKUP, stub + 0x20, (void *)traitLookupHook, "ability_lookup", NULL);
#else
    return pointWeaponTraitBranch(RVA_UNIT_TRAIT_LIST_UNIT_CALL, RVA_UNIT_TRAIT_LIST, (void *)traitListHook, "unit_trait_list") +
           pointWeaponTraitBranch(RVA_UNIT_TRAIT_LIST_ITEM_CALL, RVA_UNIT_TRAIT_LIST, (void *)itemTraitListHook, "item_trait_list") +
           pointWeaponTraitBranch(RVA_ABILITY_LOOKUP_CALL, RVA_ABILITY_LOOKUP, (void *)traitLookupHook, "ability_lookup");
#endif
}

static int lAddTraitLine(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_settop(L, 1);
    lua_getfield(L, 1, "label");
    lua_getfield(L, 1, "text");
    lua_getfield(L, 1, "icon");
    lua_getfield(L, 1, "visible");
    const char *label = lua_tostring(L, 2), *text = lua_tostring(L, 3);
    const char *mod = runningLuaMod();
    if (refuseFromDeadLuaScript(L, "coe5.addTraitLine")) {
        lua_pushboolean(L, 0);
        return 1;
    }
    const char *reason = !label || !label[0] ? "no_label" : strlen(label) > TRAIT_LINE_LABEL_MAX ? "label_too_long"
                       : !lua_isfunction(L, 5) ? "no_visible" : NULL;
    int icon = TRAIT_LINE_DEFAULT_ICON, picture = -1;
    const char *iconProblem = NULL;
    if (!reason && lua_isinteger(L, 4)) {
        lua_Integer n = lua_tointeger(L, 4);
        if (n < 0 || n >= RES_TRS_SPRITES) iconProblem = "bad_icon";
        else icon = (int)n;
    } else if (!reason && lua_type(L, 4) == LUA_TSTRING) {
        char path[MAX_PATH];
        int m = modIndexByName(mod);
        iconProblem = m < 0 ? "bad_icon" : findModImage(modFolder(m), lua_tostring(L, 4), path, sizeof path);
        if (!iconProblem && (picture = addTraitPicture(path)) < 0) iconProblem = "too_many_pictures";
    } else if (!reason && !lua_isnil(L, 4)) {
        iconProblem = "bad_icon";
    }
    if (iconProblem) writeLogWarning("trait_icon_fallback", "mod=%s label=%.63s reason=%s icon=%d", mod, label, iconProblem, TRAIT_LINE_DEFAULT_ICON);
    int lua = reason ? -1 : newLuaCallback(L, 5, 0), id = -1;
    if (!reason && (lua < 0 || (id = addTraitLine(label, text, icon, picture, 0, lua)) < 0)) reason = "out_of_memory";
    if (reason) {
        refuseLuaCall(mod, "coe5.addTraitLine", reason, "label=%.63s", label ? label : "");
        lua_pushboolean(L, 0);
        return 1;
    }
    writeLog("trait_line_added", "mod=%s label=%s id=%d", mod, label, id);
    lua_pushinteger(L, id);
    return 1;
}

static void traitLinesInit(void) {
    traitDescriptions = allocTraitDescriptions();
    if (!traitDescriptions) writeLogWarning("trait_descriptions_unavailable", "reason=no_memory_above_the_table");
    addOwnTraitLines();
    int hooked = installTraitListHook();
    writeResultLog(hooked == 3, "trait_lines_installed", "hooked=%d of 3 lines=%d descriptions=%d", hooked, traitLineCount,
                   traitDescriptions != NULL);
}
