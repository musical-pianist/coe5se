#pragma once

#ifdef _WIN32
#define OFFICIAL_SEP "\\"
#else
#define OFFICIAL_SEP "/"
#endif
#define OFFICIAL_SESSION_FILE      "official_session.txt"
#define OFFICIAL_CONFIG_BACKUP_FILE "coe5config_before_official"
#define OFFICIAL_SESSION_MODS       500
#define OFFICIAL_MOD_NAME           256

typedef struct {
    int count;
    char names[OFFICIAL_SESSION_MODS][OFFICIAL_MOD_NAME];
} OfficialModList;

static __attribute__((unused)) int officialListHas(const OfficialModList *list, const char *name) {
    for (int i = 0; i < list->count; i++)
        if (!strcmp(list->names[i], name)) return 1;
    return 0;
}

static __attribute__((unused)) int officialListAdd(OfficialModList *list, const char *name) {
    if (officialListHas(list, name)) return 1;
    if (list->count >= OFFICIAL_SESSION_MODS || strlen(name) >= OFFICIAL_MOD_NAME) return 0;
    snprintf(list->names[list->count++], OFFICIAL_MOD_NAME, "%s", name);
    return 1;
}

static __attribute__((unused)) int officialConfigListBounds(const unsigned char *data, size_t len, size_t *start, size_t *end) {
    if (len < CONFIG_MODS_OFFSET) return 0;
    unsigned version = readLittleEndian32(data);
    if (version < CONFIG_VERSION_MIN || version > CONFIG_VERSION_MAX) return 0;
    size_t pos = CONFIG_MODS_OFFSET;
    for (;;) {
        size_t nameStart = pos;
        while (pos < len && data[pos] != CONFIG_KEY) pos++;
        if (pos >= len) return 0;
        pos++;
        if (pos - 1 == nameStart) break;
    }
    *start = CONFIG_MODS_OFFSET;
    *end = pos;
    return 1;
}

static __attribute__((unused)) int officialConfigList(const unsigned char *data, size_t len, OfficialModList *out) {
    size_t start, end;
    out->count = 0;
    if (!officialConfigListBounds(data, len, &start, &end)) return 0;
    for (size_t pos = start; pos + 1 < end;) {
        char name[OFFICIAL_MOD_NAME];
        size_t n = 0;
        while (data[pos] != CONFIG_KEY) {
            if (n + 1 < sizeof name) name[n++] = (char)(data[pos] ^ CONFIG_KEY);
            pos++;
        }
        name[n] = '\0';
        pos++;
        if (out->count < OFFICIAL_SESSION_MODS) snprintf(out->names[out->count++], OFFICIAL_MOD_NAME, "%s", name);
    }
    return 1;
}

static __attribute__((unused)) const char *officialSetConfigList(unsigned char *data, size_t *len, size_t cap, const OfficialModList *list,
                                                                 const OfficialModList *drop) {
    size_t start, end, size = 1;
    if (!officialConfigListBounds(data, *len, &start, &end)) return "config_unreadable";
    for (int i = 0; i < list->count; i++)
        if (!drop || !officialListHas(drop, list->names[i])) size += strlen(list->names[i]) + 1;
    if (*len - (end - start) + size > cap) return "config_too_big";
    memmove(data + start + size, data + end, *len - end);
    *len = *len - (end - start) + size;
    size_t pos = start;
    for (int i = 0; i < list->count; i++) {
        if (drop && officialListHas(drop, list->names[i])) continue;
        for (const char *c = list->names[i]; *c; c++) data[pos++] = (unsigned char)*c ^ CONFIG_KEY;
        data[pos++] = CONFIG_KEY;
    }
    data[pos] = CONFIG_KEY;
    return NULL;
}

static __attribute__((unused)) const char *officialReadFile(const char *path, unsigned char *data, size_t cap, size_t *len) {
    FILE *f = fopen(path, "rb");
    if (!f) return "config_missing";
    *len = fread(data, 1, cap, f);
    int failed = ferror(f);
    fclose(f);
    if (failed) return "config_unreadable";
    if (*len == cap) return "config_too_big";
    return NULL;
}

static __attribute__((unused)) const char *officialReplaceFile(const char *path, const unsigned char *data, size_t len) {
    char fresh[MAX_PATH];
    if (!buildPath(fresh, sizeof fresh, __func__, "%s.new", path)) return "config_write_failed";
    if (!writeWholeFile(fresh, data, len)) {
        remove(fresh);
        return "config_write_failed";
    }
#ifdef _WIN32
    int replaced = MoveFileExA(fresh, path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    int replaced = rename(fresh, path) == 0;
#endif
    if (!replaced) {
        remove(fresh);
        return "config_write_failed";
    }
    return NULL;
}

static __attribute__((unused)) int officialWriteSession(const char *path, const char *state, const OfficialModList *enabled,
                                                        const OfficialModList *drop) {
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    int ok = fprintf(f, "# CoE5SE: the mods enabled before an official game, put back when it ends.\nversion 1\nstate %s\n", state) > 0;
    for (int i = 0; ok && i < enabled->count; i++) ok = fprintf(f, "enabled %s\n", enabled->names[i]) > 0;
    for (int i = 0; ok && i < drop->count; i++) ok = fprintf(f, "off %s\n", drop->names[i]) > 0;
    return fclose(f) == 0 && ok;
}

static __attribute__((unused)) int officialReadSession(const char *path, char *state, size_t stateSize, OfficialModList *enabled,
                                                       OfficialModList *drop) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[OFFICIAL_MOD_NAME + 16];
    int version = 0;
    state[0] = '\0';
    enabled->count = drop->count = 0;
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (!strncmp(line, "version ", 8)) version = atoi(line + 8);
        else if (!strncmp(line, "state ", 6)) snprintf(state, stateSize, "%.*s", (int)(stateSize - 1), line + 6);
        else if (!strncmp(line, "enabled ", 8)) officialListAdd(enabled, line + 8);
        else if (!strncmp(line, "off ", 4)) officialListAdd(drop, line + 4);
    }
    fclose(f);
    return version == 1 && state[0];
}

static __attribute__((unused)) int officialC5mNeedsCoe5se(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[512];
    int needs = 0;
    while (!needs && fgets(line, sizeof line, f)) {
        const char *c = line;
        while (*c == ' ' || *c == '\t') c++;
        needs = !strncmp(c, "needs_script_extender", 21) || !strncmp(c, "needs_coe5se", 12) || !strncmp(c, "needs_glu32.dll", 15);
    }
    fclose(f);
    return needs;
}

static __attribute__((unused)) int officialFolderNeedsCoe5se(const char *dir, const char *mod) {
    char path[MAX_PATH];
    return buildPath(path, sizeof path, __func__, "%s" OFFICIAL_SEP "%s.c5m", dir, mod) && officialC5mNeedsCoe5se(path);
}

static __attribute__((unused)) const char *officialSessionApply(const char *config, const char *backup, const char *session, const char *done,
                                                                const OfficialModList *enabled, const OfficialModList *drop) {
    static unsigned char data[CONFIG_MAX];
    size_t len;
    const char *reason = officialReadFile(config, data, sizeof data, &len);
    if (reason) return reason;
    if (!writeWholeFile(backup, data, len)) return "backup_write_failed";
    if (!officialWriteSession(session, "active", enabled, drop)) return "session_write_failed";
    if ((reason = officialSetConfigList(data, &len, sizeof data, enabled, drop))) return reason;
    if ((reason = officialReplaceFile(config, data, len))) return reason;
    remove(done);
    return NULL;
}

static __attribute__((unused)) const char *officialSessionRestore(const char *config, const char *session, const char *done,
                                                                  const char *doneText, const OfficialModList *enabled) {
    static unsigned char data[CONFIG_MAX];
    size_t len;
    const char *reason = officialReadFile(config, data, sizeof data, &len);
    if (reason) return reason;
    if ((reason = officialSetConfigList(data, &len, sizeof data, enabled, NULL))) return reason;
    if ((reason = officialReplaceFile(config, data, len))) return reason;
    FILE *f = fopen(done, "w");
    if (f) {
        fprintf(f, "%s\n", doneText);
        fclose(f);
    }
    remove(session);
    return NULL;
}
