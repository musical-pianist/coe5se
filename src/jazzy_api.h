#pragma once
#ifdef _WIN32
#include <windows.h>
#else
#include "platform_mac.h"
#endif
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define PLUGIN_API_VERSION 12

#define WRAP_PRIORITY_DEFAULT 0

#define RESTART_NONE     0
#define RESTART_ENDING   1
#define RESTART_STARTING 2

// ---- Game numbers ------------------------------------------------------------------------------

#define MAIN_MENU_START_NEW_GAME   1
#define MAIN_MENU_LOAD_GAME        2
#define MAIN_MENU_PREFERENCES      3
#define MAIN_MENU_NETWORK          4
#define MAIN_MENU_CREDITS          6
#define MAIN_MENU_QUIT             9
#define MAIN_MENU_MODS             10
#define MAIN_MENU_READ_TIP         11
#define GAME_MENU_STAY             0
#define GAME_MENU_CLOSE            1
#define GAME_MENU_RESIGN           6
#define GAME_MENU_SAVE             7
#define GAME_MENU_SAVE_AND_QUIT    2
#define GAME_MENU_SECTION_SAVING   0
#define GAME_MENU_SECTION_SETTINGS 1
#define MAP_ACTION_SPECIAL_POWER   5
#define MAP_ACTION_GAME_MENU       6
#define MAP_ACTION_SQUARE_INFO     96
#define MAP_ACTION_END_TURN        7
#define BOX_MESSAGES_OK            1
#define BOX_WARN_OK                1
#define BOX_FIRST_BUTTON           11
#define BOX_SECOND_BUTTON          12
#define BOX_CANCEL                 2
#define MAP_EDITOR_HIDE_TOOLBOX    2
#define KEY_NONE                   0
#define KEY_BACKSPACE              8
#define KEY_TAB                    9
#define KEY_ENTER                  13
#define KEY_ESC                    27
#define KEY_SPACE                  32
#define KEY_DELETE                 127
#define KEY_F1                     328
#define KEY_F2                     329
#define KEY_F3                     330
#define KEY_F4                     331
#define KEY_F5                     332
#define KEY_F6                     333
#define KEY_F7                     334
#define KEY_F8                     335
#define KEY_F9                     336
#define KEY_F10                    337
#define KEY_F11                    338
#define KEY_F12                    339
#define KEY_PAGE_DOWN              340
#define KEY_PAGE_UP                341
#define KEY_END                    342
#define KEY_HOME                   343
#define KEY_UP                     344
#define KEY_DOWN                   345
#define KEY_LEFT                   346
#define KEY_RIGHT                  347
#define KEY_CTRL                   100000
#define KEY_ALT                    200000
#define KEY_CTRL_SHIFT             300000
#define MESSAGE_EVERY_HUMAN        (-2)
#define MESSAGE_NOT_ADDED          (-1)

#define NET_ROLE_SINGLE            0
#define NET_ROLE_HOST              1
#define NET_ROLE_CLIENT            2
#define CONTROLLER_REMOVED         (-1)
#define CONTROLLER_HUMAN           0
#define CONTROLLER_AI              1
#define CONTROLLER_INDEPENDENT     2
#define CLASS_ABILITY_ID_AI_CLASS      203
#define CLASS_ABILITY_ID_RANDOM_SKIP   204
#define CLASS_ABILITY_ID_AI_LEVEL_MIN  210
#define CLASS_ABILITY_ID_AI_LEVEL_MAX  211
#define CLASS_ABILITY_ID_STDTROOPS     222
#define AI_CLASS_HUMAN_ONLY        (-1)
#define AI_CLASS_ANY               0
#define AI_CLASS_AI_ONLY           1
#define AI_CLASS_NEITHER           2
#define CORPSE_KIND_SMALL          0
#define CORPSE_KIND_HUMAN          1
#define CORPSE_KIND_GIANT          2
#define CORPSE_KIND_SMALL_ANIMAL   3
#define CORPSE_KIND_LARGE_ANIMAL   4
#define CORPSE_KIND_MARKATA        5
#define CORPSE_KIND_VANARA         6
#define CORPSE_KIND_BANDAR         7
#define GAME_OVER_MODE_PLAIN       4
#define GAME_OVER_MODE_FOG         9
#define NEW_WORLD_NORMAL           0
#define NEW_WORLD_NETWORK          1
#define NEW_WORLD_SKIP_PARTICIPANTS 2
#define MAP_SCREEN_OK              1
#define MAP_SETTINGS_OK            9
#define MAP_SETTINGS_SIZE_FIRST    100
#define MAP_SETTINGS_SOCIETY_FIRST 200
#define MAP_SIZE_SMALL             0
#define MAP_SIZE_MEDIUM            1
#define MAP_SIZE_LARGE             2
#define MAP_SIZE_HUGE              3
#define MAP_SIZE_ENORMOUS          4
#define START_POSITIONS_RANDOM     0
#define START_POSITIONS_CLUSTERED  1
#define START_POSITIONS_WEST_EAST  2
#define PARTICIPANTS_OK            1
#define RITUAL_REASON_MIRROR       (-2)
#define RITUAL_REASON_MAXCAST      (-3)
#define RITUAL_REASON_IMMORTAL     (-4)
#define RITUAL_REASON_MASTERY      (-6)
#define RITUAL_REASON_PLANE        (-7)
#define RITUAL_ROW_CASTABLE        1000
#define RITUAL_ROW_GREYED          12288
#define TARGET_USEFUL              1
#define TARGET_NOT_AFFECTED        (-3)
#define FILE_BROWSER_ROW_FIRST     10000
#define FILE_BROWSER_DELETE        7
#define SQUARE_INFO_OK             1
#define AI_MOVE_NONE               0
#define AI_MOVE_TARGET             1
#define AI_MOVE_EXPLORE            2
#define AI_NO_SQUARE               (-1)
#define AI_EXPLORE_BELOW           50
#define AI_FAR_EXPLORE_BELOW       10

#include "key_actions.h"

typedef enum {
    SITE_MAIN_MENU_WAIT,
    SITE_MAIN_MENU_PLAY_GAME,
    SITE_PLAY_GAME_NEW_WORLD,
    SITE_NEW_WORLD_MAP_SCREEN,
    SITE_NEW_WORLD_INIT_PARTICIPANTS,
    SITE_NEW_WORLD_CREATING_WORLD,
    SITE_MAP_SETTINGS_WAIT,
    SITE_PARTICIPANTS_WAIT,
    SITE_PARTICIPANTS_DIFFICULTY_WAIT,
    SITE_PARTICIPANTS_BUTTON_ROW,
    SITE_GAME_MENU_SAVING_SECTION_END,
    SITE_GAME_MENU_WAIT,
    SITE_RITUAL_SCREEN_ROW,
    SITE_RITUAL_SCREEN_LOCATION_CHECK,
    SITE_RITUAL_SCREEN_CAN_CAST,
    SITE_MAP_SCREEN_WAIT,
    SITE_MAP_MENU_BUTTON_TEXT,

    SITE_GAME_START,
    SITE_MONTH_START,
    SITE_PLAYER_TURN_START,
    SITE_HUMAN_TURN,
    SITE_AI_TURN,
    SITE_PLAYER_TURN_END,
    SITE_GAME_OVER,
    SITE_GAME_SAVE,
    SITE_GAME_LOAD,

    SITE_MODS_RUN,
    SITE_MOD_COMMAND,

    SITE_SQUARE_INFO_SCREEN,
    SITE_SQUARE_INFO_BUTTON_ROW,
    SITE_SQUARE_INFO_WAIT,

    SITE_TEXT_FORMAT,
    SITE_TEXT_BUILD,

    SITE_LOAD_GAME_BROWSER,
    SITE_FILE_BROWSER_ROW_DATE,
    SITE_FILE_BROWSER_WAIT,
    SITE_FILE_BROWSER_HOVERED,
    SITE_MAIN_MENU_MODS_SCREEN,
    SITE_MOD_BANNER_DRAW,
    SITE_MOD_ERROR_DIALOG,
    SITE_GAME_OVER_WAIT,
    SITE_EVENT_LINE,

    SITE_ARMY_WAIT,
    SITE_BATTLE_WAIT,
    SITE_MAP_EDITOR_WAIT,
    SITE_BATTLE_EDITOR_WAIT,
    SITE_RECRUIT_WAIT,
    SITE_VIEW_ARMY_WAIT,
    SITE_UNIT_LIST_WAIT,
    SITE_KEYBOARD_SHORTCUTS_WAIT,
    SITE_UNIT_LIST_SHEET,
    SITE_POLL_EVENT,
    SITE_KEY_TEXT_BAR,
    SITE_KEY_HELP_TEXT,
    SITE_KEY_TEXT_BUTTON,
    SITE_RUN_ONE_MOD,
    SITE_RUN_ONE_MOD_PATH,
    SITE_GAME_MENU_SETTINGS_SECTION_END,
    SITE_ELIMINATED_WARN,
    SITE_WARN_WAIT,
    SITE_TOOLTIP_LINE_DRAW,
    SITE_TURN_MESSAGES,
    SITE_MESSAGES_WAIT,
    SITE_AI_EXPLORE,
    SITE_AI_RALLY,
    SITE_AI_FRIENDLY_MOVE,
    SITE_AI_CONQUER,
    SITE_AI_FAR_EXPLORE,
    SITE_AI_RITUAL_CANDIDATE,
    SITE_AI_RECRUIT,
    SITE_AI_GATHER_TROOPS,
    SITE_AI_PICK_UP_LONER,
    SITE_AI_SPLIT_ARMY,
    SITE_CEREMONY_PERFORM,
    SITE_CEREMONY_COSTS,
    SITE_SEAL_BREAK,
    SITE_SEAL_EVENT,
    SITE_AI_MOVE_COMSTR,
    SITE_MAP_EDITOR,
    SITE_MAP_EDITOR_TOOLBOX_LINE,
    SITE_MAP_EDITOR_DRAW,
    SITE_RUN_ONE_MOD_LINE_READ,
    // @platform-split map_draw: Windows only; Mac's map screen frame wait wasn't tied to its draw pass
    SITE_MAP_DRAW,
    SITE_TOP_BAR,
    SITE_TOP_BAR_INCOME_DRAW,
    // @platform-split top_bar_income: Windows only; Mac's top bar income calls weren't looked up
    SITE_TOP_BAR_INCOME,
    // @platform-split message_box_wait: Windows only; Mac's message box wait wasn't looked up
    SITE_MESSAGE_BOX_WAIT,
    SITE_AFTER_GAME_TURN,
    SITE_DO_ALL_FIGHTS,
    SITE_MOVE_ALL_INDEPENDENTS,
    SITE_CONQUER_SQUARE,
    SITE_AI_DANGEROUS_STEP,
    SITE_AI_MAKE_GUARDS,
    SITE_AI_RITUAL_TARGET,
    SITE_DECORATE_SQUARE,
    SITE_BATTLE_DRAW,
    SITE_FIGHT_THE_FIGHT,
    SITE_BATTLE_ROUND,
    SITE_DAMAGE_UNIT,
    SITE_DOATTACK,
    SITE_BATTLE_SPELL_PICK,
    SITE_EXECUTE_RITUAL,
    SITE_NEW_UNIT,
    SITE_REMOVE_UNIT,
    SITE_TURN_UNIT_UPKEEP,
    SITE_SQUARE_UPKEEP,
    SITE_RECRUIT_BUY,
    SITE_PROMOTE_UNIT,
    SITE_MASTERY_TRANSFORM,
    SITE_BATTLE_SETUP,
    SITE_BATTLE_SETUP_SIDE,
    SITE_BATTLE_SETUP_AMBUSH,
    SITE_BATTLE_ADVANCE_UNIT,
    SITE_BATTLE_TURN_EFFECTS,
    SITE_BATTLE_TRANSFORM,
    SITE_FOUND_ITEM,
    SITE_REMOVE_DEAD_PLAYERS,
    SITE_CREATE_CITADEL,
    SITE_INQUISITION_SPAWN,
    SITE_ANIMAL_SUMMONING,
    SITE_ORDER_SLAVE_HUNT,
    SITE_ORDER_EAT_VILLAGE,
    SITE_WEAPON_HIT,
    SITE_BATTLE_TARGET_CHECK,
    SITE_UPDATE_VOID,
    SITE_VOID_SHUFFLE,
    SITE_PLANESHIFT_UNIT,
    SITE_NEW_PORTAL_PAIR,
    SITE_PLANE_DISCOVERED_PASS,
    SITE_MOVE_INFERNO_PLAYER,
    SITE_MOVE_HADES_PLAYER,
    SITE_MOVE_CELESTIAL_PLAYER,
    SITE_POPULATE_CELESTIAL,
    SITE_POPULATE_SKY,
    SITE_POPULATE_AGARTHA,
    SITE_POPULATE_INFERNO,
    SITE_POPULATE_HADES,
    SITE_POPULATE_AZTLAN,
    SITE_POPULATE_PRIMAL,
    SITE_POPULATE_ELEMENTAL,
    SITE_PLANE_RISE_TO_ELYSIUM,
    SITE_ENLARGE_PLANE,
    SITE_GEN_FLOOD_PLANE,
    SITE_ADDPLANE,
    SITE_LOAD_MAP_PLANE,
    SITE_UNIT_ADD_XP,
    SITE_USE_SPECIAL_SITE,
    SITE_LONER_STEP,
    SITE_REMOVE_TEMP_MEN,
    SITE_MOVE_WANDERING_MONSTERS,
    SITE_MOVE_PLAYER_STUPIDS,
    SITE_OVERRUN_CHECK,
    SITE_KILL_UNIT_999,
    SITE_DAMAGE_UNIT_PLAIN,
    SITE_CONTINUE_MOVE_ORDERS,
    SITE_HUMAN_MULTI_MOVE,
    SITE_GROUP_MOVE,
    SITE_MOVE_ORDER_ROUTE,
    SITE_UNIT_REMOVE_ITEM,
    SITE_MEMORIZE_SPELL,
    SITE_LEARN_RANDOM_SPELL,
    SITE_GET_START_RITUALS,
    SITE_COMMANDER_LEARN_RITUAL,
    SITE_BOLSTER_BLESSING,
    SITE_ADD_PLAYER_BLESSING,
    SITE_REMOVE_ENCHANT,
    SITE_RITUAL_PAY_COSTS,
    SITE_RITUAL_FAR_SUMMON_UNITS,
    SITE_PRIEST_FREESPAWN,
    SITE_EVENT_KILL_SQUARE,
    SITE_GIVE_ITEM_BY_NAME,
    SITE_TAKE_ITEM_BY_NAME,
    SITE_GIVE_RANDOM_ITEM,
    SITE_SQUAREEVENT_START,
    SITE_TERRAIN_PRESENT_RESCAN,
    SITE_SHOP_OFFER_MESSAGE,
    SITE_WORLD_FIRE_UPDATE,
    SITE_EVENT_VAR_CHANGED,
    SITE_CONSCRIPTION,
    SITE_GAME_STATISTICS,
    SITE_ALTER_TERRAIN,
    SITE_RUIN_SQUARE,
    SITE_DESTROY_GATEWAY,
    SITE_DIG_TUNNEL_SQUARE,
    SITE_MONTH_SQUARE_RESET,
    SITE_ROLL_CLASS,
    SITE_RESET_INCOME_CACHES,
    SITE_TEAM_START_LOCATIONS,
    SITE_TRIM_TEAM_LOCATIONS,
    SITE_SWAP_TEAM_LOCATIONS,
    SITE_CREATE_CITADELS_STEP,
    SITE_FIX_PORTALS,
    SITE_VOID_FLUCTUATIONS,
    SITE_PLANESHIFT_COMMANDER,
    SITE_MIST_SPREAD,
    SITE_TEMPERATURE_CHANGE,
    SITE_SPECIAL_SITE_POPULATE,
    SITE_UNIT_SWALLOW,
    SITE_UNIT_UNSWALLOW,
    SITE_LIFEFORCE_DRAIN_UNIT,
    SITE_UNIT_CATCH_FIRE,
    SITE_UNIT_CRUSH,
    SITE_REMOVE_BAD_FOLLOWERS,
    SITE_OVERPOP_CULL,
    SITE_MONKEY_CLOAK,
    SITE_RECYCLE_UNITS,
    SITE_RANDOM_TELEPORT,
    SITE_POP_HORRORS,
    SITE_KILL_TERRAIN_UNITS,
    SITE_KILL_SUNKEN_SHIPS,
    SITE_CREATE_BOATS,
    SITE_BATTLE_TRAPS,
    SITE_BATTLE_ENCHANT_TRAPS,
    SITE_BATTLE_SUMMON_STRING,
    SITE_PAY_SPELL_LIFEFORCE,
    SITE_WORLD_NEWS,
    SITE_SEASON_NEWS,
    SITE_CHECK_GAME_WON,
    SITE_SPY_CAUGHT,
    SITE_TOGGLE_AUTO_RECRUIT,
    SITE_CHANGE_LOYALTY,
    SITE_ALTER_TERRAIN_RECT,
    SITE_RESPAWN_VOID,
    SITE_BATTLE_SIEGE_UNIT,
    SITE_GATHER_SHIPS,
    SITE_ADVANCE_SHIPS,
    SITE_MOVE_ENCHANTS,
    SITE_MOVE_GEMS,
    SITE_TAINTED_RITUAL_EFFECT,
    SITE_LOCATION_SWAP,
    SITE_APE_POSSESSION,
    SITE_RITUAL_ASK_HEMISPHERE,
    SITE_GET_TITHES,
    SITE_ELIMINATE_PLAYER,
    SITE_FIND_START_SQUARE,
    SITE_TROLL_FOREST_SPAWN,
    SITE_AI_MAKE_DEFENDERS,
    SITE_AI_REDISTRIBUTE_ITEMS,
    SITE_AI_SHIPS,
    SITE_AI_SHIP_DROP_CREW,
    SITE_AI_SHIP_TAKE_BOAT,
    SITE_AI_SHIP_TAKE_CREW,
    SITE_CHECK_FOLLOWER_LINKS,
    SITE_STEP_BURN_FOREST,
    SITE_ORDER_BURN_FOREST,
    SITE_ORDER_CHOP_FOREST,
    SITE_LIFEFORCE_DRAIN_SQUARE,
    SITE_SPEND_ALL_AP_AT,
    SITE_SWAP_SQUARES,
    SITE_TELEPORT_UNIT,
    SITE_UNIT_CLEAR_BENEFITS,
    SITE_GAME_MENU,
    SITE_ARMY_COMMANDS_MENU,
    SITE_CEREMONY_SCREEN,
    SITE_INCOME_DETAIL_SCREEN,
    SITE_BATTLE_CREATE_TREES,
    SITE_BATTLE_DEPLOY,
    SITE_BATTLE_FIGHT_MOVE,
    SITE_BATTLE_FLY_MOVE,
    SITE_BATTLE_FLEE,
    SITE_BATTLE_FEAR_CHECK,
    SITE_BATTLE_CLOUD_STEP,
    SITE_BATTLE_ENSNARE_CHECK,
    SITE_BATTLE_ENSNARE_RELEASE,
    SITE_BATTLE_WALL_ENCHANTS,
    SITE_BATTLE_WALL_EFFECTS,
    SITE_BATTLE_START_SETUP,
    SITE_BATTLE_TUNNEL_MOVE,
    SITE_BATTLE_BANISH,
    SITE_BATTLE_DEPLOY_UNIT,
    SITE_BATTLE_AFTER_FIGHT_UPKEEP,
    SITE_BATTLE_AFTERMATH_RETURN,
    SITE_BATTLE_MR_ROLL,
    SITE_BATTLE_SUMMON_WEAPON,
    SITE_BATTLE_GATE_SWAP,
    SITE_BATTLE_SPREAD_COLLAPSE,
    SITE_UNIT_CURE_AFFLICTION,
    SITE_UNIT_HP_SHAPE_CHANGE,
    SITE_BERSERK_END,
    SITE_UNIT_RANDOM_AFFLICTION,
    SITE_REVEAL_TERRAIN_ON_PLANE,
    SITE_NEAR_TERRAIN_SQUARE,
    SITE_PLAY_SAMPLE,
    SITE_GROVE_REVERSION,
    SITE_PUT_RARE_SQUARE,
    SITE_RANDOM_ITEM_PICK,
    SITE_CREATE_RANDOM_MAP,
    SITE_WORLD_PART_TERRAIN,
    SITE_MAPGEN_REMOVE_SMALL_ISLANDS,
    SITE_MAPGEN_RIVER_CROSSINGS,
    SITE_MAPGEN_ANCIENT_FOREST,
    SITE_MAPGEN_CLEAR_AREA,
    SITE_MAPGEN_ADJUST_TEMPERATURE,
    SITE_MARK_RIVER,
    SITE_GEN_DIAGONAL_FIX,
    SITE_GEN_UPDATE_MOIST_WALLS,
    SITE_GEN_TERRAIN_BLOB,
    SITE_CREATE_CELESTIAL_PLANE,
    SITE_CREATE_SKY_PLANE,
    SITE_CREATE_AGARTHA_PLANE,
    SITE_CREATE_INFERNO_PLANE,
    SITE_CREATE_HADES_PLANE,
    SITE_CREATE_ELEMENTAL_PLANE,
    SITE_LOAD_AZTLAN_PLANE,
    SITE_LOAD_PRIMAL_PLANE,
    SITE_AUTO_RECRUIT,
    SITE_USE_LIBRARY,
    SITE_ADD_GROUND_ITEMS,
    SITE_VOID_INSANITY,
    SITE_WANDERER_PICK_TARGET,
    SITE_WANDERER_PICK_UP,
    SITE_SET_HOLDIT,
    SITE_LIBRARY_SCREEN,
    SITE_CHOP_BURN_SCREEN,
    SITE_SLAVE_HUNT_SCREEN,
    SITE_UNIT_STATS_SCREEN,
    SITE_ITEM_INFO_SCREEN,
    SITE_WEAPON_INFO_WINDOW,
    SITE_PLANE_INFO_SCREEN,
    SITE_BATTLE_INTRO_SCREEN,
    SITE_COMMANDER_SET_DONE,
    SITE_SELECT_NEXT_COMMANDER,
    SITE_SELECT_PREV_COMMANDER,
    SITE_RUN_HATREDS,
    SITE_SANDSTORM_CLOUD_UPDATE,
    SITE_TAINTED_MAGIC_PASS,
    SITE_HERO_ARRIVAL,
    SITE_CLEAN_SQUARE,
    SITE_ADJUST_FOR_SNOW,
    SITE_START_TUNE,
    SITE_SQUARE_SET_OWNER,
    SITE_TEAMLESS_OWN_TEAM,
    SITE_ADD_BENEFIT,
    SITE_SOCIETY_PRESETS,
    SITE_MOVE_STEP_AFTER,
    SITE_NUMBER_BOX,
    SITE_ENCHANTS_DESTROYED_AT,
    SITE_STORM_SPAWN,
    SITE_WILD_SPAWN,
    SITE_REVELS_SPAWN,
    SITE_HORROR_LORD_PICK,
    SITE_CLIENT_TURN,
    SITE_SETUP_SERVER,
    SITE_SQUARE_PANEL_ICON,
    SITE_SQUARE_PANEL_LAYOUT,
    SITE_SQUARE_PANEL_WRAP,
    SITE_COUNT
} CallSite;

#define UI_KEY_ENTER  0x0d
#define UI_KEY_ESCAPE (-1)

typedef struct UiApi {
    void (*screenBegin)(const char *title);
    void (*sectionBegin)(const char *header);
    void (*sectionEnd)(void);
    void (*text)(int id, const char *text);
    void (*checkbox)(int id, int *value, const char *label);
    void (*choice)(int id, int selected, const char *label);
    void (*tooltip)(const char *text);
    void (*buttonRow)(void);
    void (*bindKey)(int id, int key);
    int (*wait)(void);
    void (*listBegin)(void);
    void (*listRow)(int id, const char *text);
    void (*listEnd)(void);
} UiApi;

typedef struct CallInfo {
    unsigned long long args[6];
    unsigned long long result;
} CallInfo;

typedef struct PluginApi {
    int version;
    int (*wrap)(CallSite site, void *hook, void **next);
    uintptr_t (*callerRsp)(CallSite site);
    int (*setupAutomated)(void);
    void (*setSetupAutomated)(int on);
    int (*addModCommand)(const char *name, void (*handler)(const char *args, int mod, int level));
    int (*addGameMenuLine)(const char *label, const char *tooltip, int (*visible)(void), int (*onClick)(void));
    int (*sendMessage)(int player, const char *title, const char *text);
    const UiApi *ui;
    int (*addScreen)(const char *label, const char *tooltip, void (*open)(void));
    int (*addMainMenuButton)(const char *label, void (*onClick)(void));
    int (*observe)(CallSite site, void (*before)(const CallInfo *call), void (*after)(const CallInfo *call));
    int (*settingsPluginCount)(void);
    const char *(*settingsPluginName)(int plugin);
    void (*settingsPluginDraw)(int plugin);
    int (*settingsPluginClick)(int id);
    int (*hideMainMenuButton)(int id, const int *hidden);
    int (*addMainMenuButtonAfter)(const char *label, void (*onClick)(void), int vanillaId);
    void (*logLine)(const void *self, const char *text);
    int (*wrapWithPriority)(CallSite site, void *hook, void **next, int priority);
    int (*restartGame)(void);
    int (*restartPhase)(void);
    void *(*frameVariable)(CallSite site, const char *name);
    const int *(*setting)(const char *name);
    int (*rebindKey)(int action, int key);
    int (*addActionKey)(int action, int key);
    int (*actionKey)(int action);
} PluginApi;

#ifdef __APPLE__
#define COE5SE_DLL "coe5se.c5plugin.dylib"
#else
#define COE5SE_DLL "coe5se.c5plugin.dll"
#endif

#define COE5SE_PLUGIN_LOAD(apiName) \
    __declspec(dllexport) const int coe5sePluginApiVersion = PLUGIN_API_VERSION; \
    __declspec(dllexport) int coe5sePluginLoad(const PluginApi *apiName)

static inline const PluginApi *jazzyConnect(void) {
    HMODULE coe5se = GetModuleHandleA(COE5SE_DLL);
    typedef const PluginApi *(*GetApiFn)(int);
    GetApiFn getApi = coe5se ? (GetApiFn)(void *)GetProcAddress(coe5se, "jazzyGetApi") : NULL;
    return getApi ? getApi(PLUGIN_API_VERSION) : NULL;
}

#ifndef PRINTF_FORMAT
#if defined(__MINGW32__) && !defined(__clang__)
#define PRINTF_FORMAT(fmtIndex, firstArg) __attribute__((format(gnu_printf, fmtIndex, firstArg)))
#elif defined(__GNUC__)
#define PRINTF_FORMAT(fmtIndex, firstArg) __attribute__((format(printf, fmtIndex, firstArg)))
#else
#define PRINTF_FORMAT(fmtIndex, firstArg)
#endif
#endif

static inline PRINTF_FORMAT(1, 2) void jazzyLog(const char *fmt, ...) {
    static const PluginApi *connected;
    if (!connected) connected = jazzyConnect();
    if (!connected) return;
    char text[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(text, sizeof text, fmt, ap);
    va_end(ap);
    connected->logLine((const void *)jazzyLog, text);
}
