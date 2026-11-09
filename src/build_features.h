#pragma once

#define COE5SE_FEATURES(X) \
    X(STACK_COOKIE, "stack_cookie") X(EVENT_FINDERS, "event_finders") X(BATTLE_SNAPSHOT, "battle_snapshot") \
    X(STRENGTH_INDEX, "strength_index") X(DROWN_SEARCH, "drown_search") X(TRACE_LOG_FILE, "trace_log_file") \
    X(MAIN_MENU, "main_menu") X(GAME_MENU, "game_menu") X(MAP_EDITOR, "map_editor") \
    X(COMMANDER_LINES, "commander_lines") X(GAME_SAVE, "game_save") X(KEY_ACTIONS, "key_actions") \
    X(NEEDS_WORD, "needs_word") X(RITUAL_EXPANSION, "ritual_expansion") X(SPELL_PATH_EXPANSION, "spell_path_expansion") \
    X(SPELL_TABLE_MOVE, "spell_table_move") X(TABLE_MOVES, "table_moves") X(FANPATCH, "fanpatch") \
    X(MISSING_MODS, "missing_mods") X(SAVE_COUNT, "save_count") X(MOD_VERSIONS, "mod_versions") \
    X(SPELL_WORDS, "spell_words") X(SPELL_PATH_WORDS, "spell_path_words") X(PLANE_WORDS, "plane_words") \
    X(FACTION_WORDS, "faction_words") X(TERRAIN_LOOKS, "terrain_looks") X(BATTLE_MAPS, "battle_maps") \
    X(MONSTER_FLAGS, "monster_flags") X(HATES, "hates") X(RITUAL_WORDS, "ritual_words") \
    X(CORPSE_KINDS, "corpse_kinds") X(WEAPON_TRAITS, "weapon_traits") X(NO_EXPLODE, "no_explode") X(RANDOM_EVENTS, "random_events") \
    X(NAME_LISTS, "name_lists") X(AI_CLASS, "ai_class") X(MAXDATA, "maxdata") X(PERSONALMOD, "personalmod") X(LAN_SCRIPTS, "lan_scripts") X(SIDECAR, "sidecar") \
    X(PLUGIN_CHECKS, "plugin_checks") X(MOD_SETTINGS, "mod_settings") X(LOAD_ORDER, "load_order")

typedef enum {
#define FEATURE_ENUM(id, name) FEATURE_##id,
    COE5SE_FEATURES(FEATURE_ENUM)
#undef FEATURE_ENUM
    FEATURE_COUNT
} Feature;

static const char *const FEATURE_NAMES[FEATURE_COUNT] = {
#define FEATURE_NAME(id, name) name,
    COE5SE_FEATURES(FEATURE_NAME)
#undef FEATURE_NAME
};

#ifndef COE5SE_LOCAL_FEATURES_ON
#define COE5SE_LOCAL_FEATURES_ON ""
#endif

static const char *buildOffList = "";

static int listHasWord(const char *list, const char *word) {
    size_t n = strlen(word);
    for (const char *p = list ? list : ""; *p;) {
        while (*p == ' ') p++;
        const char *end = p;
        while (*end && *end != ' ') end++;
        if ((size_t)(end - p) == n && n && !memcmp(p, word, n)) return 1;
        p = end;
    }
    return 0;
}

static int unitOffForBuild(const char *word) {
    return listHasWord(buildOffList, word) && !listHasWord(COE5SE_LOCAL_FEATURES_ON, word);
}

static void setBuildOffList(const char *list, const char *build) {
    buildOffList = list ? list : "";
    for (const char *p = buildOffList; *p;) {
        while (*p == ' ') p++;
        const char *end = p;
        while (*end && *end != ' ') end++;
        if (end > p) {
            char word[96];
            snprintf(word, sizeof word, "%.*s", (int)(end - p) < 95 ? (int)(end - p) : 95, p);
            if (listHasWord(COE5SE_LOCAL_FEATURES_ON, word))
                writeLogWarning("feature_on_locally", "name=%s reason=local_build build=%s", word, build);
            else
                writeLogError("feature_off", "name=%s reason=off_for_game_build build=%s", word, build);
        }
        p = end;
    }
}

static int featureOn(Feature f) {
    return (unsigned)f < FEATURE_COUNT && !unitOffForBuild(FEATURE_NAMES[f]);
}

static int luaFunctionOffForBuild(const char *call) {
    char word[96];
    if (!call || strncmp(call, "coe5.", 5)) return 0;
    snprintf(word, sizeof word, "lua:%.88s", call + 5);
    return unitOffForBuild(word);
}

static int siteNameOffForBuild(const char *name) {
    char word[96];
    if (!name) return 0;
    snprintf(word, sizeof word, "site:%.88s", name);
    return unitOffForBuild(word);
}
