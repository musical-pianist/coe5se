static void finishLoadedModList(void) {
    keepPlayerModStates();
    GAME_FN(void (*)(void), RVA_RESET_MODS)();
}

#ifndef __APPLE__
static int installModVersionHooks(int *wanted) {
    *wanted = 1;
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION);
    if (!stubs) {
        writeLog("mod_version_hooks_skipped", "reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    if (!redirectCall(RVA_LOAD_GAME_RESET_MODS_CALL, RVA_RESET_MODS, stubs, (void *)finishLoadedModList, "load_game_reset_mods", NULL))
        return 0;
    modListEndHooked = 1;
    return 1;
}
#else
static int installModVersionHooks(int *wanted) {
    *wanted = 1;
    if (!pointMissingModsCall(RVA_LOAD_GAME_RESET_MODS_CALL, RVA_RESET_MODS, (void *)finishLoadedModList, "load_game_reset_mods"))
        return 0;
    modListEndHooked = 1;
    return 1;
}
#endif

static void modVersionsInit(void) {
    int wanted = 0, ok = installModVersionHooks(&wanted);
    writeInstallLog("mod_version_hooks_installed", ok, wanted);
    networkChecksInit();
    personalModListsInit();
}
