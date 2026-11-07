#define NETWORK_MODS_TITLE     "CoE5SE: Mod files differ"
#define NETWORK_WARNING_TITLE  "CoE5SE: Check failed"
#define NETWORK_CHECK_END      "CoE5SE end:"
#define NETWORK_CHECK_WARN     "CoE5SE warn:"
#define NETWORK_SCRIPT_STOP    "CoE5SE stop:"
#define NETWORK_MOD_SHOWN_MAX  80
#define CLIENT_TURN_FILE_NAME  "tmp_myturn"
#define NET_SEND_EXCEPT_NONE   (-999)
_Static_assert(POPUP_TEXT_SIZE >= NET_LOBBY_MESSAGE_MAX, "a lobby message fits the box's text");

static int runsNetworkChecks(void) { return isNetworkGame() && !isOfficialGame(); }

// ---- The game's packets -------------------------------------------------------------------------
#define TEXT_ONLY_SEND_WAIT_MS 1000
static void okBoxOrLog(const char *title, const char *text) {
    if (!isTextOnly()) {
        okBox(title, text);
        return;
    }
    writeLogWarning("box_not_shown", "reason=text_only title=%s", title);
#ifdef __APPLE__
    usleep(TEXT_ONLY_SEND_WAIT_MS * 1000);
#else
    Sleep(TEXT_ONLY_SEND_WAIT_MS);
#endif
}

static void sendLobbyMessage(const char *text) {
    GAME_FN(void (*)(void), RVA_NET_BEGIN_PACKET)();
    GAME_FN(void (*)(int), RVA_NET_PUT_BYTE)(isNetworkClient() ? NET_PACKET_LOBBY_MESSAGE : NET_PACKET_LOBBY_MESSAGE_ALL);
    GAME_FN(void (*)(const char *), RVA_NET_PUT_STRING)(text);
    GAME_FN(void (*)(int), RVA_NET_SEND_PACKET)(NET_SEND_EXCEPT_NONE);
}

static char readPacketByte(void) { return GAME_FN(char (*)(void), RVA_NET_GET_BYTE)(); }

static void addEveryPlayerMessage(const char *text) {
    int index = (int)((AddMessageFn)(gameBase + RVA_ADD_MESSAGE))(-1, MESSAGE_EVERY_HUMAN, -1, -1, 0, 0, NETWORK_WARNING_TITLE, "%s", text);
    writeLog("network_check_warned", "index=%d", index);
}

static void showModsAndEndGame(const char *text) {
    closeAndDeleteClientTurnFile();
    okBoxOrLog(NETWORK_MODS_TITLE, text);
    GAME_FN(void (*)(const char *), RVA_EXIT_GAME)("");
}

static void (*sendNetworkCheckText)(const char *text) = sendLobbyMessage;
static char (*readNetworkCheckByte)(void) = readPacketByte;
static void (*addNetworkCheckMessage)(const char *text) = addEveryPlayerMessage;
static void (*endGameOnModFiles)(const char *text) = showModsAndEndGame;

static const char *networkCheckBody(const char *message, const char *prefix) {
    size_t length = strlen(prefix);
    return strncmp(message, prefix, length) ? NULL : message + length;
}

static void sendNetworkCheck(const char *prefix, const char *text) {
    char message[NET_LOBBY_MESSAGE_MAX];
    snprintf(message, sizeof message, "%s%.*s", prefix, (int)(sizeof message - strlen(prefix) - 1), text);
    sendNetworkCheckText(message);
}

// ---- Ending and warning -------------------------------------------------------------------------
static void endLanGameOverModFiles(const char *text) {
    sendNetworkCheck(NETWORK_CHECK_END, text);
    writeLog("mod_files_differ_exit", "side=%s", isNetworkClient() ? "client" : "server");
    endGameOnModFiles(text);
}

static char lastNetworkWarning[NET_LOBBY_MESSAGE_MAX];

#define NETWORK_WARNINGS_WAITING 8
static char networkWarningsWaiting[NETWORK_WARNINGS_WAITING][NET_LOBBY_MESSAGE_MAX];
static int networkWarningWaitingCount;

static void addWaitingNetworkWarnings(void) {
    if (!networkWarningWaitingCount || netRoleNow() != NET_ROLE_HOST) return;
    for (int i = 0; i < networkWarningWaitingCount; i++) addNetworkCheckMessage(networkWarningsWaiting[i]);
    networkWarningWaitingCount = 0;
}

static void warnAllPlayersOfCheck(const char *text) {
    if (!runsNetworkChecks()) return;
    if (isNetworkClient()) {
        sendNetworkCheck(NETWORK_CHECK_WARN, text);
        return;
    }
    if (!strcmp(lastNetworkWarning, text)) return;
    snprintf(lastNetworkWarning, sizeof lastNetworkWarning, "%s", text);
    if (networkWarningWaitingCount == NETWORK_WARNINGS_WAITING) {
        writeLogWarning("network_warning_dropped", "reason=too_many_waiting");
        return;
    }
    snprintf(networkWarningsWaiting[networkWarningWaitingCount++], sizeof networkWarningsWaiting[0], "%s", text);
    writeLog("network_warning_waiting", "count=%d", networkWarningWaitingCount);
}

static void takeClientScriptStop(const char *report);

static void readServerLobbyMessage(const char *message) {
    if (!runsNetworkChecks() || isNetworkClient()) return;
    const char *why = networkCheckBody(message, NETWORK_CHECK_END);
    if (why) {
        writeLogWarning("network_check_received", "reason=mod_files_differ side=server");
        endLanGameOverModFiles(why);
        return;
    }
    why = networkCheckBody(message, NETWORK_CHECK_WARN);
    if (why) warnAllPlayersOfCheck(why);
    why = networkCheckBody(message, NETWORK_SCRIPT_STOP);
    if (why) takeClientScriptStop(why);
}

static void readClientUnknownPacket(int type) {
    if (type != NET_PACKET_LOBBY_MESSAGE_ALL || !runsNetworkChecks() || !isNetworkClient()) return;
    char message[NET_LOBBY_MESSAGE_MAX];
    size_t n = 0;
    for (char ch; (ch = readNetworkCheckByte()) != '\0';)
        if (n + 1 < sizeof message) message[n++] = ch;
    message[n] = '\0';
    const char *why = networkCheckBody(message, NETWORK_CHECK_END);
    if (!why) return;
    writeLogWarning("network_check_received", "reason=mod_files_differ side=client");
    endGameOnModFiles(why);
}

// ---- The compare --------------------------------------------------------------------------------
static const char *whatModFilesDiffer(u64 hash, u64 scripts, u64 otherHash, u64 otherScripts) {
    if (hash != otherHash && scripts != otherScripts) return "scripts, .c5m or .c5se";
    if (hash != otherHash) return ".c5m or .c5se";
    return scripts != otherScripts ? "scripts" : NULL;
}

typedef struct { char mod[NETWORK_MOD_SHOWN_MAX + 1]; const char *what; int differ; } ModFileDifference;

static void noteModFileDifference(ModFileDifference *found, const char *name, const char *what) {
    found->differ++;
    if (found->differ > 1) return;
    size_t n = 0;
    for (; name[n] && n < NETWORK_MOD_SHOWN_MAX; n++) found->mod[n] = name[n] >= ' ' && name[n] < 0x7f ? name[n] : '?';
    found->mod[n] = '\0';
    found->what = what;
}

static const char *compareModFile(const SavedModHash *saved, int free, SaveFile kind) {
    int mod = modIndexExact(saved->name);
    if (kind == SAVE_FILE_TURN) {
        const ModContentNote *note = mod >= 0 && modEnabled(mod) ? findModContentNote(saved->name) : NULL;
        if (!note || !note->hashed) return free ? NULL : "not on server";
        if (free && countsAsContentFree(note->contentFree, note->hasScript, note->personal || note->noMultiplayer)) return NULL;
        return whatModFilesDiffer(saved->hash, saved->scriptHash, note->hash, note->scriptHash);
    }
    ModFileScan scan;
    if (mod < 0 || !scanMod(mod, &scan)) return NULL;
    if (free && countsAsContentFree(scan.contentFree, scan.hasScript, scan.personal || scan.noMultiplayer)) return NULL;
    return whatModFilesDiffer(saved->hash, saved->scriptHash, scan.hash, hashModScriptsIfRun(mod, scan.noMultiplayer));
}

static int savedModIsContentFree(const SavedMods *mods, const char *name) {
    for (int i = 0; i < mods->count; i++)
        if (!strcmp(mods->mods[i].name, name)) return (mods->mods[i].flags & MOD_ADDED_NO_CONTENT) != 0;
    return 0;
}

static int savedModIsHashed(const SavedModHashes *hashes, const char *name) {
    for (int i = 0; i < hashes->count; i++)
        if (!strcmp(hashes->mods[i].name, name)) return 1;
    return 0;
}

static void compareModFiles(const SavedMods *mods, const SavedModHashes *hashes, SaveFile kind, ModFileDifference *found) {
    for (int i = 0; i < hashes->count; i++) {
        const char *what = compareModFile(&hashes->mods[i], savedModIsContentFree(mods, hashes->mods[i].name), kind);
        if (what) noteModFileDifference(found, hashes->mods[i].name, what);
    }
    if (kind != SAVE_FILE_TURN) return;
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        const ModContentNote *note = modEnabled(m) ? findModContentNote(modName(m)) : NULL;
        if (note && note->hashed && !countsAsContentFree(note->contentFree, note->hasScript, note->personal || note->noMultiplayer) &&
            !savedModIsHashed(hashes, modName(m)))
            noteModFileDifference(found, modName(m), "not on client");
    }
}

static void modFilesText(char *text, size_t size, const ModFileDifference *found) {
    size_t n = 0;
    text[0] = '\0';
    appendText(text, size, &n, "Mod '%s' differs from the version that generated the current turn.\n\nDiffers: %s\n\nSame mod files required for all players.",
               found->mod, found->what);
}

static void checkModFiles(SaveFile kind, const char *where, const SavedMods *mods, const SavedModHashes *hashes) {
    if (!runsNetworkChecks()) return;
    int hostOwnLoad = kind == SAVE_FILE_SAVE && G8(RVA_NET_SERVER) && !isNetworkClient();
    ModFileDifference found = {.what = NULL};
    int recorded = !strcmp(mods->result, "found") && !strcmp(hashes->result, "found");
    if (recorded) compareModFiles(mods, hashes, kind, &found);
    writeLog("mod_files_compared", "file=%s at=%s record=%s mods=%d differ=%d mod=%s what=%s", SAVE_FILE_NAMES[kind], where, recorded ? "found" : hashes->result,
             hashes->count, found.differ, found.differ ? found.mod : "-", found.differ ? found.what : "-");
    if (hostOwnLoad || (recorded && !found.differ)) return;
    static char text[POPUP_TEXT_SIZE];
    if (recorded) modFilesText(text, sizeof text, &found);
    else snprintf(text, sizeof text, "No mod record in %s file.\n\nSame mod files required for all players.", kind == SAVE_FILE_TURN ? "turn" : "game");
    writeLogWarning("mod_files_differ", "reason=%s file=%s at=%s mod=%s what=%s", recorded ? "differs" : "no_record", SAVE_FILE_NAMES[kind], where,
                    recorded ? found.mod : "-", recorded ? found.what : "-");
    endLanGameOverModFiles(text);
}

static void checkLoadedSaveModFiles(void) { checkModFiles(SAVE_FILE_SAVE, "open", &savedMods, &savedModHashes); }

static void checkTailModFiles(SaveFile kind, const SaveChunk *modc, const SaveChunk *modh) {
    if (!runsNetworkChecks()) return;
    SavedMods mods = {"none", NULL, 0};
    SavedModHashes hashes = {"none", NULL, 0};
    if (modc) mods.result = readModContentPayload(modc->payload, modc->size, &mods);
    if (modh) hashes.result = readModHashPayload(modh->payload, modh->size, &hashes);
    if (strcmp(mods.result, "found")) free(mods.mods), mods.mods = NULL, mods.count = 0;
    if (strcmp(hashes.result, "found")) free(hashes.mods), hashes.mods = NULL, hashes.count = 0;
    checkModFiles(kind, "tail", &mods, &hashes);
    free(mods.mods);
    free(hashes.mods);
}

// ---- CoE5SE's own binary -------------------------------------------------------------------------
#define NETWORK_BINARY_PLATFORMS 2
static u64 reportedBinaryHashes[NETWORK_BINARY_PLATFORMS];
static int reportedBinarySeen[NETWORK_BINARY_PLATFORMS];
static unsigned char binaryToldSlots[PLAYER_SLOTS + 1];

static void shownSlotName(int slot, char *out, size_t size) {
    const char *name = slot >= 0 && slot < PLAYER_SLOTS ? (const char *)playerSlotAddress(RVA_SLOT_NAME, slot, PLAYER_BLOCK_SIZE) : "";
    size_t n = 0;
    for (; name[n] && n + 1 < size; n++) out[n] = name[n] >= ' ' && name[n] < 0x7f ? name[n] : '?';
    out[n] = '\0';
    if (!n) snprintf(out, size, "unknown");
}

static void checkReportedBinary(const SaveChunk *vers, int platform) {
    if (!runsNetworkChecks() || isNetworkClient()) return;
    u64 hash = 0;
    int player = -1;
    const char *result = vers ? readCoe5seReportChunk(vers, &hash, &player) : "none";
    int found = !strcmp(result, "found") && platform >= 0 && platform < NETWORK_BINARY_PLATFORMS, same = 0;
    if (found) {
        if (!reportedBinarySeen[SAVE_PLATFORM]) {
            reportedBinarySeen[SAVE_PLATFORM] = 1;
            reportedBinaryHashes[SAVE_PLATFORM] = ownBinaryHash();
        }
        if (!reportedBinarySeen[platform]) {
            reportedBinarySeen[platform] = 1;
            reportedBinaryHashes[platform] = hash;
        }
        same = reportedBinaryHashes[platform] == hash;
    }
    writeLog("coe5se_binary_compared", "result=%s written_on=%s player=%d hash=%016llx same=%d", result, savePlatformName(platform), player,
             (unsigned long long)hash, same);
    if (same) return;
    int slot = player >= 0 && player < PLAYER_SLOTS ? player : PLAYER_SLOTS;
    if (binaryToldSlots[slot]) return;
    binaryToldSlots[slot] = 1;
    char name[33], text[NET_LOBBY_MESSAGE_MAX];
    shownSlotName(player, name, sizeof name);
    snprintf(text, sizeof text, "CoE5SE files differ between players, same version.\n\nPlayer: %s", name);
    writeLogWarning("coe5se_binary_differs", "reason=%s player=%d", found ? "differs" : "no_report", player);
    warnAllPlayersOfCheck(text);
}

// ---- The client's turn file ---------------------------------------------------------------------
static char clientTurnFilePath[MAX_PATH];
static void *clientTurnFile;

static void noteClientTurnFile(const char *path, void *file) {
    clientTurnFilePath[0] = '\0';
    clientTurnFile = NULL;
    if (!runsNetworkChecks() || !isNetworkClient()) return;
    const char *name = strrchr(path, '/');
    if (strcmp(name ? name + 1 : path, CLIENT_TURN_FILE_NAME) || strlen(path) >= sizeof clientTurnFilePath) return;
    snprintf(clientTurnFilePath, sizeof clientTurnFilePath, "%s", path);
    clientTurnFile = file;
}

static void deleteClientTurnFile(void) {
    if (!clientTurnFilePath[0]) return;
    int removed = remove(clientTurnFilePath) == 0;
    writeResultLog(removed, "client_turn_file_deleted", "file=%s%s", CLIENT_TURN_FILE_NAME, removed ? "" : " reason=remove_failed");
    clientTurnFilePath[0] = '\0';
    clientTurnFile = NULL;
}

static void closeAndDeleteClientTurnFile(void) {
    if (!clientTurnFile) return;
#ifdef __APPLE__
    fclose((FILE *)clientTurnFile);
#else
    GAME_FN(GameFcloseFn, RVA_FCLOSE)(clientTurnFile);
#endif
    deleteClientTurnFile();
}

// ---- The hooks ----------------------------------------------------------------------------------
typedef void (*GameTraceFn)(const char *format, ...);

static void traceClientUnknownPacket(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int type = va_arg(ap, int);
    va_end(ap);
    GAME_FN(GameTraceFn, RVA_TRACE)(format, type);
    readClientUnknownPacket(type);
}

static void traceServerLobbyMessage(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    const char *message = va_arg(ap, const char *);
    va_end(ap);
    GAME_FN(GameTraceFn, RVA_TRACE)(format, message);
    readServerLobbyMessage(message);
}

#define NETWORK_CHECK_HOOKS 3
#ifndef __APPLE__
#define NETWORK_CHECK_STUB_SIZE 0x20
static int installNetworkCheckHooks(void) {
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION);
    if (!stubs) {
        writeLog("network_check_hooks_skipped", "reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    return redirectCall(RVA_NET_CLIENT_UNKNOWN_TRACE, RVA_TRACE, stubs, (void *)traceClientUnknownPacket, "net_client_unknown_trace", NULL) +
           redirectCall(RVA_NET_EVENTS_UNKNOWN_TRACE, RVA_TRACE, stubs + NETWORK_CHECK_STUB_SIZE, (void *)traceClientUnknownPacket, "net_events_unknown_trace", NULL) +
           redirectCall(RVA_NET_SERVER_LOBBYMSG_TRACE, RVA_TRACE, stubs + 2 * NETWORK_CHECK_STUB_SIZE, (void *)traceServerLobbyMessage, "net_server_lobbymsg_trace", NULL);
}
#else
static int installNetworkCheckHooks(void) {
    return pointMissingModsCall(RVA_NET_CLIENT_UNKNOWN_TRACE, RVA_TRACE, (void *)traceClientUnknownPacket, "net_client_unknown_trace") +
           pointMissingModsCall(RVA_NET_EVENTS_UNKNOWN_TRACE, RVA_TRACE, (void *)traceClientUnknownPacket, "net_events_unknown_trace") +
           pointMissingModsCall(RVA_NET_SERVER_LOBBYMSG_TRACE, RVA_TRACE, (void *)traceServerLobbyMessage, "net_server_lobbymsg_trace");
}
#endif

static void networkChecksInit(void) {
    writeInstallLog("network_check_hooks_installed", installNetworkCheckHooks(), NETWORK_CHECK_HOOKS);
}
