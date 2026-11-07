// @platform-split preview_world: Windows only for now; on Mac the coe5 calls don't exist, so the Plane Generator shows no button
#pragma once

#define PREVIEW_SEATS_MAX      24
#define PREVIEW_HUMAN_SLOT     0
#define PREVIEW_CARD_DIAMOND_ID (-9)
#define PREVIEW_MAP_SIZE_COUNT 5
#define PREVIEW_OCCURRENCE_MAX 2048
#define PREVIEW_HASH_BASIS     2166136261u
#define PREVIEW_HASH_PRIME     16777619u
#define PREVIEW_CHURN_INTS     15
#define PREVIEW_CHURN_COUNT_A  50
#define PREVIEW_CHURN_COUNT_B  8000
#define WILDER_OFF             1
#define WILDER_ON              2
#define START_FLAG_CLUSTERED   0x8
#define START_FLAG_WEST_EAST   0x10
#define PREVIEW_DOCKS_MAX      4
#define PREVIEW_HOVER_TEXT_SIZE 200
#define PREVIEW_HOVER_SLOT     (TOOLTIP_SLOTS - 1)

typedef struct {
    int asked;
    int active;
    int ending;
    int centered, seats, firstMonthDone;
    int seed, mapSize, society, wilder, startPositions, uniqueRandom, tries;
    int seatClass[PREVIEW_SEATS_MAX];
    int settingsFrame;
    char plane[PLANE_NAME_SIZE];
    void *owner;
    int savedAutosave, savedSociety, savedWilder, savedStartFlags;
    int savedChurn[PREVIEW_CHURN_INTS];
    unsigned long long startedMs;
} PreviewWorld;
static PreviewWorld previewWorld;
static void previewReadAllLines(void);
static void previewNoteWorldEnding(void);
static void previewTerrainStepAll(int source);
static void previewNoteNewUnit(const CallInfo *call);
static double previewBuildSeconds = -1;

static struct { unsigned key; int count; } previewOccurrences[PREVIEW_OCCURRENCE_MAX];

static int previewWorldOn(void) { return previewWorld.active; }
static int previewWorldEnding(void) { return previewWorld.active && previewWorld.ending; }
static int previewWorldAsked(void) { return previewWorld.asked; }

static void dropUnusedPreviewAsk(void) {
    if (!previewWorld.asked) return;
    previewWorld.asked = 0;
    writeLog("preview_world", "phase=dropped reason=no_play_game");
}

// ---- The dice, pure ------------------------------------------------------------------------------
static UNUSED unsigned previewHashText(const char *s, unsigned h) {
    for (; *s; s++) h = (h ^ (unsigned char)*s) * PREVIEW_HASH_PRIME;
    return h;
}

static UNUSED unsigned previewHashInt(unsigned h, int value) { return (h ^ (unsigned)value) * PREVIEW_HASH_PRIME; }

static UNUSED unsigned previewDiceKey(int seed, const char *what, int a, int b) {
    unsigned h = previewHashText(what, PREVIEW_HASH_BASIS ^ (unsigned)seed);
    return previewHashInt(previewHashInt(h, a), b);
}

static UNUSED int previewOccurrence(unsigned key) {
    if (!key) key = 1;
    for (int n = 0; n < PREVIEW_OCCURRENCE_MAX; n++) {
        int i = (int)((key + (unsigned)n) % PREVIEW_OCCURRENCE_MAX);
        if (previewOccurrences[i].key == key) return ++previewOccurrences[i].count;
        if (!previewOccurrences[i].key) {
            previewOccurrences[i].key = key, previewOccurrences[i].count = 1;
            return 1;
        }
    }
    return 0;
}

static UNUSED int previewMapSettingsAnswer(int frame, int mapSize, int society) {
    return frame == 0 ? MAP_SETTINGS_SIZE_FIRST + mapSize : frame == 1 ? MAP_SETTINGS_SOCIETY_FIRST + society : MAP_SETTINGS_OK;
}

static UNUSED int previewMapIdIsOrder(int id) {
    static const short ORDERS[] = {1, 2, 4, 5, MAP_ACTION_END_TURN, 0xe, 0xf, 0x10, 0x11, 0x12, 0x26, 0x27, 0x28, 0x4b, 0x4c, 0x4f, 0x61, 0x62, 0x44d};
    for (size_t i = 0; i < sizeof ORDERS / sizeof ORDERS[0]; i++)
        if (id == ORDERS[i]) return 1;
    if (id >= 0x1e && id <= 0x25) return 1;
    int magnitude = id < 0 ? -id : id;
    return magnitude >= MAP_COMMANDER_ID && magnitude < 2 * MAP_COMMANDER_ID;
}

static UNUSED int previewStartFlags(int flags, int startPositions) {
    return (flags & ~(START_FLAG_CLUSTERED | START_FLAG_WEST_EAST)) |
           (startPositions == START_POSITIONS_CLUSTERED ? START_FLAG_CLUSTERED : startPositions == START_POSITIONS_WEST_EAST ? START_FLAG_WEST_EAST : 0);
}

static UNUSED int previewSeatController(int slot, int seats) {
    return slot >= seats ? CONTROLLER_REMOVED : slot == PREVIEW_HUMAN_SLOT ? CONTROLLER_HUMAN : CONTROLLER_AI;
}

static int previewMenuIdIsRefused(int id) { return id == GAME_MENU_SAVE || id == GAME_MENU_SAVE_AND_QUIT || id == GAME_MENU_RESIGN; }

static UNUSED const char *previewStartRefusal(int inMainMenu, int network, int active, const char *plane, long long seed, long long mapSize,
                                       long long society, long long startPositions, long long seats) {
    return !inMainMenu ? "not_in_main_menu"
         : network ? "network_game"
         : active ? "already_active"
         : !plane || !plane[0] || strlen(plane) >= PLANE_NAME_SIZE ? "bad_plane"
         : seed < INT_MIN || seed > INT_MAX ? "bad_seed"
         : mapSize < 0 || mapSize >= PREVIEW_MAP_SIZE_COUNT ? "bad_map_size"
         : society < 0 || society >= SOCIETY_TABLE_MAX ? "bad_society"
         : startPositions < START_POSITIONS_RANDOM || startPositions > START_POSITIONS_WEST_EAST ? "bad_start_positions"
         : seats < 1 || seats > PREVIEW_SEATS_MAX ? "bad_seats"
         : NULL;
}

// ---- The docks, pure -----------------------------------------------------------------------------
typedef struct { float x, y, w, h; } PreviewDock;
static PreviewDock previewDocks[PREVIEW_DOCKS_MAX];
static int previewDockCount;
static char previewHoverText[PREVIEW_HOVER_TEXT_SIZE];

enum { PREVIEW_PRESS_NONE, PREVIEW_PRESS_DOCK, PREVIEW_PRESS_MAP };
static int previewPress;

static UNUSED int previewPointerOverDocks(const PreviewDock *docks, int count, float x, float y) {
    for (int i = 0; i < count; i++)
        if (x >= docks[i].x && x < docks[i].x + docks[i].w && y >= docks[i].y && y < docks[i].y + docks[i].h) return 1;
    return 0;
}

static UNUSED int previewPressStep(int press, int buttons, int over) {
    if (!buttons) return PREVIEW_PRESS_NONE;
    return press != PREVIEW_PRESS_NONE ? press : over ? PREVIEW_PRESS_DOCK : PREVIEW_PRESS_MAP;
}

static UNUSED int previewDockTakesInput(int press, int over) { return press == PREVIEW_PRESS_DOCK || (press == PREVIEW_PRESS_NONE && over); }

#define PREVIEW_LONG_PRESS_MS    500
#define PREVIEW_LONG_PRESS_MOVED 25.0f
typedef struct {
    int held, id, settled;
    int letGo;
    unsigned long long at;
    float x, y, moved;
} PreviewLeftPress;
static PreviewLeftPress previewLeftPress;

static UNUSED int previewLeftPressStep(PreviewLeftPress *press, int *buttons, int id, int onSquare, float x, float y, unsigned long long now) {
    if (*buttons != 1) {
        int kept = press->held ? press->id : 0;
        press->held = 0;
        press->letGo = *buttons == 0;
        return kept ? kept : id;
    }
    *buttons = 0;
    if (!press->held) {
        *press = (PreviewLeftPress){.held = 1, .id = id, .settled = id != 0 || !onSquare || !press->letGo, .at = now, .x = x, .y = y};
        return 0;
    }
    press->moved += fabsf(x - press->x) + fabsf(y - press->y);
    press->x = x, press->y = y;
    if (now - press->at < PREVIEW_LONG_PRESS_MS) return 0;
    int kept = press->id;
    press->id = 0;
    if (kept) return kept;
    if (!press->settled && press->moved < PREVIEW_LONG_PRESS_MOVED) *buttons = 2;
    press->settled = 1;
    return 0;
}

// ---- A tool's text field, pure -------------------------------------------------------------------
#define PREVIEW_FIELD_KEYS_MAX 50
#define PREVIEW_FIELD_TEXT_MAX 4
#define SDL_SCANCODE_COUNT     512
#define SDL_TEXT_INPUT         0x303
#define SDL_EVENT_REPEAT       13
#define SDL_EVENT_SCANCODE     16
#define SDL_EVENT_MOD          24
#define SDL_EVENT_TEXT         12
#define SDLK_F4                0x4000003d
#define SDLK_KP_ENTER          0x40000058
#define SDLK_M                 0x6d
#define SDL_KMOD_ALT           0x300
#define SDL_KMOD_GUI           0xc00
typedef struct { int key; char text[PREVIEW_FIELD_TEXT_MAX + 1]; } PreviewFieldKey;
typedef struct {
    int on;
    int count;
    PreviewFieldKey keys[PREVIEW_FIELD_KEYS_MAX];
    unsigned char held[SDL_SCANCODE_COUNT];
    int heldDown;
} PreviewField;
static PreviewField previewField;

static const char *const PREVIEW_FIELD_LETTERS[] = {"\xc3\xa5", "\xc3\xa4", "\xc3\xb6", "\xc3\xbc", "\xc3\xab", "\xc3\xaa", "\xc3\xa9", "\xc3\xa8",
                                                    "\xc3\x85", "\xc3\x84", "\xc3\x96", "\xc3\x9c", "\xc3\x8b", "\xc3\x8a", "\xc3\x89", "\xc3\x88"};

static int previewFieldTextFits(const char *text) {
    if ((unsigned char)text[0] >= 0x20 && (unsigned char)text[0] < 0x7f && !text[1]) return 1;
    for (size_t i = 0; i < sizeof PREVIEW_FIELD_LETTERS / sizeof PREVIEW_FIELD_LETTERS[0]; i++)
        if (!strcmp(text, PREVIEW_FIELD_LETTERS[i])) return 1;
    return 0;
}

static int previewFieldKeyIsWindowKey(int sym, int mod) {
    return ((mod & SDL_KMOD_ALT) && (sym == KEY_TAB || sym == KEY_ENTER || sym == SDLK_F4)) || ((mod & SDL_KMOD_GUI) && sym == SDLK_M);
}

static void previewFieldAddKey(PreviewField *field, int key, const char *text) {
    if (field->count >= PREVIEW_FIELD_KEYS_MAX) return;
    PreviewFieldKey *k = &field->keys[field->count++];
    k->key = key;
    snprintf(k->text, sizeof k->text, "%s", text);
}

static int previewFieldStep(PreviewField *field, int taking, const unsigned char *e) {
    unsigned type;
    memcpy(&type, e, sizeof type);
    if (type == SDL_WINDOW_EVENT) {
        if (e[SDL_EVENT_WINDOW_EVENT] == SDL_FOCUS_GAINED || e[SDL_EVENT_WINDOW_EVENT] == SDL_FOCUS_LOST) {
            memset(field->held, 0, sizeof field->held);
            field->heldDown = 0;
        }
        return 0;
    }
    if (type == SDL_TEXT_INPUT) {
        const char *text = (const char *)e + SDL_EVENT_TEXT;
        if (!taking) return field->heldDown;
        if (previewFieldTextFits(text)) previewFieldAddKey(field, 0, text);
        return 1;
    }
    if (type != SDL_KEY_DOWN && type != SDL_KEY_UP) return 0;
    int scancode, sym;
    unsigned short mod;
    memcpy(&scancode, e + SDL_EVENT_SCANCODE, sizeof scancode);
    memcpy(&sym, e + SDL_EVENT_SYM, sizeof sym);
    memcpy(&mod, e + SDL_EVENT_MOD, sizeof mod);
    int slot = scancode >= 0 && scancode < SDL_SCANCODE_COUNT;
    if (type == SDL_KEY_UP) {
        if (slot && field->held[scancode]) field->heldDown = 0;
        if (slot) field->held[scancode] = 0;
        return 0;
    }
    if (!taking) {
        field->heldDown = slot && field->held[scancode];
        return field->heldDown;
    }
    if (previewFieldKeyIsWindowKey(sym, mod)) return 0;
    if (slot) field->held[scancode] = 1;
    field->heldDown = 1;
    int repeat = e[SDL_EVENT_REPEAT];
    if (sym == KEY_BACKSPACE) previewFieldAddKey(field, KEY_BACKSPACE, "");
    else if (!repeat && sym == KEY_ESC) previewFieldAddKey(field, KEY_ESC, "");
    else if (!repeat && (sym == KEY_ENTER || sym == SDLK_KP_ENTER)) previewFieldAddKey(field, KEY_ENTER, "");
    return 1;
}

static int previewFieldTakesEvent(const void *event) {
    return previewFieldStep(&previewField, previewField.on && previewWorld.active && mapScreenWaiting > 0, event);
}

// ---- The dice in the game ------------------------------------------------------------------------
#ifndef __APPLE__
static const unsigned PREVIEW_CHURN_RVAS[PREVIEW_CHURN_INTS] = {
    RVA_RAND_CHURN_0, RVA_RAND_CHURN_1, RVA_RAND_CHURN_2, RVA_RAND_CHURN_3, RVA_RAND_CHURN_4, RVA_RAND_CHURN_5, RVA_RAND_CHURN_6, RVA_RAND_CHURN_7,
    RVA_RAND_CHURN_8, RVA_RAND_CHURN_9, RVA_RAND_CHURN_10, RVA_RAND_CHURN_11, RVA_RAND_CHURN_12, RVA_RAND_CHURN_COUNT_A, RVA_RAND_CHURN_COUNT_B,
};

static void previewSetDice(unsigned value) {
    for (int i = 0; i < PREVIEW_CHURN_INTS - 2; i++) G32(PREVIEW_CHURN_RVAS[i]) = 0;
    G32(RVA_RAND_CHURN_COUNT_A) = PREVIEW_CHURN_COUNT_A, G32(RVA_RAND_CHURN_COUNT_B) = PREVIEW_CHURN_COUNT_B;
    GAME_FN(void (*)(int), RVA_RANDOM_SET_SEED)((int)value);
    GAME_FN(void (*)(unsigned), RVA_CRT_SRAND)(value);
}

static unsigned previewCrtRandState(void) { return *(unsigned *)(GAME_FN(char *(*)(void), RVA_CRT_THREAD_DATA)() + CRT_RAND_STATE_OFFSET); }

static void previewRestartDice(const char *what, int a, int b) {
    if (!previewWorld.active) return;
    unsigned key = previewDiceKey(previewWorld.seed, what, a, b);
    previewSetDice(previewHashInt(key, previewOccurrence(key)));
}

static void previewRestartDiceNamed(const char *what, const char *name, int b) {
    if (!previewWorld.active) return;
    previewRestartDice(what, (int)previewHashText(name, 5381), b);
}

static void previewRestartDiceSquare(const char *monster, int area, int x, int y) {
    if (!previewWorld.active) return;
    unsigned h = previewHashText(monster, PREVIEW_HASH_BASIS ^ (unsigned)previewWorld.seed);
    previewSetDice(previewHashInt(previewHashInt(previewHashInt(h, area), x), y));
}

static void previewRestartDiceEvent(const char *line, int level) {
    if (!previewWorld.active || level != 3) return;
    if (strncmp(line, "playerevent", 11) && strncmp(line, "squareevent", 11)) return;
    previewRestartDice("event", (int)previewHashText((const char *)GAME_PTR(RVA_EVENT_FILE), (unsigned)G32(RVA_MOD_LINE)), G32(RVA_ACTIVE_PLAYER));
}

// ---- The world's steps ---------------------------------------------------------------------------
static int (*nextPreviewNewWorld)(int);
static int (*nextPreviewMapScreen)(void);
static int (*nextPreviewMapSettingsWait)(int, int);
static void (*nextPreviewCreatingWorld)(int, const char *);
static int (*nextPreviewMapWait)(int, int);

static int previewNewWorld(int mode) {
    if (!previewWorld.active) return nextPreviewNewWorld(mode);
    apiSetSetupAutomated(1);
    int result = nextPreviewNewWorld(NEW_WORLD_SKIP_PARTICIPANTS);
    apiSetSetupAutomated(0);
    previewSetSource(PREVIEW_SOURCE_WORLD);
    previewTerrainStepAll(PREVIEW_SOURCE_WORLD);
    return result;
}

static int previewMapScreen(void) {
    if (!previewWorld.active) return nextPreviewMapScreen();
    previewWorld.settingsFrame = 0;
    previewSetDice((unsigned)previewWorld.seed);
    nextPreviewMapScreen();
    G32(RVA_WILDER) = previewWorld.wilder ? WILDER_ON : WILDER_OFF;
    G16(RVA_START_FLAGS) = (short)previewStartFlags(G16(RVA_START_FLAGS), previewWorld.startPositions);
    return MAP_SCREEN_OK;
}

static int previewMapSettingsWait(int block, int mode) {
    if (!previewWorld.active) return nextPreviewMapSettingsWait(block, mode);
    return previewMapSettingsAnswer(previewWorld.settingsFrame++, previewWorld.mapSize, previewWorld.society);
}

static void writePreviewSeats(void) {
    for (int slot = 0; slot < PREVIEW_SEATS_MAX; slot++) {
        int controller = previewSeatController(slot, previewWorld.seats);
        *(short *)playerSlotAddress(RVA_SLOT_CONTROLLER, slot, 2) = (short)controller;
        *(short *)playerSlotAddress(RVA_SLOT_CLASS, slot, 2) = controller == CONTROLLER_REMOVED ? (short)-1 : (short)previewWorld.seatClass[slot];
    }
    *(int *)(siteCallerSp(SITE_NEW_WORLD_CREATING_WORLD) + FRAME_NEW_WORLD_UNIQUE_RANDOM) = previewWorld.uniqueRandom;
}

static void previewFirstMonthDone(const CallInfo *call) {
    (void)call;
    if (!previewWorld.active || previewWorld.firstMonthDone) return;
    previewWorld.firstMonthDone = 1;
    previewTerrainStepAll(PREVIEW_SOURCE_MONTH);
    previewSetSource(PREVIEW_SOURCE_WORLD);
}

static void previewCreatingWorld(int kind, const char *text) {
    if (previewWorld.active) {
        if (!G32(RVA_GAME_START_PREP_DONE)) GAME_FN(void (*)(void), RVA_GAME_START_PREP)();
        writePreviewSeats();
        previewSetDice((unsigned)previewWorld.seed);
    }
    nextPreviewCreatingWorld(kind, text);
}

static void previewNewGameWorld(int w, int h, int noPlanes) {
    if (previewWorld.active) {
        unsigned value = previewWorld.tries ? previewDiceKey(previewWorld.seed, "try", previewWorld.tries, 0) : (unsigned)previewWorld.seed;
        previewWorld.tries++;
        memset(previewOccurrences, 0, sizeof previewOccurrences);
        previewSetDice(value);
        previewLinesStartTry();
    }
    GAME_FN(void (*)(int, int, int), RVA_NEW_GAME_WORLD)(w, h, noPlanes);
    previewTerrainStepAll(PREVIEW_SOURCE_WORLD);
}

static void previewPopulateElysium(void) {
    previewRestartDice("pop", 0, 0);
    previewSetSource(PREVIEW_SOURCE_POPULATION);
    GAME_FN(void (*)(void), RVA_POPULATE_ELYSIUM)();
}

static int previewProgressWait(unsigned long long a, int mode) {
    if (!previewWorld.active) return GAME_FN(int (*)(unsigned long long, int), RVA_WAIT_FOR_CLICK)(a, mode);
    int x = G32(RVA_RANDOM_XOR), i = G32(RVA_RANDOM_INDEX), s = G32(RVA_RANDOM_SHIFT), churn[PREVIEW_CHURN_INTS];
    unsigned crt = previewCrtRandState();
    for (int k = 0; k < PREVIEW_CHURN_INTS; k++) churn[k] = G32(PREVIEW_CHURN_RVAS[k]);
    int result = GAME_FN(int (*)(unsigned long long, int), RVA_WAIT_FOR_CLICK)(a, mode);
    for (int k = 0; k < PREVIEW_CHURN_INTS; k++) G32(PREVIEW_CHURN_RVAS[k]) = churn[k];
    G32(RVA_RANDOM_XOR) = x, G32(RVA_RANDOM_INDEX) = i, G32(RVA_RANDOM_SHIFT) = s;
    GAME_FN(void (*)(unsigned), RVA_CRT_SRAND)(crt);
    return result;
}

#define PREVIEW_PLANE_OBSERVER(NAME, WHAT, PLANE) \
    static void previewDice##NAME(const CallInfo *call) { (void)call; previewRestartDice(WHAT, PLANE, 0); }
PREVIEW_PLANE_OBSERVER(BuildElysium, "build", 0)
PREVIEW_PLANE_OBSERVER(BuildCelestial, "build", 1)
PREVIEW_PLANE_OBSERVER(BuildSky, "build", 2)
PREVIEW_PLANE_OBSERVER(BuildAgartha, "build", 3)
PREVIEW_PLANE_OBSERVER(BuildInferno, "build", 4)
PREVIEW_PLANE_OBSERVER(BuildHades, "build", 5)
PREVIEW_PLANE_OBSERVER(BuildAztlan, "build", 6)
PREVIEW_PLANE_OBSERVER(BuildPrimal, "build", 7)
PREVIEW_PLANE_OBSERVER(BuildElemental, "build", 8)
#define PREVIEW_POP_OBSERVER(NAME, PLANE) \
    static void previewDice##NAME(const CallInfo *call) { (void)call; previewRestartDice("pop", PLANE, 0); previewSetSource(PREVIEW_SOURCE_POPULATION - (PLANE)); }
PREVIEW_POP_OBSERVER(PopCelestial, 1)
PREVIEW_POP_OBSERVER(PopSky, 2)
PREVIEW_POP_OBSERVER(PopAgartha, 3)
PREVIEW_POP_OBSERVER(PopInferno, 4)
PREVIEW_POP_OBSERVER(PopHades, 5)
PREVIEW_POP_OBSERVER(PopAztlan, 6)
PREVIEW_POP_OBSERVER(PopPrimal, 7)
PREVIEW_POP_OBSERVER(PopElemental, 8)
static void previewDiceMonth(const CallInfo *call) { (void)call; previewRestartDice("month", G32(RVA_TURN), 0); previewSetSource(PREVIEW_SOURCE_MONTH); }

static int previewPlaneIndex(void) {
    int custom = customPlaneIndex(previewWorld.plane);
    if (custom >= 0) return custom;
    for (int i = 0; i < PLANE_RECORDS_GAME; i++)
        if (!strcasecmp(VANILLA_PLANE_NAMES[i], previewWorld.plane)) return i;
    return -1;
}

static void previewHumanTurn(const CallInfo *call) {
    if (!previewWorld.active || (int)call->args[0] != PREVIEW_HUMAN_SLOT) return;
    previewTerrainStepAll(PREVIEW_SOURCE_EVENT);
    int last = G32(RVA_LAST_PLANE);
    for (int plane = 0; plane <= last; plane++) setPlaneField(plane, PLANE_FLAGS, planeField(plane, PLANE_FLAGS) | PLANE_FLAG_DISCOVERED);
    revealMapForPlayer(PREVIEW_HUMAN_SLOT, -1);
}

static int previewPointerOverDockNow(void) { return previewPointerOverDocks(previewDocks, previewDockCount, GF(RVA_UI_MOUSE_X), GF(RVA_UI_MOUSE_Y)); }

static void previewMapViewUpdate(float seconds) {
    void (*update)(float) = GAME_FN(void (*)(float), RVA_MAP_VIEW_UPDATE);
    if (!previewWorld.active || mapScreenWaiting <= 0 || !previewDockCount) {
        update(seconds);
        return;
    }
    int buttons = G32(RVA_MOUSE_BUTTONS), over = previewPointerOverDockNow();
    previewPress = previewPressStep(previewPress, buttons, over);
    if (!previewDockTakesInput(previewPress, over)) {
        update(seconds);
        return;
    }
    float x = GF(RVA_UI_MOUSE_X), y = GF(RVA_UI_MOUSE_Y);
    G32(RVA_MAP_WHEEL_LAST) = G32(RVA_WHEEL_FRAME);
    G32(RVA_MOUSE_BUTTONS) = 0;
    GF(RVA_UI_MOUSE_X) = (float)G32(RVA_SCREEN_W) / 2, GF(RVA_UI_MOUSE_Y) = (float)G32(RVA_SCREEN_H) / 2;
    update(seconds);
    GF(RVA_UI_MOUSE_X) = x, GF(RVA_UI_MOUSE_Y) = y;
    G32(RVA_MOUSE_BUTTONS) = buttons;
}

static int previewKeepDockClick(int id) {
    if (!previewDockCount) return id;
    int over = previewPointerOverDockNow(), buttons = G32(RVA_MOUSE_BUTTONS);
    previewPress = previewPressStep(previewPress, buttons, over);
    if (over) G32(RVA_SQUARE_INFO_X) = -1, G32(RVA_SQUARE_INFO_Y) = -1;
    if (previewPress == PREVIEW_PRESS_DOCK) {
        G32(RVA_MOUSE_BUTTONS) = 0;
        return 0;
    }
    id = previewLeftPressStep(&previewLeftPress, &buttons, id, G32(RVA_SQUARE_INFO_X) >= 0, GF(RVA_UI_MOUSE_X) * uiScaleX(), GF(RVA_UI_MOUSE_Y) * uiScaleY(), GetTickCount64());
    G32(RVA_MOUSE_BUTTONS) = buttons;
    return id;
}

static void previewBeforeMapDraw(void) {
    if (!previewWorld.active || !previewDockCount) return;
    char **table = (char **)GAME_PTR(RVA_TOOLTIP_TABLE);
    if (!previewPointerOverDockNow()) {
        if (G32(RVA_TOOLTIP_SHOWN) == PREVIEW_HOVER_SLOT) G32(RVA_TOOLTIP_SHOWN) = -1;
        return;
    }
    if (previewHoverText[0]) table[PREVIEW_HOVER_SLOT] = previewHoverText;
    G32(RVA_TOOLTIP_SHOWN) = previewHoverText[0] ? PREVIEW_HOVER_SLOT : -1;
}

static void previewHideCommanderCards(void) {
    int last = G32(RVA_GUI_LAST_SLOT);
    for (int slot = 0; slot <= last; slot++) {
        int id = guiId(slot);
        if (id == PREVIEW_CARD_DIAMOND_ID || (id >= MAP_COMMANDER_ID && id < 2 * MAP_COMMANDER_ID)) hideGuiSlot(slot);
    }
}

static int previewMapWait(int block, int mode) {
    if (!previewWorld.active) return nextPreviewMapWait(block, mode);
    if (!previewWorld.centered) {
        previewWorld.centered = 1;
        int plane = previewPlaneIndex();
        if (plane < 0) writeLogWarning("preview_world_plane_missing", "reason=not_in_this_world plane=%s", previewWorld.plane);
        int shown = plane < 0 ? 0 : plane;
        GAME_FN(void (*)(int, int), RVA_CENTER_MAP_ON)(planeField(0, PLANE_X) + planeField(0, PLANE_W) / 2, planeField(0, PLANE_Y) + planeField(0, PLANE_H) / 2);
        GAME_FN(void (*)(int, int), RVA_CENTER_MAP_ON)(planeField(shown, PLANE_X) + planeField(shown, PLANE_W) / 2,
                                                       planeField(shown, PLANE_Y) + planeField(shown, PLANE_H) / 2);
        previewBuildSeconds = (double)(GetTickCount64() - previewWorld.startedMs) / 1000.0;
        writeLog("preview_world", "phase=landed plane=%s index=%d seed=%d seconds=%.1f society=%d map_w=%d map_h=%d", previewWorld.plane, plane,
                 previewWorld.seed, previewBuildSeconds, G16(RVA_SOCIETY), G32(RVA_MAP_W), G32(RVA_MAP_H));
    }
    G32(RVA_SELECTED_COMMANDER) = -1;
    hideGuiId(MAP_ACTION_END_TURN);
    previewHideCommanderCards();
    int id = previewKeepDockClick(nextPreviewMapWait(block, mode));
    return previewMapIdIsOrder(id) ? 0 : id;
}

static int installPreviewWorldHooks(void) {
    static int tried, installed;
    if (tried) return installed;
    tried = 1;
    char owner[64];
    moduleFileName((void *)previewNewWorld, owner, sizeof owner);
    const WrapPlace outermost = {.priority = WRAP_PRIORITY_DEFAULT, .layer = WRAP_LAYER_OUTERMOST};
    int ok = insertWrap(SITE_PLAY_GAME_NEW_WORLD, (void *)previewNewWorld, (void **)&nextPreviewNewWorld, outermost, owner, NULL);
    ok &= insertWrap(SITE_NEW_WORLD_MAP_SCREEN, (void *)previewMapScreen, (void **)&nextPreviewMapScreen, outermost, owner, NULL);
    ok &= insertWrap(SITE_MAP_SETTINGS_WAIT, (void *)previewMapSettingsWait, (void **)&nextPreviewMapSettingsWait, outermost, owner, NULL);
    ok &= insertWrap(SITE_NEW_WORLD_CREATING_WORLD, (void *)previewCreatingWorld, (void **)&nextPreviewCreatingWorld, outermost, owner, NULL);
    ok &= insertWrap(SITE_MAP_SCREEN_WAIT, (void *)previewMapWait, (void **)&nextPreviewMapWait, outermost, owner, NULL);
    ok &= pointCustomPlaneCall(RVA_NEW_WORLD_NEW_GAME_CALL, RVA_NEW_GAME_WORLD, (void *)previewNewGameWorld, "preview_new_game_world");
    ok &= pointCustomPlaneCall(RVA_NEW_WORLD_POPULATE_CALL, RVA_POPULATE_ELYSIUM, (void *)previewPopulateElysium, "preview_populate_elysium");
    ok &= pointCustomPlaneCall(RVA_PROGRESS_TEXT_WAIT_CALL, RVA_WAIT_FOR_CLICK, (void *)previewProgressWait, "preview_progress_wait");
    ok &= pointCustomPlaneCall(RVA_MAP_VIEW_UPDATE_CALL, RVA_MAP_VIEW_UPDATE, (void *)previewMapViewUpdate, "preview_map_view_update");
    ok &= apiObserve(SITE_HUMAN_TURN, previewHumanTurn, NULL);
    ok &= apiObserve(SITE_NEW_UNIT, NULL, previewNoteNewUnit);
    ok &= apiObserve(SITE_AFTER_GAME_TURN, previewDiceMonth, previewFirstMonthDone);
    ok &= apiObserve(SITE_CREATE_RANDOM_MAP, previewDiceBuildElysium, NULL);
    ok &= apiObserve(SITE_CREATE_CELESTIAL_PLANE, previewDiceBuildCelestial, NULL);
    ok &= apiObserve(SITE_CREATE_SKY_PLANE, previewDiceBuildSky, NULL);
    ok &= apiObserve(SITE_CREATE_AGARTHA_PLANE, previewDiceBuildAgartha, NULL);
    ok &= apiObserve(SITE_CREATE_INFERNO_PLANE, previewDiceBuildInferno, NULL);
    ok &= apiObserve(SITE_CREATE_HADES_PLANE, previewDiceBuildHades, NULL);
    ok &= apiObserve(SITE_LOAD_AZTLAN_PLANE, previewDiceBuildAztlan, NULL);
    ok &= apiObserve(SITE_LOAD_PRIMAL_PLANE, previewDiceBuildPrimal, NULL);
    ok &= apiObserve(SITE_CREATE_ELEMENTAL_PLANE, previewDiceBuildElemental, NULL);
    ok &= apiObserve(SITE_POPULATE_CELESTIAL, previewDicePopCelestial, NULL);
    ok &= apiObserve(SITE_POPULATE_SKY, previewDicePopSky, NULL);
    ok &= apiObserve(SITE_POPULATE_AGARTHA, previewDicePopAgartha, NULL);
    ok &= apiObserve(SITE_POPULATE_INFERNO, previewDicePopInferno, NULL);
    ok &= apiObserve(SITE_POPULATE_HADES, previewDicePopHades, NULL);
    ok &= apiObserve(SITE_POPULATE_AZTLAN, previewDicePopAztlan, NULL);
    ok &= apiObserve(SITE_POPULATE_PRIMAL, previewDicePopPrimal, NULL);
    ok &= apiObserve(SITE_POPULATE_ELEMENTAL, previewDicePopElemental, NULL);
    installed = ok;
    writeLog("preview_world_hooks", "ok=%d", ok);
    return installed;
}

static void beginPreviewWorldState(void) {
    previewWorld.savedAutosave = G32(RVA_AUTOSAVE_ON);
    previewWorld.savedSociety = G16(RVA_SOCIETY);
    previewWorld.savedWilder = G32(RVA_WILDER);
    previewWorld.savedStartFlags = G16(RVA_START_FLAGS);
    for (int k = 0; k < PREVIEW_CHURN_INTS; k++) previewWorld.savedChurn[k] = G32(PREVIEW_CHURN_RVAS[k]);
    G32(RVA_AUTOSAVE_ON) = 0;
    previewWorld.startedMs = GetTickCount64();
}

static void endPreviewWorldState(void) {
    if (G32(RVA_TOOLTIP_SHOWN) == PREVIEW_HOVER_SLOT) G32(RVA_TOOLTIP_SHOWN) = -1;
    G32(RVA_AUTOSAVE_ON) = previewWorld.savedAutosave;
    G16(RVA_SOCIETY) = (short)previewWorld.savedSociety;
    G32(RVA_WILDER) = previewWorld.savedWilder;
    G16(RVA_START_FLAGS) = (short)previewWorld.savedStartFlags;
    for (int k = 0; k < PREVIEW_CHURN_INTS; k++) G32(PREVIEW_CHURN_RVAS[k]) = previewWorld.savedChurn[k];
    GAME_FN(void (*)(void), RVA_RANDOM_SEED_FROM_CLOCK)();
    GAME_FN(void (*)(void), RVA_CRT_SEED_FROM_CLOCK)();
}

static void endPreviewWorldNow(void) {
    previewNoteWorldEnding();
    previewWorld.ending = 1;
    G32(RVA_RESIGN) = 1;
    *(short *)playerSlotAddress(RVA_SLOT_CONTROLLER, PREVIEW_HUMAN_SLOT, 2) = CONTROLLER_REMOVED;
    writeLog("preview_world", "phase=ending turn=%d", G32(RVA_TURN));
}
static unsigned long long previewClockMs(void) { return GetTickCount64() - previewWorld.startedMs; }
#else
static unsigned long long previewClockMs(void) { return 0; }
static UNUSED void previewRestartDice(const char *what, int a, int b) { (void)what, (void)a, (void)b; }
static void previewRestartDiceNamed(const char *what, const char *name, int b) { (void)what, (void)name, (void)b; }
static void previewRestartDiceSquare(const char *monster, int area, int x, int y) { (void)monster, (void)area, (void)x, (void)y; }
static void previewRestartDiceEvent(const char *line, int level) { (void)line, (void)level; }
static int installPreviewWorldHooks(void) { return 0; }
static void beginPreviewWorldState(void) {}
static void endPreviewWorldState(void) {}
static void endPreviewWorldNow(void) {}
static void previewBeforeMapDraw(void) {}
#endif

// ---- The main menu and the game menu ask --------------------------------------------------------
static void beginPreviewWorld(void) {
    if (!previewWorld.asked) return;
    previewWorld.asked = 0, previewWorld.active = 1, previewWorld.ending = 0, previewWorld.centered = 0, previewWorld.tries = 0, previewWorld.firstMonthDone = 0;
    previewDockCount = 0, previewHoverText[0] = 0, previewPress = PREVIEW_PRESS_NONE, previewLeftPress = (PreviewLeftPress){0};
    previewField.on = 0, previewField.count = 0;
    beginPreviewWorldState();
    previewResultsStart();
    previewReadAllLines();
    writeLog("preview_world", "phase=started plane=%s seed=%d map_size=%d society=%d wilder=%d start_positions=%d seats=%d saved_society=%d saved_autosave=%d", previewWorld.plane,
             previewWorld.seed, previewWorld.mapSize, previewWorld.society, previewWorld.wilder, previewWorld.startPositions, previewWorld.seats, previewWorld.savedSociety,
             previewWorld.savedAutosave);
}

static void finishPreviewWorld(void) {
    if (!previewWorld.active) return;
    endPreviewWorldState();
    previewDockCount = 0, previewHoverText[0] = 0, previewPress = PREVIEW_PRESS_NONE, previewLeftPress = (PreviewLeftPress){0};
    previewField.on = 0, previewField.count = 0;
    previewWorld.active = 0, previewWorld.ending = 0, previewWorld.owner = NULL;
    previewResultsOn = 0;
    writeLog("preview_world", "phase=ended");
}

#define PREVIEW_LAYOUT_ELEMENT_SIZE 0x70
static void previewDropGameMenuRows(void) {
    if (!previewWorld.active) return;
    int last = G32(RVA_LAYOUT_LAST);
    while (last >= 0) {
        int id = *(int *)(gameBase + RVA_LAYOUT_ELEMENTS + (size_t)last * PREVIEW_LAYOUT_ELEMENT_SIZE);
        if (id != GAME_MENU_SAVE && id != GAME_MENU_SAVE_AND_QUIT && id != GAME_MENU_RESIGN) break;
        last--;
    }
    G32(RVA_LAYOUT_LAST) = last;
}

static void previewBeforeGameMenuWait(void) {
    if (!previewWorld.active) return;
    hideGuiId(GAME_MENU_SAVE);
    hideGuiId(GAME_MENU_SAVE_AND_QUIT);
    hideGuiId(GAME_MENU_RESIGN);
}

static int previewShowsMenuLine(const void *script) { return !previewWorld.active || script == previewWorld.owner; }

// ---- The coe5 calls ------------------------------------------------------------------------------
static UNUSED int lStartPreviewWorld(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_settop(L, 1);
    lua_getfield(L, 1, "plane");
    lua_getfield(L, 1, "seed");
    lua_getfield(L, 1, "mapSize");
    lua_getfield(L, 1, "society");
    lua_getfield(L, 1, "wilder");
    lua_getfield(L, 1, "seats");
    lua_getfield(L, 1, "startPositions");
    lua_getfield(L, 1, "uniqueRandom");
    const char *plane = luaL_checkstring(L, 2);
    lua_Integer seed = luaL_checkinteger(L, 3), mapSize = luaL_checkinteger(L, 4), society = luaL_checkinteger(L, 5);
    int wilder = lua_toboolean(L, 6);
    luaL_checktype(L, 7, LUA_TTABLE);
    lua_Integer seats = luaL_len(L, 7);
    lua_Integer startPositions = luaL_optinteger(L, 8, START_POSITIONS_CLUSTERED);
    int uniqueRandom = lua_toboolean(L, 9);
    int classCount = G32(RVA_CLASS_COUNT);
    const char *reason = previewStartRefusal(mainMenuWaiting > 0, isNetworkGame(), previewWorld.active || previewWorld.asked, plane, seed, mapSize, society,
                                             startPositions, seats);
    int classes[PREVIEW_SEATS_MAX];
    for (int i = 0; !reason && i < seats; i++) {
        lua_geti(L, 7, i + 1);
        lua_Integer cls = lua_isinteger(L, -1) ? lua_tointeger(L, -1) : -1;
        lua_pop(L, 1);
        if (cls < 0 || cls > classCount) reason = "bad_seat_class";
        else classes[i] = (int)cls;
    }
    if (!reason && !installPreviewWorldHooks()) reason = "hooks_not_installed";
    if (reason) {
        refuseLuaCall(runningLuaMod(), "coe5.startPreviewWorld", reason, "plane=%.40s seed=%lld map_size=%lld society=%lld start_positions=%lld seats=%lld", plane,
                      (long long)seed, (long long)mapSize, (long long)society, (long long)startPositions, (long long)seats);
        lua_pushboolean(L, 0);
        return 1;
    }
    memset(&previewWorld, 0, sizeof previewWorld);
    previewWorld.asked = 1, previewWorld.seed = (int)seed, previewWorld.mapSize = (int)mapSize, previewWorld.society = (int)society;
    previewWorld.wilder = wilder, previewWorld.startPositions = (int)startPositions, previewWorld.uniqueRandom = uniqueRandom, previewWorld.seats = (int)seats;
    memcpy(previewWorld.seatClass, classes, sizeof classes[0] * (size_t)seats);
    snprintf(previewWorld.plane, sizeof previewWorld.plane, "%s", plane);
    previewWorld.owner = luaScriptOf(L);
    lua_pushboolean(L, 1);
    return 1;
}

static UNUSED int lEndPreviewWorld(lua_State *L) {
    const char *reason = !previewWorld.active ? "no_preview_world" : previewWorld.ending ? "already_ending" : G32(RVA_ACTIVE_PLAYER) != PREVIEW_HUMAN_SLOT ? "not_at_first_turn" : NULL;
    if (reason) {
        refuseLuaCall(runningLuaMod(), "coe5.endPreviewWorld", reason, "active=%d", previewWorld.active);
        lua_pushboolean(L, 0);
        return 1;
    }
    endPreviewWorldNow();
    lua_pushboolean(L, 1);
    return 1;
}

static UNUSED int lPreviewWorldActive(lua_State *L) {
    lua_pushboolean(L, previewWorld.active);
    if (previewBuildSeconds < 0) lua_pushnil(L);
    else lua_pushnumber(L, previewBuildSeconds);
    lua_pushinteger(L, (lua_Integer)previewClockMs());
    return 3;
}

static UNUSED int lSetPreviewDocks(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_Integer count = luaL_len(L, 1);
    PreviewDock docks[PREVIEW_DOCKS_MAX];
    const char *reason = !previewWorld.active ? "no_preview_world" : count > PREVIEW_DOCKS_MAX ? "too_many_docks" : NULL;
    for (int i = 0; !reason && i < count; i++) {
        static const char *const FIELDS[] = {"x", "y", "w", "h"};
        float values[4] = {0, 0, 0, 0};
        lua_geti(L, 1, i + 1);
        if (lua_type(L, -1) != LUA_TTABLE) reason = "bad_dock";
        for (int k = 0; k < 4 && !reason; k++) {
            lua_getfield(L, -1, FIELDS[k]);
            if (!lua_isnumber(L, -1)) reason = "bad_dock";
            else values[k] = (float)lua_tonumber(L, -1);
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
        if (!reason && (values[2] <= 0 || values[3] <= 0)) reason = "bad_dock";
        if (!reason) docks[i] = (PreviewDock){values[0], values[1], values[2], values[3]};
    }
    if (reason) {
        refuseLuaCall(runningLuaMod(), "coe5.setPreviewDocks", reason, "count=%lld", (long long)count);
        lua_pushboolean(L, 0);
        return 1;
    }
    memcpy(previewDocks, docks, sizeof docks[0] * (size_t)count);
    previewDockCount = (int)count;
    lua_pushboolean(L, 1);
    return 1;
}

static UNUSED int lSetPreviewHoverText(lua_State *L) {
    const char *text = luaL_optstring(L, 1, "");
    if (!previewWorld.active) {
        refuseLuaCall(runningLuaMod(), "coe5.setPreviewHoverText", "no_preview_world", "length=%zu", strlen(text));
        lua_pushboolean(L, 0);
        return 1;
    }
    snprintf(previewHoverText, sizeof previewHoverText, "%.199s", text);
    lua_pushboolean(L, 1);
    return 1;
}

static UNUSED int lSetPreviewKeyField(lua_State *L) {
    int on = lua_toboolean(L, 1);
    if (!previewWorld.active) {
        refuseLuaCall(runningLuaMod(), "coe5.setPreviewKeyField", "no_preview_world", "on=%d", on);
        lua_pushboolean(L, 0);
        return 1;
    }
    previewField.on = on, previewField.count = 0;
    lua_pushboolean(L, 1);
    return 1;
}

static UNUSED int lTakePreviewFieldKeys(lua_State *L) {
    if (!previewWorld.active) {
        refuseLuaCall(runningLuaMod(), "coe5.takePreviewFieldKeys", "no_preview_world", "count=%d", previewField.count);
        lua_pushnil(L);
        return 1;
    }
    lua_createtable(L, previewField.count, 0);
    for (int i = 0; i < previewField.count; i++) {
        if (previewField.keys[i].key) lua_pushinteger(L, previewField.keys[i].key);
        else lua_pushstring(L, previewField.keys[i].text);
        lua_seti(L, -2, i + 1);
    }
    previewField.count = 0;
    return 1;
}

static UNUSED int lTextWidth(lua_State *L) {
    const char *text = luaL_checkstring(L, 1);
    float size = (float)luaL_checknumber(L, 2);
    const char *reason = !previewWorld.active ? "no_preview_world" : size <= 0 ? "out_of_range" : NULL;
    if (reason) {
        refuseLuaCall(runningLuaMod(), "coe5.ui.textWidth", reason, "size=%g", (double)size);
        lua_pushnil(L);
        return 1;
    }
    if (!luaMayDraw(L, "coe5.ui.textWidth")) {
        lua_pushnil(L);
        return 1;
    }
    char cut[DRAW_TEXT_SIZE];
    text = fitGameText((void *)lTextWidth, "coe5.ui.textWidth", text, cut, sizeof cut);
    double width = measureUiText(text, size);
    if (width < 0) {
        refuseLuaCall(runningLuaMod(), "coe5.ui.textWidth", "no_font", "size=%g", (double)size);
        lua_pushnil(L);
        return 1;
    }
    lua_pushnumber(L, width);
    return 1;
}

static int lDeclaredPlanes(lua_State *L);
static int lPreviewLineResults(lua_State *L);
static int lSetPreviewLineEdits(lua_State *L);
static int lPreviewLineIndex(lua_State *L);
static int lPreviewArmySource(lua_State *L);
static int lPreviewSquareSource(lua_State *L);
static int lPreviewEventResults(lua_State *L);
static int lPreviewScriptResults(lua_State *L);
