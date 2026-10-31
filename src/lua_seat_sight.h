// @platform-split seat_sight: Windows only; the Mac build refuses both calls

#define PLANE_SEEN_ROW 11

typedef struct {
    unsigned char *own;
    int columns, rows, shown;
    unsigned game;
} SeatSight;
static SeatSight seatSights[PLAYER_SLOTS];

static unsigned char *squareSeenByte(int x, int y, int player) {
    return gameBase + RVA_SQUARE_RECORDS + ((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE + SQUARE_SEEN + (size_t)player;
}

static const char *seatSightRefusal(int network, int activePlayer, lua_Integer seat, lua_Integer player, int seatController) {
    if (network) return "network_game";
    if (activePlayer < 0) return "no_game";
    if (seat < 0 || seat >= PLAYER_SLOTS || player < 0 || player >= PLAYER_SLOTS) return "bad_player";
    if (seat == player) return "same_player";
    if (seatController != CONTROLLER_HUMAN) return "seat_not_human";
    return NULL;
}

static void dropStaleSeatSight(int seat, unsigned game, int columns, int rows) {
    SeatSight *k = &seatSights[seat];
    if (!k->own || (k->game == game && k->columns == columns && k->rows == rows)) return;
    free(k->own);
    *k = (SeatSight){.shown = -1};
}

static int showSeatSight(int seat, int player, unsigned game, int columns, int rows) {
    dropStaleSeatSight(seat, game, columns, rows);
    SeatSight *k = &seatSights[seat];
    if (!k->own) {
        if (columns <= 0 || rows <= 0 || columns > MAP_COLUMNS_MAX || rows > MAP_COLUMN_SQUARES) return 0;
        k->own = malloc((size_t)columns * (size_t)rows);
        if (!k->own) return -1;
        for (int x = 0; x < columns; x++)
            for (int y = 0; y < rows; y++) k->own[(size_t)x * rows + y] = *squareSeenByte(x, y, seat);
        k->columns = columns, k->rows = rows, k->game = game;
    }
    for (int x = 0; x < columns; x++)
        for (int y = 0; y < rows; y++) *squareSeenByte(x, y, seat) = *squareSeenByte(x, y, player);
    k->shown = player;
    return columns * rows;
}

static int restoreSeatSight(int seat, unsigned game, int columns, int rows) {
    dropStaleSeatSight(seat, game, columns, rows);
    SeatSight *k = &seatSights[seat];
    if (!k->own) return 0;
    for (int x = 0; x < k->columns; x++)
        for (int y = 0; y < k->rows; y++) *squareSeenByte(x, y, seat) = k->own[(size_t)x * k->rows + y];
    int written = k->columns * k->rows;
    free(k->own);
    *k = (SeatSight){.shown = -1};
    return written;
}

static void redrawSeatSight(int seat) {
#ifndef __APPLE__
    memset(gameBase + RVA_PLAYER_PLANE_SEEN + (size_t)seat * PLANE_SEEN_ROW, 0xff, PLANE_SEEN_ROW);
    GAME_FN(void (*)(int), RVA_UPDATE_DRAW_SQUARES)(seat);
    GAME_FN(int (*)(int, int), RVA_MAP_BG_MARK)(-1, seat);
    for (int index = PLANE_ELYSIUM2 + 1; index <= lastCustomPlaneIndex(); index++) GAME_FN(int (*)(int, int), RVA_MAP_BG_MARK)(index, seat);
    GAME_FN(void (*)(void), RVA_RESET_SQUARE_CACHE)();
#else
    (void)seat;
#endif
}

static int lShowSight(lua_State *L) {
    lua_Integer seat = luaL_checkinteger(L, 1), player = luaL_checkinteger(L, 2);
    int inRange = seat >= 0 && seat < PLAYER_SLOTS;
    int controller = inRange ? PG16(RVA_SLOT_CONTROLLER + 2 * (size_t)seat) : CONTROLLER_REMOVED;
    const char *reason = seatSightRefusal(isNetworkGame(), G32(RVA_ACTIVE_PLAYER), seat, player, controller);
#ifdef __APPLE__
    if (!reason) reason = "windows_only";
#endif
    int written = 0;
    if (!reason) {
        written = showSeatSight((int)seat, (int)player, playGameSerial, G32(RVA_MAP_DEPTH), G32(RVA_MAP_H));
        if (written < 0) reason = "out_of_memory";
    }
    LuaScript *s = luaScriptOf(L);
    if (reason) {
        refuseLuaCall(s ? s->mod : "?", "coe5.showSight", reason, "seat=%lld player=%lld", (long long)seat, (long long)player);
        lua_pushboolean(L, 0);
        return 1;
    }
    redrawSeatSight((int)seat);
    writeLog("seat_sight_shown", "mod=%s seat=%lld player=%lld squares=%d", s ? s->mod : "?", (long long)seat, (long long)player, written);
    lua_pushboolean(L, 1);
    return 1;
}

static int lRestoreSight(lua_State *L) {
    lua_Integer seat = luaL_checkinteger(L, 1);
    const char *reason = seat < 0 || seat >= PLAYER_SLOTS ? "bad_player" : G32(RVA_ACTIVE_PLAYER) < 0 ? "no_game" : NULL;
#ifdef __APPLE__
    if (!reason) reason = "windows_only";
#endif
    int written = reason ? 0 : restoreSeatSight((int)seat, playGameSerial, G32(RVA_MAP_DEPTH), G32(RVA_MAP_H));
    if (!reason && !written) reason = "not_shown";
    LuaScript *s = luaScriptOf(L);
    if (reason) {
        refuseLuaCall(s ? s->mod : "?", "coe5.restoreSight", reason, "seat=%lld", (long long)seat);
        lua_pushboolean(L, 0);
        return 1;
    }
    redrawSeatSight((int)seat);
    writeLog("seat_sight_restored", "mod=%s seat=%lld squares=%d", s ? s->mod : "?", (long long)seat, written);
    lua_pushboolean(L, 1);
    return 1;
}
