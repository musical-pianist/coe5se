#pragma once

// ---- Restart prompt after the Mods screen -----------------------------------------------------
static struct CodeMod { char folder[MAX_PATH]; int enabled; } *codeMods;
static int codeModCount, codeModCapacity, codeModSnapshotFailed;
static u64 (*nextModsScreen)(u64, u64, u64, u64);

#define MOD_FILE_SUFFIX ".c5m"
#define SIDECAR_SUFFIX  ".c5se"

#define SIDECAR_LINE_PREFIX "#!@"

static const char *sidecarLineText(const char *line) {
    while (*line == ' ' || *line == '\t' || *line == '\r' || *line == '\n') line++;
    size_t n = sizeof SIDECAR_LINE_PREFIX - 1;
    return strncmp(line, SIDECAR_LINE_PREFIX, n) ? NULL : line + n;
}

#define INCLUDE_WORD     "include"
#define INCLUDE_FILE_MAX 198

static int readIncludeLine(const char *line, char *file, size_t size) {
    while (*line == ' ' || *line == '\t' || *line == '\r' || *line == '\n') line++;
    size_t n = sizeof INCLUDE_WORD - 1;
    if (strncmp(line, INCLUDE_WORD, n)) return 0;
    const char *p = line + n;
    if (*p && *p != ' ' && *p != '\t' && *p != '"' && *p != '\r' && *p != '\n' && *p != '#') return 0;
    while (*p == ' ' || *p == '\t') p++;
    if (*p != '"') return -1;
    const char *end = strchr(++p, '"');
    if (!end || (size_t)(end - p) >= size) return -1;
    snprintf(file, size, "%.*s", (int)(end - p), p);
    for (p = end + 1; *p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'; p++) {}
    return *p && *p != '#' ? -1 : 1;
}

static char **includeLabels;
static int includeLabelCount, includeLabelCapacity;

static int includeLabelIndex(const char *file) {
    for (int i = 0; i < includeLabelCount; i++)
        if (!strcmp(includeLabels[i] + 1, file)) return i;
    char *label = malloc(strlen(file) + 2);
    if (!label || !growList((void **)&includeLabels, &includeLabelCapacity, includeLabelCount + 1, sizeof *includeLabels)) {
        free(label);
        return -1;
    }
    snprintf(label, strlen(file) + 2, "/%s", file);
    includeLabels[includeLabelCount] = label;
    return includeLabelCount++;
}

static const char *includeLabelText(int index) {
    return index >= 0 && index < includeLabelCount ? includeLabels[index] : SIDECAR_SUFFIX;
}

static const char *modFolder(int mod) {
    return (const char *)(gameBase + RVA_MOD_FOLDERS + (size_t)mod * MOD_RECORD_SIZE);
}

static const char *modName(int mod);

static int modEnabled(int mod) {
    return *(short *)(gameBase + RVA_MOD_NAMES + (size_t)mod * MOD_RECORD_SIZE + MOD_RECORD_ENABLED) != 0;
}

static void setModEnabled(int mod, int on) {
    *(short *)(gameBase + RVA_MOD_NAMES + (size_t)mod * MOD_RECORD_SIZE + MOD_RECORD_ENABLED) = (short)(on != 0);
}

static int folderHasPlugin(const char *dir) {
    char pattern[MAX_PATH];
    if (!buildPath(pattern, sizeof pattern, __func__, "%s" PATH_SEP "*" PLUGIN_SUFFIX, dir)) return 0;
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return 0;
    FindClose(h);
    return 1;
}

static int modHasFile(int mod, const char *suffix) {
    char path[MAX_PATH];
    if (!buildPath(path, sizeof path, __func__, "%s" PATH_SEP "%s%s", modFolder(mod), modName(mod), suffix)) return 0;
    FILE *f = fopen(path, "rb");
    if (f) fclose(f);
    return f != NULL;
}

#define LUA_COMMANDS_SUFFIX "_commands.lua"

static int modHasCode(int mod) {
    return folderHasPlugin(modFolder(mod));
}

static void takeCodeModSnapshot(void) {
    codeModCount = 0;
    codeModSnapshotFailed = 0;
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        if (!modHasCode(m)) continue;
        if (!growList((void **)&codeMods, &codeModCapacity, codeModCount + 1, sizeof codeMods[0])) {
            codeModSnapshotFailed = 1;
            writeLog("mods_with_code_snapshot_failed", "reason=out_of_memory count=%d", codeModCount);
            return;
        }
        snprintf(codeMods[codeModCount].folder, MAX_PATH, "%s", modFolder(m));
        codeMods[codeModCount++].enabled = modEnabled(m);
    }
}

static int codeModsChanged(void) {
    if (codeModSnapshotFailed) return 0;
    int seen = 0;
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        if (!modHasCode(m)) continue;
        int k = 0;
        while (k < codeModCount && _stricmp(codeMods[k].folder, modFolder(m))) k++;
        if (k == codeModCount) {
            if (modEnabled(m)) return 1;
            continue;
        }
        if (codeMods[k].enabled != modEnabled(m)) return 1;
        seen++;
    }
    for (int k = 0; k < codeModCount; k++)
        if (codeMods[k].enabled) seen--;
    return seen < 0;
}

static int dllModOnSinceSnapshot(void) {
    if (codeModSnapshotFailed) return -1;
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        if (!modEnabled(m) || !folderHasPlugin(modFolder(m))) continue;
        int k = 0;
        while (k < codeModCount && _stricmp(codeMods[k].folder, modFolder(m))) k++;
        if (k == codeModCount || !codeMods[k].enabled) return m;
    }
    return -1;
}

#ifdef __APPLE__
static void relaunchGame(const char *reason) {
    char exe[MAX_PATH];
    GetModuleFileNameA(NULL, exe, sizeof exe);
    pid_t pid;
    int error = posix_spawn(&pid, exe, NULL, NULL, *_NSGetArgv(), *_NSGetEnviron());
    if (error) {
        writeLog("restart_failed", "error=%d", error);
        return;
    }
    writeLog("restarting", "reason=%s", reason);
    exit(0);
}
#else
static void relaunchGame(const char *reason) {
    char exe[MAX_PATH], dir[MAX_PATH], cmd[2048];
    GetModuleFileNameA(NULL, exe, sizeof exe);
    snprintf(dir, sizeof dir, "%s", exe);
    char *slash = strrchr(dir, '\\');
    if (slash) *slash = '\0';
    snprintf(cmd, sizeof cmd, "%s", GetCommandLineA());
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof si);
    si.cb = sizeof si;
    PROCESS_INFORMATION pi;
    if (!CreateProcessA(exe, cmd, NULL, NULL, FALSE, 0, NULL, dir, &si, &pi)) {
        writeLog("restart_failed", "error=%lu", GetLastError());
        return;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    writeLog("restarting", "reason=%s", reason);
    ExitProcess(0);
}
#endif

static u64 modsScreenWrapper(u64 a1, u64 a2, u64 a3, u64 a4) {
    unsigned int readingsBefore = settingsPassCount(), spellPassesBefore = spellPassCount();
    u64 result = nextModsScreen(a1, a2, a3, a4);
    rereadSettingsWithoutPass(readingsBefore);
    spellsAfterModsScreen(spellPassesBefore);
    if (!codeModsChanged()) return result;
    writeLog("mods_with_code_changed", "count=%d", codeModCount);
    if (messageBox("CoE5SE: Restart needed", "Mod with DLL turned on or off.\nRestart required. Restart now?",
                   "Yes", "No", 0, 0, 0) == 1)
        relaunchGame("mods_with_code_changed");
    takeCodeModSnapshot();
    return result;
}

static void folderAndFile(const char *path, char *out, size_t outSize) {
    char copy[MAX_PATH];
    out[0] = '\0';
    if (!buildPath(copy, sizeof copy, __func__, "%s", path)) return;
    for (char *c = copy; *c; c++) if (*c == '/') *c = '\\';
    char *file = strrchr(copy, '\\');
    if (!file) {
        buildPath(out, outSize, __func__, "%s", copy);
        return;
    }
    *file = '\0';
    char *folder = strrchr(copy, '\\');
    buildPath(out, outSize, __func__, "%s\\%s", folder ? folder + 1 : copy, file + 1);
}

typedef struct {
    char name[MAX_PATH], source[16], path[MAX_PATH], hash[SHA256_HEX_LEN + 1];
    PluginState state;
} FoundLine;
#define FOUND_LINE_MAX (3 * MAX_PATH)

static int readFoundLine(char *line, FoundLine *out) {
    line[strcspn(line, "\r\n")] = '\0';
    char *field[5] = {line};
    int n = 1;
    for (char *c = line; *c && n < 5; c++)
        if (*c == '|') {
            *c = '\0';
            field[n++] = c + 1;
        }
    if (n < 4) return 0;
    snprintf(out->name, sizeof out->name, "%s", field[0]);
    snprintf(out->source, sizeof out->source, "%s", field[1]);
    out->state = pluginStateNamed(field[2]);
    snprintf(out->path, sizeof out->path, "%s", field[3]);
    snprintf(out->hash, sizeof out->hash, "%s", n > 4 ? field[4] : "");
    return 1;
}

static int foundAtLaunch(const char *path) {
    typedef char FoundPath[MAX_PATH];
    static FoundPath *seen;
    static int seenCount = -1, seenCapacity;
    if (seenCount < 0) {
        seenCount = 0;
        static char line[FOUND_LINE_MAX];
        char file[MAX_PATH];
        FoundLine found;
        FILE *f = coe5seUserFile(PLUGINS_FOUND_FILE, file, sizeof file) ? fopen(file, "r") : NULL;
        while (f && fgets(line, sizeof line, f)) {
            if (!readFoundLine(line, &found)) continue;
            if (!growList((void **)&seen, &seenCapacity, seenCount + 1, sizeof seen[0])) break;
            folderAndFile(found.path, seen[seenCount], MAX_PATH);
            if (seen[seenCount][0]) seenCount++;
        }
        if (f) fclose(f);
    }
    char key[MAX_PATH];
    folderAndFile(path, key, sizeof key);
    for (int i = 0; key[0] && i < seenCount; i++)
        if (!_stricmp(seen[i], key)) return 1;
    return 0;
}

static int modHasUnloadedPlugin(int mod) {
    if (mod < 0 || mod > G32(RVA_MOD_LAST_INDEX)) return 0;
    const char *dir = (const char *)(gameBase + RVA_MOD_FOLDERS + (size_t)mod * MOD_RECORD_SIZE);
    char pattern[MAX_PATH], path[MAX_PATH];
    if (!buildPath(pattern, sizeof pattern, __func__, "%s" PATH_SEP "*" PLUGIN_SUFFIX, dir)) return 0;
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return 0;
    int unloaded = 0;
    do {
        if (!buildPath(path, sizeof path, __func__, "%s" PATH_SEP "%s", dir, fd.cFileName)) unloaded = 1;
        else unloaded = GetModuleHandleA(path) == NULL && !foundAtLaunch(path);
    } while (!unloaded && FindNextFileA(h, &fd));
    FindClose(h);
    return unloaded;
}

static int anyEnabledModWaitsForItsPlugin(void) {
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++)
        if (modEnabled(m) && modHasUnloadedPlugin(m)) return 1;
    return 0;
}
