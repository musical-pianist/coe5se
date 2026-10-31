// @platform-split game_save: Windows' calls pass (name, FILE *); CoE5SE's outermost wrapper passes (path, name) on
#ifndef __aarch64__

static void *saveFileOpened;
static const char *saveFileOpenedPath;

static void noteSaveFileOpened(const char *path, void *file) {
    saveFileOpened = file;
    saveFileOpenedPath = path;
}

static void *saveFileOpenHook(const char *path, const char *mode) {
    void *file = GAME_FN(void *(*)(const char *, const char *), RVA_FOPEN)(path, mode);
    if (file) noteSaveFileOpened(path, file);
    return file;
}

static void *gameSaveFile;
static int saveGameToOpenFile(const char *path, const char *name) {
    (void)path;
    void *file = gameSaveFile;
    gameSaveFile = NULL;
    if (!file) {
        writeLog("game_save_skipped", "name=%s reason=no_open_file", name);
        return 0;
    }
    return GAME_FN(int (*)(const char *, void *), RVA_SAVE_GAME)(name, file);
}

static int (*nextGameSave)(const char *path, const char *name);
static int gameSaveOutermost(const char *name, void *file) {
    const char *path = file && file == saveFileOpened ? saveFileOpenedPath : "";
    if (!path[0]) writeLog("game_save_path_unknown", "name=%s", name);
    saveFileOpened = NULL;
    gameSaveFile = file;
    int result = nextGameSave(path, name);
    if (gameSaveFile) {
        writeLog("game_save_not_written", "name=%s", name);
        GAME_FN(int (*)(void *), RVA_FCLOSE)(gameSaveFile);
        gameSaveFile = NULL;
    }
    return result;
}

#define GAME_SAVE_STUB_SIZE 0x20
static int hookSaveFileOpens(void) {
    static const unsigned int CALLS[] = {RVA_AUTORUN_SAVE_FOPEN_CALL, RVA_AUTOSAVE_SAVE_FOPEN_CALL,
                                         RVA_AUTOSAVE_FUNCTION_SAVE_FOPEN_CALL, RVA_SAVE_DIALOG_SAVE_FOPEN_CALL};
    static const char *const NAMES[] = {"autorun_save_fopen", "autosave_fopen", "autosave_function_fopen", "save_dialog_fopen"};
    static int hooked;
    if (hooked) return 1;
    for (size_t i = 0; i < sizeof CALLS / sizeof CALLS[0]; i++) {
        unsigned char *call = gameBase + CALLS[i];
        int rel;
        memcpy(&rel, call + 1, 4);
        if (call[0] != 0xe8 || call + 5 + rel != gameBase + RVA_FOPEN) {
            writeLog("hook_skipped", "site=%s reason=not_the_expected_call address=%p", NAMES[i], (void *)call);
            return 0;
        }
    }
    _Static_assert(sizeof CALLS / sizeof CALLS[0] * GAME_SAVE_STUB_SIZE <= 0x1000, "every stub fits the one page allocNear gives");
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION);
    if (!stubs) {
        writeLog("hook_skipped", "site=save_fopen reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    for (size_t i = 0; i < sizeof CALLS / sizeof CALLS[0]; i++)
        redirectCall(CALLS[i], RVA_FOPEN, stubs + GAME_SAVE_STUB_SIZE * i, (void *)saveFileOpenHook, NAMES[i], NULL);
    hooked = 1;
    return 1;
}

static void gameSaveInit(void) {
    char owner[64];
    moduleFileName((void *)gameSaveOutermost, owner, sizeof owner);
    const WrapPlace outermost = {.priority = WRAP_PRIORITY_DEFAULT, .layer = WRAP_LAYER_OUTERMOST};
    writeLog("game_save_site_installed", "ok=%d",
             insertWrap(SITE_GAME_SAVE, (void *)gameSaveOutermost, (void **)&nextGameSave, outermost, owner, NULL));
}
#endif
