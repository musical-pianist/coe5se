// @platform-split preview_world: Windows only for now; on Mac the coe5 calls don't exist, so the Plane Generator shows no button
#pragma once

#define PREVIEW_RESULT_ROWS 2000

typedef struct {
    char mod[MOD_NAME_MAX], file[64], text[160], stopText[160];
    char actions[480], turns[80];
    char held[200];
    int line, passes, ran, ranPass, stopped, stopLine;
    unsigned long long planes, named;
    unsigned long long changed;
} PreviewEventRow;

typedef struct { int row, line; char text[200]; } PreviewEventAction;

enum { PREVIEW_CHANGE_NEW, PREVIEW_CHANGE_MONSTER, PREVIEW_CHANGE_OWNER, PREVIEW_CHANGE_MOVED, PREVIEW_CHANGE_KILLED };
typedef struct { int unit, action, prev, x, y; short from, to, plane, kind; } PreviewUnitChange;

enum { PREVIEW_SCRIPT_CALL, PREVIEW_SCRIPT_REFUSED, PREVIEW_SCRIPT_ERROR, PREVIEW_SCRIPT_WRAP };
typedef struct {
    char mod[MOD_NAME_MAX], file[64], call[64], reason[32], text[200];
    int line, count, kind;
} PreviewScriptRow;

static int previewResultsOn, previewResultsTried, previewScriptKeep, previewResultsShort;
static int previewRefusalCount;
static PreviewEventRow *previewEventRows;
static int previewEventRowCount, previewEventRowCapacity, previewEventNow = -1;
static PreviewScriptRow *previewScriptRows;
static int previewScriptRowCount, previewScriptRowCapacity;
static PreviewEventAction *previewEventActions;
static int previewEventActionCount, previewEventActionCapacity;
static PreviewUnitChange *previewUnitChanges;
static int previewUnitChangeCount, previewUnitChangeCapacity;
static int previewEventsInSync;
static char previewEventHeld[sizeof ((PreviewEventRow *)0)->held];

static void previewNoteEventAction(int row, const char *line, int number);
static void previewEventLineBefore(int level);

#define PREVIEW_SCRIPT_PLACES 256
typedef struct { char mod[MOD_NAME_MAX], file[64], call[64]; int line; } PreviewScriptPlace;
static PreviewScriptPlace previewScriptPlaces[PREVIEW_SCRIPT_PLACES];
static int previewScriptPlaceCount, previewScriptPlaceNow = -1;

static UNUSED void previewScriptPlaceEnter(const char *mod, const char *file, int line, const char *call) {
    previewScriptPlaceNow = -1;
    for (int i = 0; i < previewScriptPlaceCount; i++) {
        const PreviewScriptPlace *p = &previewScriptPlaces[i];
        if (p->line == line && !strcmp(p->call, call) && !strcmp(p->file, file) && !strcmp(p->mod, mod)) {
            previewScriptPlaceNow = i;
            return;
        }
    }
    if (previewScriptPlaceCount >= PREVIEW_SCRIPT_PLACES) return;
    PreviewScriptPlace *p = &previewScriptPlaces[previewScriptPlaceCount];
    snprintf(p->mod, sizeof p->mod, "%s", mod);
    snprintf(p->file, sizeof p->file, "%s", file);
    snprintf(p->call, sizeof p->call, "%s", call);
    p->line = line;
    previewScriptPlaceNow = previewScriptPlaceCount++;
}

static UNUSED void previewResultsStart(void) {
    previewScriptPlaceCount = 0, previewScriptPlaceNow = -1;
    previewEventRowCount = previewScriptRowCount = previewScriptKeep = previewResultsTried = previewResultsShort = 0;
    previewEventActionCount = previewUnitChangeCount = previewEventsInSync = 0;
    previewEventNow = -1;
    previewResultsOn = 1;
}

static UNUSED void previewResultsStartTry(void) {
    if (!previewResultsTried) previewScriptKeep = previewScriptRowCount;
    previewResultsTried = 1;
    previewScriptRowCount = previewScriptKeep;
    previewEventRowCount = previewEventActionCount = previewUnitChangeCount = previewEventsInSync = 0;
    previewEventNow = -1;
}

static void previewResultsFull(const char *what) {
    if (!previewResultsShort) writeLogWarning("preview_results_short", "reason=too_many what=%s rows=%d", what, PREVIEW_RESULT_ROWS);
    previewResultsShort = 1;
}

static UNUSED void previewScriptNote(const char *mod, const char *file, int line, const char *call, const char *reason, int kind, const char *text) {
    if (!previewResultsOn) return;
    for (int i = previewScriptRowCount - 1; i >= 0; i--) {
        PreviewScriptRow *r = &previewScriptRows[i];
        if (r->kind == kind && r->line == line && !strcmp(r->call, call) && !strcmp(r->mod, mod) && !strcmp(r->file, file) && !strcmp(r->reason, reason) && !strcmp(r->text, text)) {
            r->count++;
            return;
        }
    }
    if (previewScriptRowCount >= PREVIEW_RESULT_ROWS || !growList((void **)&previewScriptRows, &previewScriptRowCapacity, previewScriptRowCount + 1, sizeof previewScriptRows[0])) {
        previewResultsFull("scripts");
        return;
    }
    PreviewScriptRow *r = &previewScriptRows[previewScriptRowCount++];
    memset(r, 0, sizeof *r);
    snprintf(r->mod, sizeof r->mod, "%s", mod);
    snprintf(r->file, sizeof r->file, "%s", file);
    snprintf(r->call, sizeof r->call, "%s", call);
    snprintf(r->reason, sizeof r->reason, "%s", reason);
    snprintf(r->text, sizeof r->text, "%s", text);
    r->line = line, r->kind = kind, r->count = 1;
}

static int previewLineWord(const char *p, const char *word, int prefix) {
    size_t n = strlen(word);
    if (strncmp(p, word, n)) return 0;
    return prefix || !p[n] || isspace((unsigned char)p[n]);
}

static int planeIndexNamed(const char *name);

static int previewEventLinePlane(const char *line) {
    const char *p = line;
    while (*p && !isspace((unsigned char)*p)) p++;
    while (isspace((unsigned char)*p)) p++;
    if (*p != '"') return isdigit((unsigned char)*p) ? atoi(p) : -1;
    const char *end = strchr(p + 1, '"');
    char name[64];
    if (!end || end - p - 1 >= (int)sizeof name) return -1;
    snprintf(name, sizeof name, "%.*s", (int)(end - p - 1), p + 1);
    return planeIndexNamed(name);
}

static void previewAddEventLine(char *list, size_t size, const char *line) {
    size_t used = strlen(list);
    if (used + strlen(line) + 2 > size) return;
    snprintf(list + used, size - used, "%s%s", used ? "\n" : "", line);
}

static int previewEventTurnLine(const char *line) { return previewLineWord(line + 1, "turnnbr", 0) || previewLineWord(line + 1, "minturnnbr", 0); }

static void previewNoteEventWords(PreviewEventRow *r, const char *line) {
    int names = previewLineWord(line, "+plane", 0) || previewLineWord(line, "randloc", 0);
    if (names) {
        int plane = previewEventLinePlane(line);
        if (plane >= 0 && plane < 64) r->named |= 1ULL << plane;
    }
    if (*line == '+' || *line == '-') {
        if (previewEventTurnLine(line)) previewAddEventLine(r->turns, sizeof r->turns, line);
        return;
    }
    if (!names) previewAddEventLine(r->actions, sizeof r->actions, line);
}

static UNUSED void previewNoteEventLine(const char *mod, const char *file, const char *line, int before) {
    if (!previewResultsOn) return;
    while (*line == ' ' || *line == '\t') line++;
    if (!*line || *line == '#') return;
    int after = G32(RVA_EVENT_STATE), number = G32(RVA_MOD_LINE);
    if (previewLineWord(line, "playerevent", 0) || previewLineWord(line, "squareevent", 0)) {
        previewEventNow = -1;
        for (int i = 0; i < previewEventRowCount; i++)
            if (previewEventRows[i].line == number && !strcmp(previewEventRows[i].mod, mod) && !strcmp(previewEventRows[i].file, file)) previewEventNow = i;
        if (previewEventNow < 0) {
            if (previewEventRowCount >= PREVIEW_RESULT_ROWS || !growList((void **)&previewEventRows, &previewEventRowCapacity, previewEventRowCount + 1, sizeof previewEventRows[0])) {
                previewResultsFull("events");
                return;
            }
            PreviewEventRow *added = &previewEventRows[previewEventRowCount];
            memset(added, 0, sizeof *added);
            snprintf(added->mod, sizeof added->mod, "%s", mod);
            snprintf(added->file, sizeof added->file, "%s", file);
            snprintf(added->text, sizeof added->text, "%.159s", line);
            added->line = number;
            previewEventNow = previewEventRowCount++;
        }
        previewEventRows[previewEventNow].passes++;
        previewEventHeld[0] = 0;
        return;
    }
    if (previewEventNow < 0) return;
    PreviewEventRow *r = &previewEventRows[previewEventNow];
    if (previewLineWord(line, "endevent", 0)) {
        previewEventNow = -1;
        return;
    }
    if (r->passes == 1) previewNoteEventWords(r, line);
    if (*line == '+' || *line == '-') {
        if (before >= 1 && after >= 1) previewAddEventLine(previewEventHeld, sizeof previewEventHeld, line);
        if (before >= 1 && after == -1) {
            r->stopped++;
            if (number > r->stopLine) {
                r->stopLine = number;
                snprintf(r->stopText, sizeof r->stopText, "%.159s", line);
                if (!r->ran) snprintf(r->held, sizeof r->held, "%s", previewEventHeld);
            }
        }
        return;
    }
    if (before < 1) return;
    if (!previewLineWord(line, "randloc", 1) && !previewLineWord(line, "setloc", 1)) previewNoteEventAction(previewEventNow, line, number);
    if (after < 1) return;
    int x = G32(RVA_EVENT_TARGET_X), y = G32(RVA_EVENT_TARGET_Y);
    if (x >= 0 && y >= 0 && x < G32(RVA_MAP_W) && y < G32(RVA_MAP_H)) {
        int plane = GAME_FN(int (*)(int, int), RVA_PLANE_AT)(x, y);
        if (plane >= 0 && plane < 64) r->planes |= 1ULL << plane;
    }
    if (previewLineWord(line, "randloc", 1) || previewLineWord(line, "setloc", 1)) return;
    if (r->ranPass != r->passes) {
        if (!r->ran) snprintf(r->held, sizeof r->held, "%s", previewEventHeld);
        r->ran++, r->ranPass = r->passes;
    }
}
