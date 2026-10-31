#define PLANE_ICON_FALLBACK  PLANE_FILES_DIR PATH_SEP "plane_icon_%d.tga"
#define PLANE_ROW_GREY       0.3f
#define PLANE_ICON_FLAGS     4
#define PLANE_ICON_FLAGS_GREY 0xe
#define PLANE_PICKER_GREY_ID 2
#define PLANE_GAME_OVER_GREY_ID 0
#define PLANE_DISCOVERED_SLOTS   24
#define PLANE_DISCOVERED_SHARE   10
#define PLANE_DISCOVERED_MIN     20
#define CONTROLLER_REMOTE_HUMAN  5
#define PLANE_TEXT_VIEWED    "%s (currently viewing this plane) (right click = info)"
#define PLANE_TEXT_SHOWN     "%s (select to view this plane) (right click = info)"
#define PLANE_TEXT_UNKNOWN   "Unknown plane (cannot view this plane)"
#define PLANE_TEXT_PICK_UNKNOWN     "unknown plane"
#define PLANE_TEXT_PICK_UNAVAILABLE "This plane is currently unavailable"

typedef void (*GameTextFn)(const char *fmt, ...);

static CustomPlane *customPlaneAt(int index) {
    for (int i = 0; i < customPlaneCount; i++)
        if (customPlanes[i].index == index && !customPlanes[i].skipped && !strncmp((const char *)planeRecord(index), customPlanes[i].name, PLANE_NAME_SIZE))
            return &customPlanes[i];
    return NULL;
}

static int customPlaneIconLike(const CustomPlane *p) {
    return !p->icon[0] && p->recipe >= 0 && p->recipe < PLANE_ELYSIUM2 ? p->recipe : -1;
}

static int customPlaneSprite(CustomPlane *p, int viewed) {
    int like = customPlaneIconLike(p);
    if (like >= 0) return (viewed ? PLANE_SPRITE_VIEWED : PLANE_SPRITE_SHOWN) + like;
    int slot = (int)(p - customPlanes), sprite = MISC_TRS_SPRITES + slot;
    int *const *replaced = (int *const *)GAME_PTR(RVA_SPRITE_REPLACED);
    const int *list = replaced[SPRITE_KIND_ICON];
    if (list && list[sprite] >= 0) return sprite;
    static int refusedOnce[CUSTOM_PLANE_TARGET];
    char path[MAX_PATH];
    int coe5se = modIndexExact(COE5SE_MOD);
    if (p->icon[0]) snprintf(path, sizeof path, "%s", p->icon);
    else if (coe5se < 0 || !buildPath(path, sizeof path, __func__, "%s" PATH_SEP PLANE_ICON_FALLBACK, modFolder(coe5se), slot + 1)) path[0] = 0;
    int image = -1;
#ifndef __APPLE__
    if (path[0]) image = GAME_FN(int (*)(int, int, int, const char *), RVA_REPLACE_SPRITE)(SPRITE_KIND_ICON, sprite, 0, path);
#else
    if (path[0]) image = GAME_FN(int (*)(int, int, const char *), RVA_REPLACE_SPRITE)(SPRITE_KIND_ICON, sprite, path);
#endif
    list = replaced[SPRITE_KIND_ICON];
    if (image < 0 || !list || list[sprite] < 0) {
        if (!refusedOnce[p - customPlanes]++) writeLog("plane_icon_refused", "mod=%s name=%s sprite=%d path=%s", p->mod, p->name, sprite, path[0] ? path : "-");
        return -1;
    }
    writeLog("plane_icon_registered", "mod=%s name=%s sprite=%d path=%s", p->mod, p->name, sprite, path);
    return sprite;
}

static int customPlaneSeen(int player, int plane) {
    if (player < 0) return 1;
    if (player >= PLAYER_SLOTS) return 0;
    int x0 = planeField(plane, PLANE_X), y0 = planeField(plane, PLANE_Y), w = planeField(plane, PLANE_W), h = planeField(plane, PLANE_H);
    const unsigned char *squares = GAME_PTR(RVA_SQUARE_RECORDS);
    for (int x = x0; x < x0 + w && x < MAP_COLUMNS_MAX; x++)
        for (int y = y0; y < y0 + h && y < MAP_COLUMN_SQUARES; y++)
            if ((signed char)squares[((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE + SQUARE_SEEN + player] > 0) return 1;
    return 0;
}

static int lastCustomPlaneIndex(void) {
    if (!planeTableMoved) return -1;
    int last = G32(RVA_LAST_PLANE);
    return last < PLANE_RECORDS_MOVED ? last : PLANE_INDEX_LAST_MOVED;
}

// ---- Discovered planes and Reveal Map -----------------------------------------------------------

static int planeDiscovered(int index) { return (planeField(index, PLANE_FLAGS) & PLANE_FLAG_DISCOVERED) != 0; }

static void markCustomPlanesDiscovered(void) {
    int last = lastCustomPlaneIndex(), mapW = G32(RVA_MAP_W), mapH = G32(RVA_MAP_H);
    const unsigned char *squares = GAME_PTR(RVA_SQUARE_RECORDS);
    const short *controllers = (const short *)playerTableAddress(RVA_SLOT_CONTROLLER);
    for (int index = PLANE_ELYSIUM2 + 1; index <= last; index++) {
        if (planeDiscovered(index)) continue;
        int x0 = planeField(index, PLANE_X), y0 = planeField(index, PLANE_Y), w = planeField(index, PLANE_W), h = planeField(index, PLANE_H);
        int need = w * h / PLANE_DISCOVERED_SHARE < PLANE_DISCOVERED_MIN ? PLANE_DISCOVERED_MIN : w * h / PLANE_DISCOVERED_SHARE;
        for (int player = 0; player < PLANE_DISCOVERED_SLOTS && !planeDiscovered(index); player++) {
            if (controllers[player] != CONTROLLER_HUMAN && controllers[player] != CONTROLLER_REMOTE_HUMAN) continue;
            int seen = 0;
            for (int x = x0 < 0 ? 0 : x0; x < x0 + w && x < mapW && x < MAP_COLUMNS_MAX; x++)
                for (int y = y0 < 0 ? 0 : y0; y < y0 + h && y < mapH && y < MAP_COLUMN_SQUARES; y++)
                    seen += (signed char)squares[((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE + SQUARE_SEEN + player] > 0;
            if (seen >= need) {
                setPlaneField(index, PLANE_FLAGS, planeField(index, PLANE_FLAGS) | PLANE_FLAG_DISCOVERED);
                writeLog("plane_discovered", "index=%d name=%.31s player=%d seen=%d", index, (const char *)planeRecord(index), player, seen);
            }
        }
    }
}

typedef long long (*GameDeadUnitsPassFn)(long long a0);
static long long planeDiscoveredNextHook(long long a0) {
    markCustomPlanesDiscovered();
    return GAME_FN(GameDeadUnitsPassFn, RVA_DEAD_UNITS_PASS)(a0);
}

typedef void (*GameRevealMapFn)(int player, int plane);
static void revealDiscoveredCustomPlanes(int player, GameRevealMapFn reveal) {
    int last = lastCustomPlaneIndex();
    for (int index = PLANE_ELYSIUM2 + 1; index <= last; index++)
        if (customPlaneAt(index) && planeDiscovered(index)) reveal(player, index);
}

static void revealMapPlaneHook(int player, int plane) {
    GameRevealMapFn reveal = GAME_FN(GameRevealMapFn, RVA_REVEAL_MAP_FOR);
    reveal(player, plane);
    if (plane == 0) revealDiscoveredCustomPlanes(player, reveal);
}

// ---- The top bar and the map editor's plane bar -------------------------------------------------
typedef struct {
    int count;
    int ids;
    float lastX, prevX;
} PlaneIconRow;
static PlaneIconRow topBarPlaneRow, editorPlaneRow;

static void drawPlaneIconAt(int id, int sprite, int grey, float x, float y, float w, float h, const char *text, const char *name) {
    if (grey)
        GAME_FN(void (*)(int, int, int, int, float, float, float, float, float, float, float, float), RVA_ICON_BUTTON_TINTED_AT)(
            0, SPRITE_KIND_ICON, sprite, PLANE_ICON_FLAGS_GREY, x, y, w, h, PLANE_ROW_GREY, PLANE_ROW_GREY, PLANE_ROW_GREY, 1.0f);
    else
#ifndef __APPLE__
        GAME_FN(void (*)(int, int, int, int, float, float, float, float), RVA_ICON_BUTTON_AT)(id, SPRITE_KIND_ICON, sprite, PLANE_ICON_FLAGS, x, y, w, h);
#else
        // @platform-split icon_button_at: Mac's fixed-place icon has the kind and flags built in
        GAME_FN(void (*)(int, int, float, float, float, float), RVA_ICON_BUTTON_AT)(id, sprite, x, y, w, h);
#endif
    GAME_FN(GameTextFn, RVA_TEXT)(text, name);
}

static void planeRowText(PlaneIconRow *row, int editor, const char *fmt, const char *name) {
    const unsigned char *e = GAME_PTR(RVA_GUI_TABLE) + (size_t)(G32(RVA_GUI_NEXT_SLOT) - 1) * GUI_STRIDE;
    int id;
    float x, y, w, h;
    memcpy(&id, e, 4), memcpy(&x, e + 4, 4), memcpy(&y, e + GUI_ELEMENT_Y, 4), memcpy(&w, e + GUI_ELEMENT_WIDTH, 4), memcpy(&h, e + 0x10, 4);
    GAME_FN(GameTextFn, RVA_TEXT)(fmt, name);
    if (row->count && x <= row->lastX) row->count = 0;
    if (!row->count) row->ids = 0;
    row->prevX = row->count ? row->lastX : x - w;
    row->lastX = x;
    row->ids |= id != 0;
    int slots = G32(RVA_DEBUG_LEVEL) >= 2 ? PLANE_RECORDS_GAME : PLANE_RECORDS_GAME - 1;
    if (++row->count < slots) return;
    row->count = 0;
    int last = lastCustomPlaneIndex(), view = G32(RVA_MAP_VIEW_PLANE), player = G32(RVA_ACTIVE_PLAYER);
    float step = row->lastX - row->prevX;
    for (int index = PLANE_ELYSIUM2 + 1, k = 1; index <= last; index++) {
        CustomPlane *p = customPlaneAt(index);
        int sprite = p ? customPlaneSprite(p, view == index) : -1;
        if (sprite < 0) continue;
        const char *planeName = (const char *)planeRecord(index);
        int shown = editor ? planeField(index, PLANE_W) > 0 : customPlaneSeen(player, index);
        int buttonId = row->ids ? PLANE_BUTTON_ID_FIRST + index : 0;
        drawPlaneIconAt(buttonId, sprite, !shown, row->lastX + step * (float)k++, y, w, h,
                        !shown ? PLANE_TEXT_UNKNOWN : view == index ? PLANE_TEXT_VIEWED : PLANE_TEXT_SHOWN, planeName);
    }
}

static void topBarPlaneTextHook(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const char *name = va_arg(args, const char *);
    va_end(args);
    planeRowText(&topBarPlaneRow, 0, fmt, name);
}
static void editorPlaneTextHook(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const char *name = va_arg(args, const char *);
    va_end(args);
    planeRowText(&editorPlaneRow, 1, fmt, name);
}

// ---- The location picker and the Game Over map view --------------------------------------------
static struct { int active, player, multiplane, startPlane; } planePicker;

typedef long long (*PlaneScreenFn)(void *title, long long player, void *x, void *y, long long multiplane, long long a6, long long a7,
                                   long long a8, int a9);
static long long planeScreenHook(void *title, long long player, void *x, void *y, long long multiplane, long long a6, long long a7,
                                 long long a8, int a9) {
    int outer = planePicker.active;
    planePicker.active = 1, planePicker.player = (int)player, planePicker.multiplane = (int)multiplane;
    planePicker.startPlane = G32(RVA_MAP_VIEW_PLANE);
    long long result = GAME_FN(PlaneScreenFn, RVA_PLANE_SCREEN)(title, player, x, y, multiplane, a6, a7, a8, a9);
    planePicker.active = outer;
    return result;
}

static void planeRowButton(int id, int sprite, int grey, const char *text) {
    int element = GAME_FN(int (*)(int, int, int, int), RVA_SPRITE_BUTTON)(id, SPRITE_KIND_ICON, sprite, grey ? PLANE_ICON_FLAGS_GREY : PLANE_ICON_FLAGS);
    if (grey && element >= 0) {
        float rgba[4] = {PLANE_ROW_GREY, PLANE_ROW_GREY, PLANE_ROW_GREY, 1.0f};
        memcpy(GAME_PTR(RVA_LAYOUT_ELEMENTS) + (size_t)element * LAYOUT_STRIDE + LAYOUT_ELEMENT_COLOUR, rgba, sizeof rgba);
    }
    tooltip(text);
}

static void drawCustomPlaneButtonRow(int gameOver) {
    int last = lastCustomPlaneIndex(), view = G32(RVA_MAP_VIEW_PLANE), opened = 0;
    for (int index = PLANE_ELYSIUM2 + 1; index <= last; index++) {
        CustomPlane *p = customPlaneAt(index);
        int sprite = p ? customPlaneSprite(p, view == index) : -1;
        if (sprite < 0) continue;
        if (!opened++) rowOpen();
        const char *planeName = (const char *)planeRecord(index);
        if (view == index) planeRowButton(PLANE_BUTTON_ID_FIRST + index, sprite, 0, planeName);
        else if (gameOver && !planeDiscovered(index)) planeRowButton(PLANE_GAME_OVER_GREY_ID, sprite, 1, PLANE_TEXT_PICK_UNKNOWN);
        else if (!gameOver && !customPlaneSeen(planePicker.player, index)) planeRowButton(PLANE_PICKER_GREY_ID, sprite, 1, PLANE_TEXT_PICK_UNKNOWN);
        else if (!gameOver && planePicker.multiplane == 1 && planePicker.startPlane != index)
            planeRowButton(PLANE_PICKER_GREY_ID, sprite, 1, PLANE_TEXT_PICK_UNAVAILABLE);
        else planeRowButton(PLANE_BUTTON_ID_FIRST + index, sprite, 0, planeName);
    }
    if (opened) rowClose();
}

static int pickerPlaneRowEndHook(void) {
    if (planePicker.active && planePicker.multiplane > 0) drawCustomPlaneButtonRow(0);
    return GAME_FN(int (*)(void), RVA_LAYOUT_NEW)();
}
static int gameOverPlaneRowEndHook(void) {
    drawCustomPlaneButtonRow(1);
    return GAME_FN(int (*)(void), RVA_LAYOUT_NEW)();
}

// ---- The plane info screen ----------------------------------------------------------------------
static int planeInfoIconHook(int id, int kind, int sprite, int flags) {
    CustomPlane *p = sprite - PLANE_SPRITE_SHOWN > PLANE_ELYSIUM2 ? customPlaneAt(sprite - PLANE_SPRITE_SHOWN) : NULL;
    int own = p ? customPlaneSprite(p, 0) : -1;
    return GAME_FN(int (*)(int, int, int, int), RVA_SPRITE_BUTTON)(id, kind, own >= 0 ? own : sprite, flags);
}

// @platform-split plane_button_text: Windows has one text call per kind of slot, Mac one shared call per row
static int installCustomPlaneViewHooks(int *wanted) {
    static const struct { unsigned int callRva, targetRva; void *hook; const char *name; } CALLS[] = {
        {RVA_TOP_BAR_PLANE_SHOWN_TEXT_CALL,  RVA_TEXT,          (void *)topBarPlaneTextHook,     "top_bar_plane_text"},
        {RVA_EDITOR_PLANE_SHOWN_TEXT_CALL,   RVA_TEXT,          (void *)editorPlaneTextHook,     "editor_plane_text"},
#ifndef __APPLE__
        {RVA_TOP_BAR_PLANE_VIEWED_TEXT_CALL, RVA_TEXT,          (void *)topBarPlaneTextHook,     "top_bar_plane_text_viewed"},
        {RVA_TOP_BAR_PLANE_UNKNOWN_TEXT_CALL, RVA_TEXT,         (void *)topBarPlaneTextHook,     "top_bar_plane_text_unknown"},
        {RVA_EDITOR_PLANE_VIEWED_TEXT_CALL,  RVA_TEXT,          (void *)editorPlaneTextHook,     "editor_plane_text_viewed"},
        {RVA_EDITOR_PLANE_UNKNOWN_TEXT_CALL, RVA_TEXT,          (void *)editorPlaneTextHook,     "editor_plane_text_unknown"},
#endif
        {RVA_PLANE_SCREEN_CALL_RITUAL_A,     RVA_PLANE_SCREEN,  (void *)planeScreenHook,         "plane_screen_ritual_a"},
        {RVA_PLANE_SCREEN_CALL_RITUAL_B,     RVA_PLANE_SCREEN,  (void *)planeScreenHook,         "plane_screen_ritual_b"},
        {RVA_PLANE_SCREEN_CALL_OTHER,        RVA_PLANE_SCREEN,  (void *)planeScreenHook,         "plane_screen_other"},
        {RVA_PLANE_SCREEN_ROW_END_CALL,      RVA_LAYOUT_NEW,    (void *)pickerPlaneRowEndHook,   "plane_screen_row_end"},
        {RVA_GAME_OVER_PLANE_ROW_END_CALL,   RVA_LAYOUT_NEW,    (void *)gameOverPlaneRowEndHook, "game_over_plane_row_end"},
        {RVA_PLANE_INFO_ICON_CALL,           RVA_SPRITE_BUTTON, (void *)planeInfoIconHook,       "plane_info_icon"},
        {RVA_REVEAL_MAP_CALL,                RVA_REVEAL_MAP_FOR, (void *)revealMapPlaneHook,     "reveal_map"},
        {RVA_PLANE_DISCOVERED_NEXT_CALL,     RVA_DEAD_UNITS_PASS, (void *)planeDiscoveredNextHook, "plane_discovered"},
    };
    int ok = 0;
    *wanted = (int)(sizeof CALLS / sizeof CALLS[0]);
    for (int i = 0; i < *wanted; i++) ok += pointCustomPlaneCall(CALLS[i].callRva, CALLS[i].targetRva, CALLS[i].hook, CALLS[i].name);
    return ok;
}
