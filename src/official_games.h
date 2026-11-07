#pragma once

static int officialMode;
// @platform-split official_block: Windows redirects the call through a stub in a near page, Mac through a trampoline in the header padding
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

#define OFFICIAL_BLOCK_TEXT "Blocked with CoE5SE enabled. This is to keep you from being banned in multiplayer."
#define OFFICIAL_BLOCK_PARSING_TEXT "CoE5SE: Parsing Mods"

#define OFFICIAL_BLOCK_START_FAILED "Mod check failed:\nCoE5 didn't start.\n\nStill blocked.\nMod list unchanged."
static void showOfficialBlockDialog(void) {
    officialTellDialog(OFFICIAL_BLOCK_TEXT);
}

static void startOfficialWatch(void) {
    char id[32];
    snprintf(id, sizeof id, "%lu", (unsigned long)GetCurrentProcessId());
    officialSetEnvironment(OFFICIAL_WATCH_ENV, id);
    relaunchGameWith("official_watch", NULL);
    officialSetEnvironment(OFFICIAL_WATCH_ENV, NULL);
}

static int officialBlockUsesGameBox(const void *build, int codeMatches) {
    return build && codeMatches;
}

static int askOfficialModCheck(const char *text) {
    return messageBox(OFFICIAL_GAMES_TITLE, text, "Restart", "Cancel", 0, 0, ESCAPE_KEY()) == 1;
}

static void tellOfficialModCheck(const char *text) {
    messageBox(OFFICIAL_GAMES_TITLE, text, "Ok", "", 0, UI_KEY_ENTER, 0);
}

static void drawOfficialModCheck(void) {
    GAME_FN(uint64_t (*)(int, const char *, ...), RVA_PROGRESS_TEXT)(-1, OFFICIAL_BLOCK_PARSING_TEXT);
}

static int runOfficialModCheck(void) {
    static const OfficialModCheckUi UI = {askOfficialModCheck, tellOfficialModCheck, drawOfficialModCheck};
    HMODULE loader = GetModuleHandleA(LOADER_DLL_NAME);
    int (*run)(const OfficialModCheckUi *) = loader ? (int (*)(const OfficialModCheckUi *))(void *)GetProcAddress(loader, "coe5seRunModCheck") : NULL;
    if (!run) {
        writeLogError("official_mod_check_started", "reason=loader_has_no_mod_check");
        return 0;
    }
    return run(&UI);
}

static int officialBlockRestart(int (*check)(void), void (*start)(void)) {
    if (!check()) return 0;
    start();
    return 1;
}

static void officialBlockHook(void) {
    writeLog("official_block_shown", "at=network_screen");
    if (!officialBlockUsesGameBox(gameBuild, gameCodeMatches)) {
        showOfficialBlockDialog();
        return;
    }
    if (officialBlockRestart(runOfficialModCheck, startOfficialWatch)) tellOfficialModCheck(OFFICIAL_BLOCK_START_FAILED);
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

static void startOfficialMode(void) {
    officialMode = 1;
    identifyGameBuild();
    writeLog("coe5se_version", "api=%d game_build=%s game_version=%s", PLUGIN_API_VERSION, gameBuildId,
             gameBuild ? gameBuild->gameVersion : "unknown");
    endBlockedOfficialProcess();
}
