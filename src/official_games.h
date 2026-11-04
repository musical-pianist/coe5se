#pragma once

static int officialMode;
static int officialModsAllowed;

__declspec(dllexport) int coe5seOfficialModsAllowed(void) { return officialMode && officialModsAllowed; }

#define OFFICIAL_GAMES_TITLE "CoE5SE: Official games"
#define OFFICIAL_GAMES_TEXT                                                                                    \
    "Normally, Script Extender doesn't allow\nofficial Illwinter server connections.\n\nHowever, right now it supports a " \
    "local-only fix\nthat DOES NOT AFFECT ILLWINTER SERVERS.\nDo you want to restart the game with this fix?"

static int writeOfficialSessionFile(void) {
    static unsigned char data[CONFIG_MAX];
    static OfficialModList enabled, drop;
    char config[MAX_PATH], session[MAX_PATH];
    size_t len;
    if (!buildPath(config, sizeof config, __func__, "%s" PATH_SEP CONFIG_FILE, userDir()) ||
        !coe5seUserFile(OFFICIAL_SESSION_FILE, session, sizeof session))
        return 0;
    const char *reason = officialReadFile(config, data, sizeof data, &len);
    if (!reason && !officialConfigList(data, len, &enabled)) reason = "config_unreadable";
    if (reason) {
        writeLogError("official_session_written", "ok=0 reason=%s", reason);
        return 0;
    }
    drop.count = 0;
    officialListAdd(&drop, COE5SE_MOD);
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        if (!modEnabled(m) || !officialListHas(&enabled, modName(m))) continue;
        if (officialFolderNeedsCoe5se(modFolder(m), modName(m))) officialListAdd(&drop, modName(m));
    }
    int ok = officialWriteSession(session, "pending", &enabled, &drop);
    writeResultLog(ok, "official_session_written", "ok=%d enabled=%d off=%d", ok, enabled.count, drop.count);
    return ok;
}

static void officialGamesBox(void) {
    writeLog("official_games_box", "shown=1");
    int answer = messageBox(OFFICIAL_GAMES_TITLE, OFFICIAL_GAMES_TEXT, "Restart", "Cancel", 0, 0, ESCAPE_KEY());
    writeLog("official_games_answer", "restart=%d", answer == 1);
    if (answer != 1) return;
    writeOfficialSessionFile();
    relaunchGameWith("official_games", LOBBY_CLIENT_SWITCH);
}

// @platform-split official_games_box: Windows redirects the call through a stub in a near page, Mac through a trampoline in the header padding
static int redirectLobbyCallAt(unsigned char *call, unsigned char *target, void (*hook)(void), const char *label) {
#ifdef __APPLE__
    if (blTarget(call) != target) {
        writeLog("hook_skipped", "site=%s reason=not_the_expected_call", label);
        return 0;
    }
    unsigned char *trampoline = newTrampoline((void *)hook);
    uint32_t word = trampoline ? blTo(call, trampoline) : 0;
    if (trampoline && writeGameCode(call, &word, 4)) return 1;
    writeLog("hook_skipped", "site=%s reason=%s", label, trampoline ? "code_unwritable" : "no_padding");
    return 0;
#else
    unsigned char *stub = allocNear(call);
    if (!stub) {
        writeLog("hook_skipped", "site=%s reason=no_near_page", label);
        return 0;
    }
    makePagesReadExecute(stub, 0x1000);
    return redirectCall((unsigned int)(call - gameBase), (unsigned int)(target - gameBase), stub, (void *)hook, label,
                        NULL);
#endif
}

static int redirectLobbyCall(void (*hook)(void), const char *label) {
    return redirectLobbyCallAt(GAME_PTR(RVA_NET_SCREEN_LOBBY_CALL), GAME_PTR(RVA_LOBBY_START), hook, label);
}

#define OFFICIAL_BLOCK_TEXT "Normally, Script Extender doesn't allow official Illwinter server connections."

static void showOfficialBlockDialog(void) {
#ifdef __APPLE__
    CFOptionFlags response = kCFUserNotificationDefaultResponse;
    CFUserNotificationDisplayAlert(0, kCFUserNotificationCautionAlertLevel, NULL, NULL, NULL, CFSTR(OFFICIAL_GAMES_TITLE),
                                   CFSTR(OFFICIAL_BLOCK_TEXT), CFSTR("Ok"), NULL, NULL, &response);
#else
    MessageBoxA(NULL, OFFICIAL_BLOCK_TEXT, OFFICIAL_GAMES_TITLE, MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
#endif
}

static void officialBlockHook(void) {
    writeLog("official_block_shown", "at=network_screen");
    showOfficialBlockDialog();
}

static LobbyCallShape officialLobbyShape;
static const unsigned char *officialLobbyCode;
static int officialLobbySearched;

static int findOfficialLobbyCall(void) {
    if (officialLobbySearched) return !officialLobbyShape.reason;
    officialLobbySearched = 1;
    size_t size;
    if (!findGameCode(gameBase, &officialLobbyCode, &size)) {
        officialLobbyShape.reason = "no_code_section";
    } else {
#ifdef __aarch64__
        officialLobbyShape = findLobbyCallShape(officialLobbyCode, size, 1);
#else
        officialLobbyShape = findLobbyCallShape(officialLobbyCode, size, 0);
#endif
    }
    if (officialLobbyShape.reason) {
        writeLogError("official_block_unavailable", "reason=%s matches=%d", officialLobbyShape.reason, officialLobbyShape.matches);
        return 0;
    }
    unsigned long long address = (unsigned long long)(officialLobbyCode + officialLobbyShape.call - gameBase);
#ifdef __APPLE__
    address += 0x100000000ULL;
#endif
    writeLog("official_lobby_call_found", "address=0x%llx", address);
    return 1;
}

static void installOfficialBlock(void) {
    if (!findOfficialLobbyCall()) return;
    unsigned char *call = (unsigned char *)officialLobbyCode + officialLobbyShape.call;
    unsigned char *target = (unsigned char *)officialLobbyCode + officialLobbyShape.target;
    int ok = redirectLobbyCallAt(call, target, officialBlockHook, "official_block");
    writeResultLog(ok, "official_block_installed", "ok=%d", ok);
}

static void endBlockedOfficialProcess(void) {
    writeLog("official_block_shown", "at=official_launch");
    showOfficialBlockDialog();
    writeLog("official_block_exit", "game_version=%s", gameBuild ? gameBuild->gameVersion : "unknown");
#ifdef __APPLE__
    exit(0);
#else
    ExitProcess(0);
#endif
}

static void installOfficialGamesBox(void) {
    const char *refusal = officialGamesRefusal(gameCodeMatches, gameBuild ? gameBuild->gameVersion : NULL);
    if (refusal) {
        writeLog("hook_skipped", "site=official_games_box reason=%s", refusal);
        if (officialGamesBlocked(gameBuild ? gameBuild->gameVersion : NULL)) installOfficialBlock();
        return;
    }
    int ok = redirectLobbyCall(officialGamesBox, "official_games_box");
    writeResultLog(ok, "official_games_box_installed", "ok=%d", ok);
}

static int officialCodeMatches(void) {
    const unsigned char *code;
    size_t size;
    char hex[GAME_CODE_SHA256_HEX_LEN + 1] = "";
    if (!gameBuild || !findGameCode(gameBase, &code, &size)) {
        writeLogError("official_code_check", "match=0 reason=%s", gameBuild ? "no_code_section" : "unknown_game_build");
        return 0;
    }
    int hashed = terrainFix.installed
                     ? hashCodeWithBytes(code, size, terrainFix.site, terrainFix.original, terrainFix.siteSize, hex)
                     : hashMemorySha256(code, size, hex);
    if (!hashed) {
        writeLogError("official_code_check", "match=0 reason=hash_failed bytes=%zu", size);
        return 0;
    }
    if (strcmp(hex, gameBuild->codeSha256)) {
        writeLogError("official_code_check", "match=0 reason=game_code_differs sha256=%s", hex);
        return 0;
    }
    if (terrainFix.installed && !terrainFixInPlace()) {
        writeLogError("official_code_check", "match=0 reason=fix_bytes_differ");
        return 0;
    }
    writeLog("official_code_check", "match=1 sha256=%s fix_bytes=%zu", hex, terrainFix.installed ? terrainFix.siteSize : (size_t)0);
    return 1;
}

static void startOfficialMode(void) {
    officialMode = 1;
    identifyGameBuild();
    writeLog("coe5se_version", "api=%d game_build=%s game_version=%s", PLUGIN_API_VERSION, gameBuildId,
             gameBuild ? gameBuild->gameVersion : "unknown");
    int fixes = installModTerrainFix();
    if (officialGamesBlocked(gameBuild ? gameBuild->gameVersion : NULL)) endBlockedOfficialProcess();
    officialModsAllowed = !officialGamesRefusal(gameCodeMatches, gameBuild ? gameBuild->gameVersion : NULL);
    int matches = officialCodeMatches();
    if (!matches && fixes && removeModTerrainFix("official_code_differs")) fixes = 0;
    writeResultLog(matches, "official_mode", "fixes=%d code_matches=%d", fixes, matches);
}
