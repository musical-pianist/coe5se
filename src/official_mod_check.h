#pragma once

#define OFFICIAL_MOD_CHECK_SWITCH     "--textonly"
#define OFFICIAL_MOD_CHECK_END_TEXT   "--textonly can only be used for servers."
#define OFFICIAL_MOD_CHECK_STOP_TEXT  "*** Error in mod '"
#define OFFICIAL_MOD_CHECK_RUN_ON_TEXT "Maybe disconnect from server"
#define OFFICIAL_MOD_CHECK_PARSE_TEXT "Parsing mod: "
#define OFFICIAL_MOD_CHECK_OUTPUT_MAX (4 * 1024 * 1024)
#define OFFICIAL_MOD_CHECK_SECONDS    300
#define OFFICIAL_STUB_SECONDS     30
#define OFFICIAL_BOX_MAX          4096
#define OFFICIAL_BOX_ROWS         8
#define OFFICIAL_MESSAGE_MAX      160

#define OFFICIAL_BOX_WIDTH       48
#define OFFICIAL_BOX_QUESTION                                                                                  \
    "Blocked with CoE5SE enabled. This is to keep you\nfrom being banned in multiplayer.\n\nRestart without CoE5SE? " \
    "Mods with a Mod Error\nare disabled while in the multiplayer lobby"
#define OFFICIAL_BOX_FAILED_HEAD OFFICIAL_BOX_QUESTION ":\n\n"
#define OFFICIAL_BOX_CLEAN OFFICIAL_BOX_QUESTION "."
#define OFFICIAL_BOX_NOT_READ    "Mod check failed:\n%s\n\nStill blocked.\nMod list unchanged."

typedef enum {
    OFFICIAL_MOD_CHECK_CLEAN,
    OFFICIAL_MOD_CHECK_MOD_ERROR,
    OFFICIAL_MOD_CHECK_MOD_STOPPED,
    OFFICIAL_MOD_CHECK_NO_VERDICT,
} OfficialModCheckKind;

typedef struct {
    OfficialModCheckKind kind;
    char mod[OFFICIAL_MOD_NAME];
    int line;
    char message[OFFICIAL_MESSAGE_MAX];
    const char *reason;
} OfficialModCheckResult;

static __attribute__((unused)) const char *officialLastText(const char *text, size_t len, const char *needle) {
    size_t size = strlen(needle);
    if (!text || len < size) return NULL;
    for (size_t at = len - size + 1; at-- > 0;)
        if (!memcmp(text + at, needle, size)) return text + at;
    return NULL;
}

static __attribute__((unused)) int officialModFromPath(const char *path, size_t len, char *out, size_t size) {
    size_t start = len;
    while (start > 0 && path[start - 1] != '/' && path[start - 1] != '\\') start--;
    size_t n = len - start;
    if (n > 4 && !memcmp(path + len - 4, ".c5m", 4)) n -= 4;
    if (!n || n >= size) return 0;
    memcpy(out, path + start, n);
    out[n] = '\0';
    return 1;
}

static __attribute__((unused)) void officialModCheckRead(const char *text, size_t len, OfficialModCheckResult *result) {
    memset(result, 0, sizeof *result);
    const char *end = text + len, *stop = officialLastText(text, len, OFFICIAL_MOD_CHECK_STOP_TEXT);
    if (stop) {
        const char *path = stop + strlen(OFFICIAL_MOD_CHECK_STOP_TEXT);
        const char *at = officialLastText(path, (size_t)(end - path), "' at line ");
        if (!at || !officialModFromPath(path, (size_t)(at - path), result->mod, sizeof result->mod)) {
            result->kind = OFFICIAL_MOD_CHECK_NO_VERDICT;
            result->reason = "error_outside_a_mod";
            return;
        }
        result->kind = OFFICIAL_MOD_CHECK_MOD_ERROR;
        at += strlen("' at line ");
        char number[16];
        size_t n = 0;
        while (at < end && n + 1 < sizeof number && (*at == '-' || (*at >= '0' && *at <= '9'))) number[n++] = *at++;
        number[n] = '\0';
        result->line = atoi(number);
        const char *trace = officialLastText(text, (size_t)(stop - text), "\nError in mod '");
        if (trace) {
            at = trace + 1;
            while (at < end && *at != '\n') at++;
        }
        while (at < end && (*at == '\r' || *at == '\n')) at++;
        for (n = 0; at < end && *at != '\r' && *at != '\n' && n + 1 < sizeof result->message; n++) result->message[n] = *at++;
        result->message[n] = '\0';
        const char *runOn = officialLastText(result->message, n, OFFICIAL_MOD_CHECK_RUN_ON_TEXT);
        if (runOn) result->message[runOn - result->message] = '\0';
        return;
    }
    if (officialLastText(text, len, OFFICIAL_MOD_CHECK_END_TEXT)) {
        result->kind = OFFICIAL_MOD_CHECK_CLEAN;
        return;
    }
    const char *parse = officialLastText(text, len, OFFICIAL_MOD_CHECK_PARSE_TEXT);
    if (parse) {
        const char *path = parse + strlen(OFFICIAL_MOD_CHECK_PARSE_TEXT);
        const char *at = officialLastText(path, (size_t)(end - path), " (level ");
        if (at && officialModFromPath(path, (size_t)(at - path), result->mod, sizeof result->mod)) {
            result->kind = OFFICIAL_MOD_CHECK_MOD_STOPPED;
            return;
        }
    }
    result->kind = OFFICIAL_MOD_CHECK_NO_VERDICT;
    result->reason = "no_end_text";
}

typedef struct {
    const OfficialModList *enabled;
    OfficialModList *drop;
    int runs;
    int failureCount;
    OfficialModCheckResult failures[OFFICIAL_BOX_ROWS];
    const char *reason;
} OfficialModCheck;

typedef enum { OFFICIAL_NEXT_RUN_AGAIN, OFFICIAL_NEXT_READY, OFFICIAL_NEXT_GIVE_UP } OfficialModCheckNext;

static __attribute__((unused)) void officialModCheckBegin(OfficialModCheck *modCheck, const OfficialModList *enabled, OfficialModList *drop) {
    memset(modCheck, 0, sizeof *modCheck);
    modCheck->enabled = enabled;
    modCheck->drop = drop;
}

static __attribute__((unused)) OfficialModCheckNext officialModCheckStep(OfficialModCheck *modCheck, const OfficialModCheckResult *result) {
    modCheck->runs++;
    if (result->kind == OFFICIAL_MOD_CHECK_CLEAN) return OFFICIAL_NEXT_READY;
    if (result->kind == OFFICIAL_MOD_CHECK_NO_VERDICT) {
        modCheck->reason = result->reason;
        return OFFICIAL_NEXT_GIVE_UP;
    }
    if (!officialListHas(modCheck->enabled, result->mod)) {
        modCheck->reason = "named_mod_not_on_list";
        return OFFICIAL_NEXT_GIVE_UP;
    }
    if (officialListHas(modCheck->drop, result->mod)) {
        modCheck->reason = "named_mod_already_off";
        return OFFICIAL_NEXT_GIVE_UP;
    }
    if (!officialListAdd(modCheck->drop, result->mod)) {
        modCheck->reason = "list_full";
        return OFFICIAL_NEXT_GIVE_UP;
    }
    if (modCheck->failureCount < OFFICIAL_BOX_ROWS) modCheck->failures[modCheck->failureCount] = *result;
    modCheck->failureCount++;
    return OFFICIAL_NEXT_RUN_AGAIN;
}

static __attribute__((unused)) void officialWrapBoxText(const char *text, int width, char *out, size_t size) {
    size_t used = 0;
    int column = 0;
    for (const char *c = text; *c && used + 2 < size;) {
        if (*c == '\n') {
            out[used++] = *c++;
            column = 0;
        } else if (*c == ' ') {
            size_t next = strcspn(c + 1, " \n");
            out[used++] = column + 1 + (int)next > width ? '\n' : ' ';
            column = out[used - 1] == '\n' ? 0 : column + 1;
            c++;
        } else {
            if (column >= width) {
                out[used++] = '\n';
                column = 0;
            }
            out[used++] = *c++;
            column++;
        }
    }
    out[used] = '\0';
}

static __attribute__((unused)) void officialFailedModsText(const OfficialModCheck *modCheck, char *out, size_t size) {
    static char rows[OFFICIAL_BOX_MAX];
    size_t used = 0;
    int written = snprintf(rows, sizeof rows, "%s", OFFICIAL_BOX_FAILED_HEAD);
    used = written > 0 ? (size_t)written : 0;
    int shown = modCheck->failureCount < OFFICIAL_BOX_ROWS ? modCheck->failureCount : OFFICIAL_BOX_ROWS;
    for (int i = 0; i < shown && used < sizeof rows; i++) {
        const OfficialModCheckResult *f = &modCheck->failures[i];
        written = snprintf(rows + used, sizeof rows - used, "%s%.48s", i ? "\n" : "", f->mod);
        if (written > 0) used += (size_t)written;
    }
    if (used < sizeof rows && modCheck->failureCount > shown) snprintf(rows + used, sizeof rows - used, "\nand %d more", modCheck->failureCount - shown);
    officialWrapBoxText(rows, OFFICIAL_BOX_WIDTH, out, size);
}

static __attribute__((unused)) const char *officialNotReadWords(const char *reason) {
    static const struct { const char *reason, *words; } WORDS[] = {
        {"time_limit", "no answer from CoE5 in time."},
        {"no_mod_check_text", "no text only mode in this CoE5 patch."},
        {"no_end_text", "CoE5 stopped before the last mod."},
        {"error_outside_a_mod", "a Mod Error outside any mod."},
        {"named_mod_not_on_list", "a Mod Error in a disabled mod."},
        {"named_mod_already_off", "a Mod Error with the mod disabled."},
        {"coe5se_turned_on", "CoE5SE enabled during the mod check."},
        {"start_failed", "CoE5 didn't start."},
    };
    for (size_t i = 0; i < sizeof WORDS / sizeof WORDS[0]; i++)
        if (!strcmp(WORDS[i].reason, reason)) return WORDS[i].words;
    return "the mod list couldn't be changed.";
}

static __attribute__((unused)) void officialNotReadText(const char *reason, char *out, size_t size) {
    char words[OFFICIAL_BOX_MAX];
    snprintf(words, sizeof words, OFFICIAL_BOX_NOT_READ, officialNotReadWords(reason));
    officialWrapBoxText(words, OFFICIAL_BOX_WIDTH, out, size);
}

static __attribute__((unused)) int officialModCheckKeepsArg(const char *arg) {
    static const char *const EXACT[] = {"--nomods", "--steam", "--nosteam"};
    static const char *const PREFIX[] = {"--keyfile=", "--loadmod="};
    for (size_t i = 0; i < sizeof EXACT / sizeof EXACT[0]; i++)
        if (!strcmp(arg, EXACT[i])) return 1;
    for (size_t i = 0; i < sizeof PREFIX / sizeof PREFIX[0]; i++)
        if (!strncmp(arg, PREFIX[i], strlen(PREFIX[i]))) return 1;
    return 0;
}

static __attribute__((unused)) int officialArgIsNetwork(const char *arg) {
    static const char *const NETWORK[] = {"--lobbyclient", "--lobbygame", "--lobbyserver", "--seclobby", "--server", "--client"};
    for (size_t i = 0; i < sizeof NETWORK / sizeof NETWORK[0]; i++)
        if (!strcmp(arg, NETWORK[i])) return 1;
    return 0;
}

static __attribute__((unused)) int officialModCheckArgs(int count, char *const *args, const char **out, int capacity) {
    int n = 0;
    if (capacity < 3 || count < 1) return 0;
    out[n++] = args[0];
    out[n++] = OFFICIAL_MOD_CHECK_SWITCH;
    out[n++] = "-d";
    for (int i = 1; i < count && n < capacity; i++)
        if (officialModCheckKeepsArg(args[i])) out[n++] = args[i];
    return n;
}

static __attribute__((unused)) int officialFileHasTexts(const char *path, const char *const *texts, int count) {
    enum { PIECE = 1024 * 1024, OVERLAP = 256 };
    unsigned found = 0, all = (1u << count) - 1;
    char *piece = malloc(PIECE + OVERLAP);
    FILE *f = piece ? fopen(path, "rb") : NULL;
    if (!f) {
        free(piece);
        return 0;
    }
    size_t kept = 0, n;
    while (found != all && (n = fread(piece + kept, 1, PIECE, f)) > 0) {
        size_t size = kept + n;
        for (int i = 0; i < count; i++)
            if (!(found & (1u << i)) && officialLastText(piece, size, texts[i])) found |= 1u << i;
        kept = size < OVERLAP ? size : OVERLAP;
        memmove(piece, piece + size - kept, kept);
    }
    fclose(f);
    free(piece);
    return found == all;
}

static __attribute__((unused)) int officialLogShowsStubPassed(const char *text, size_t len, unsigned long pid) {
    char tail[40];
    snprintf(tail, sizeof tail, " pid=%lu", pid);
    size_t tailSize = strlen(tail);
    const char *end = text + len;
    for (const char *line = text; line < end;) {
        const char *stop = memchr(line, '\n', (size_t)(end - line));
        const char *lineEnd = stop ? stop : end;
        size_t size = (size_t)(lineEnd - line);
        if (size && line[size - 1] == '\r') size--;
        if (size > tailSize && !memcmp(line + size - tailSize, tail, tailSize) &&
            officialLastText(line, size, "event=loader_inactive") && officialLastText(line, size, "reason=no_coe5se_mod"))
            return 1;
        line = stop ? stop + 1 : end;
    }
    return 0;
}

static __attribute__((unused)) const char *officialStripForChild(const char *config, const char *backup, const char *session, const char *done,
                                                                 const char *doneText, const OfficialModList *enabled, const OfficialModList *drop) {
    if (!writeAutoEnableFile(done, doneText)) return "done_write_failed";
    return officialSessionStrip(config, backup, session, enabled, drop);
}

static __attribute__((unused)) int officialInsertLibraryValue(const char *current, const char *library, char *out, size_t size) {
    if (!library[0] || strchr(library, ':')) return 0;
    int written = current && current[0] ? snprintf(out, size, "%s:%s", current, library) : snprintf(out, size, "%s", library);
    return written > 0 && (size_t)written < size;
}

typedef struct {
    void *context;
    int (*hasModCheckTexts)(void *context);
    int (*startModCheck)(void *context);
    int (*startLobby)(void *context);
    void (*waitForStub)(void *context);
    int (*waitModCheck)(void *context);
    void (*waitLobby)(void *context);
    size_t (*readOutput)(void *context, char *out, size_t size);
    int (*ask)(void *context, const char *text);
    void (*tell)(void *context, const char *text);
    void (*startNormalGame)(void *context);
} OfficialWatchOps;

typedef struct {
    const char *config, *backup, *session, *done, *doneText;
} OfficialWatchPaths;

static __attribute__((unused)) int officialConfigHasCoe5se(const char *path) {
    static unsigned char data[CONFIG_MAX];
    static OfficialModList list;
    size_t len;
    return !officialReadFile(path, data, sizeof data, &len) && officialConfigList(data, len, &list) && officialListHas(&list, COE5SE_MOD);
}

static __attribute__((unused)) void officialWatchPutListBack(const OfficialWatchPaths *paths, const OfficialModList *enabled, const char *at) {
    const char *reason = officialSessionRestore(paths->config, paths->session, paths->done, paths->doneText, enabled);
    writeLog("official_mods_restored", "ok=%d at=%s mods=%d reason=%s", reason == NULL, at, enabled->count, reason ? reason : "-");
}

static __attribute__((unused)) int officialModCheckRun(const OfficialWatchOps *ops, const OfficialWatchPaths *paths) {
    static OfficialModList enabled, drop;
    static OfficialModCheck modCheck;
    static unsigned char data[CONFIG_MAX];
    static char box[OFFICIAL_BOX_MAX], words[OFFICIAL_BOX_MAX];
    static OfficialModCheckResult result;
    size_t configSize;
    const char *why = officialReadFile(paths->config, data, sizeof data, &configSize);
    if (!why && !officialConfigList(data, configSize, &enabled)) why = "config_unreadable";
    drop.count = 0;
    officialListAdd(&drop, COE5SE_MOD);
    officialModCheckBegin(&modCheck, &enabled, &drop);
    char *output = malloc(OFFICIAL_MOD_CHECK_OUTPUT_MAX + 1);
    if (!why) why = !output ? "out_of_memory" : ops->hasModCheckTexts(ops->context) ? NULL : "no_mod_check_text";
    const char *noLobby = getenv("COE5SE_OFFICIAL_NO_LOBBY");
    writeLog("official_mod_check_started", "enabled=%d no_lobby=%d reason=%s", enabled.count, noLobby && noLobby[0], why ? why : "-");
    const char *backup = paths->backup;
    int changed = 0;
    while (!why) {
        why = officialStripForChild(paths->config, backup, paths->session, paths->done, paths->doneText, &enabled, &drop);
        backup = NULL;
        changed = 1;
        if (why) break;
        if (!ops->startModCheck(ops->context)) {
            why = "start_failed";
            break;
        }
        ops->waitForStub(ops->context);
        remove(paths->done);
        int ended = ops->waitModCheck(ops->context);
        size_t len = ended ? ops->readOutput(ops->context, output, OFFICIAL_MOD_CHECK_OUTPUT_MAX) : 0;
        output[len] = '\0';
        officialModCheckRead(output, len, &result);
        if (!ended || officialConfigHasCoe5se(paths->config)) {
            memset(&result, 0, sizeof result);
            result.kind = OFFICIAL_MOD_CHECK_NO_VERDICT;
            result.reason = ended ? "coe5se_turned_on" : "time_limit";
        }
        writeLog("official_mod_check_run", "run=%d result=%d mod=%s line=%d reason=%s message=%.120s", modCheck.runs + 1, result.kind, result.mod, result.line,
                 result.reason ? result.reason : "-", result.message);
        OfficialModCheckNext next = officialModCheckStep(&modCheck, &result);
        if (next == OFFICIAL_NEXT_READY) break;
        if (next == OFFICIAL_NEXT_GIVE_UP) why = modCheck.reason;
    }
    free(output);
    int go = !why;
    if (why) {
        writeLogWarning("official_mod_check_no_verdict", "reason=%s runs=%d", why, modCheck.runs);
        officialNotReadText(why, words, sizeof words);
    } else {
        writeLog("official_mod_check_clean", "runs=%d off=%d", modCheck.runs, modCheck.failureCount);
    }
    if (changed) officialWatchPutListBack(paths, &enabled, "mod_check");
    if (why) {
        ops->tell(ops->context, words);
        return 0;
    }
    if (modCheck.failureCount) officialFailedModsText(&modCheck, box, sizeof box);
    else snprintf(box, sizeof box, "%s", OFFICIAL_BOX_CLEAN);
    go = ops->ask(ops->context, box);
    writeLog("official_mod_check_answer", "restart=%d", go);
    if (!go) return 0;
    if (!officialWriteSession(paths->session, "pending", &enabled, &drop)) {
        writeLogError("official_session_written", "ok=0 reason=session_write_failed");
        officialNotReadText("session_write_failed", words, sizeof words);
        ops->tell(ops->context, words);
        return 0;
    }
    return 1;
}

static __attribute__((unused)) void officialLobbyRun(const OfficialWatchOps *ops, const OfficialWatchPaths *paths) {
    static OfficialModList enabled, drop;
    char state[32];
    if (!officialReadSession(paths->session, state, sizeof state, &enabled, &drop) || strcmp(state, "pending")) {
        writeLogError("official_watch", "ok=0 reason=no_pending_session");
        ops->startNormalGame(ops->context);
        return;
    }
    writeLog("official_watch", "ok=1 enabled=%d off=%d", enabled.count, drop.count);
    const char *why = officialStripForChild(paths->config, paths->backup, paths->session, paths->done, paths->doneText, &enabled, &drop);
    if (why || !ops->startLobby(ops->context)) {
        writeLogError("official_lobby_started", "ok=0 reason=%s", why ? why : "start_failed");
        officialWatchPutListBack(paths, &enabled, "watcher");
        ops->startNormalGame(ops->context);
        return;
    }
    writeLog("official_lobby_started", "ok=1 off=%d", drop.count);
    ops->waitForStub(ops->context);
    remove(paths->done);
    ops->waitLobby(ops->context);
    writeLog("official_lobby_ended", "at=watcher");
    officialWatchPutListBack(paths, &enabled, "watcher");
}
