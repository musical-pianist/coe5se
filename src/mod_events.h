#pragma once

#define MOD_EVENT_TEXT_MAX  80
#define EVENT_FILE_SIZE     2500

typedef struct {
    char mod[MOD_NAME_MAX];
    int fileOffset;
    int kind;
    int line;
    int firstLine, lineCount;
    char message[MOD_EVENT_TEXT_MAX + 1];
} ModEvent;
typedef struct { int offset, line; } ModEventLine;

static ModEvent *modEvents;
static int modEventCount, modEventCapacity;
static ModEventLine *modEventLines;
static int modEventLineCount, modEventLineCapacity;
static char *modEventText;
static int modEventTextUsed, modEventTextCapacity;
static int modEventOpen;

static void modEventsBeforePass(int level) {
    if (level != 2) return;
    modEventCount = modEventLineCount = modEventTextUsed = 0;
    modEventOpen = 0;
}

static int keepModEventText(const char *text) {
    int length = (int)strlen(text) + 1, offset = modEventTextUsed;
    if (!growList((void **)&modEventText, &modEventTextCapacity, modEventTextUsed + length, 1)) return -1;
    memcpy(modEventText + offset, text, (size_t)length);
    modEventTextUsed += length;
    return offset;
}

static int keepModEventLine(const char *line) {
    if (!growList((void **)&modEventLines, &modEventLineCapacity, modEventLineCount + 1, sizeof modEventLines[0])) return 0;
    int offset = keepModEventText(line);
    if (offset < 0) return 0;
    modEventLines[modEventLineCount++] = (ModEventLine){offset, G32(RVA_MOD_LINE)};
    return 1;
}

static void noteModEventLine(const char *line, int level, const char *mod, const char *file) {
    if (level != 2) return;
    int start = eventWordIs(line, "playerevent") ? PLAYER_EVENT : eventWordIs(line, "squareevent") ? SQUARE_EVENT : 0;
    if (start) {
        if (!growList((void **)&modEvents, &modEventCapacity, modEventCount + 1, sizeof modEvents[0])) {
            writeLogWarning("mod_event_dropped", "mod=%s line=%d reason=no_memory", mod, G32(RVA_MOD_LINE));
            modEventOpen = 0;
            return;
        }
        ModEvent *e = &modEvents[modEventCount];
        memset(e, 0, sizeof *e);
        snprintf(e->mod, sizeof e->mod, "%s", mod);
        e->kind = start;
        e->line = G32(RVA_MOD_LINE);
        e->firstLine = modEventLineCount;
        e->fileOffset = keepModEventText(file);
        modEventOpen = e->fileOffset >= 0 && keepModEventLine(line);
        if (!modEventOpen) writeLogWarning("mod_event_dropped", "mod=%s line=%d reason=no_memory", mod, e->line);
        return;
    }
    if (!modEventOpen) return;
    ModEvent *e = &modEvents[modEventCount];
    if (!keepModEventLine(line)) {
        writeLogWarning("mod_event_dropped", "mod=%s line=%d reason=no_memory", e->mod, e->line);
        modEventLineCount = e->firstLine;
        modEventOpen = 0;
        return;
    }
    if (!e->message[0] && eventWordIs(line, "message")) {
        const char *text = line + 7;
        while (*text == ' ' || *text == '\t') text++;
        if (*text == '"') {
            const char *close = strchr(text + 1, '"');
            text = close ? close + 1 : text + strlen(text);
        } else text += strcspn(text, " \t");
        while (*text == ' ' || *text == '\t') text++;
        if (*text == '"') text++;
        size_t length = strlen(text);
        if (length && text[length - 1] == '"') length--;
        snprintf(e->message, sizeof e->message, "%.*s", (int)(length < MOD_EVENT_TEXT_MAX ? length : MOD_EVENT_TEXT_MAX), text);
    }
    if (eventWordIs(line, "endevent")) {
        e->lineCount = modEventLineCount - e->firstLine;
        modEventCount++;
        modEventOpen = 0;
    }
}

static const char *modEventLineText(const ModEvent *e, int i) { return modEventText + modEventLines[e->firstLine + i].offset; }
static int isModEventTrigger(const char *line) { return line[0] == '+' || line[0] == '-'; }

static int lModEvents(lua_State *L) {
    lua_createtable(L, modEventCount, 0);
    for (int n = 0; n < modEventCount; n++) {
        const ModEvent *e = &modEvents[n];
        lua_createtable(L, 0, 5);
        lua_pushstring(L, e->mod);
        lua_setfield(L, -2, "mod");
        setIntField(L, "line", e->line);
        lua_pushstring(L, e->kind == SQUARE_EVENT ? "squareevent" : "playerevent");
        lua_setfield(L, -2, "kind");
        lua_pushstring(L, e->message);
        lua_setfield(L, -2, "message");
        lua_createtable(L, 0, 0);
        int needs = 0;
        for (int i = 1; i < e->lineCount; i++)
            if (isModEventTrigger(modEventLineText(e, i))) {
                lua_pushstring(L, modEventLineText(e, i));
                lua_rawseti(L, -2, ++needs);
            }
        lua_setfield(L, -2, "needs");
        lua_rawseti(L, -2, n + 1);
    }
    return 1;
}

static const char *modEventRunRefusal(lua_Integer index, int hasSquare, lua_Integer x, lua_Integer y) {
    if (G32(RVA_ACTIVE_PLAYER) < 0) return "no_game";
    if (G32(RVA_MOD_LINE) != -1 || G32(RVA_EVENT_STATE) != 0) return "event_running";
    if (index < 1 || index > modEventCount) return "bad_event";
    if (modEvents[index - 1].kind == SQUARE_EVENT && !hasSquare) return "needs_square";
    if (hasSquare && (x < 0 || y < 0 || x >= G32(RVA_MAP_W) || y >= G32(RVA_MAP_H) || x >= MAP_COLUMNS_MAX || y >= MAP_COLUMN_SQUARES))
        return "off_map";
    return NULL;
}

static int lRunModEvent(lua_State *L) {
    const char *caller = runningLuaMod() ? runningLuaMod() : "-";
    lua_Integer index = luaL_checkinteger(L, 1);
    int hasSquare = !lua_isnoneornil(L, 2);
    lua_Integer x = hasSquare ? luaL_checkinteger(L, 2) : -1, y = hasSquare ? luaL_checkinteger(L, 3) : -1;
    const char *problem = modEventRunRefusal(index, hasSquare, x, y);
    if (problem) {
        refuseLuaCall(caller, "coe5.runModEvent", problem, "index=%lld x=%lld y=%lld", (long long)index, (long long)x, (long long)y);
        lua_pushboolean(L, 0);
        return 1;
    }
    const ModEvent *e = &modEvents[index - 1];
    static char savedFile[EVENT_FILE_SIZE];
    char *file = (char *)(gameBase + RVA_EVENT_FILE);
    memcpy(savedFile, file, sizeof savedFile);
    int savedLine = G32(RVA_MOD_LINE);
    snprintf(file, sizeof savedFile, "%s", modEventText + e->fileOffset);
    int skipped = 0;
    for (int i = 0; i < e->lineCount; i++) {
        const char *line = modEventLineText(e, i);
        if (isModEventTrigger(line)) {
            skipped++;
            continue;
        }
        G32(RVA_MOD_LINE) = modEventLines[e->firstLine + i].line;
        eventLineHook(line, 3);
        if (i == 0 && e->kind == SQUARE_EVENT && G32(RVA_EVENT_STATE) == SQUARE_EVENT) {
            memset(eventMarks(), 0, SQUARE_EVENT_A_SIZE);
            eventMarks()[(size_t)x * MAP_COLUMN_SQUARES + (size_t)y] = 1;
            keepEventStart();
        }
    }
    G32(RVA_EVENT_STATE) = 0;
    G32(RVA_MOD_LINE) = savedLine;
    memcpy(file, savedFile, sizeof savedFile);
    writeLog("lua_mod_event_run", "mod=%s event_mod=%s line=%d kind=%s x=%lld y=%lld skipped=%d", caller, e->mod, e->line,
             e->kind == SQUARE_EVENT ? "squareevent" : "playerevent", (long long)x, (long long)y, skipped);
    lua_pushboolean(L, 1);
    return 1;
}
