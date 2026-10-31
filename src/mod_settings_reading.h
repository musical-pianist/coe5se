#pragma once

// ---- Reading a mod's settings files ---------------------------------------------------------------
static int settingsFileTried(const char *mod) {
    for (int i = 0; i < triedModCount; i++)
        if (!_stricmp(triedMods[i], mod)) return 1;
    return 0;
}

static char *readSettingsFileText(const char *mod, const char *path, int in) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *text = size >= 0 ? malloc((size_t)size + 1) : NULL;
    if (!text) {
        fclose(f);
        if (in) refuseSidecarLine(in, "-", mod, "out_of_memory", "size=%ld", size);
        else refuseIniLine("-", mod, "out_of_memory", "file=%s.ini size=%ld", mod, size);
        return NULL;
    }
    size_t got = fread(text, 1, (size_t)size, f);
    fclose(f);
    text[got] = '\0';
    return text;
}

// ---- The files a reading included -----------------------------------------------------------------
typedef struct { char mod[MOD_NAME_MAX], path[MAX_PATH]; } SettingsInclude;
static SettingsInclude *settingsIncludes;
static int settingsIncludeCount, settingsIncludeCapacity;

static void forgetSettingsIncludes(const char *mod) {
    int kept = 0;
    for (int i = 0; i < settingsIncludeCount; i++)
        if (_stricmp(settingsIncludes[i].mod, mod)) settingsIncludes[kept++] = settingsIncludes[i];
    settingsIncludeCount = kept;
}

static int settingsIncludeRead(const char *mod, const char *path) {
    for (int i = 0; i < settingsIncludeCount; i++)
        if (!_stricmp(settingsIncludes[i].mod, mod) && !strcmp(settingsIncludes[i].path, path)) return 1;
    return 0;
}

static void noteSettingsInclude(const char *mod, const char *path) {
    if (!growList((void **)&settingsIncludes, &settingsIncludeCapacity, settingsIncludeCount + 1, sizeof *settingsIncludes))
        return;
    SettingsInclude *e = &settingsIncludes[settingsIncludeCount++];
    snprintf(e->mod, sizeof e->mod, "%s", mod);
    snprintf(e->path, sizeof e->path, "%s", path);
}

static u64 settingsIncludeStamp(const char *mod) {
    u64 hash = 14695981039346656037ull;
    for (int i = 0; i < settingsIncludeCount; i++) {
        if (_stricmp(settingsIncludes[i].mod, mod)) continue;
        FileStamp stamp;
        fileStampOf(settingsIncludes[i].path, &stamp);
        for (size_t k = 0; k < sizeof stamp; k++) hash = (hash ^ ((const unsigned char *)&stamp)[k]) * 1099511628211ull;
    }
    return hash;
}

static void logIgnoredIni(const char *mod, ...) {
    va_list ap;
    va_start(ap, mod);
    logRefusal("settings_ini_ignored", "-", "mod", mod, "c5se_has_settings", "file=%s.ini", ap);
    va_end(ap);
}

static void readSettingsIn(const char *mod, const char *dir) {
    if (!settingsFileTried(mod) && growList((void **)&triedMods, &triedModCapacity, triedModCount + 1, sizeof triedMods[0]))
        snprintf(triedMods[triedModCount++], MOD_NAME_MAX, "%s", mod);
    noteSettingsIniNow();
    IniParse r;
    startSettingsReading(&r, mod);
    r.dir = dir;
    forgetSettingsIncludes(mod);
    char path[MAX_PATH], *text;
    static const struct { const char *suffix; int in; } files[] = {{MOD_FILE_SUFFIX, SETTINGS_IN_C5M}, {SIDECAR_SUFFIX, SETTINGS_IN_C5SE}};
    for (size_t i = 0; i < sizeof files / sizeof files[0]; i++) {
        if (buildPath(path, sizeof path, __func__, "%s" PATH_SEP "%s%s", dir, mod, files[i].suffix) &&
            (text = readSettingsFileText(mod, path, files[i].in)) != NULL) {
            readSidecarSettingsText(&r, text, files[i].in);
            free(text);
        }
    }
    if (!buildPath(path, sizeof path, __func__, "%s" PATH_SEP "%s.ini", dir, mod)) {
    } else if (r.sidecarBlocks) {
        if (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES) logIgnoredIni(mod, mod);
    } else if ((text = readSettingsFileText(mod, path, 0)) != NULL) {
        readIniText(&r, text);
        free(text);
    }
    resolveKeyConflicts(mod);
}

static void readSettingsFile(const char *mod) {
    int m = modIndexByName(mod);
    if (m < 0) return;
    readSettingsIn(modName(m), modFolder(m));
}

// ---- A reading whose files haven't changed -------------------------------------------------------
typedef struct {
    char mod[MOD_NAME_MAX];
    char dir[MAX_PATH];
    FileStamp c5m, sidecar, ini;
    u64 includes;
    unsigned int picks;
    int pageLines;
    int fileBadLines;
    SettingRef *sidecarRefs;
    int sidecarRefCount, sidecarRefCapacity;
    int valid;
} SettingsRead;

static SettingsRead *settingsReads;
static int settingsReadCount, settingsReadCapacity;

static SettingsRead *settingsReadOf(const char *mod) {
    for (int i = 0; i < settingsReadCount; i++)
        if (!_stricmp(settingsReads[i].mod, mod)) return &settingsReads[i];
    return NULL;
}

static void forgetSettingsRead(const char *mod) {
    SettingsRead *e = settingsReadOf(mod);
    if (e) e->valid = 0;
}

static int settingsReadNow(const char *mod, SettingsRead *now) {
    int m = modIndexByName(mod);
    if (m < 0) return 0;
    char path[MAX_PATH];
    memset(now, 0, sizeof *now);
    snprintf(now->mod, sizeof now->mod, "%s", modName(m));
    if (strlen(modFolder(m)) >= sizeof now->dir) return 0;
    snprintf(now->dir, sizeof now->dir, "%s", modFolder(m));
    if (!buildPath(path, sizeof path, __func__, "%s" PATH_SEP "%s" MOD_FILE_SUFFIX, now->dir, now->mod)) return 0;
    fileStampOf(path, &now->c5m);
    if (!buildPath(path, sizeof path, __func__, "%s" PATH_SEP "%s" SIDECAR_SUFFIX, now->dir, now->mod)) return 0;
    fileStampOf(path, &now->sidecar);
    if (!buildPath(path, sizeof path, __func__, "%s" PATH_SEP "%s.ini", now->dir, now->mod)) return 0;
    fileStampOf(path, &now->ini);
    now->includes = settingsIncludeStamp(now->mod);
    noteSettingsIniNow();
    now->picks = savedPicks.generation;
    now->pageLines = pluginPageLines(now->mod);
    return 1;
}

static int keepSettingsRead(const SettingsRead *now) {
    SettingsRead *e = settingsReadOf(now->mod);
    if (!e || !e->valid || strcmp(e->dir, now->dir) || !sameFileStamp(&e->c5m, &now->c5m) || !sameFileStamp(&e->sidecar, &now->sidecar) ||
        !sameFileStamp(&e->ini, &now->ini) || e->includes != now->includes || e->picks != now->picks || e->pageLines != now->pageLines)
        return 0;
    BadLine *mine = NULL;
    if (e->fileBadLines && !(mine = malloc((size_t)e->fileBadLines * sizeof *mine))) return 0;
    if (e->sidecarRefCount && !growList((void **)&reading.refs, &reading.refCapacity, e->sidecarRefCount, sizeof reading.refs[0])) {
        free(mine);
        return 0;
    }
    memcpy(reading.refs, e->sidecarRefs, (size_t)e->sidecarRefCount * sizeof reading.refs[0]);
    reading.refCount = e->sidecarRefCount;
    int kept = 0, n = 0, had = 0;
    for (int i = 0; i < badLineCount; i++) {
        if (_stricmp(badLines[i].mod, now->mod)) badLines[kept++] = badLines[i];
        else if (had++ < e->fileBadLines) mine[n++] = badLines[i];
    }
    if (n) memcpy(&badLines[kept], mine, (size_t)n * sizeof *mine);
    badLineCount = kept + n;
    free(mine);
    if (had) leftOutVersion++;
    return 1;
}

static void rememberSettingsRead(const SettingsRead *now) {
    SettingsRead *e = settingsReadOf(now->mod);
    if (!e) {
        if (!growList((void **)&settingsReads, &settingsReadCapacity, settingsReadCount + 1, sizeof settingsReads[0])) return;
        e = &settingsReads[settingsReadCount++];
        memset(e, 0, sizeof *e);
    }
    SettingRef *refs = e->sidecarRefs;
    int refCapacity = e->sidecarRefCapacity;
    *e = *now;
    e->sidecarRefs = refs;
    e->sidecarRefCapacity = refCapacity;
    for (int i = 0; i < reading.refCount; i++)
        if (!reading.refs[i].inSidecar) return;
    if (reading.refCount && !growList((void **)&e->sidecarRefs, &e->sidecarRefCapacity, reading.refCount, sizeof e->sidecarRefs[0])) return;
    if (reading.refCount) memcpy(e->sidecarRefs, reading.refs, (size_t)reading.refCount * sizeof e->sidecarRefs[0]);
    e->sidecarRefCount = reading.refCount;
    for (int i = 0; i < badLineCount; i++) e->fileBadLines += !_stricmp(badLines[i].mod, now->mod);
    e->valid = 1;
}

// ---- The game's reading of the settings blocks ---------------------------------------------------
static struct {
    int open;
    int mod, level, line;
    unsigned int fileId;
} gameSettingBlock;

static int settingLineHook(const char *line, size_t len, int mod, int level) {
    int number = G32(RVA_MOD_LINE), inSidecar = readingSidecarNow();
    unsigned int fileId = modFileNow();
    if (mod != gameSettingBlock.mod || level != gameSettingBlock.level || fileId != gameSettingBlock.fileId ||
        number <= gameSettingBlock.line)
        gameSettingBlock.open = 0;
    gameSettingBlock.mod = mod;
    gameSettingBlock.level = level;
    gameSettingBlock.fileId = fileId;
    gameSettingBlock.line = number;
    char word[24];
    snprintf(word, sizeof word, "%.*s", (int)(len < sizeof word ? len : sizeof word - 1), line);
    if (isSettingBlockStart(word)) {
        gameSettingBlock.open = 1;
        if (!inSidecar && level == 1 && reading.active && !reading.disabled && mod >= 0 && mod <= G32(RVA_MOD_LAST_INDEX)) {
            SettingProblem p;
            settingProblem(&p, "setting_in_c5m", "settings go in %.32s%s or on %s lines", modName(mod), SIDECAR_SUFFIX, SIDECAR_LINE_PREFIX);
            rejectC5mLine(modName(mod), word, number, &p, line);
        }
        return 1;
    }
    if (gameSettingBlock.open && isSettingBlockWord(word)) return 1;
    gameSettingBlock.open = 0;
    return 0;
}

static void resetModSettings(const char *mod) {
    forgetSettingsRead(mod);
    for (int i = 0; i < settingCount; i++)
        if (!_stricmp(settings[i]->mod, mod)) settings[i]->declared = settings[i]->value = 0;
    int kept = 0;
    for (int i = 0; i < badLineCount; i++)
        if (_stricmp(badLines[i].mod, mod)) badLines[kept++] = badLines[i];
    if (kept != badLineCount) leftOutVersion++;
    badLineCount = kept;
    kept = 0;
    for (int i = 0; i < modPageCount; i++)
        if (_stricmp(modPages[i].mod, mod)) modPages[kept++] = modPages[i];
    modPageCount = kept;
}

// ---- A mod's reading -----------------------------------------------------------------------------
static void startReading(const char *mod) {
    SettingRef *refs = reading.refs;
    int refCapacity = reading.refCapacity;
    memset(&reading, 0, sizeof reading);
    reading.refs = refs;
    reading.refCapacity = refCapacity;
    reading.active = 1;
    snprintf(reading.mod, sizeof reading.mod, "%s", mod);
    passReadings++;
    reading.disabled = !modEnabledByName(mod);
    SettingsRead now;
    int known = !reading.disabled && settingsReadNow(mod, &now);
    if (known && keepSettingsRead(&now)) return;
    resetModSettings(mod);
    if (reading.disabled) return;
    readSettingsFile(mod);
    if (known) {
        now.includes = settingsIncludeStamp(now.mod);
        rememberSettingsRead(&now);
    }
}

static void finishReading(void) {
    if (!reading.active) return;
    reading.active = 0;
    if (reading.disabled) return;
    SettingProblem p;
    char source[160];
    for (int i = 0; i < reading.refCount; i++) {
        const SettingRef *r = &reading.refs[i];
        const ModSetting *s = findDeclaredSetting(reading.mod, r->name);
        int max = s && s->optionCount ? s->optionCount - 1 : 1;
        snprintf(source, sizeof source, "%s %s%s", r->word, r->name, r->hasIndex ? " <index>" : "");
        if (!s) settingProblem(&p, "unknown_setting", "%s %.32s: no such setting in this mod", r->word, r->name);
        else if (s->rejected) continue;
        else if (r->trailing) settingProblem(&p, "trailing_text", "%s %.32s: text after the index", r->word, r->name);
        else if (s->optionCount && !r->hasIndex)
            settingProblem(&p, "no_index", "%s %.32s: a choice needs an index (0 to %d)", r->word, r->name, max);
        else if (r->hasIndex && s->isKey && !keyValueIsValid(r->index))
            settingProblem(&p, "index_out_of_range", "%s %.32s: %d is not a key value", r->word, r->name, r->index);
        else if (r->hasIndex && !s->isKey && (r->index < 0 || r->index > max))
            settingProblem(&p, "index_out_of_range", "%s %.32s: index %d is not 0 to %d", r->word, r->name, r->index, max);
        else continue;
        if (r->inSidecar) rejectSidecarLine(r->inSidecar, reading.mod, r->word, r->line, &p, source);
        else rejectC5mLine(reading.mod, r->word, r->line, &p, source);
    }
}

static void noteModLine(const char *mod, int level) {
    int number = G32(RVA_MOD_LINE);
    if (level != 1 || number < 1 || !mod[0]) return;
    if (!reading.active || _stricmp(reading.mod, mod) || number < reading.line) {
        finishReading();
        startReading(mod);
    } else if (number == reading.line) {
        return;
    }
    reading.line = number;
}

// ---- Readings outside the mod passes -------------------------------------------------------------
static void readEnabledSettingsFiles(int all) {
    finishReading();
    int read = 0;
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        if (!modEnabled(m) || !modName(m)[0] || (!all && settingsFileTried(modName(m)))) continue;
        resetModSettings(modName(m));
        readSettingsFile(modName(m));
        read++;
    }
    if (read) writeLog("settings_files_read", "mods=%d all=%d", read, all);
}

static void readUnreadSettingsFiles(void) {
    readEnabledSettingsFiles(0);
}

static unsigned int settingsPassCount(void) { return passReadings; }
static void rereadSettingsWithoutPass(unsigned int before) {
    if (passReadings == before) readEnabledSettingsFiles(1);
}

static int noSettingValue;
static const int *pluginSetting(void *caller, const char *mod, const char *dir, const char *name) {
    SettingProblem p;
    if (checkSettingName("name", name, &p)) {
        noSettingValue = 0;
        return (refuseApiCall(caller, "setting", p.reason, "name=%s mod=%s", name ? name : "", mod), &noSettingValue);
    }
    finishReading();
    if (!settingsFileTried(mod)) {
        resetModSettings(mod);
        readSettingsIn(mod, dir);
    }
    int created;
    ModSetting *s = settingEntry(mod, name, &created);
    if (!s) {
        noSettingValue = 0;
        return (refuseApiCall(caller, "setting", "out_of_memory", "name=%s mod=%s", name, mod), &noSettingValue);
    }
    if (!s->declared) refuseApiCall(caller, "setting", "not_declared", "name=%s mod=%s", name, mod);
    return &s->value;
}
