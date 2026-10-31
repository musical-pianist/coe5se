#define SAVE_MODC_VERSION      1
#define SAVE_MODH_VERSION      2
#define MOD_ADDED_NO_CONTENT   1
#define MOD_LINE_MAX           50000

// ---- What each mod's files add ------------------------------------------------------------------
static const char *const CONTENT_FREE_WORDS[] = {
    "icon", "description", "coeversion", "modversion", "modprio", "requires",
    "needs_script_extender", "needs_coe5se", "needs_glu32.dll_and_CoE5SE_enabled_at_launch", "maxdata",
};

static int isContentFreeWord(const char *word) {
    for (size_t i = 0; i < sizeof CONTENT_FREE_WORDS / sizeof CONTENT_FREE_WORDS[0]; i++)
        if (!strcmp(word, CONTENT_FREE_WORDS[i])) return 1;
    return 0;
}

static int firstModWord(const char *line, char *word, size_t size) {
    int crLed = 0;
    for (; *line && strchr(" \t\r\n", *line); line++) crLed |= *line == '\r' || *line == '\n';
    size_t n = 0;
    for (; *line && !strchr(" \t\r\n\"#", *line); line++)
        if (n + 1 < size) n++, word[n - 1] = *line;
    word[n] = '\0';
    return crLed && n;
}

static const char *const UNHASHED_WORDS[] = {"icon", "description", "needs_script_extender", "needs_coe5se", "needs_glu32.dll_and_CoE5SE_enabled_at_launch"};
#define CONTENT_HASH_START  0xcbf29ce484222325ULL
#define CONTENT_HASH_PRIME  0x100000001b3ULL

static int isUnhashedWord(const char *word) {
    for (size_t i = 0; i < sizeof UNHASHED_WORDS / sizeof UNHASHED_WORDS[0]; i++)
        if (!strcmp(word, UNHASHED_WORDS[i])) return 1;
    return 0;
}

static int hashContentLine(const char *line, u64 *hash) {
    u64 h = *hash;
    int quoted = 0, space = 0, kept = 0;
    for (; *line; line++) {
        unsigned char ch = (unsigned char)*line;
        if (ch == '\r' || ch == '\n') ch = ' ';
        if (!quoted && ch == '#') break;
        if (!quoted && (ch == ' ' || ch == '\t')) {
            space = kept > 0;
            continue;
        }
        if (space) h = (h ^ ' ') * CONTENT_HASH_PRIME, space = 0;
        if (ch == '"') quoted = !quoted;
        h = (h ^ ch) * CONTENT_HASH_PRIME;
        kept++;
    }
    if (!kept) return 0;
    *hash = (h ^ '\n') * CONTENT_HASH_PRIME;
    return 1;
}

typedef struct { int settingsOpen, contentFree, longLine; u64 hash; } ModFileScan;

static void scanModLine(const char *line, ModFileScan *scan) {
    char word[64];
    int crLed = firstModWord(line, word, sizeof word);
    if (!word[0]) {
        hashContentLine(line, &scan->hash);
        return;
    }
    if (crLed) {
        hashContentLine(line, &scan->hash);
        scan->settingsOpen = 0;
        if (!isContentFreeWord(word)) scan->contentFree = 0;
        return;
    }
    if (isSettingBlockStart(word)) {
        scan->settingsOpen = 1;
        return;
    }
    if (scan->settingsOpen && isSettingBlockWord(word)) return;
    scan->settingsOpen = 0;
    if (!isContentFreeWord(word)) scan->contentFree = 0;
    if (!isUnhashedWord(word)) hashContentLine(line, &scan->hash);
}

#define SCAN_SIDECAR  0
#define SCAN_C5M      1
#define SCAN_INCLUDED 2
static int scanModFile(int mod, const char *path, ModFileScan *scan, int kind) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    static char line[MOD_LINE_MAX + 2];
    long at = ftell(f);
    while (fgets(line, sizeof line, f)) {
        long next = ftell(f);
        size_t n = next > at ? (size_t)(next - at) : strlen(line);
        at = next;
        int ends = n && line[n - 1] == '\n';
        if (scan->longLine || (n == sizeof line - 1 && !ends)) {
            scan->contentFree = 0;
            hashContentLine(line, &scan->hash);
            scan->longLine = !ends;
        } else {
            const char *own = kind == SCAN_C5M ? sidecarLineText(line) : NULL, *text = own ? own : line;
            char file[INCLUDE_FILE_MAX + 2], included[MAX_PATH];
            if ((own || kind == SCAN_SIDECAR) && readIncludeLine(text, file, sizeof file) > 0) {
                hashContentLine(text, &scan->hash);
                scan->settingsOpen = 0;
                if (!findIncludedFile(mod, file, included, sizeof included)) scanModFile(mod, included, scan, SCAN_INCLUDED);
                continue;
            }
            scanModLine(text, scan);
        }
    }
    fclose(f);
    scan->longLine = 0;
    return 1;
}

static int scanMod(int mod, ModFileScan *scan) {
    *scan = (ModFileScan){.contentFree = 1, .hash = CONTENT_HASH_START};
    char path[MAX_PATH];
    if (!buildPath(path, sizeof path, __func__, "%s" PATH_SEP "%s" MOD_FILE_SUFFIX, modFolder(mod), modName(mod)) || !scanModFile(mod, path, scan, SCAN_C5M)) {
        scan->contentFree = 0;
        return 0;
    }
    if (!buildPath(path, sizeof path, __func__, "%s" PATH_SEP "%s" SIDECAR_SUFFIX, modFolder(mod), modName(mod))) {
        scan->contentFree = 0;
        return 0;
    }
    scanModFile(mod, path, scan, SCAN_SIDECAR);
    return 1;
}

typedef struct { char name[MOD_NAME_MAX]; int contentFree, hashed; u64 hash; } ModContentNote;
static ModContentNote *modContentNotes;
static int modContentNoteCount, modContentNoteCapacity;

static ModContentNote *findModContentNote(const char *name) {
    for (int i = 0; i < modContentNoteCount; i++)
        if (!strcmp(modContentNotes[i].name, name)) return &modContentNotes[i];
    return NULL;
}

static void noteModContent(int mod) {
    if (mod < 0 || mod > G32(RVA_MOD_LAST_INDEX)) return;
    ModContentNote *note = findModContentNote(modName(mod));
    if (!note) {
        if (!growList((void **)&modContentNotes, &modContentNoteCapacity, modContentNoteCount + 1, sizeof *note)) {
            writeLog("mod_content_note_out_of_memory", "mod=%s", modName(mod));
            return;
        }
        note = &modContentNotes[modContentNoteCount++];
        snprintf(note->name, sizeof note->name, "%s", modName(mod));
    }
    ModFileScan scan;
    note->hashed = scanMod(mod, &scan);
    note->contentFree = scan.contentFree;
    note->hash = scan.hash;
}

// ---- Writing ------------------------------------------------------------------------------------
static void addModHashChunk(SaveList *list) {
    SaveBuffer b = {0};
    int count = 0;
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        const ModContentNote *note = modEnabled(m) ? findModContentNote(modName(m)) : NULL;
        count += note && note->hashed;
    }
    saveBufferInt(&b, SAVE_MODH_VERSION);
    saveBufferInt(&b, count);
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        const ModContentNote *note = modEnabled(m) ? findModContentNote(modName(m)) : NULL;
        if (!note || !note->hashed) continue;
        saveBufferString(&b, modName(m));
        saveBufferInt(&b, (int)(note->hash & 0xffffffffu));
        saveBufferInt(&b, (int)(note->hash >> 32));
    }
    saveBufferInt(&b, (int)b.size + 8);
    saveBufferInt(&b, SAVE_TAG_MODH);
    saveListAdd(list, SAVE_TAG_MODH, &b);
}

static void addModContentChunk(SaveList *list) {
    addModHashChunk(list);
    SaveBuffer b = {0};
    int count = 0;
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) count += modEnabled(m);
    saveBufferInt(&b, SAVE_MODC_VERSION);
    saveBufferInt(&b, count);
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        if (!modEnabled(m)) continue;
        const ModContentNote *note = findModContentNote(modName(m));
        saveBufferString(&b, modName(m));
        saveBufferInt(&b, note && note->contentFree ? MOD_ADDED_NO_CONTENT : 0);
    }
    saveBufferInt(&b, (int)b.size + 8);
    saveBufferInt(&b, SAVE_TAG_MODC);
    saveListAdd(list, SAVE_TAG_MODC, &b);
}

// ---- Reading ------------------------------------------------------------------------------------
typedef struct { char name[SAVE_STRING_MAX + 1]; int flags; } SavedModEntry;
typedef struct {
    const char *result;
    SavedModEntry *mods;
    int count;
} SavedMods;
static SavedMods savedMods = {"none", NULL, 0};

static void freeSavedMods(SavedMods *saved) {
    free(saved->mods);
    *saved = (SavedMods){"none", NULL, 0};
}

static const char *readModContentPayload(const unsigned char *payload, int size, SavedMods *saved) {
    if (size < 16) return "bad";
    SaveCursor c = {payload, payload + size - 8, 0};
    int version = saveCursorInt(&c), count = saveCursorInt(&c);
    if (c.failed) return "bad";
    if (version != SAVE_MODC_VERSION) return version > SAVE_MODC_VERSION ? "newer_version" : "bad";
    if (count < 0 || count > SAVE_MODS_MAX) return "bad";
    saved->mods = calloc((size_t)count + 1, sizeof *saved->mods);
    if (!saved->mods) return "bad";
    for (int i = 0; i < count; i++) {
        saveCursorString(&c, saved->mods[i].name, sizeof saved->mods[i].name);
        saved->mods[i].flags = saveCursorInt(&c);
        if (c.failed) return "bad";
    }
    if (c.at != c.end) return "bad";
    saved->count = count;
    return "found";
}

static int readFileInt(FILE *f, int *value) {
    unsigned char le[4];
    *value = 0;
    if (fread(le, 1, 4, f) != 4) return 0;
    *value = (int)((unsigned)le[0] | (unsigned)le[1] << 8 | (unsigned)le[2] << 16 | (unsigned)le[3] << 24);
    return 1;
}

typedef struct { char name[SAVE_STRING_MAX + 1]; u64 hash; } SavedModHash;
typedef struct {
    const char *result;
    SavedModHash *mods;
    int count;
} SavedModHashes;
static SavedModHashes savedModHashes = {"none", NULL, 0};

static void freeSavedModHashes(SavedModHashes *saved) {
    free(saved->mods);
    *saved = (SavedModHashes){"none", NULL, 0};
}

static const SavedModHash *findSavedModHash(const char *name) {
    for (int i = 0; i < savedModHashes.count; i++)
        if (!strcmp(savedModHashes.mods[i].name, name)) return &savedModHashes.mods[i];
    return NULL;
}

static const char *readModHashPayload(const unsigned char *payload, int size, SavedModHashes *saved) {
    if (size < 16) return "bad";
    SaveCursor c = {payload, payload + size - 8, 0};
    int version = saveCursorInt(&c), count = saveCursorInt(&c);
    if (c.failed) return "bad";
    if (version != SAVE_MODH_VERSION) return version > SAVE_MODH_VERSION ? "newer_version" : "bad";
    if (count < 0 || count > SAVE_MODS_MAX) return "bad";
    saved->mods = calloc((size_t)count + 1, sizeof *saved->mods);
    if (!saved->mods) return "bad";
    for (int i = 0; i < count; i++) {
        saveCursorString(&c, saved->mods[i].name, sizeof saved->mods[i].name);
        unsigned low = (unsigned)saveCursorInt(&c), high = (unsigned)saveCursorInt(&c);
        saved->mods[i].hash = (u64)high << 32 | low;
        if (c.failed) return "bad";
    }
    if (c.at != c.end) return "bad";
    saved->count = count;
    return "found";
}

static unsigned char *readTrailedChunk(FILE *f, long end, int tag, int *size, const char **result) {
    int trailerSize, trailerTag, headTag, headSize;
    *result = "none";
    if (end < 16 || fseek(f, end - 8, SEEK_SET) || !readFileInt(f, &trailerSize) || !readFileInt(f, &trailerTag) || trailerTag != tag)
        return NULL;
    *result = "bad";
    if (trailerSize < 16 || trailerSize > SAVE_CHUNK_MAX || (long)trailerSize + 8 > end) return NULL;
    if (fseek(f, end - trailerSize - 8, SEEK_SET) || !readFileInt(f, &headTag) || !readFileInt(f, &headSize) || headTag != tag ||
        headSize != trailerSize)
        return NULL;
    unsigned char *payload = malloc((size_t)headSize);
    if (!payload) return NULL;
    if (fread(payload, 1, (size_t)headSize, f) != (size_t)headSize) {
        free(payload);
        return NULL;
    }
    *size = headSize;
    *result = "found";
    return payload;
}

static const char *readSavedModContent(const char *path, SavedMods *saved, SavedModHashes *hashes) {
    freeSavedMods(saved);
    freeSavedModHashes(hashes);
    FILE *f = fopen(path, "rb");
    if (!f) return hashes->result = "bad", saved->result = "bad";
    int ints[3] = {0}, ok = !fseek(f, 0, SEEK_END), payloadSize = 0;
    long size = ok ? ftell(f) : -1;
    const char *result = "none", *hashResult = "none";
    if (size >= 28 && !fseek(f, size - 12, SEEK_SET)) {
        for (int i = 0; i < 3; i++) readFileInt(f, &ints[i]);
        if (ints[0] == SAVE_TAG_END && !ints[1] && ints[2] == SAVE_LIST_MAGIC) {
            unsigned char *payload = readTrailedChunk(f, size - 12, SAVE_TAG_MODC, &payloadSize, &result);
            if (payload) result = readModContentPayload(payload, payloadSize, saved);
            free(payload);
            if (payload && !strcmp(result, "found")) {
                payload = readTrailedChunk(f, size - 12 - payloadSize - 8, SAVE_TAG_MODH, &payloadSize, &hashResult);
                if (payload) hashResult = readModHashPayload(payload, payloadSize, hashes);
                free(payload);
            }
        }
    }
    fclose(f);
    if (strcmp(result, "found")) freeSavedMods(saved);
    if (strcmp(hashResult, "found")) freeSavedModHashes(hashes);
    hashes->result = hashResult;
    return saved->result = result;
}

// ---- Saves whose mods are all content-free ------------------------------------------------------
static int loadGameOpenHooked, modListEndHooked, saveModListHooked;

static struct {
    unsigned char *enabled;
    int count, capacity, noted;
} playerModStates;

static void notePlayerModStates(void) {
    int count = G32(RVA_MOD_LAST_INDEX) + 1;
    playerModStates.noted = 0;
    if (count > 0 && !growList((void **)&playerModStates.enabled, &playerModStates.capacity, count, 1)) {
        writeLog("player_mod_states_not_kept", "mods=%d reason=out_of_memory", count);
        return;
    }
    for (int m = 0; m < count; m++) playerModStates.enabled[m] = (unsigned char)modEnabled(m);
    playerModStates.count = count > 0 ? count : 0;
    playerModStates.noted = 1;
}

static int keepPlayerModStates(void) {
    if (!playerModStates.noted) {
        writeLog("player_mod_states_not_kept", "reason=not_noted");
        return -1;
    }
    playerModStates.noted = 0;
    int kept = 0;
    for (int m = 0; m < playerModStates.count && m <= G32(RVA_MOD_LAST_INDEX); m++) {
        int player = playerModStates.enabled[m], saved = modEnabled(m);
        if (player == saved) continue;
        ModFileScan scan;
        int contentFree = scanMod(m, &scan) && scan.contentFree;
        writeLog("load_mod_state", "mod=%s player=%d save=%d used=%s", modName(m), player, saved, contentFree ? "player" : "save");
        if (!contentFree) continue;
        setModEnabled(m, player);
        kept++;
    }
    return kept;
}

static int modListLeftOut;

static int chooseModListLeftOut(void) {
    const char *reason = "all_content_free", *mod = "-";
    int enabled = 0;
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        if (!modEnabled(m)) continue;
        enabled++;
        const ModContentNote *note = findModContentNote(modName(m));
        if (!strcmp(reason, "all_content_free") && (!note || !note->contentFree)) {
            reason = note ? "content" : "not_read";
            mod = modName(m);
        }
    }
    if (!saveModListHooked || !loadGameOpenHooked || !modListEndHooked) reason = "hooks_missing", mod = "-";
    else if (isNetworkGame()) reason = "network_game", mod = "-";
    int leftOut = !strcmp(reason, "all_content_free");
    writeLog("save_mod_list", "list=%s mods=%d reason=%s mod=%s", leftOut ? "empty" : "full", enabled, reason, mod);
    return leftOut;
}

typedef void (*WriteModNameFn)(const char *name, void *file);
static void writeSavedModName(const char *name, void *file, WriteModNameFn writeName) {
    ptrdiff_t offset = name - (const char *)(gameBase + RVA_MOD_NAMES);
    if (offset < 0 || offset % MOD_RECORD_SIZE || offset / MOD_RECORD_SIZE > G32(RVA_MOD_LAST_INDEX)) {
        writeLog("save_mod_list", "list=full reason=not_a_mod_record offset=%lld", (long long)offset);
        modListLeftOut = 0;
    } else {
        int mod = (int)(offset / MOD_RECORD_SIZE), first = 0;
        while (first < mod && !modEnabled(first)) first++;
        if (first == mod) modListLeftOut = chooseModListLeftOut();
    }
    if (!modListLeftOut) writeName(name, file);
}

static UNUSED size_t writeSavedModChecksum(const void *checksum, size_t size, size_t count, FILE *file) {
    return modListLeftOut ? count : fwrite(checksum, size, count, file);
}

// ---- The load -----------------------------------------------------------------------------------
static int modIndexExact(const char *name) {
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++)
        if (!strcmp(modName(m), name)) return m;
    return -1;
}

static int isSkippableMissingMod(const char *name) {
    if (!name[0] || modIndexExact(name) >= 0) return 0;
    const SavedModEntry *entry = NULL;
    for (int i = 0; i < savedMods.count && !entry; i++)
        if (!strcmp(savedMods.mods[i].name, name)) entry = &savedMods.mods[i];
    if (entry && entry->flags & MOD_ADDED_NO_CONTENT) {
        writeLog("save_mod_skipped", "mod=%s reason=added_no_content", name);
        return 1;
    }
    writeLog("save_mod_missing", "mod=%s reason=%s record=%s", name,
             entry ? "added_content" : strcmp(savedMods.result, "found") ? "no_record" : "not_in_record", savedMods.result);
    return 0;
}

static void readLoadedSavePlanes(const char *path);
static void readLoadedSaveFactions(const char *path);

static void *openLoadedSave(const char *path, void *file) {
    playerModStates.noted = 0;
    if (file) {
        notePlayerModStates();
        readSavedModContent(path, &savedMods, &savedModHashes);
        writeLog("save_mod_record_read", "result=%s mods=%d", savedMods.result, savedMods.count);
        writeLog("save_mod_hashes_read", "result=%s mods=%d", savedModHashes.result, savedModHashes.count);
        readLoadedSavePlanes(path);
        readLoadedSaveFactions(path);
    }
    return file;
}

#ifdef __APPLE__
#define SAVED_MOD_NAME_AFTER_CRC LOAD_GAME_MOD_NAME_AFTER_CRC
#else
#define SAVED_MOD_NAME_AFTER_CRC LOAD_MOD_LIST_NAME_AFTER_CRC
#endif
typedef size_t (*SavedModFreadFn)(void *data, size_t size, size_t count, void *file);
static size_t readSavedModChecksum(void *checksum, size_t size, size_t count, void *file, SavedModFreadFn readBytes) {
    size_t result = readBytes(checksum, size, count, file);
    char *name = (char *)checksum + SAVED_MOD_NAME_AFTER_CRC;
    if (!isSkippableMissingMod(name)) return result;
    if (modIndexExact(COE5SE_MOD) < 0) {
        writeLog("save_mod_not_skipped", "mod=%s reason=no_stand_in", name);
        return result;
    }
    snprintf(name, MOD_NAME_MAX, "%s", COE5SE_MOD);
    memset(checksum, 0, 4);
    return result;
}

#ifndef __APPLE__
static void *loadGameOpenHook(const char *path, const char *mode) {
    return openLoadedSave(path, GAME_FN(void *(*)(const char *, const char *), RVA_FOPEN)(path, mode));
}

static size_t readSavedModChecksumHook(void *checksum, size_t size, size_t count, void *file) {
    return readSavedModChecksum(checksum, size, count, file, GAME_FN(SavedModFreadFn, RVA_FREAD));
}

#define MISSING_MODS_STUB_SIZE 0x20
static int installMissingModsHooks(int *wanted) {
    *wanted = 2;
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION);
    if (!stubs) {
        writeLog("missing_mods_hooks_skipped", "reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    loadGameOpenHooked = redirectCall(RVA_LOAD_GAME_FOPEN_CALL, RVA_FOPEN, stubs, (void *)loadGameOpenHook, "load_game_fopen", NULL);
    return loadGameOpenHooked + redirectCall(RVA_LOAD_GAME_MOD_CRC_READ, RVA_FREAD, stubs + MISSING_MODS_STUB_SIZE,
                                             (void *)readSavedModChecksumHook, "load_game_mod_checksum", NULL);
}

static void writeSavedModNameHook(const char *name, void *file) {
    writeSavedModName(name, file, GAME_FN(WriteModNameFn, RVA_WRITE_STRING));
}

static size_t writeSavedModChecksumHook(const void *checksum, size_t size, size_t count, void *file) {
    return modListLeftOut ? count : GAME_FN(size_t (*)(const void *, size_t, size_t, void *), RVA_FWRITE)(checksum, size, count, file);
}

static int hookSavedModListWrites(unsigned int checksumRva, unsigned int nameRva, unsigned char *stubs, const char *checksumName,
                                  const char *nameName) {
    if (!redirectCall(checksumRva, RVA_FWRITE, stubs, (void *)writeSavedModChecksumHook, checksumName, NULL))
        return 0;
    if (!redirectCall(nameRva, RVA_WRITE_STRING, stubs + MISSING_MODS_STUB_SIZE, (void *)writeSavedModNameHook, nameName, NULL))
        return 1;
    return 2;
}

#define SAVE_MOD_LIST_HOOKS 4
static int installSaveModListHooks(void) {
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION);
    if (!stubs) {
        writeLog("save_mod_list_hooks_skipped", "reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    int ok = hookSavedModListWrites(RVA_SAVE_GAME_MOD_CRC_WRITE, RVA_SAVE_GAME_MOD_NAME_WRITE, stubs, "save_game_mod_checksum",
                                    "save_game_mod_name");
    saveModListHooked = ok == 2;
    return ok + hookSavedModListWrites(RVA_SAVE_MOD_LIST_CRC_WRITE, RVA_SAVE_MOD_LIST_NAME_WRITE, stubs + 2 * MISSING_MODS_STUB_SIZE,
                                       "server_save_mod_checksum", "server_save_mod_name");
}
#else
static void *loadGameOpenHook(const char *path, const char *mode) { return openLoadedSave(path, fopen(path, mode)); }

static size_t freadSavedMod(void *data, size_t size, size_t count, void *file) { return fread(data, size, count, (FILE *)file); }
static size_t readSavedModChecksumHook(void *checksum, size_t size, size_t count, FILE *file) {
    return readSavedModChecksum(checksum, size, count, file, freadSavedMod);
}

static int pointMissingModsCall(unsigned int callRva, unsigned int targetRva, void *hook, const char *name) {
    if (blTarget(gameBase + callRva) != gameBase + targetRva) {
        writeLog("hook_skipped", "site=%s reason=not_the_expected_call va=0x%llx", name, callRva + 0x100000000ULL);
        return 0;
    }
    unsigned char *trampoline = newTrampoline(hook);
    uint32_t word = trampoline ? blTo(gameBase + callRva, trampoline) : 0;
    if (trampoline && writeGameCode(gameBase + callRva, &word, 4)) return 1;
    writeLog("hook_skipped", "site=%s reason=%s", name, trampoline ? "code_unwritable" : "no_padding");
    return 0;
}

static int installMissingModsHooks(int *wanted) {
    *wanted = 2;
    loadGameOpenHooked = pointMissingModsCall(RVA_LOAD_GAME_FOPEN_CALL, RVA_FOPEN, (void *)loadGameOpenHook, "load_game_fopen");
    return loadGameOpenHooked +
           pointMissingModsCall(RVA_LOAD_GAME_MOD_CRC_READ, RVA_FREAD, (void *)readSavedModChecksumHook, "load_game_mod_checksum");
}

static void writeSavedModNameHook(const char *name, void *file) {
    writeSavedModName(name, file, GAME_FN(WriteModNameFn, RVA_WRITE_STRING));
}

#define SAVE_MOD_LIST_HOOKS 2
static int installSaveModListHooks(void) {
    if (!pointMissingModsCall(RVA_SAVE_GAME_MOD_CRC_WRITE, RVA_FWRITE, (void *)writeSavedModChecksum, "save_game_mod_checksum"))
        return 0;
    if (!pointMissingModsCall(RVA_SAVE_GAME_MOD_NAME_WRITE, RVA_WRITE_STRING, (void *)writeSavedModNameHook, "save_game_mod_name"))
        return 1;
    saveModListHooked = 1;
    return 2;
}
#endif

static void missingModsInit(void) {
    int wanted = 0, ok = installMissingModsHooks(&wanted);
    writeLog("missing_mods_hooks_installed", "ok=%d of %d", ok, wanted);
    writeLog("save_mod_list_hooks_installed", "ok=%d of %d", installSaveModListHooks(), SAVE_MOD_LIST_HOOKS);
}
