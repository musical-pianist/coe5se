// @platform-split preview_world: Windows only for now; on Mac the coe5 calls don't exist, so the Plane Generator shows no button
#pragma once

#define PREVIEW_LINE_TEXT_SIZE 300
#define PREVIEW_LINE_BLAMES    4
#define PREVIEW_INCLUDE_DEPTH  4
#define PREVIEW_ARMY_SOURCES   32

#define PREVIEW_PIECE_COPIES 32

typedef struct {
    char plane[PLANE_NAME_SIZE], mod[MOD_NAME_MAX], file[64], word[20], zone[PLANE_NAME_SIZE], reason[32];
    char text[PREVIEW_LINE_TEXT_SIZE];
    int line;
    char fileText[PREVIEW_LINE_TEXT_SIZE];
    int off;
    int edited;
    int refused;
    int failed;
    int overridden;
    int by;
    int from, to;
    int did, of;
    int units;
    int w, h;
    int ran;
    int wrote;
    int lost, lostTo;
    int takerCount;
    struct { int line, squares; } takers[6];
    int blameCount, blame[PREVIEW_LINE_BLAMES];
    int order, orderOf;
    int copyCount;
    struct { int x, y; char why[16]; } copies[PREVIEW_PIECE_COPIES];
} PreviewLine;

typedef struct { int square, source; short terrain; } PreviewTerrainWrite;

static PreviewLine *previewLineRows;
static int previewLineRowCount, previewLineRowCapacity;
static PreviewTerrainWrite *previewTerrainWrites;
static int previewTerrainWriteCount, previewTerrainWriteCapacity;
static short *previewTerrainSeen;
static int *previewUnitSources;
static int previewSourceNow = PREVIEW_SOURCE_WORLD;
typedef struct { int x, y; short monster, owner, alive; } PreviewUnitSeen;
static PreviewUnitSeen *previewUnitsSeen;
static int *previewUnitLastChange;

static struct {
    int on, row, blockLine, zoneLine, populateOffLine, zoneUnitsLine;
    char zone[PLANE_NAME_SIZE];
} previewReading;

static int previewLinesReading(void) { return previewReading.on; }

// ---- Edits ------------------------------------------------------------------------------------------
#define PREVIEW_ADDED_LINE 100000
typedef struct {
    char mod[MOD_NAME_MAX], plane[PLANE_NAME_SIZE];
    char text[PREVIEW_LINE_TEXT_SIZE];
    char changed[PREVIEW_LINE_TEXT_SIZE];
    int occurrence, seen;
    int off, added;
} PreviewLineEdit;

static PreviewLineEdit *previewLineEdits;
static int previewLineEditCount, previewLineEditCapacity, previewLineEditState;
static char previewEditPlane[PLANE_NAME_SIZE];
static int previewEditMod = -1;
static int previewPassEnding;

#define PREVIEW_SKIPPED_WORDS 64
typedef struct { char mod[MOD_NAME_MAX], word[32], file[64]; int line; } PreviewSkippedWord;
static PreviewSkippedWord previewSkippedWords[PREVIEW_SKIPPED_WORDS];
static int previewSkippedCount;

static int previewLinesSame(const char *a, const char *b) {
    size_t la = strlen(a), lb = strlen(b);
    while (la && (a[la - 1] == ' ' || a[la - 1] == '\t' || a[la - 1] == '\r' || a[la - 1] == '\n')) la--;
    while (lb && (b[lb - 1] == ' ' || b[lb - 1] == '\t' || b[lb - 1] == '\r' || b[lb - 1] == '\n')) lb--;
    return la == lb && !memcmp(a, b, la);
}

static void previewEditsRewind(void) {
    for (int i = 0; i < previewLineEditCount; i++) previewLineEdits[i].seen = 0;
    previewEditPlane[0] = 0, previewEditMod = -1;
}

static const char *previewEditAtPass(const char *line, int mod, int *off, int *adds, int *addCount) {
    *off = 0, *addCount = 0;
    if (mod != previewEditMod) previewEditPlane[0] = 0, previewEditMod = mod;
    const char *result = line;
    const char *name = modName(mod);
    size_t lineLen = strcspn(line, " \t");
    char opening[PLANE_NAME_SIZE + 2];
    opening[0] = 0;
    if (spellWordIs(line, lineLen, NEWPLANE_WORD)) {
        const char *p = line + lineLen;
        skipSpaces(&p);
        if (readSpellQuoted(&p, opening, sizeof opening) < 0) opening[0] = 0;
    }
    for (int i = 0; i < previewLineEditCount; i++) {
        PreviewLineEdit *e = &previewLineEdits[i];
        if (strcmp(e->mod, name) || strcmp(e->plane, opening[0] ? opening : previewEditPlane) || !previewLinesSame(line, e->text)) continue;
        if (++e->seen != e->occurrence) continue;
        if (e->added) {
            if (*addCount < PREVIEW_EDIT_ADDS_MAX) adds[(*addCount)++] = i;
        } else if (e->off) *off = 1;
        else result = e->changed;
    }
    size_t len = strcspn(result, " \t");
    if (opening[0]) snprintf(previewEditPlane, sizeof previewEditPlane, "%.*s", (int)sizeof previewEditPlane - 1, opening);
    else if (!*off && !spellWordIs(result, len, NEWPLANE_WORD) && !isPlaneBlockWord(result, len)) previewEditPlane[0] = 0;
    return result;
}

static int previewEditsArmed(int level) { return previewLineEditState == 1 && level == 2 && previewLineEditCount > 0; }

static void previewEditsAfterPass(void) {
    previewPassEnding = 0;
    if (previewLineEditState == 2) previewLineEditCount = 0, previewLineEditState = 0;
    if (previewLineEditState == 1) previewLineEditState = 2, previewEditsRewind();
}

static void previewRunAddedLine(int edit, int mod, int level) {
    int saved = G32(RVA_MOD_LINE);
    G32(RVA_MOD_LINE) = PREVIEW_ADDED_LINE + edit;
    runModCommandLine(previewLineEdits[edit].changed, mod, level);
    G32(RVA_MOD_LINE) = saved;
}

static UNUSED void previewNoteWorldEnding(void) { previewPassEnding = 1, previewSkippedCount = 0; }

static const char *planeFileBaseName(const char *path);
static void previewNoteSkippedWord(const char *word, int mod, int level) {
    if (level != 2) return;
    refuseC5mLine(word, modName(mod), "unknown_word", "line=%d", G32(RVA_MOD_LINE));
    if (previewSkippedCount >= PREVIEW_SKIPPED_WORDS) return;
    PreviewSkippedWord *s = &previewSkippedWords[previewSkippedCount++];
    snprintf(s->mod, sizeof s->mod, "%s", modName(mod));
    snprintf(s->word, sizeof s->word, "%.31s", word);
    snprintf(s->file, sizeof s->file, "%.63s", planeFileBaseName((const char *)GAME_PTR(RVA_EVENT_FILE)));
    s->line = G32(RVA_MOD_LINE);
}

static int previewSkipsUnknownWords(void) { return previewWorld.active || previewPassEnding; }

// ---- Reading the lines ----------------------------------------------------------------------------
static PreviewLine *previewAddLineRow(const char *mod, const char *file, int line, const char *word, size_t wordLen, const char *text) {
    if (!growList((void **)&previewLineRows, &previewLineRowCapacity, previewLineRowCount + 1, sizeof previewLineRows[0])) {
        writeLogError("preview_lines_short", "reason=out_of_memory rows=%d", previewLineRowCount);
        return NULL;
    }
    PreviewLine *l = &previewLineRows[previewLineRowCount++];
    memset(l, 0, sizeof *l);
    snprintf(l->mod, sizeof l->mod, "%s", mod);
    snprintf(l->file, sizeof l->file, "%.63s", file);
    snprintf(l->word, sizeof l->word, "%.*s", (int)(wordLen < sizeof l->word ? wordLen : sizeof l->word - 1), word);
    snprintf(l->text, sizeof l->text, "%.*s", (int)sizeof l->text - 1, text);
    l->line = line, l->from = l->to = -1;
    return l;
}

static int previewBlameOf(const char *reason) {
    if (!strcmp(reason, "no_zone")) return previewReading.zoneLine;
    if (!strcmp(reason, "zone_populate_off")) return previewReading.populateOffLine;
    if (!strcmp(reason, "zone_has_units")) return previewReading.zoneUnitsLine;
    return 0;
}

static void previewNoteLineRefused(const char *reason) {
    if (previewReading.row < 0) return;
    PreviewLine *l = &previewLineRows[previewReading.row];
    l->refused = 1, l->by = previewBlameOf(reason);
    snprintf(l->reason, sizeof l->reason, "%.31s", reason);
}

static void previewNoteLayoutRefused(int line, const char *reason, int by) {
    for (int i = previewLineRowCount - 1; i >= 0; i--) {
        PreviewLine *l = &previewLineRows[i];
        if (l->line != line || strcmp(l->plane, planeBlock.name)) continue;
        l->refused = 1, l->by = by;
        snprintf(l->reason, sizeof l->reason, "%.31s", reason);
        return;
    }
}

static void previewNoteBlockEnded(const char *reason) {
    if (!reason)
        for (int i = 0; i < refusedPlaneCount; i++)
            if (!strcasecmp(refusedPlanes[i].name, planeBlock.name) && refusedPlanes[i].line == planeBlock.line) reason = refusedPlanes[i].reason;
    if (!reason) return;
    previewNoteLayoutRefused(planeBlock.line, reason, 0);
    for (int i = previewLineRowCount - 1; i >= 0 && previewLineRows[i].line > planeBlock.line && !strcmp(previewLineRows[i].plane, planeBlock.name); i--) {
        PreviewLine *l = &previewLineRows[i];
        if (l->refused) continue;
        l->refused = 1, l->by = planeBlock.line;
        snprintf(l->reason, sizeof l->reason, "block_refused");
    }
}

static void previewMarkOverridden(void) {
    static const char *const ONE_VALUE[] = {"planesize", "planefill", "planelike", "planepopulate", "planeicon", "planefile"};
    for (int i = 0; i < previewLineRowCount; i++) {
        PreviewLine *a = &previewLineRows[i];
        if (a->refused || a->off) continue;
        int oneValue = 0, swap = !strcmp(a->word, "planeswap") || !strcmp(a->word, "planezoneswap");
        for (size_t k = 0; k < sizeof ONE_VALUE / sizeof ONE_VALUE[0]; k++) oneValue |= !strcmp(a->word, ONE_VALUE[k]);
        if (!oneValue && !swap) continue;
        for (int j = previewLineRowCount - 1; j > i; j--) {
            const PreviewLine *b = &previewLineRows[j];
            if (b->refused || b->off || strcmp(a->plane, b->plane) || strcmp(a->file, b->file)) continue;
            int same = !strcmp(a->word, b->word) && (!swap || (a->from == b->from && !strcmp(a->zone, b->zone)));
            if (!same) continue;
            a->overridden = 1, a->by = b->line;
            break;
        }
        if (a->overridden || strcmp(a->word, "planepopulate")) continue;
        for (int j = 0; j < previewLineRowCount; j++) {
            const PreviewLine *b = &previewLineRows[j];
            if (b->refused || b->off || strcmp(b->word, "planeunits") || strcmp(a->plane, b->plane)) continue;
            a->overridden = 1, a->by = b->line;
            break;
        }
    }
}

static void previewRunPlaneWord(const char *line, size_t len, const char *args, int mod) {
    static const struct { const char *word; void (*run)(const char *, int, int); } WORDS[] = {
        {"planesize", planesizeLine}, {"planefill", planefillLine}, {"planelike", planelikeLine}, {"planepopulate", planepopulateLine},
        {"planeicon", planeiconLine}, {"planefile", planefileLine}, {"planepiece", planepieceLine}, {"planezone", planezoneLine},
        {"planezoneswap", planezoneswapLine}, {"planezoneunits", planezoneunitsLine}, {"planezonepopulate", planezonepopulateLine},
        {"planeunits", planeunitsLine},
    };
    if (spellWordIs(line, len, "planeswap")) {
        planeswapBlockLine(args, mod);
        return;
    }
    for (size_t i = 0; i < sizeof WORDS / sizeof WORDS[0]; i++)
        if (spellWordIs(line, len, WORDS[i].word)) {
            WORDS[i].run(args, mod, 2);
            return;
        }
}

static void previewFinishLineRow(PreviewLine *l, const char *line, size_t len) {
    snprintf(l->plane, sizeof l->plane, "%s", planeBlock.name);
    if (spellWordIs(line, len, NEWPLANE_WORD)) {
        previewReading.blockLine = l->line, previewReading.zoneLine = previewReading.populateOffLine = previewReading.zoneUnitsLine = 0;
        previewReading.zone[0] = 0;
        return;
    }
    if (spellWordIs(line, len, "planezone")) {
        previewReading.zoneLine = l->line, previewReading.populateOffLine = previewReading.zoneUnitsLine = 0;
        snprintf(previewReading.zone, sizeof previewReading.zone, "%s", planeBlock.zoneOpen ? planeBlock.layout.zones[planeBlock.zoneOpen - 1].name : "");
    }
    if (planeBlock.refused && !l->refused) {
        l->refused = 1, l->by = previewReading.blockLine;
        snprintf(l->reason, sizeof l->reason, "block_refused");
    }
    int zoneWord = spellWordIs(line, len, "planezone") || spellWordIs(line, len, "planezoneswap") || spellWordIs(line, len, "planezoneunits") || spellWordIs(line, len, "planezonepopulate");
    if (zoneWord) snprintf(l->zone, sizeof l->zone, "%s", previewReading.zone);
    if (l->refused) return;
    const PlaneZone *z = planeBlock.zoneOpen ? &planeBlock.layout.zones[planeBlock.zoneOpen - 1] : NULL;
    if (spellWordIs(line, len, "planezonepopulate")) previewReading.populateOffLine = l->line;
    if (spellWordIs(line, len, "planezoneunits")) previewReading.zoneUnitsLine = l->line;
    if (spellWordIs(line, len, "planeswap") && planeBlock.swapCount) {
        l->from = planeBlock.swapFrom[planeBlock.swapCount - 1], l->to = planeBlock.swapTo[planeBlock.swapCount - 1];
    } else if (spellWordIs(line, len, "planezoneswap") && z && z->swapCount) {
        l->from = z->swapFrom[z->swapCount - 1], l->to = z->swapTo[z->swapCount - 1];
    }
}

static int previewWordWasSkipped(const char *line, size_t len, int mod, int number) {
    const char *name = modName(mod);
    for (int i = 0; i < previewSkippedCount; i++) {
        const PreviewSkippedWord *s = &previewSkippedWords[i];
        if (s->line == number && !strcmp(s->mod, name) && strlen(s->word) == len && !strncmp(s->word, line, len)) return 1;
    }
    return 0;
}

#define PREVIEW_MONSTER_OWNER_NAME 48
#define PREVIEW_MOD_TERRAIN_FIRST 500
typedef struct { char name[PREVIEW_MONSTER_OWNER_NAME], mod[MOD_NAME_MAX]; int terrain; } PreviewContentOwner;
static PreviewContentOwner *previewContentOwners;
static int previewContentOwnerCount, previewContentOwnerCapacity;

static void previewNoteNewContent(const char *line, size_t len, const char *args, int mod) {
    int isTerrain = spellWordIs(line, len, "selectterr"), terrain = isTerrain ? atoi(args) : 0;
    if (isTerrain && terrain < PREVIEW_MOD_TERRAIN_FIRST) return;
    if ((!isTerrain && !spellWordIs(line, len, "newmonster")) || !growList((void **)&previewContentOwners, &previewContentOwnerCapacity, previewContentOwnerCount + 1, sizeof previewContentOwners[0])) return;
    const char *p = args;
    PreviewContentOwner *o = &previewContentOwners[previewContentOwnerCount];
    if (!isTerrain && readSpellQuoted(&p, o->name, sizeof o->name) <= 0) return;
    snprintf(o->mod, sizeof o->mod, "%s", modName(mod));
    o->terrain = terrain;
    previewContentOwnerCount++;
}

static void previewPushContentOwners(lua_State *L, int terrains) {
    lua_newtable(L);
    for (int i = 0; i < previewContentOwnerCount; i++) {
        const PreviewContentOwner *o = &previewContentOwners[i];
        if ((o->terrain != 0) != terrains) continue;
        lua_pushstring(L, o->mod);
        if (terrains) lua_seti(L, -2, o->terrain);
        else lua_setfield(L, -2, o->name);
    }
}

static void previewReadOneLine(int mod, const char *file, int number, const char *line, const char *fromFile, int off) {
    size_t len = strcspn(line, " \t");
    const char *args = line + len;
    skipSpaces(&args);
    int opens = spellWordIs(line, len, NEWPLANE_WORD);
    if (!opens && !isPlaneBlockWord(line, len)) {
        if (planeBlock.open && previewWordWasSkipped(line, len, mod, number)) {
            PreviewLine *row = previewAddLineRow(modName(mod), file, number, line, len, line);
            if (row) {
                snprintf(row->plane, sizeof row->plane, "%s", planeBlock.name);
                row->refused = 1, row->off = off;
                snprintf(row->reason, sizeof row->reason, "unknown_word");
            }
        }
        finishPlaneBlock();
        return;
    }
    if (!opens && !planeBlock.open) return;
    G32(RVA_MOD_LINE) = number;
    previewReading.row = -1;
    if (opens) finishPlaneBlock();
    PreviewLine *row = previewAddLineRow(modName(mod), file, number, line, len, line);
    if (row) previewReading.row = (int)(row - previewLineRows);
    if (row && (!fromFile || strcmp(fromFile, line))) {
        row->edited = 1;
        if (fromFile) snprintf(row->fileText, sizeof row->fileText, "%.*s", (int)sizeof row->fileText - 1, fromFile);
    }
    if (off) {
        if (row) {
            row->off = 1;
            snprintf(row->plane, sizeof row->plane, "%s", planeBlock.name);
            if (spellWordIs(line, len, "planezone") || spellWordIs(line, len, "planezoneswap") || spellWordIs(line, len, "planezoneunits") || spellWordIs(line, len, "planezonepopulate"))
                snprintf(row->zone, sizeof row->zone, "%s", previewReading.zone);
        }
        return;
    }
    if (opens) newplaneLine(args, mod, 2);
    else previewRunPlaneWord(line, len, args, mod);
    if (previewReading.row >= 0) previewFinishLineRow(&previewLineRows[previewReading.row], line, len);
}

static void previewReadLinesOfFile(int mod, const char *path, int depth) {
    FILE *f = fopen(path, "rb");
    if (!f) return;
    static char lines[PREVIEW_INCLUDE_DEPTH + 1][PLANE_LINE_MAX];
    char *text = lines[depth];
    int number = 0;
    const char *file = planeFileBaseName(path);
    finishPlaneBlock();
    while (fgets(text, PLANE_LINE_MAX, f)) {
        number++;
        text[strcspn(text, "\r\n")] = 0;
        const char *line = text;
        if (!strncmp(line, "#!@", 3)) line += 3;
        skipSpaces(&line);
        if (!*line || *line == '#') continue;
        size_t len = strcspn(line, " \t");
        const char *args = line + len;
        skipSpaces(&args);
        if (spellWordIs(line, len, "include") && depth < PREVIEW_INCLUDE_DEPTH) {
            const char *p = args;
            char name[INCLUDE_FILE_MAX + 2], included[MAX_PATH];
            int nameLen = readSpellQuoted(&p, name, sizeof name);
            finishPlaneBlock();
            if (nameLen > 0 && nameLen <= INCLUDE_FILE_MAX && buildPath(included, sizeof included, "previewLines", "%s/%s", modFolder(mod), name)) previewReadLinesOfFile(mod, included, depth + 1);
            continue;
        }
        previewNoteNewContent(line, len, args, mod);
        int off = 0, adds[PREVIEW_EDIT_ADDS_MAX], addCount = 0;
        const char *fromFile = line;
        if (previewLineEditState == 2) line = previewEditAtPass(line, mod, &off, adds, &addCount);
        previewReadOneLine(mod, file, number, line, fromFile, off);
        for (int a = 0; a < addCount; a++) previewReadOneLine(mod, file, PREVIEW_ADDED_LINE + adds[a], previewLineEdits[adds[a]].changed, NULL, 0);
    }
    finishPlaneBlock();
    fclose(f);
}

static UNUSED void previewReadAllLines(void) {
    int savedLine = G32(RVA_MOD_LINE);
    previewReading.on = 1;
    previewLineRowCount = 0;
    previewContentOwnerCount = 0;
    previewEditsRewind();
    int last = G32(RVA_MOD_LAST_INDEX);
    for (int m = 0; m <= last; m++) {
        if (!modEnabled(m)) continue;
        char path[MAX_PATH];
        if (buildPath(path, sizeof path, "previewLines", "%s/%s.c5m", modFolder(m), modName(m))) previewReadLinesOfFile(m, path, 0);
        if (buildPath(path, sizeof path, "previewLines", "%s/%s.c5se", modFolder(m), modName(m))) previewReadLinesOfFile(m, path, 0);
    }
    planeBlock.open = 0;
    previewMarkOverridden();
    previewReading.on = 0;
    G32(RVA_MOD_LINE) = savedLine;
    previewLineEditCount = 0, previewLineEditState = 0;
    writeLog("preview_lines", "rows=%d", previewLineRowCount);
}

// ---- The build's numbers ----------------------------------------------------------------------------
static int previewLineRowOf(const CustomPlane *p, int line) {
    if (!previewWorld.active) return -1;
    for (int i = 0; i < previewLineRowCount; i++)
        if (previewLineRows[i].line == line && !strcmp(previewLineRows[i].plane, p->name)) return i;
    return -1;
}

static int previewLineRowOfWord(const CustomPlane *p, const char *word) {
    if (!previewWorld.active) return -1;
    for (int i = previewLineRowCount - 1; i >= 0; i--)
        if (!previewLineRows[i].refused && !previewLineRows[i].off && !strcmp(previewLineRows[i].word, word) && !strcmp(previewLineRows[i].plane, p->name)) return i;
    return -1;
}

static int previewPlacedThisTry;

static UNUSED void previewLinesStartTry(void) {
    previewResultsStartTry();
    if (!previewWorld.active) return;
    previewPlacedThisTry = 0;
    for (int i = 0; i < previewLineRowCount; i++) {
        PreviewLine *l = &previewLineRows[i];
        l->did = l->of = l->units = l->ran = l->wrote = l->blameCount = l->failed = l->copyCount = l->order = 0;
        if (!l->refused) l->reason[0] = 0;
    }
    previewTerrainWriteCount = 0;
    previewSourceNow = PREVIEW_SOURCE_WORLD;
    if (!previewTerrainSeen) previewTerrainSeen = malloc(sizeof *previewTerrainSeen * MAP_COLUMNS_MAX * MAP_COLUMN_SQUARES);
    if (!previewUnitSources) previewUnitSources = malloc(sizeof *previewUnitSources * UNIT_TABLE_MAX);
    if (!previewUnitsSeen) previewUnitsSeen = malloc(sizeof *previewUnitsSeen * UNIT_TABLE_MAX);
    if (!previewUnitLastChange) previewUnitLastChange = malloc(sizeof *previewUnitLastChange * UNIT_TABLE_MAX);
    if (!previewTerrainSeen || !previewUnitSources || !previewUnitsSeen || !previewUnitLastChange) {
        writeLogError("preview_sources_short", "reason=out_of_memory");
        return;
    }
    memset(previewTerrainSeen, 0xff, sizeof *previewTerrainSeen * MAP_COLUMNS_MAX * MAP_COLUMN_SQUARES);
    memset(previewUnitSources, 0xff, sizeof *previewUnitSources * UNIT_TABLE_MAX);
    memset(previewUnitLastChange, 0xff, sizeof *previewUnitLastChange * UNIT_TABLE_MAX);
}

static void previewLinesPlanesPlaced(void) {
    if (!previewWorld.active) return;
    if (previewPlacedThisTry) previewLinesStartTry();
    previewPlacedThisTry = 1;
}

static void previewNoteTerrainWrite(int square, int source, short terrain) {
    if (!growList((void **)&previewTerrainWrites, &previewTerrainWriteCapacity, previewTerrainWriteCount + 1, sizeof previewTerrainWrites[0])) {
        writeLogError("preview_sources_short", "reason=out_of_memory writes=%d", previewTerrainWriteCount);
        return;
    }
    previewTerrainWrites[previewTerrainWriteCount++] = (PreviewTerrainWrite){square, source, terrain};
    if (source >= 0 && source < previewLineRowCount) previewLineRows[source].wrote++;
    previewTerrainSeen[square] = terrain;
}

static void previewTerrainCompare(int x0, int y0, int w, int h, int source) {
    if (!previewWorld.active || !previewTerrainSeen) return;
    for (int x = x0; x < x0 + w && x < MAP_COLUMNS_MAX; x++)
        for (int y = y0; y < y0 + h && y < MAP_COLUMN_SQUARES; y++) {
            short terrain = *(short *)(squareRecordAt(x, y) + SQUARE_TERRAIN_TYPE);
            int square = x * MAP_COLUMN_SQUARES + y;
            if (previewTerrainSeen[square] != terrain) previewNoteTerrainWrite(square, source, terrain);
        }
}

static void previewTerrainStepPlane(const CustomPlane *p, int source) {
    if (!previewWorld.active || p->index < 0) return;
    previewTerrainCompare(planeField(p->index, PLANE_X), planeField(p->index, PLANE_Y), p->w, p->h, source);
}

static void previewTerrainStepWord(const CustomPlane *p, const char *word) {
    if (!previewWorld.active) return;
    int row = previewLineRowOfWord(p, word);
    previewTerrainStepPlane(p, row >= 0 ? row : PREVIEW_SOURCE_WORLD);
}

static UNUSED void previewTerrainStepAll(int source) {
    if (!previewWorld.active) return;
    int last = G32(RVA_LAST_PLANE);
    for (int plane = 0; plane <= last; plane++) {
        int w = planeField(plane, PLANE_W), h = planeField(plane, PLANE_H);
        if (w > 0 && h > 0) previewTerrainCompare(planeField(plane, PLANE_X), planeField(plane, PLANE_Y), w, h, source);
    }
}

static void previewTerrainAfterPass(int level) {
    if (level != 3) return;
    previewTerrainStepAll(PREVIEW_SOURCE_EVENT);
    previewEventsInSync = 0;
}

static void previewPieceAsked(const CustomPlane *p, const PlanePiece *piece, int order, int orderOf) {
    int row = previewLineRowOf(p, piece->line);
    if (row < 0) return;
    PreviewLine *l = &previewLineRows[row];
    l->of = piece->count, l->w = piece->w, l->h = piece->h, l->order = order, l->orderOf = orderOf;
}

static void previewPieceCopy(PreviewLine *l, int x, int y, const char *why) {
    if (l->copyCount >= PREVIEW_PIECE_COPIES) return;
    l->copies[l->copyCount].x = x, l->copies[l->copyCount].y = y;
    snprintf(l->copies[l->copyCount].why, sizeof l->copies[0].why, "%.15s", why ? why : "");
    l->copyCount++;
}

static void previewPieceStamped(const CustomPlane *p, const PlanePiece *piece, int x, int y) {
    int row = previewLineRowOf(p, piece->line);
    if (row < 0) return;
    previewLineRows[row].did++;
    previewPieceCopy(&previewLineRows[row], x - planeField(p->index, PLANE_X), y - planeField(p->index, PLANE_Y), NULL);
    previewTerrainCompare(x, y, piece->w, piece->h, row);
}

static void previewPieceSkipped(const CustomPlane *p, const PlanePiece *piece, const char *why) {
    int row = previewLineRowOf(p, piece->line);
    if (row < 0) return;
    PreviewLine *l = &previewLineRows[row];
    snprintf(l->reason, sizeof l->reason, "%.31s", why);
    previewPieceCopy(l, 0, -1, why);
    for (int i = 0; i < p->layout.placedCount; i++) {
        int placed = p->layout.placed[i].piece;
        if (placed < 0 || p->layout.pieces[placed].line == piece->line) continue;
        int known = 0;
        for (int k = 0; k < l->blameCount; k++) known |= l->blame[k] == p->layout.pieces[placed].line;
        if (!known && l->blameCount < PREVIEW_LINE_BLAMES) l->blame[l->blameCount++] = p->layout.pieces[placed].line;
    }
}

static void previewSwapChanged(const CustomPlane *p, int line, int x, int y) {
    int row = previewLineRowOf(p, line);
    if (row < 0) return;
    previewLineRows[row].did++;
    if (previewTerrainSeen) previewNoteTerrainWrite(x * MAP_COLUMN_SQUARES + y, row, *(short *)(squareRecordAt(x, y) + SQUARE_TERRAIN_TYPE));
}

static void previewLineFailed(const CustomPlane *p, int line, const char *reason) {
    int row = previewLineRowOf(p, line);
    if (row < 0) return;
    previewLineRows[row].failed = 1;
    snprintf(previewLineRows[row].reason, sizeof previewLineRows[row].reason, "%.31s", reason);
}

static void previewZoneOverlapped(const CustomPlane *p, int z) {
    const PlaneLayout *layout = &p->layout;
    int row = previewLineRowOf(p, layout->zones[z].line);
    if (row < 0 || z == 0) return;
    for (int i = 0; i < layout->placedCount; i++) {
        if (layout->placed[i].piece != layout->zones[z].piece) continue;
        snprintf(previewLineRows[row].reason, sizeof previewLineRows[row].reason, "zone_overlap");
        previewLineRows[row].by = layout->zones[z - 1].line;
        return;
    }
}

static void previewLineDid(const CustomPlane *p, int line, int did) {
    int row = previewLineRowOf(p, line);
    if (row >= 0) previewLineRows[row].did = did;
}

static void previewUnitsStocked(int row, int rolls, int hits, int units) {
    if (row < 0) return;
    previewLineRows[row].of += rolls, previewLineRows[row].did += hits, previewLineRows[row].units += units;
}

static void previewSetSource(int source) {
    if (previewWorld.active) previewSourceNow = source;
}

static void previewSourcePopulation(const CustomPlane *p) {
    if (!previewWorld.active) return;
    int row = previewLineRowOfWord(p, "planepopulate");
    if (row < 0) row = previewLineRowOfWord(p, "planelike");
    if (row >= 0) previewLineRows[row].ran = 1;
    previewSourceNow = row >= 0 ? row : PREVIEW_SOURCE_POPULATION - p->index;
}

static UNUSED void previewNoteNewUnit(const CallInfo *call) {
    if (!previewWorld.active || !previewUnitSources) return;
    int unit = (int)call->result;
    if (unit < 0 || unit >= UNIT_TABLE_MAX) return;
    if (previewScriptPlaceNow >= 0) previewUnitSources[unit] = PREVIEW_SOURCE_SCRIPT_PLACE - previewScriptPlaceNow;
    else previewUnitSources[unit] = G32(RVA_EVENT_STATE) >= 1 ? PREVIEW_SOURCE_EVENT : previewSourceNow;
    if (previewUnitLastChange) previewUnitLastChange[unit] = -1;
}

// ---- What events changed -----------------------------------------------------------------------------
static int previewPlaneOfSquare(int x, int y) {
    int last = G32(RVA_LAST_PLANE);
    for (int plane = 0; plane <= last; plane++) {
        int x0 = planeField(plane, PLANE_X), y0 = planeField(plane, PLANE_Y);
        if (x >= x0 && y >= y0 && x < x0 + planeField(plane, PLANE_W) && y < y0 + planeField(plane, PLANE_H)) return plane;
    }
    return -1;
}

static void previewUnitsLook(void) {
    if (!previewUnitsSeen) return;
    memset(previewUnitsSeen, 0, sizeof *previewUnitsSeen * UNIT_TABLE_MAX);
    int last = G32(RVA_UNIT_COUNT);
    for (int u = 0; u <= last && u < UNIT_TABLE_MAX; u++) {
        size_t at = (size_t)u * UNIT_SIZE;
        PreviewUnitSeen *seen = &previewUnitsSeen[u];
        seen->alive = (short)G16(RVA_UNIT_HP + at) > 0;
        seen->monster = (short)G16(RVA_UNIT_MONSTER + at), seen->owner = (short)G16(RVA_UNIT_PLAYER + at);
        seen->x = (int)G32(RVA_UNIT_X + at), seen->y = (int)G32(RVA_UNIT_X + 4 + at);
    }
}

static void previewEventLineBefore(int level) {
    if (level != 3 || !previewResultsOn || previewEventsInSync) return;
    previewEventsInSync = 1;
    previewTerrainStepAll(PREVIEW_SOURCE_WORLD);
    previewUnitsLook();
}

static int previewEventActionFor(int row, const char *line, int number) {
    for (int i = previewEventActionCount - 1; i >= 0; i--)
        if (previewEventActions[i].row == row && previewEventActions[i].line == number) return i;
    if (!growList((void **)&previewEventActions, &previewEventActionCapacity, previewEventActionCount + 1, sizeof previewEventActions[0])) {
        previewResultsFull("event_actions");
        return -1;
    }
    PreviewEventAction *a = &previewEventActions[previewEventActionCount];
    a->row = row, a->line = number;
    snprintf(a->text, sizeof a->text, "%.199s", line);
    return previewEventActionCount++;
}

static void previewNoteUnitChange(int action, int unit, int kind, int from, int to, int x, int y) {
    if (!growList((void **)&previewUnitChanges, &previewUnitChangeCapacity, previewUnitChangeCount + 1, sizeof previewUnitChanges[0])) {
        previewResultsFull("unit_changes");
        return;
    }
    int plane = previewPlaneOfSquare(x, y);
    previewUnitChanges[previewUnitChangeCount] = (PreviewUnitChange){unit, action, previewUnitLastChange[unit], x, y, (short)from, (short)to, (short)plane, (short)kind};
    previewUnitLastChange[unit] = previewUnitChangeCount++;
    if (plane >= 0 && plane < 64) previewEventRows[previewEventActions[action].row].changed |= 1ULL << plane;
}

static void previewNoteEventAction(int row, const char *line, int number) {
    if (!previewResultsOn || !previewUnitsSeen || !previewUnitLastChange || !previewUnitSources) return;
    int action = previewEventActionFor(row, line, number);
    if (action < 0) return;
    int source = PREVIEW_SOURCE_EVENT_ACTION - action;
    int last = G32(RVA_UNIT_COUNT);
    for (int u = 0; u <= last && u < UNIT_TABLE_MAX; u++) {
        size_t at = (size_t)u * UNIT_SIZE;
        PreviewUnitSeen *seen = &previewUnitsSeen[u];
        int alive = (short)G16(RVA_UNIT_HP + at) > 0;
        if (!alive && !seen->alive) continue;
        int monster = (short)G16(RVA_UNIT_MONSTER + at), owner = (short)G16(RVA_UNIT_PLAYER + at), x = (int)G32(RVA_UNIT_X + at), y = (int)G32(RVA_UNIT_X + 4 + at);
        if (alive && !seen->alive) {
            previewUnitSources[u] = source;
            previewNoteUnitChange(action, u, PREVIEW_CHANGE_NEW, monster, monster, x, y);
        } else if (!alive) previewNoteUnitChange(action, u, PREVIEW_CHANGE_KILLED, seen->monster, seen->monster, seen->x, seen->y);
        else if (monster != seen->monster) previewNoteUnitChange(action, u, PREVIEW_CHANGE_MONSTER, seen->monster, monster, x, y);
        else if (owner != seen->owner) previewNoteUnitChange(action, u, PREVIEW_CHANGE_OWNER, monster, monster, x, y);
        else if (x != seen->x || y != seen->y) previewNoteUnitChange(action, u, PREVIEW_CHANGE_MOVED, monster, monster, x, y);
        else continue;
        seen->alive = (short)alive, seen->monster = (short)monster, seen->owner = (short)owner, seen->x = x, seen->y = y;
    }
    int firstWrite = previewTerrainWriteCount;
    previewTerrainStepAll(source);
    for (int i = firstWrite; i < previewTerrainWriteCount; i++) {
        int square = previewTerrainWrites[i].square, plane = previewPlaneOfSquare(square / MAP_COLUMN_SQUARES, square % MAP_COLUMN_SQUARES);
        if (plane >= 0 && plane < 64) previewEventRows[row].changed |= 1ULL << plane;
    }
}

static int previewActionOfSource(int source) {
    int action = PREVIEW_SOURCE_EVENT_ACTION - source;
    return source <= PREVIEW_SOURCE_EVENT_ACTION && action < previewEventActionCount ? action : -1;
}

// ---- The answers ----------------------------------------------------------------------------------
static void previewTallyLostWrites(void) {
    for (int i = 0; i < previewLineRowCount; i++) previewLineRows[i].lost = previewLineRows[i].lostTo = previewLineRows[i].takerCount = 0;
    if (!previewTerrainWriteCount) return;
    int *lastAt = malloc(sizeof *lastAt * MAP_COLUMNS_MAX * MAP_COLUMN_SQUARES);
    int *takenBy = calloc((size_t)previewLineRowCount + 1, sizeof *takenBy);
    int *counted = calloc((size_t)MAP_COLUMNS_MAX * MAP_COLUMN_SQUARES, sizeof *counted);
    if (!lastAt || !takenBy || !counted) {
        free(lastAt);
        free(takenBy);
        free(counted);
        writeLogError("preview_sources_short", "reason=out_of_memory");
        return;
    }
    memset(lastAt, 0xff, sizeof *lastAt * MAP_COLUMNS_MAX * MAP_COLUMN_SQUARES);
    for (int i = 0; i < previewTerrainWriteCount; i++) lastAt[previewTerrainWrites[i].square] = i;
    for (int row = 0; row < previewLineRowCount; row++) {
        PreviewLine *l = &previewLineRows[row];
        memset(takenBy, 0, sizeof *takenBy * ((size_t)previewLineRowCount + 1));
        for (int i = 0; i < previewTerrainWriteCount; i++) {
            const PreviewTerrainWrite *w = &previewTerrainWrites[i];
            if (w->source != row) continue;
            int last = lastAt[w->square], by = previewTerrainWrites[last].source;
            if (last == i || by == row || counted[w->square] == row + 1) continue;
            counted[w->square] = row + 1;
            l->lost++;
            takenBy[by >= 0 && by < previewLineRowCount ? by : previewLineRowCount]++;
        }
        for (int t = 0; t < (int)(sizeof l->takers / sizeof l->takers[0]); t++) {
            int best = -1;
            for (int slot = 0; slot <= previewLineRowCount; slot++)
                if (takenBy[slot] > 0 && (best < 0 || takenBy[slot] > takenBy[best])) best = slot;
            if (best < 0) break;
            l->takers[t].line = best < previewLineRowCount ? previewLineRows[best].line : 0, l->takers[t].squares = takenBy[best];
            l->takerCount = t + 1;
            takenBy[best] = 0;
        }
        if (l->takerCount) l->lostTo = l->takers[0].line;
    }
    free(lastAt);
    free(takenBy);
    free(counted);
}

static UNUSED const char *previewLineState(const PreviewLine *l) {
    if (l->off) return "off";
    if (l->refused) return "refused";
    if (l->failed) return "failed";
    if (l->overridden) return "overridden";
    const char *w = l->word;
    if (!strcmp(w, "planepiece")) return l->did == 0 ? "failed" : l->did < l->of ? "partly" : "worked";
    if (!strcmp(w, "planeunits") || !strcmp(w, "planezoneunits")) return l->of == 0 ? "nothing" : l->did == 0 ? "rolledNo" : "worked";
    if (!strcmp(w, "planeswap") || !strcmp(w, "planezoneswap") || !strcmp(w, "planezone")) return l->did == 0 ? "nothing" : "worked";
    if (!strcmp(w, NEWPLANE_WORD) || !strcmp(w, "planesize") || !strcmp(w, "planezonepopulate")) return "worked";
    if (!strcmp(w, "planelike") || !strcmp(w, "planefill") || !strcmp(w, "planefile") || !strcmp(w, "planepopulate")) return l->ran || l->wrote ? "worked" : "notMeasured";
    return "notMeasured";
}

static UNUSED int previewNameDistance(const char *a, const char *b) {
    int la = (int)strlen(a), lb = (int)strlen(b);
    if (la > 15) la = 15;
    if (lb > 15) lb = 15;
    int row[16];
    for (int j = 0; j <= lb; j++) row[j] = j;
    for (int i = 1; i <= la; i++) {
        int diagonal = row[0];
        row[0] = i;
        for (int j = 1; j <= lb; j++) {
            int was = row[j], cost = tolower((unsigned char)a[i - 1]) == tolower((unsigned char)b[j - 1]) ? 0 : 1;
            int best = diagonal + cost;
            if (row[j] + 1 < best) best = row[j] + 1;
            if (row[j - 1] + 1 < best) best = row[j - 1] + 1;
            row[j] = best, diagonal = was;
        }
    }
    return row[lb];
}

static UNUSED const char *previewCloseName(const char *name, const unsigned char *table, size_t size, int count) {
    const char *best = NULL;
    int bestDistance = (int)strlen(name) / 3 + 2;
    for (int i = 0; i < count; i++) {
        const char *other = (const char *)table + (size_t)i * size;
        if (!other[0]) continue;
        int distance = previewNameDistance(name, other);
        if (distance < bestDistance) best = other, bestDistance = distance;
    }
    return best;
}

static UNUSED void previewCloseFromLine(const PreviewLine *l, const unsigned char *table, size_t size, int count, char *out, size_t outSize) {
    const char *p = l->text;
    out[0] = 0;
    while ((p = strchr(p, '"')) != NULL) {
        const char *end = strchr(p + 1, '"');
        if (!end) return;
        char name[64];
        snprintf(name, sizeof name, "%.*s", (int)(end - p - 1 < 63 ? end - p - 1 : 63), p + 1);
        int found = 0;
        for (int i = 0; i < count && !found; i++) found = !strcasecmp((const char *)table + (size_t)i * size, name);
        if (!found) {
            const char *close = previewCloseName(name, table, size, count);
            if (close) snprintf(out, outSize, "%.47s", close);
            return;
        }
        p = end + 1;
    }
}

static UNUSED void previewPushSource(lua_State *L, int source) {
    int action = previewActionOfSource(source);
    if (source <= PREVIEW_SOURCE_SCRIPT_PLACE && action < 0) {
        int place = PREVIEW_SOURCE_SCRIPT_PLACE - source;
        if (place < previewScriptPlaceCount) {
            const PreviewScriptPlace *p = &previewScriptPlaces[place];
            lua_pushinteger(L, p->line), lua_setfield(L, -2, "scriptLine");
            lua_pushstring(L, p->file), lua_setfield(L, -2, "file");
            lua_pushstring(L, p->mod), lua_setfield(L, -2, "mod");
            lua_pushstring(L, p->call), lua_setfield(L, -2, "text");
        }
        lua_pushstring(L, "script"), lua_setfield(L, -2, "step");
        return;
    }
    if (action >= 0) {
        const PreviewEventAction *a = &previewEventActions[action];
        const PreviewEventRow *r = &previewEventRows[a->row];
        lua_pushinteger(L, r->line), lua_setfield(L, -2, "eventLine");
        lua_pushinteger(L, a->line), lua_setfield(L, -2, "actionLine");
        lua_pushstring(L, r->file), lua_setfield(L, -2, "file");
        lua_pushstring(L, r->mod), lua_setfield(L, -2, "mod");
        lua_pushstring(L, a->text), lua_setfield(L, -2, "text");
        lua_pushstring(L, "event"), lua_setfield(L, -2, "step");
        return;
    }
    if (source >= 0 && source < previewLineRowCount) {
        const PreviewLine *l = &previewLineRows[source];
        lua_pushinteger(L, l->line), lua_setfield(L, -2, "line");
        lua_pushstring(L, l->file), lua_setfield(L, -2, "file");
        lua_pushstring(L, l->mod), lua_setfield(L, -2, "mod");
        lua_pushstring(L, l->word), lua_setfield(L, -2, "word");
        lua_pushstring(L, l->text), lua_setfield(L, -2, "text");
        lua_pushstring(L, l->plane), lua_setfield(L, -2, "plane");
        return;
    }
    const char *step = source == PREVIEW_SOURCE_MONTH ? "month" : source == PREVIEW_SOURCE_EVENT ? "event" : source == PREVIEW_SOURCE_SCRIPT ? "script"
                     : source == PREVIEW_SOURCE_GATEWAY ? "gateway" : source <= PREVIEW_SOURCE_POPULATION ? "population" : "world";
    lua_pushstring(L, step), lua_setfield(L, -2, "step");
    if (source <= PREVIEW_SOURCE_POPULATION) lua_pushinteger(L, PREVIEW_SOURCE_POPULATION - source), lua_setfield(L, -2, "planeIndex");
}

typedef struct { int row, square; } PreviewRowSquare;

static int comparePreviewRowSquares(const void *a, const void *b) {
    const PreviewRowSquare *x = a, *y = b;
    return x->row != y->row ? x->row - y->row : x->square - y->square;
}

static void previewPushRowSquares(lua_State *L, const PreviewRowSquare *pairs, int n, int row, const char *field, const char *countField) {
    lua_newtable(L);
    int out = 0;
    for (int i = 0; i < n; i++) {
        if (pairs[i].row != row) continue;
        int run = 1;
        while (i + run < n && pairs[i + run].row == row && pairs[i + run].square == pairs[i].square) run++;
        lua_newtable(L);
        lua_pushinteger(L, pairs[i].square / MAP_COLUMN_SQUARES), lua_setfield(L, -2, "x");
        lua_pushinteger(L, pairs[i].square % MAP_COLUMN_SQUARES), lua_setfield(L, -2, "y");
        if (countField) lua_pushinteger(L, run), lua_setfield(L, -2, countField);
        lua_seti(L, -2, ++out);
        i += run - 1;
    }
    lua_setfield(L, -2, field);
}

static UNUSED int lSetPreviewLineEdits(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    if (!previewWorld.active) {
        refuseLuaCall(runningLuaMod(), "coe5.setPreviewLineEdits", "no_preview_world", "edits=%d", (int)lua_rawlen(L, 1));
        lua_pushboolean(L, 0);
        return 1;
    }
    int n = (int)lua_rawlen(L, 1);
    previewLineEditCount = 0;
    if (n && !growList((void **)&previewLineEdits, &previewLineEditCapacity, n, sizeof previewLineEdits[0])) {
        refuseLuaCall(runningLuaMod(), "coe5.setPreviewLineEdits", "out_of_memory", "edits=%d", n);
        lua_pushboolean(L, 0);
        return 1;
    }
    for (int i = 1; i <= n; i++) {
        lua_rawgeti(L, 1, i);
        if (lua_type(L, -1) != LUA_TTABLE) {
            lua_pop(L, 1);
            previewLineEditCount = 0;
            refuseLuaCall(runningLuaMod(), "coe5.setPreviewLineEdits", "bad_edit", "row=%d", i);
            lua_pushboolean(L, 0);
            return 1;
        }
        lua_getfield(L, -1, "mod");
        lua_getfield(L, -2, "plane");
        lua_getfield(L, -3, "text");
        lua_getfield(L, -4, "changed");
        lua_getfield(L, -5, "occurrence");
        lua_getfield(L, -6, "off");
        lua_getfield(L, -7, "added");
        const char *mod = lua_tostring(L, -7), *plane = lua_tostring(L, -6), *text = lua_tostring(L, -5), *changed = lua_tostring(L, -4);
        if (!mod || !plane || !text || (!changed && !lua_toboolean(L, -2))) {
            lua_pop(L, 8);
            previewLineEditCount = 0;
            refuseLuaCall(runningLuaMod(), "coe5.setPreviewLineEdits", "bad_edit", "row=%d", i);
            lua_pushboolean(L, 0);
            return 1;
        }
        PreviewLineEdit *e = &previewLineEdits[previewLineEditCount++];
        memset(e, 0, sizeof *e);
        snprintf(e->mod, sizeof e->mod, "%s", mod);
        snprintf(e->plane, sizeof e->plane, "%s", plane);
        snprintf(e->text, sizeof e->text, "%.*s", (int)sizeof e->text - 1, text);
        snprintf(e->changed, sizeof e->changed, "%.*s", (int)sizeof e->changed - 1, changed ? changed : "");
        e->occurrence = lua_isinteger(L, -3) ? (int)lua_tointeger(L, -3) : 1;
        e->off = lua_toboolean(L, -2), e->added = lua_toboolean(L, -1);
        lua_pop(L, 8);
    }
    previewLineEditState = n ? 1 : 0;
    previewEditsRewind();
    writeLog("preview_edits", "edits=%d", n);
    lua_pushboolean(L, 1);
    return 1;
}

static UNUSED int lPreviewLineIndex(lua_State *L) {
    if (!previewWorld.active) {
        refuseLuaCall(runningLuaMod(), "coe5.previewLineIndex", "no_preview_world", "rows=%d", previewLineRowCount);
        lua_pushnil(L);
        return 1;
    }
    lua_newtable(L);
    lua_newtable(L);
    for (int i = 0; i < previewLineRowCount; i++) {
        const PreviewLine *l = &previewLineRows[i];
        lua_newtable(L);
        lua_pushstring(L, l->plane), lua_setfield(L, -2, "plane");
        lua_pushstring(L, l->mod), lua_setfield(L, -2, "mod");
        lua_pushstring(L, l->file), lua_setfield(L, -2, "file");
        lua_pushinteger(L, l->line), lua_setfield(L, -2, "line");
        lua_pushstring(L, l->word), lua_setfield(L, -2, "word");
        lua_pushstring(L, l->text), lua_setfield(L, -2, "text");
        lua_pushstring(L, previewLineState(l)), lua_setfield(L, -2, "state");
        if (l->from >= 0) lua_pushinteger(L, l->from), lua_setfield(L, -2, "from");
        if (l->to >= 0) lua_pushinteger(L, l->to), lua_setfield(L, -2, "to");
        lua_seti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "lines");
    previewPushContentOwners(L, 0);
    lua_setfield(L, -2, "monsters");
    previewPushContentOwners(L, 1);
    lua_setfield(L, -2, "terrains");
    lua_createtable(L, previewSkippedCount, 0);
    for (int i = 0; i < previewSkippedCount; i++) {
        const PreviewSkippedWord *s = &previewSkippedWords[i];
        lua_createtable(L, 0, 5);
        lua_pushstring(L, s->mod), lua_setfield(L, -2, "mod");
        lua_pushstring(L, s->file), lua_setfield(L, -2, "file");
        lua_pushinteger(L, s->line), lua_setfield(L, -2, "line");
        lua_pushstring(L, s->word), lua_setfield(L, -2, "word");
        if (previewNameDistance(s->word, NEWPLANE_WORD) < (int)strlen(s->word) / 3 + 2) lua_pushstring(L, NEWPLANE_WORD), lua_setfield(L, -2, "close");
        lua_seti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "skipped");
    lua_createtable(L, luaScriptCount, 0);
    for (int i = 0; i < luaScriptCount; i++) lua_pushstring(L, luaScripts[i]->mod), lua_seti(L, -2, i + 1);
    lua_setfield(L, -2, "scripts");
    lua_newtable(L);
    for (int i = 0, n = 0; i < loadedScriptFileCount; i++) {
        const LoadedScriptFile *f = &loadedScriptFiles[i];
        if (hashLoadedScriptFile(f->path) == f->hash) continue;
        lua_createtable(L, 0, 2);
        lua_pushstring(L, f->mod), lua_setfield(L, -2, "mod");
        lua_pushstring(L, planeFileBaseName(f->path)), lua_setfield(L, -2, "file");
        lua_seti(L, -2, ++n);
    }
    lua_setfield(L, -2, "changedScripts");
    return 1;
}

static UNUSED int lPreviewLineResults(lua_State *L) {
    const char *name = luaL_checkstring(L, 1);
    if (!previewWorld.active) {
        refuseLuaCall(runningLuaMod(), "coe5.previewLineResults", "no_preview_world", "plane=%.40s", name);
        lua_pushnil(L);
        return 1;
    }
    const CustomPlane *p = findCustomPlane(name);
    int built = p && !p->skipped && p->index >= 0 && previewTerrainSeen && previewUnitSources;
    PreviewRowSquare *unitPairs = NULL, *writePairs = NULL;
    int unitPairCount = 0, writePairCount = 0;
    if (built) {
        int last = G32(RVA_UNIT_COUNT);
        unitPairs = malloc(sizeof *unitPairs * (size_t)(last + 1));
        writePairs = malloc(sizeof *writePairs * (size_t)(previewTerrainWriteCount + 1));
        for (int u = 0; unitPairs && u <= last && u < UNIT_TABLE_MAX; u++) {
            int source = previewUnitSources[u];
            if (source < 0 || source >= previewLineRowCount || strcmp(previewLineRows[source].plane, name) || G16(RVA_UNIT_HP + (size_t)u * UNIT_SIZE) <= 0) continue;
            unitPairs[unitPairCount++] = (PreviewRowSquare){source, G32(RVA_UNIT_X + (size_t)u * UNIT_SIZE) * MAP_COLUMN_SQUARES + G32(RVA_UNIT_X + 4 + (size_t)u * UNIT_SIZE)};
        }
        for (int i = 0; writePairs && i < previewTerrainWriteCount; i++) {
            int source = previewTerrainWrites[i].source;
            if (source >= 0 && source < previewLineRowCount && !strcmp(previewLineRows[source].plane, name)) writePairs[writePairCount++] = (PreviewRowSquare){source, previewTerrainWrites[i].square};
        }
        if (unitPairs) qsort(unitPairs, (size_t)unitPairCount, sizeof *unitPairs, comparePreviewRowSquares);
        if (writePairs) qsort(writePairs, (size_t)writePairCount, sizeof *writePairs, comparePreviewRowSquares);
    }
    previewTallyLostWrites();
    lua_newtable(L);
    int out = 0;
    for (int i = 0; i < previewLineRowCount; i++) {
        PreviewLine *l = &previewLineRows[i];
        if (strcmp(l->plane, name)) continue;
        if (!built && p && p->skipped && !l->refused) {
            int opens = !strcmp(l->word, NEWPLANE_WORD);
            l->failed = opens, l->refused = !opens;
            snprintf(l->reason, sizeof l->reason, "%s", opens ? "no_room" : "plane_left_out");
            if (!opens) l->by = p->declLine;
        }
        lua_newtable(L);
        lua_pushinteger(L, l->line), lua_setfield(L, -2, "line");
        lua_pushstring(L, l->file), lua_setfield(L, -2, "file");
        lua_pushstring(L, l->mod), lua_setfield(L, -2, "mod");
        lua_pushstring(L, l->word), lua_setfield(L, -2, "word");
        lua_pushstring(L, l->text), lua_setfield(L, -2, "text");
        lua_pushstring(L, previewLineState(l)), lua_setfield(L, -2, "state");
        if (l->off) lua_pushboolean(L, 1), lua_setfield(L, -2, "off");
        if (l->edited) lua_pushboolean(L, 1), lua_setfield(L, -2, "edited");
        if (l->fileText[0]) lua_pushstring(L, l->fileText), lua_setfield(L, -2, "fileText");
        if (l->line >= PREVIEW_ADDED_LINE) lua_pushboolean(L, 1), lua_setfield(L, -2, "added");
        if (l->zone[0]) lua_pushstring(L, l->zone), lua_setfield(L, -2, "zone");
        if (l->reason[0]) lua_pushstring(L, l->reason), lua_setfield(L, -2, "reason");
        if (l->by) lua_pushinteger(L, l->by), lua_setfield(L, -2, "by");
        if (l->from >= 0) lua_pushinteger(L, l->from), lua_setfield(L, -2, "from");
        if (l->to >= 0) lua_pushinteger(L, l->to), lua_setfield(L, -2, "to");
        lua_pushinteger(L, l->did), lua_setfield(L, -2, "did");
        lua_pushinteger(L, l->of), lua_setfield(L, -2, "of");
        lua_pushinteger(L, l->units), lua_setfield(L, -2, "placed");
        lua_pushinteger(L, l->wrote), lua_setfield(L, -2, "wrote");
        if (l->lost) {
            lua_pushinteger(L, l->lost), lua_setfield(L, -2, "lost");
            if (l->lostTo) lua_pushinteger(L, l->lostTo), lua_setfield(L, -2, "lostTo");
            lua_createtable(L, l->takerCount, 0);
            for (int k = 0; k < l->takerCount; k++) {
                lua_createtable(L, 0, 2);
                lua_pushinteger(L, l->takers[k].line), lua_setfield(L, -2, "line");
                lua_pushinteger(L, l->takers[k].squares), lua_setfield(L, -2, "squares");
                lua_seti(L, -2, k + 1);
            }
            lua_setfield(L, -2, "takers");
        }
        if (l->w) {
            lua_pushinteger(L, l->w), lua_setfield(L, -2, "w");
            lua_pushinteger(L, l->h), lua_setfield(L, -2, "h");
        }
        if (l->blameCount) {
            lua_newtable(L);
            for (int k = 0; k < l->blameCount; k++) lua_pushinteger(L, l->blame[k]), lua_seti(L, -2, k + 1);
            lua_setfield(L, -2, "blames");
        }
        if (l->order) {
            lua_pushinteger(L, l->order), lua_setfield(L, -2, "order");
            lua_pushinteger(L, l->orderOf), lua_setfield(L, -2, "orderOf");
        }
        if (l->copyCount) {
            lua_newtable(L);
            for (int k = 0; k < l->copyCount; k++) {
                lua_newtable(L);
                if (l->copies[k].y >= 0) {
                    lua_pushinteger(L, l->copies[k].x), lua_setfield(L, -2, "x");
                    lua_pushinteger(L, l->copies[k].y), lua_setfield(L, -2, "y");
                } else lua_pushstring(L, l->copies[k].why), lua_setfield(L, -2, "why");
                lua_seti(L, -2, k + 1);
            }
            lua_setfield(L, -2, "copies");
        }
        if (!strcmp(l->reason, "unknown_word")) {
            const char *best = NULL;
            int bestDistance = (int)strlen(l->word) / 3 + 2;
            for (size_t k = 0; k < sizeof PLANE_BLOCK_WORDS / sizeof PLANE_BLOCK_WORDS[0]; k++) {
                int distance = previewNameDistance(l->word, PLANE_BLOCK_WORDS[k]);
                if (distance < bestDistance) best = PLANE_BLOCK_WORDS[k], bestDistance = distance;
            }
            if (best) lua_pushstring(L, best), lua_setfield(L, -2, "close");
        }
        if (!strcmp(l->reason, "no_such_monster") || !strcmp(l->reason, "unknown_terrain")) {
            char close[48];
            if (l->reason[0] == 'n') previewCloseFromLine(l, monsterRecord(0), MONSTER_SIZE, tableRecords(TABLE_MONSTER), close, sizeof close);
            else previewCloseFromLine(l, GAME_PTR(RVA_TERRAIN_TABLE), TERRAIN_SIZE, G32(RVA_LAST_TERRAIN) + 1, close, sizeof close);
            if (close[0]) lua_pushstring(L, close), lua_setfield(L, -2, "close");
        }
        if (built && !strcmp(l->word, "planeunits") && p->layout.units.count && p->populate >= 0) {
            lua_pushstring(L, VANILLA_PLANE_NAMES[p->populate]), lua_setfield(L, -2, "replaces");
        }
        if (built && l->from >= 0 && (!strcmp(l->word, "planeswap") || !strcmp(l->word, "planezoneswap"))) {
            int x0 = planeField(p->index, PLANE_X), y0 = planeField(p->index, PLANE_Y), other = -1, left = 0;
            const unsigned char *fromRecord = GAME_PTR(RVA_TERRAIN_TABLE) + (size_t)l->from * TERRAIN_SIZE;
            for (int x = x0; x < x0 + p->w; x++)
                for (int y = y0; y < y0 + p->h; y++) {
                    short t = *(short *)(squareRecordAt(x, y) + SQUARE_TERRAIN_TYPE);
                    if (t == l->from || t < 0 || t >= TERRAIN_TABLE_MAX) continue;
                    if (recordNameIs(GAME_PTR(RVA_TERRAIN_TABLE) + (size_t)t * TERRAIN_SIZE, TERRAIN_NAME_SIZE, (const char *)fromRecord)) other = t, left++;
                }
            if (other >= 0) {
                lua_pushinteger(L, other), lua_setfield(L, -2, "sameNameTerrain");
                lua_pushinteger(L, left), lua_setfield(L, -2, "sameNameLeft");
            }
        }
        int alive = 0;
        for (int k = 0; k < unitPairCount; k++) alive += unitPairs[k].row == i;
        lua_pushinteger(L, alive), lua_setfield(L, -2, "units");
        previewPushRowSquares(L, unitPairs, unitPairCount, i, "armies", "units");
        previewPushRowSquares(L, writePairs, writePairCount, i, "squares", NULL);
        if (built && !strcmp(l->word, "planepiece")) {
            lua_newtable(L);
            int rects = 0;
            for (int k = 0; k < p->layout.placedCount; k++) {
                const PlacedPiece *r = &p->layout.placed[k];
                if (r->piece < 0 || p->layout.pieces[r->piece].line != l->line) continue;
                lua_newtable(L);
                lua_pushinteger(L, r->x), lua_setfield(L, -2, "x");
                lua_pushinteger(L, r->y), lua_setfield(L, -2, "y");
                lua_pushinteger(L, r->w), lua_setfield(L, -2, "w");
                lua_pushinteger(L, r->h), lua_setfield(L, -2, "h");
                lua_seti(L, -2, ++rects);
            }
            lua_setfield(L, -2, "rects");
        }
        if (built && l->zone[0] && (!strcmp(l->word, "planezone") || !strcmp(l->word, "planezonepopulate"))) {
            int zone = -1;
            for (int z = 0; z < p->layout.zoneCount; z++)
                if (!strcmp(p->layout.zones[z].name, l->zone)) zone = z;
            int x0 = planeField(p->index, PLANE_X), y0 = planeField(p->index, PLANE_Y), squares = 0;
            lua_newtable(L);
            for (int x = x0; zone >= 0 && x < x0 + p->w; x++)
                for (int y = y0; y < y0 + p->h; y++) {
                    if (planeZoneAt(&p->layout, x, y) != zone) continue;
                    lua_newtable(L);
                    lua_pushinteger(L, x), lua_setfield(L, -2, "x");
                    lua_pushinteger(L, y), lua_setfield(L, -2, "y");
                    lua_seti(L, -2, ++squares);
                }
            lua_setfield(L, -2, "squares");
            if (!strcmp(l->word, "planezonepopulate")) lua_pushinteger(L, squares), lua_setfield(L, -2, "did");
        }
        lua_seti(L, -2, ++out);
    }
    free(unitPairs);
    free(writePairs);
    return 1;
}

static int previewReadSquareArgs(lua_State *L, const char *call, int *x, int *y) {
    lua_Integer ax = luaL_checkinteger(L, 1), ay = luaL_checkinteger(L, 2);
    const char *reason = !previewWorld.active ? "no_preview_world" : ax < 0 || ay < 0 || ax >= MAP_COLUMNS_MAX || ay >= MAP_COLUMN_SQUARES ? "bad_square" : NULL;
    if (reason) {
        refuseLuaCall(runningLuaMod(), call, reason, "x=%lld y=%lld", (long long)ax, (long long)ay);
        return 0;
    }
    *x = (int)ax, *y = (int)ay;
    return 1;
}

static int previewSameChanges(int a, int b) {
    int ca = previewUnitLastChange ? previewUnitLastChange[a] : -1, cb = previewUnitLastChange ? previewUnitLastChange[b] : -1;
    while (ca >= 0 && cb >= 0) {
        const PreviewUnitChange *x = &previewUnitChanges[ca], *y = &previewUnitChanges[cb];
        if (x->action != y->action || x->from != y->from) return 0;
        ca = x->prev, cb = y->prev;
    }
    return ca < 0 && cb < 0;
}

static UNUSED int lPreviewArmySource(lua_State *L) {
    int x, y;
    if (!previewReadSquareArgs(L, "coe5.previewArmySource", &x, &y)) {
        lua_pushnil(L);
        return 1;
    }
    struct { int source, monster, units, unit; } found[PREVIEW_ARMY_SOURCES];
    int count = 0, last = G32(RVA_UNIT_COUNT);
    for (int u = 0; previewUnitSources && u <= last && u < UNIT_TABLE_MAX; u++) {
        if (G16(RVA_UNIT_HP + (size_t)u * UNIT_SIZE) <= 0 || G32(RVA_UNIT_X + (size_t)u * UNIT_SIZE) != x || G32(RVA_UNIT_X + 4 + (size_t)u * UNIT_SIZE) != y) continue;
        int source = previewUnitSources[u], monster = G16(RVA_UNIT_MONSTER + (size_t)u * UNIT_SIZE), at = -1;
        for (int i = 0; i < count && at < 0; i++)
            if (found[i].source == source && found[i].monster == monster && previewSameChanges(found[i].unit, u)) at = i;
        if (at < 0 && count < PREVIEW_ARMY_SOURCES) {
            at = count++;
            found[at].source = source, found[at].monster = monster, found[at].units = 0, found[at].unit = u;
        }
        if (at >= 0) found[at].units++;
    }
    lua_newtable(L);
    for (int i = 0; i < count; i++) {
        lua_newtable(L);
        lua_pushinteger(L, found[i].monster), lua_setfield(L, -2, "monster");
        lua_pushinteger(L, found[i].units), lua_setfield(L, -2, "units");
        previewPushSource(L, found[i].source);
        int chain[32], links = 0;
        for (int c = previewUnitLastChange ? previewUnitLastChange[found[i].unit] : -1; c >= 0 && links < 32; c = previewUnitChanges[c].prev) chain[links++] = c;
        if (links) {
            lua_pushinteger(L, previewUnitChanges[chain[links - 1]].from), lua_setfield(L, -2, "placedAs");
            lua_newtable(L);
            for (int k = links - 1, out = 0; k >= 0; k--) {
                const PreviewUnitChange *c = &previewUnitChanges[chain[k]];
                if (c->kind == PREVIEW_CHANGE_NEW) continue;
                lua_newtable(L);
                previewPushSource(L, PREVIEW_SOURCE_EVENT_ACTION - c->action);
                lua_pushinteger(L, c->from), lua_setfield(L, -2, "from");
                lua_pushinteger(L, c->to), lua_setfield(L, -2, "to");
                lua_seti(L, -2, ++out);
            }
            lua_setfield(L, -2, "changes");
        }
        lua_seti(L, -2, i + 1);
    }
    return 1;
}

static UNUSED int lPreviewSquareSource(lua_State *L) {
    int x, y;
    if (!previewReadSquareArgs(L, "coe5.previewSquareSource", &x, &y)) {
        lua_pushnil(L);
        return 1;
    }
    int square = x * MAP_COLUMN_SQUARES + y, out = 0;
    lua_newtable(L);
    for (int i = 0; i < previewTerrainWriteCount; i++) {
        if (previewTerrainWrites[i].square != square) continue;
        lua_newtable(L);
        lua_pushinteger(L, previewTerrainWrites[i].terrain), lua_setfield(L, -2, "terrain");
        previewPushSource(L, previewTerrainWrites[i].source);
        lua_seti(L, -2, ++out);
    }
    return 1;
}

typedef struct { int units, left, later, killed, squares, squaresLeft; } PreviewActionTally;

static int comparePreviewActionOrder(const void *a, const void *b) {
    const PreviewEventAction *x = &previewEventActions[*(const int *)a], *y = &previewEventActions[*(const int *)b];
    return x->row != y->row ? x->row - y->row : x->line - y->line;
}

static int previewTallyEventActions(int plane, PreviewActionTally *tally, PreviewRowSquare **armies, int *armyCount, PreviewRowSquare **squares, int *squareCount) {
    int last = G32(RVA_UNIT_COUNT), kills = 0, writes = 0;
    for (int i = 0; i < previewUnitChangeCount; i++) {
        const PreviewUnitChange *c = &previewUnitChanges[i];
        if (c->plane != plane) continue;
        if (c->kind == PREVIEW_CHANGE_KILLED) tally[c->action].killed++, kills++;
        else if (c->prev < 0 || previewUnitChanges[c->prev].action != c->action) tally[c->action].units++;
    }
    for (int i = 0; i < previewTerrainWriteCount; i++) writes += previewActionOfSource(previewTerrainWrites[i].source) >= 0;
    int *lastAt = malloc(sizeof *lastAt * MAP_COLUMNS_MAX * MAP_COLUMN_SQUARES);
    PreviewRowSquare *written = malloc(sizeof *written * (size_t)(writes + 1));
    PreviewRowSquare *changed = malloc(sizeof *changed * (size_t)(writes + kills + 1));
    PreviewRowSquare *stand = malloc(sizeof *stand * (size_t)(last + 2) * 4);
    if (!lastAt || !written || !changed || !stand) {
        free(lastAt);
        free(written);
        free(changed);
        free(stand);
        writeLogError("preview_sources_short", "reason=out_of_memory");
        return 0;
    }
    int standCount = 0, standMost = (last + 2) * 4, writtenCount = 0, changedCount = 0;
    for (int u = 0; u <= last && u < UNIT_TABLE_MAX; u++) {
        int change = previewUnitLastChange ? previewUnitLastChange[u] : -1;
        if (change < 0 || (short)G16(RVA_UNIT_HP + (size_t)u * UNIT_SIZE) <= 0) continue;
        int x = (int)G32(RVA_UNIT_X + (size_t)u * UNIT_SIZE), y = (int)G32(RVA_UNIT_X + 4 + (size_t)u * UNIT_SIZE);
        int here = previewPlaneOfSquare(x, y) == plane, inLast = 1, lastAction = previewUnitChanges[change].action;
        for (int c = change; c >= 0; c = previewUnitChanges[c].prev) {
            const PreviewUnitChange *k = &previewUnitChanges[c];
            inLast = inLast && k->action == lastAction;
            if (k->plane != plane || k->kind == PREVIEW_CHANGE_KILLED) continue;
            if (k->prev >= 0 && previewUnitChanges[k->prev].action == k->action) continue;
            if (!(here && inLast)) tally[k->action].later++;
        }
        if (!here) continue;
        tally[lastAction].left++;
        for (int c = change; c >= 0 && standCount < standMost; c = previewUnitChanges[c].prev) stand[standCount++] = (PreviewRowSquare){previewEventActions[previewUnitChanges[c].action].row, x * MAP_COLUMN_SQUARES + y};
    }
    memset(lastAt, 0xff, sizeof *lastAt * MAP_COLUMNS_MAX * MAP_COLUMN_SQUARES);
    for (int i = 0; i < previewTerrainWriteCount; i++) lastAt[previewTerrainWrites[i].square] = i;
    for (int i = 0; i < previewTerrainWriteCount; i++) {
        int action = previewActionOfSource(previewTerrainWrites[i].source), square = previewTerrainWrites[i].square;
        if (action < 0 || previewPlaneOfSquare(square / MAP_COLUMN_SQUARES, square % MAP_COLUMN_SQUARES) != plane) continue;
        written[writtenCount++] = (PreviewRowSquare){action, square};
        changed[changedCount++] = (PreviewRowSquare){previewEventActions[action].row, square};
        if (lastAt[square] == i) tally[action].squaresLeft++;
    }
    for (int i = 0; i < previewUnitChangeCount; i++) {
        const PreviewUnitChange *c = &previewUnitChanges[i];
        if (c->plane == plane && c->kind == PREVIEW_CHANGE_KILLED) changed[changedCount++] = (PreviewRowSquare){previewEventActions[c->action].row, c->x * MAP_COLUMN_SQUARES + c->y};
    }
    qsort(written, (size_t)writtenCount, sizeof *written, comparePreviewRowSquares);
    for (int i = 0; i < writtenCount; i++)
        if (!i || written[i].row != written[i - 1].row || written[i].square != written[i - 1].square) tally[written[i].row].squares++;
    qsort(changed, (size_t)changedCount, sizeof *changed, comparePreviewRowSquares);
    qsort(stand, (size_t)standCount, sizeof *stand, comparePreviewRowSquares);
    free(lastAt);
    free(written);
    *armies = stand, *armyCount = standCount, *squares = changed, *squareCount = changedCount;
    return 1;
}

static UNUSED int lPreviewEventResults(lua_State *L) {
    int plane = (int)luaL_optinteger(L, 1, -1);
    if (!previewWorld.active) {
        refuseLuaCall(runningLuaMod(), "coe5.previewEventResults", "no_preview_world", "rows=%d", previewEventRowCount);
        lua_pushnil(L);
        return 1;
    }
    PreviewActionTally *tally = plane >= 0 ? calloc((size_t)previewEventActionCount + 1, sizeof *tally) : NULL;
    int *order = plane >= 0 ? malloc(sizeof *order * ((size_t)previewEventActionCount + 1)) : NULL;
    PreviewRowSquare *armies = NULL, *squares = NULL;
    int armyCount = 0, squareCount = 0, known = tally && order && previewTallyEventActions(plane, tally, &armies, &armyCount, &squares, &squareCount), nextAction = 0;
    for (int i = 0; known && i < previewEventActionCount; i++) order[i] = i;
    if (known) qsort(order, (size_t)previewEventActionCount, sizeof *order, comparePreviewActionOrder);
    lua_createtable(L, previewEventRowCount, 0);
    for (int i = 0; i < previewEventRowCount; i++) {
        const PreviewEventRow *r = &previewEventRows[i];
        lua_createtable(L, 0, 16);
        lua_pushstring(L, r->mod), lua_setfield(L, -2, "mod");
        lua_pushstring(L, r->file), lua_setfield(L, -2, "file");
        lua_pushstring(L, r->text), lua_setfield(L, -2, "text");
        lua_pushstring(L, r->actions), lua_setfield(L, -2, "actions");
        lua_pushstring(L, r->turns), lua_setfield(L, -2, "turns");
        lua_pushstring(L, r->held), lua_setfield(L, -2, "held");
        lua_newtable(L);
        for (int p = 0, n = 0; p < 64; p++) {
            if (!(r->named >> p & 1)) continue;
            lua_pushinteger(L, p);
            lua_rawseti(L, -2, ++n);
        }
        lua_setfield(L, -2, "named");
        lua_pushinteger(L, r->line), lua_setfield(L, -2, "line");
        lua_pushinteger(L, r->passes), lua_setfield(L, -2, "passes");
        lua_pushinteger(L, r->ran), lua_setfield(L, -2, "ran");
        lua_pushinteger(L, r->stopped), lua_setfield(L, -2, "stopped");
        if (r->stopped) {
            lua_pushinteger(L, r->stopLine), lua_setfield(L, -2, "stopLine");
            lua_pushstring(L, r->stopText), lua_setfield(L, -2, "stopText");
        }
        lua_newtable(L);
        for (int p = 0, n = 0; p < 64; p++) {
            if (!(r->planes >> p & 1)) continue;
            lua_pushinteger(L, p);
            lua_seti(L, -2, ++n);
        }
        lua_setfield(L, -2, "planes");
        lua_newtable(L);
        for (int p = 0, n = 0; p < 64; p++) {
            if (!(r->changed >> p & 1)) continue;
            lua_pushinteger(L, p);
            lua_seti(L, -2, ++n);
        }
        lua_setfield(L, -2, "changed");
        if (known) {
            lua_newtable(L);
            for (int n = 0; nextAction < previewEventActionCount && previewEventActions[order[nextAction]].row == i; nextAction++) {
                const PreviewEventAction *a = &previewEventActions[order[nextAction]];
                const PreviewActionTally *t = &tally[order[nextAction]];
                if (!t->units && !t->killed && !t->squares) continue;
                lua_createtable(L, 0, 7);
                lua_pushinteger(L, a->line), lua_setfield(L, -2, "line");
                lua_pushstring(L, a->text), lua_setfield(L, -2, "text");
                lua_pushinteger(L, t->units), lua_setfield(L, -2, "units");
                lua_pushinteger(L, t->left), lua_setfield(L, -2, "left");
                lua_pushinteger(L, t->later), lua_setfield(L, -2, "later");
                lua_pushinteger(L, t->killed), lua_setfield(L, -2, "killed");
                lua_pushinteger(L, t->squares), lua_setfield(L, -2, "squares");
                lua_pushinteger(L, t->squaresLeft), lua_setfield(L, -2, "squaresLeft");
                lua_seti(L, -2, ++n);
            }
            lua_setfield(L, -2, "did");
            previewPushRowSquares(L, armies, armyCount, i, "armies", "units");
            previewPushRowSquares(L, squares, squareCount, i, "squares", NULL);
        }
        lua_seti(L, -2, i + 1);
    }
    free(tally);
    free(order);
    free(armies);
    free(squares);
    return 1;
}

static UNUSED int lPreviewScriptResults(lua_State *L) {
    if (!previewWorld.active) {
        refuseLuaCall(runningLuaMod(), "coe5.previewScriptResults", "no_preview_world", "rows=%d", previewScriptRowCount);
        lua_pushnil(L);
        return 1;
    }
    static const char *const KINDS[] = {"call", "refused", "error", "wrap"};
    lua_createtable(L, previewScriptRowCount, 0);
    for (int i = 0; i < previewScriptRowCount; i++) {
        const PreviewScriptRow *r = &previewScriptRows[i];
        lua_createtable(L, 0, 8);
        lua_pushstring(L, r->mod), lua_setfield(L, -2, "mod");
        lua_pushstring(L, r->file), lua_setfield(L, -2, "file");
        lua_pushstring(L, r->call), lua_setfield(L, -2, "call");
        lua_pushinteger(L, r->line), lua_setfield(L, -2, "line");
        lua_pushinteger(L, r->count), lua_setfield(L, -2, "count");
        lua_pushstring(L, KINDS[r->kind]), lua_setfield(L, -2, "kind");
        if (r->reason[0]) lua_pushstring(L, r->reason), lua_setfield(L, -2, "reason");
        if (r->text[0]) lua_pushstring(L, r->text), lua_setfield(L, -2, "text");
        lua_seti(L, -2, i + 1);
    }
    return 1;
}
