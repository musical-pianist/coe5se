#pragma once

static int modIsPersonal(const char *modName) {
    const ModContentNote *note = findModContentNote(modName);
    return note && note->personal && !note->noMultiplayer && note->contentFree;
}

static int modIsNoMultiplayer(const char *modName) {
    const ModContentNote *note = findModContentNote(modName);
    return note && note->noMultiplayer;
}

static int endHostLoadOverNoMultiplayerDeclared(const char *missing, const char *what) {
    if (netRoleNow() != NET_ROLE_HOST) return 0;
    for (const char *open = strchr(missing, '('); open; open = strchr(open + 1, '(')) {
        const char *close = strchr(open, ')');
        if (!close) break;
        char mod[MOD_NAME_MAX];
        snprintf(mod, sizeof mod, "%.*s", (int)(close - open - 1), open + 1);
        if (!modIsNoMultiplayer(mod)) continue;
        writeLogWarning("save_mod_not_for_network", "mod=%s reason=nomultiplayer_%s", mod, what);
        endLoadOverNoMultiplayerMod(mod);
        return 1;
    }
    return 0;
}

// ---- A host's nomultiplayer mods -------------------------------------------------------------------
static int *noMultiplayerModsOff;
static int noMultiplayerModsOffCount, noMultiplayerModsOffCapacity;

static int turnOffNoMultiplayerMods(void) {
    noMultiplayerModsOffCount = 0;
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        if (!modEnabled(m) || !modIsNoMultiplayer(modName(m))) continue;
        if (!growList((void **)&noMultiplayerModsOff, &noMultiplayerModsOffCapacity, noMultiplayerModsOffCount + 1, sizeof noMultiplayerModsOff[0])) {
            writeLogError("nomultiplayer_mod_not_off", "mod=%s reason=out_of_memory", modName(m));
            continue;
        }
        noMultiplayerModsOff[noMultiplayerModsOffCount++] = m;
        setModEnabled(m, 0);
        writeLog("nomultiplayer_mod_off", "mod=%s", modName(m));
    }
    return noMultiplayerModsOffCount;
}

static void turnNoMultiplayerModsBackOn(void) {
    for (int i = 0; i < noMultiplayerModsOffCount; i++) setModEnabled(noMultiplayerModsOff[i], 1);
    writeLog("nomultiplayer_mods_back_on", "mods=%d", noMultiplayerModsOffCount);
    noMultiplayerModsOffCount = 0;
}

static void takeModKindLine(const char *word, const char *inC5mReason, const char *args, int mod, int level) {
    if (level != 2) return;
    const char *name = spellModName(mod);
    int line = G32(RVA_MOD_LINE);
    if (!readingSidecarOf(mod) && !readingPrefixedLine()) {
        refuseC5mLine(word, name, inC5mReason, "line=%d", line);
        return;
    }
    if (!modKindArgsAreEmpty(args)) refuseC5mLine(word, name, "text_after_word", "line=%d text=%.40s", line, args);
}

static void personalmodLine(const char *args, int mod, int level) {
    takeModKindLine(PERSONALMOD_WORD, "personalmod_in_c5m", args, mod, level);
}

static void nomultiplayerLine(const char *args, int mod, int level) {
    takeModKindLine(NOMULTIPLAYER_WORD, "nomultiplayer_in_c5m", args, mod, level);
}

static void modKindWordsInit(void) {
    int ok = apiAddModCommand("personalmod", personalmodLine);
    writeResultLog(ok, "personalmod_word_installed", "ok=%d", ok);
    ok = apiAddModCommand("nomultiplayer", nomultiplayerLine);
    writeResultLog(ok, "nomultiplayer_word_installed", "ok=%d", ok);
}
