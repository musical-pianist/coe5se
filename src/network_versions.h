#include "coe5se_version.h"

#define SAVE_VERS_VERSION         1
#define NETWORK_VERSIONS_TITLE    "CoE5SE: Incompatible versions"
#define NETWORK_VERSION_SHOWN_MAX 40
_Static_assert(sizeof COE5SE_VERSION >= 4, "a VERS payload is 16 bytes or more, the least readTrailedChunk reads");

// ---- Writing ------------------------------------------------------------------------------------
static void addCoe5seVersionChunk(SaveList *list) {
    SaveBuffer b = {0};
    saveBufferInt(&b, SAVE_VERS_VERSION);
    saveBufferString(&b, COE5SE_VERSION);
    saveBufferInt(&b, (int)b.size + 8);
    saveBufferInt(&b, SAVE_TAG_VERS);
    saveListAdd(list, SAVE_TAG_VERS, &b);
}

// ---- Reading ------------------------------------------------------------------------------------
static const char *readCoe5seVersionPayload(const unsigned char *payload, int size, char *version, size_t versionSize) {
    version[0] = '\0';
    if (size < 13) return "bad";
    int trailerSize = (int)((unsigned)payload[size - 8] | (unsigned)payload[size - 7] << 8 | (unsigned)payload[size - 6] << 16 |
                            (unsigned)payload[size - 5] << 24);
    int trailerTag = (int)((unsigned)payload[size - 4] | (unsigned)payload[size - 3] << 8 | (unsigned)payload[size - 2] << 16 |
                           (unsigned)payload[size - 1] << 24);
    if (trailerSize != size || trailerTag != SAVE_TAG_VERS) return "bad";
    SaveCursor c = {payload, payload + size - 8, 0};
    int format = saveCursorInt(&c);
    saveCursorString(&c, version, versionSize);
    if (c.failed || format < 1 || (format == SAVE_VERS_VERSION && c.at != c.end)) {
        version[0] = '\0';
        return "bad";
    }
    return "found";
}

static const char *readCoe5seVersionChunk(const SaveChunk *chunk, char *version, size_t size) {
    return readCoe5seVersionPayload(chunk->payload, chunk->size, version, size);
}

static const char *readSavedCoe5seVersion(const char *path, char *version, size_t versionSize) {
    version[0] = '\0';
    FILE *f = fopen(path, "rb");
    if (!f) return "bad";
    int ints[3] = {0}, size = 0;
    long end = !fseek(f, 0, SEEK_END) ? ftell(f) : -1;
    unsigned char *payload = NULL;
    const char *result = "none";
    if (end >= 28 && !fseek(f, end - 12, SEEK_SET)) {
        for (int i = 0; i < 3; i++) readFileInt(f, &ints[i]);
        if (ints[0] == SAVE_TAG_END && !ints[1] && ints[2] == SAVE_LIST_MAGIC) {
            long at = end - 12;
            for (int step = 0; step < 5 && !payload && !strcmp(result, "none"); step++) {
                int trailer[2] = {0};
                if (at < 16 || fseek(f, at - 8, SEEK_SET) || !readFileInt(f, &trailer[0]) || !readFileInt(f, &trailer[1])) break;
                if (trailer[1] == SAVE_TAG_VERS) payload = readTrailedChunk(f, at, SAVE_TAG_VERS, &size, &result);
                else if (trailer[1] == SAVE_TAG_MODC || trailer[1] == SAVE_TAG_MODH || trailer[1] == SAVE_TAG_PLNS ||
                         trailer[1] == SAVE_TAG_FCTN)
                    at -= (long)trailer[0] + 8;
                else break;
            }
        }
    }
    fclose(f);
    if (payload) result = readCoe5seVersionPayload(payload, size, version, versionSize);
    free(payload);
    return result;
}

// ---- The check ----------------------------------------------------------------------------------
static void shownCoe5seVersion(const char *result, const char *version, char *out, size_t size) {
    if (strcmp(result, "found") || !version[0]) {
        snprintf(out, size, "unknown");
        return;
    }
    size_t n = 0;
    for (; version[n] && n < NETWORK_VERSION_SHOWN_MAX && n + 1 < size; n++)
        out[n] = version[n] > ' ' && version[n] < 0x7f ? version[n] : '?';
    out[n] = '\0';
}

static size_t coe5seVersionsText(char *text, size_t size, SaveFile kind, int client, const char *fileVersion) {
    size_t n = 0;
    const char *fileSide = kind == SAVE_FILE_TURN ? "Client" : "Server";
    text[0] = '\0';
    appendText(text, size, &n, "CoE5SE versions differ:\n\n%s: %s\n%s: %s\n\nSame version required for all players.", fileSide,
               fileVersion, client ? "Client" : "Server", COE5SE_VERSION);
    return n;
}

static void showVersionsAndEndGame(const char *text) {
    okBox(NETWORK_VERSIONS_TITLE, text);
    GAME_FN(void (*)(const char *), RVA_EXIT_GAME)("");
}
static void (*endGameOnVersions)(const char *text) = showVersionsAndEndGame;

static void checkCoe5seVersion(SaveFile kind, const char *where, const char *result, const char *version) {
    int network = isNetworkGame();
    const char *compared = strcmp(result, "found") ? (strcmp(result, "none") ? "bad_record" : "no_record")
                           : strcmp(version, COE5SE_VERSION)  ? "differs"
                                                              : "same";
    char shown[NETWORK_VERSION_SHOWN_MAX + 1];
    shownCoe5seVersion(result, version, shown, sizeof shown);
    int hostOwnLoad = kind == SAVE_FILE_SAVE && G8(RVA_NET_SERVER) && !isNetworkClient();
    if (!network || !strcmp(compared, "same") || hostOwnLoad) {
        writeLog("coe5se_version_checked", "file=%s at=%s result=%s saved=%s running=%s network=%d", SAVE_FILE_NAMES[kind], where, compared,
                 shown, COE5SE_VERSION, network);
        return;
    }
    writeLogWarning("coe5se_version_checked", "reason=%s file=%s at=%s result=%s saved=%s running=%s network=1", compared,
                    SAVE_FILE_NAMES[kind], where, compared, shown, COE5SE_VERSION);
    static char text[POPUP_TEXT_SIZE];
    coe5seVersionsText(text, sizeof text, kind, isNetworkClient(), shown);
    writeLog("coe5se_versions_differ_exit", "file=%s result=%s", SAVE_FILE_NAMES[kind], compared);
    endGameOnVersions(text);
}

static void checkLoadedSaveVersion(const char *path) {
    char version[SAVE_STRING_MAX + 1];
    const char *result = readSavedCoe5seVersion(path, version, sizeof version);
    coe5seVersionCheckedAtOpen = 1;
    checkCoe5seVersion(SAVE_FILE_SAVE, "open", result, version);
}
