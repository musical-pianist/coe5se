#pragma once

#define CORPSE_MASK_HUMANOID       0xc7
#define CORPSE_MASK_ANIMAL         0x38
#define CORPSE_MASK_ALL            0xff
#define CORPSE_COUNT_MAX           32767
#define CORPSE_KIND_LEAVES_NONE    SQUARE_CORPSE_KINDS
#define RITUAL_FX_RAISE_DEAD       915
#define RITUAL_FX_RAISE_ANIMALS    967
#define RITUAL_FX_PUT_CORPSES      1070
#define PUT_CORPSES_SCALED         10000
#define RAISE_SUMMON_CONTROL       0x3e5
#define RAISE_SUMMON_FAIL_PLAYER   0x18
#define ABILITY_VOIDSANITY         99
#define INSANITY_HALVED            (-9998)
#define INSANITY_MAX               100
#define CONTROLLER_REMOTE_HUMAN    5
#define CORPSE_CASTS_MAX           4
#define CUSTOM_CORPSE_KINDS_MAX    200
#define CUSTOM_CORPSE_NAME_SIZE    64
#define CORPSE_BASE_BYPASS         (-1)
#define CORPSE_STORED_CUSTOM       (CORPSE_KIND_LEAVES_NONE + 2)

static const struct {
    const char *name;
    int mask;
} CORPSE_KIND_WORDS[] = {
    {"small", 1 << CORPSE_KIND_SMALL},           {"human", 1 << CORPSE_KIND_HUMAN},     {"giant", 1 << CORPSE_KIND_GIANT},
    {"smallanimal", 1 << CORPSE_KIND_SMALL_ANIMAL}, {"largeanimal", 1 << CORPSE_KIND_LARGE_ANIMAL},
    {"markata", 1 << CORPSE_KIND_MARKATA},       {"vanara", 1 << CORPSE_KIND_VANARA},   {"bandar", 1 << CORPSE_KIND_BANDAR},
    {"humanoid", CORPSE_MASK_HUMANOID},          {"animal", CORPSE_MASK_ANIMAL},        {"all", CORPSE_MASK_ALL},
};

enum { CORPSE_RULE_PUT, CORPSE_RULE_RAISE_DEAD, CORPSE_RULE_RAISE_ANIMALS, CORPSE_RULE_RAISE_CORPSES, CORPSE_RULE_DROPPED };

typedef struct {
    int ritual, type, occurrence, mask, custom, line;
    char mod[64], monster[MONSTER_NAME_SIZE];
} CorpseRule;

static CorpseRule *corpseRules;
static int corpseRuleCount, corpseRuleCapacity;
static unsigned char *monsterCorpseKinds;
static int monsterCorpseKindCount;

typedef struct {
    int ritual, unit, human, percent, puts, raiseDeads, raiseAnimals;
} CorpseCast;

static CorpseCast corpseCasts[CORPSE_CASTS_MAX];
static int corpseCastDepth;

static int readCorpseKinds(const char **p, int *mask) {
    size_t len = strcspn(*p, " \t\r\n");
    for (size_t i = 0; i < sizeof CORPSE_KIND_WORDS / sizeof CORPSE_KIND_WORDS[0]; i++) {
        if (strlen(CORPSE_KIND_WORDS[i].name) != len || strncmp(CORPSE_KIND_WORDS[i].name, *p, len)) continue;
        *mask = CORPSE_KIND_WORDS[i].mask;
        *p += len;
        return 1;
    }
    return 0;
}

static int onlyCorpseKind(int mask) {
    for (int k = 0; k < SQUARE_CORPSE_KINDS; k++)
        if (mask == 1 << k) return k;
    return -1;
}

static short *squareCorpses(int x, int y) {
    if (x < 0 || y < 0 || x >= G32(RVA_MAP_DEPTH) || y >= G32(RVA_MAP_H) || x >= MAP_COLUMNS_MAX || y >= MAP_COLUMN_SQUARES) return NULL;
    return (short *)(gameBase + RVA_SQUARE_RECORDS + ((size_t)x * MAP_COLUMN_SQUARES + (size_t)y) * SQUARE_SIZE + SQUARE_CORPSES);
}

static int countCorpses(const short *corpses, int mask) {
    int count = 0;
    for (int k = 0; k < SQUARE_CORPSE_KINDS; k++)
        if (mask & 1 << k) count += corpses[k];
    return count;
}

static void addCorpses(short *corpses, int kind, int n) {
    int total = corpses[kind] + n;
    corpses[kind] = (short)(total > CORPSE_COUNT_MAX ? CORPSE_COUNT_MAX : total);
}

static int takeCorpses(short *corpses, int mask, int n) {
    int taken = 0;
    for (int round = 0; round < n && taken < n; round++)
        for (int k = 0; k < SQUARE_CORPSE_KINDS && taken < n; k++)
            if (mask & 1 << k && corpses[k] > 0) corpses[k]--, taken++;
    return taken;
}

// ---- Custom kinds: newcorpsekind, and CoE5SE's own counts per square --------------------------------
typedef struct {
    char name[CUSTOM_CORPSE_NAME_SIZE];
    int base;
    int declared;
    char mod[64];
} CustomCorpseKind;

static CustomCorpseKind customCorpseKinds[CUSTOM_CORPSE_KINDS_MAX];
static int customCorpseKindCount;

typedef struct {
    int key;
    short count;
} CustomCorpseCell;

static CustomCorpseCell *customCorpseCells;
static int customCorpseCellCapacity, customCorpseCellUsed;

static unsigned char *changedCorpseSquareBits;
static int *changedCorpseSquares, changedCorpseSquareCount, changedCorpseSquareCapacity;

static void noteCustomCorpseSquare(int square) {
    int squares = MAP_COLUMNS_MAX * MAP_COLUMN_SQUARES;
    if (square < 0 || square >= squares) return;
    if (!changedCorpseSquareBits && !(changedCorpseSquareBits = calloc((size_t)squares / 8 + 1, 1))) return;
    if (changedCorpseSquareBits[square / 8] & 1 << square % 8) return;
    if (!growList((void **)&changedCorpseSquares, &changedCorpseSquareCapacity, changedCorpseSquareCount + 1, sizeof *changedCorpseSquares)) return;
    changedCorpseSquareBits[square / 8] |= (unsigned char)(1 << square % 8);
    changedCorpseSquares[changedCorpseSquareCount++] = square;
}

static void forgetChangedCorpseSquares(void) {
    for (int i = 0; i < changedCorpseSquareCount; i++) changedCorpseSquareBits[changedCorpseSquares[i] / 8] = 0;
    changedCorpseSquareCount = 0;
}

static int findCustomCorpseKind(const char *name) {
    for (int i = 0; i < customCorpseKindCount; i++)
        if (!strcasecmp(customCorpseKinds[i].name, name)) return i;
    return -1;
}

static int addCustomCorpseKind(const char *name, int base) {
    int k = findCustomCorpseKind(name);
    if (k >= 0 || customCorpseKindCount >= CUSTOM_CORPSE_KINDS_MAX) return k;
    CustomCorpseKind *kind = &customCorpseKinds[customCorpseKindCount];
    memset(kind, 0, sizeof *kind);
    snprintf(kind->name, sizeof kind->name, "%s", name);
    kind->base = base;
    return customCorpseKindCount++;
}

static int customCorpseKindDeclared(int k) {
    return k >= 0 && k < customCorpseKindCount && customCorpseKinds[k].declared;
}

static int customCorpseKey(int square, int k) {
    return square * CUSTOM_CORPSE_KINDS_MAX + k + 1;
}

static CustomCorpseCell *customCorpseCell(int square, int k, int make) {
    int key = customCorpseKey(square, k);
    if (make && (customCorpseCellUsed + 1) * 10 > customCorpseCellCapacity * 7) {
        int capacity = customCorpseCellCapacity ? customCorpseCellCapacity * 2 : 256;
        CustomCorpseCell *grown = calloc((size_t)capacity, sizeof *grown);
        if (!grown) {
            writeLogError("custom_corpses_dropped", "reason=out_of_memory cells=%d", customCorpseCellUsed);
            return NULL;
        }
        for (int i = 0; i < customCorpseCellCapacity; i++) {
            if (!customCorpseCells[i].key) continue;
            unsigned at = (unsigned)customCorpseCells[i].key * 2654435761u & (unsigned)(capacity - 1);
            while (grown[at].key) at = (at + 1) & (unsigned)(capacity - 1);
            grown[at] = customCorpseCells[i];
        }
        free(customCorpseCells);
        customCorpseCells = grown;
        customCorpseCellCapacity = capacity;
    }
    if (!customCorpseCellCapacity) return NULL;
    unsigned mask = (unsigned)(customCorpseCellCapacity - 1), at = (unsigned)key * 2654435761u & mask;
    while (customCorpseCells[at].key && customCorpseCells[at].key != key) at = (at + 1) & mask;
    if (customCorpseCells[at].key == key) return &customCorpseCells[at];
    if (!make) return NULL;
    customCorpseCells[at].key = key;
    customCorpseCells[at].count = 0;
    customCorpseCellUsed++;
    return &customCorpseCells[at];
}

static void clearCustomCorpses(void) {
    forgetChangedCorpseSquares();
    free(customCorpseCells);
    customCorpseCells = NULL;
    customCorpseCellCapacity = customCorpseCellUsed = 0;
}

static int customCorpseCount(int square, int k) {
    const CustomCorpseCell *cell = customCorpseCell(square, k, 0);
    return cell ? cell->count : 0;
}

static int corpseSquareIndex(const short *corpses) {
    return (int)(((const unsigned char *)corpses - (gameBase + RVA_SQUARE_RECORDS + SQUARE_CORPSES)) / SQUARE_SIZE);
}

static void fitCustomCorpses(const short *corpses) {
    if (!customCorpseKindCount || !customCorpseCellUsed || !corpses) return;
    int square = corpseSquareIndex(corpses);
    for (int base = 0; base < SQUARE_CORPSE_KINDS; base++) {
        int held = 0;
        for (int k = 0; k < customCorpseKindCount; k++)
            if (customCorpseKinds[k].base == base) held += customCorpseCount(square, k);
        int excess = held - corpses[base];
        while (excess > 0) {
            for (int k = 0; k < customCorpseKindCount && excess > 0; k++) {
                if (customCorpseKinds[k].base != base) continue;
                CustomCorpseCell *cell = customCorpseCell(square, k, 0);
                if (cell && cell->count > 0) cell->count--, excess--, noteCustomCorpseSquare(square);
            }
        }
    }
}

static void fitAllCustomCorpses(void) {
    for (int i = 0; i < customCorpseCellCapacity; i++) {
        if (!customCorpseCells[i].key || !customCorpseCells[i].count) continue;
        int square = (customCorpseCells[i].key - 1) / CUSTOM_CORPSE_KINDS_MAX;
        fitCustomCorpses((const short *)(gameBase + RVA_SQUARE_RECORDS + (size_t)square * SQUARE_SIZE + SQUARE_CORPSES));
    }
}

static int addCustomCorpses(short *corpses, int k, int n) {
    if (n <= 0) return 0;
    CustomCorpseCell *cell = customCorpseCell(corpseSquareIndex(corpses), k, 1);
    if (!cell) return 0;
    int add = n > CORPSE_COUNT_MAX - cell->count ? CORPSE_COUNT_MAX - cell->count : n;
    int base = customCorpseKinds[k].base;
    if (base >= 0) {
        int before = corpses[base];
        addCorpses(corpses, base, add);
        add = corpses[base] - before;
    }
    cell->count = (short)(cell->count + add);
    if (add) noteCustomCorpseSquare(corpseSquareIndex(corpses));
    return add;
}

static int takeCustomCorpses(short *corpses, int k, int n) {
    CustomCorpseCell *cell = customCorpseCell(corpseSquareIndex(corpses), k, 0);
    int take = !cell || n <= 0 ? 0 : n < cell->count ? n : cell->count;
    if (!take) return 0;
    cell->count = (short)(cell->count - take);
    noteCustomCorpseSquare(corpseSquareIndex(corpses));
    int base = customCorpseKinds[k].base;
    if (base >= 0) corpses[base] = (short)(corpses[base] - take);
    return take;
}

static int countSelectedCorpses(const short *corpses, int mask, int custom) {
    fitCustomCorpses(corpses);
    return custom >= 0 ? customCorpseCount(corpseSquareIndex(corpses), custom) : countCorpses(corpses, mask);
}

static int takeSelectedCorpses(short *corpses, int mask, int custom, int n) {
    fitCustomCorpses(corpses);
    if (custom >= 0) return takeCustomCorpses(corpses, custom, n);
    int taken = takeCorpses(corpses, mask, n);
    fitCustomCorpses(corpses);
    return taken;
}

static void moveCustomCorpseBase(int k, int base) {
    int old = customCorpseKinds[k].base;
    customCorpseKinds[k].base = base;
    if (old == base) return;
    for (int i = 0; i < customCorpseCellCapacity; i++) {
        CustomCorpseCell *cell = &customCorpseCells[i];
        if (!cell->key || (cell->key - 1) % CUSTOM_CORPSE_KINDS_MAX != k || !cell->count) continue;
        short *corpses = (short *)(gameBase + RVA_SQUARE_RECORDS + (size_t)((cell->key - 1) / CUSTOM_CORPSE_KINDS_MAX) * SQUARE_SIZE + SQUARE_CORPSES);
        int n = cell->count;
        if (old >= 0) n = n < corpses[old] ? n : corpses[old], corpses[old] = (short)(corpses[old] - n);
        if (base >= 0) {
            int before = corpses[base];
            addCorpses(corpses, base, n);
            n = corpses[base] - before;
        }
        cell->count = (short)n;
    }
}

static int isCorpseKindWord(const char *name) {
    for (size_t i = 0; i < sizeof CORPSE_KIND_WORDS / sizeof CORPSE_KIND_WORDS[0]; i++)
        if (!strcasecmp(CORPSE_KIND_WORDS[i].name, name)) return 1;
    return 0;
}

static char unknownCorpseKindName[CUSTOM_CORPSE_NAME_SIZE];

static int readCorpseKind(const char **p, int *mask, int *custom) {
    *custom = -1;
    if (**p != '"') return readCorpseKinds(p, mask);
    const char *end = strchr(*p + 1, '"');
    size_t n = end ? (size_t)(end - *p - 1) : 0;
    if (!end || !n || n >= CUSTOM_CORPSE_NAME_SIZE) return 0;
    memcpy(unknownCorpseKindName, *p + 1, n), unknownCorpseKindName[n] = '\0';
    int k = findCustomCorpseKind(unknownCorpseKindName);
    if (!customCorpseKindDeclared(k)) return -1;
    *mask = 0, *custom = k, *p = end + 1;
    return 1;
}

static int ritualEffectCount(int ritual, int id) {
    unsigned char *record = ritualWordRecord(ritual);
    int count = 0;
    for (int s = 0; s < RITUAL_FX_MAX && ritualWordEffect(record, s)[0]; s++) count += ritualWordEffect(record, s)[0] == id;
    return count;
}

static long long ritualEffectValueAt(int ritual, int id, int nth) {
    unsigned char *record = ritualWordRecord(ritual);
    for (int s = 0; s < RITUAL_FX_MAX && ritualWordEffect(record, s)[0]; s++)
        if (ritualWordEffect(record, s)[0] == id && nth-- == 0) return ritualWordEffect(record, s)[1];
    return 0;
}

static const CorpseRule *corpseRuleFor(int ritual, int type, int occurrence) {
    for (int i = 0; i < corpseRuleCount; i++)
        if (corpseRules[i].ritual == ritual && corpseRules[i].type == type && corpseRules[i].occurrence == occurrence) return &corpseRules[i];
    return NULL;
}

static int addCorpseRule(const char *word, int mod, int type, int occurrence, int mask, int custom, const char *monster) {
    if (!growList((void **)&corpseRules, &corpseRuleCapacity, corpseRuleCount + 1, sizeof corpseRules[0])) {
        refuseC5mLine(word, spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
        return 0;
    }
    CorpseRule *r = &corpseRules[corpseRuleCount++];
    memset(r, 0, sizeof *r);
    r->ritual = G32(RVA_RITUAL_CURRENT);
    r->type = type;
    r->occurrence = occurrence;
    r->mask = mask;
    r->custom = custom;
    r->line = G32(RVA_MOD_LINE);
    snprintf(r->mod, sizeof r->mod, "%s", spellModName(mod));
    if (monster) snprintf(r->monster, sizeof r->monster, "%s", monster);
    return 1;
}

static int currentCorpseRitual(const char *word, int mod) {
    int record = G32(RVA_RITUAL_CURRENT);
    if (record >= 0 && record < ritualSlotCount()) return record;
    refuseC5mLine(word, spellModName(mod), "no_ritual", "line=%d", G32(RVA_MOD_LINE));
    return -1;
}

static const char *corpseWordLineHook(const char *line, size_t len, int mod, int level) {
    if (level != 2) return line;
    static char rewritten[64];
    int type;
    const char *word;
    if (len == 10 && !strncmp(line, "putcorpses", len)) type = CORPSE_RULE_PUT, word = "putcorpses";
    else if (len == 9 && !strncmp(line, "raisedead", len)) type = CORPSE_RULE_RAISE_DEAD, word = "raisedead";
    else if (len == 12 && !strncmp(line, "raiseanimals", len)) type = CORPSE_RULE_RAISE_ANIMALS, word = "raiseanimals";
    else return line;
    const char *p = line + len;
    long long n = 0;
    skipSpaces(&p);
    if (type == CORPSE_RULE_PUT && !readSpellNumber(&p, &n)) return line;
    skipSpaces(&p);
    if (isLineEnd(p)) return line;
    int mask = 0, custom = -1, read = readCorpseKind(&p, &mask, &custom);
    if (read < 0) {
        refuseC5mLine(word, spellModName(mod), "unknown_corpse_kind", "line=%d name=%.63s", G32(RVA_MOD_LINE), unknownCorpseKindName);
        return NULL;
    }
    if (!read || (skipSpaces(&p), !isLineEnd(p))) {
        refuseC5mLine(word, spellModName(mod), "bad_kind", "line=%d text=%.40s", G32(RVA_MOD_LINE), line + len);
        return NULL;
    }
    if (custom >= 0 && type != CORPSE_RULE_PUT) {
        refuseC5mLine(word, spellModName(mod), "custom_kind", "line=%d name=%.63s", G32(RVA_MOD_LINE), customCorpseKinds[custom].name);
        return NULL;
    }
    int allowed = type == CORPSE_RULE_RAISE_DEAD ? CORPSE_MASK_HUMANOID : type == CORPSE_RULE_RAISE_ANIMALS ? CORPSE_MASK_ANIMAL : CORPSE_MASK_ALL;
    if (mask & ~allowed) {
        refuseC5mLine(word, spellModName(mod), type == CORPSE_RULE_RAISE_DEAD ? "not_humanoid" : "not_animal", "line=%d text=%.40s",
                      G32(RVA_MOD_LINE), line + len);
        return NULL;
    }
    if (type == CORPSE_RULE_PUT && custom < 0 && onlyCorpseKind(mask) < 0) {
        refuseC5mLine(word, spellModName(mod), "not_one_kind", "line=%d text=%.40s", G32(RVA_MOD_LINE), line + len);
        return NULL;
    }
    if (type == CORPSE_RULE_PUT && n <= 0) {
        refuseC5mLine(word, spellModName(mod), "bad_value", "line=%d value=%lld min=1", G32(RVA_MOD_LINE), n);
        return NULL;
    }
    int ritual = currentCorpseRitual(word, mod);
    if (ritual < 0) return NULL;
    int id = type == CORPSE_RULE_PUT ? RITUAL_FX_PUT_CORPSES : type == CORPSE_RULE_RAISE_DEAD ? RITUAL_FX_RAISE_DEAD : RITUAL_FX_RAISE_ANIMALS;
    if (!addCorpseRule(word, mod, type, ritualEffectCount(ritual, id), mask, custom, NULL)) return NULL;
    if (type == CORPSE_RULE_PUT) snprintf(rewritten, sizeof rewritten, "putcorpses %lld", n);
    else snprintf(rewritten, sizeof rewritten, "%s", word);
    return rewritten;
}

static int copyCorpseRules(int source, int target) {
    int kept = 0;
    for (int i = 0; i < corpseRuleCount; i++)
        if (corpseRules[i].ritual != target) corpseRules[kept++] = corpseRules[i];
    corpseRuleCount = kept;
    for (int i = 0; i < kept; i++) {
        if (corpseRules[i].ritual != source) continue;
        if (!growList((void **)&corpseRules, &corpseRuleCapacity, corpseRuleCount + 1, sizeof corpseRules[0])) return 0;
        corpseRules[corpseRuleCount] = corpseRules[i];
        corpseRules[corpseRuleCount++].ritual = target;
    }
    return 1;
}

static int storeMonsterCorpseKind(int monster, int stored) {
    if (monster >= monsterCorpseKindCount) {
        if (!stored) return 1;
        int count = tableRecords(TABLE_MONSTER);
        unsigned char *grown = realloc(monsterCorpseKinds, (size_t)count);
        if (!grown) return 0;
        memset(grown + monsterCorpseKindCount, 0, (size_t)(count - monsterCorpseKindCount));
        monsterCorpseKinds = grown;
        monsterCorpseKindCount = count;
    }
    monsterCorpseKinds[monster] = (unsigned char)stored;
    return 1;
}

static int copyMonsterCorpseKind(int source, int target) {
    return storeMonsterCorpseKind(target, source < monsterCorpseKindCount ? monsterCorpseKinds[source] : 0);
}

static void corpsekindLine(const char *args, int mod, int level) {
    if (level != 2) return;
    const char *p = args;
    int mask = 0, custom = -1, read = 0, stored = -1;
    skipSpaces(&p);
    size_t len = strcspn(p, " \t\r\n");
    if (len == 1 && p[0] == '0') stored = 0, p += 1;
    else if (len == 4 && !strncmp(p, "none", 4)) stored = CORPSE_KIND_LEAVES_NONE + 1, p += 4;
    else if ((read = readCorpseKind(&p, &mask, &custom)) > 0)
        stored = custom >= 0 ? CORPSE_STORED_CUSTOM + custom : onlyCorpseKind(mask) >= 0 ? onlyCorpseKind(mask) + 1 : -1;
    skipSpaces(&p);
    if (read < 0) {
        refuseC5mLine("corpsekind", spellModName(mod), "unknown_corpse_kind", "line=%d name=%.63s", G32(RVA_MOD_LINE), unknownCorpseKindName);
        return;
    }
    if (stored < 0 || !isLineEnd(p)) {
        refuseC5mLine("corpsekind", spellModName(mod), "bad_kind", "line=%d text=%.40s", G32(RVA_MOD_LINE), args);
        return;
    }
    int monster = G32(RVA_MONSTER_CURRENT);
    if (monster < 0 || monster >= tableRecords(TABLE_MONSTER)) {
        refuseC5mLine("corpsekind", spellModName(mod), "no_monster", "line=%d", G32(RVA_MOD_LINE));
        return;
    }
    if (!storeMonsterCorpseKind(monster, stored)) refuseC5mLine("corpsekind", spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
}

static void raisecorpsesLine(const char *args, int mod, int level) {
    if (level != 2) return;
    const char *p = args;
    int mask = 0, custom = -1;
    char name[MONSTER_NAME_SIZE] = "";
    skipSpaces(&p);
    int ok = readCorpseKind(&p, &mask, &custom);
    if (ok < 0) {
        refuseC5mLine("raisecorpses", spellModName(mod), "unknown_corpse_kind", "line=%d name=%.63s", G32(RVA_MOD_LINE), unknownCorpseKindName);
        return;
    }
    skipSpaces(&p);
    if (ok && *p == '"') {
        const char *end = strchr(p + 1, '"');
        size_t n = end ? (size_t)(end - p - 1) : 0;
        ok = end && n > 0 && n < sizeof name;
        if (ok) memcpy(name, p + 1, n), name[n] = '\0', p = end + 1;
    } else ok = 0;
    skipSpaces(&p);
    if (!ok || !isLineEnd(p)) {
        refuseC5mLine("raisecorpses", spellModName(mod), "bad_value", "line=%d text=%.60s", G32(RVA_MOD_LINE), args);
        return;
    }
    if (currentCorpseRitual("raisecorpses", mod) < 0) return;
    addCorpseRule("raisecorpses", mod, CORPSE_RULE_RAISE_CORPSES, 0, mask, custom, name);
}

static void newcorpsekindLine(const char *args, int mod, int level) {
    if (level != 2) return;
    const char *p = args;
    char name[CUSTOM_CORPSE_NAME_SIZE] = "";
    skipSpaces(&p);
    const char *end = *p == '"' ? strchr(p + 1, '"') : NULL;
    size_t n = end ? (size_t)(end - p - 1) : 0;
    if (!end || !n) {
        refuseC5mLine("newcorpsekind", spellModName(mod), "no_name", "line=%d text=%.60s", G32(RVA_MOD_LINE), args);
        return;
    }
    if (n >= sizeof name) {
        refuseC5mLine("newcorpsekind", spellModName(mod), "name_too_long", "line=%d max=%d", G32(RVA_MOD_LINE), CUSTOM_CORPSE_NAME_SIZE - 1);
        return;
    }
    memcpy(name, p + 1, n), name[n] = '\0', p = end + 1;
    skipSpaces(&p);
    int mask = 0, base = -2;
    size_t len = strcspn(p, " \t\r\n");
    if (len == 19 && !strncmp(p, "bypassvanillacorpse", len)) base = CORPSE_BASE_BYPASS, p += len;
    else if (len && readCorpseKinds(&p, &mask)) base = onlyCorpseKind(mask) >= 0 ? onlyCorpseKind(mask) : -3;
    skipSpaces(&p);
    if (base == -3) {
        refuseC5mLine("newcorpsekind", spellModName(mod), "not_one_kind", "line=%d name=%.63s", G32(RVA_MOD_LINE), name);
        return;
    }
    if (base == -2 || !isLineEnd(p)) {
        refuseC5mLine("newcorpsekind", spellModName(mod), "no_vanilla_kind", "line=%d name=%.63s text=%.40s", G32(RVA_MOD_LINE), name, p);
        return;
    }
    if (isCorpseKindWord(name) || !strcasecmp(name, "none") || !strcasecmp(name, "bypassvanillacorpse")) {
        refuseC5mLine("newcorpsekind", spellModName(mod), "name_taken", "line=%d name=%.63s", G32(RVA_MOD_LINE), name);
        return;
    }
    int k = findCustomCorpseKind(name);
    if (customCorpseKindDeclared(k) && customCorpseKinds[k].base != base) {
        refuseC5mLine("newcorpsekind", spellModName(mod), "other_base", "line=%d name=%.63s first_mod=%.63s", G32(RVA_MOD_LINE), name,
                      customCorpseKinds[k].mod);
        return;
    }
    if (customCorpseKindDeclared(k)) return;
    if (k < 0 && (k = addCustomCorpseKind(name, base)) < 0) {
        refuseC5mLine("newcorpsekind", spellModName(mod), "too_many_kinds", "line=%d name=%.63s max=%d", G32(RVA_MOD_LINE), name,
                      CUSTOM_CORPSE_KINDS_MAX);
        return;
    }
    moveCustomCorpseBase(k, base);
    customCorpseKinds[k].declared = 1;
    snprintf(customCorpseKinds[k].mod, sizeof customCorpseKinds[k].mod, "%s", spellModName(mod));
}

static void corpseWordsBeforePass(int level) {
    if (level != 2) return;
    corpseRuleCount = 0;
    for (int k = 0; k < customCorpseKindCount; k++) customCorpseKinds[k].declared = 0;
    if (monsterCorpseKinds) memset(monsterCorpseKinds, 0, (size_t)monsterCorpseKindCount);
}

static void corpseWordsAfterPass(int level) {
    if (level != 2) return;
    for (int i = 0; i < corpseRuleCount; i++) {
        CorpseRule *r = &corpseRules[i];
        if (r->type != CORPSE_RULE_RAISE_CORPSES || monsterByName(r->monster) >= 0) continue;
        refuseC5mLine("raisecorpses", r->mod, "unknown_monster", "line=%d monster=%.48s", r->line, r->monster);
        r->type = CORPSE_RULE_DROPPED;
    }
}

// ---- Events: +mincorpses and removecorpses with kinds ----------------------------------------------
static int corpseEventSquare(short *corpses, int mask, int custom, int n, int remove) {
    if (remove) {
        if (corpses) takeSelectedCorpses(corpses, mask, custom, n);
        return 1;
    }
    return (corpses ? countSelectedCorpses(corpses, mask, custom) : 0) >= n;
}

static int corpseEventLine(const char *line, int level) {
    int sign = 0, remove = 0;
    const char *p;
    if ((line[0] == '+' || line[0] == '-') && !strncmp(line + 1, "mincorpses", 10) && (line[11] == ' ' || line[11] == '\t'))
        sign = line[0] == '+' ? 1 : -1, p = line + 11;
    else if (!strncmp(line, "removecorpses", 13) && (line[13] == ' ' || line[13] == '\t')) remove = 1, p = line + 13;
    else return 0;
    long long n = 0;
    skipSpaces(&p);
    if (!readSpellNumber(&p, &n)) return 0;
    skipSpaces(&p);
    if (isLineEnd(p)) return 0;
    if (level != 3 || G32(RVA_EVENT_STATE) < 1) return 1;
    int mask = 0, custom = -1, read = readCorpseKind(&p, &mask, &custom);
    if (read <= 0 || (skipSpaces(&p), !isLineEnd(p))) {
        refuseC5mLine(remove ? "removecorpses" : sign > 0 ? "+mincorpses" : "-mincorpses", "-", read < 0 ? "unknown_corpse_kind" : "bad_kind",
                      "line=%.60s", line);
        if (!remove) G32(RVA_EVENT_STATE) = EVENT_FAILED;
        return 1;
    }
    int count = n > CORPSE_COUNT_MAX * SQUARE_CORPSE_KINDS ? CORPSE_COUNT_MAX * SQUARE_CORPSE_KINDS : n < 0 ? 0 : (int)n;
    if (G32(RVA_EVENT_STATE) == PLAYER_EVENT) {
        int x = G32(RVA_EVENT_TARGET_X), y = G32(RVA_EVENT_TARGET_Y);
        if (x < 0 || y < 0) return 1;
        int holds = corpseEventSquare(squareCorpses(x, y), mask, custom, count, remove);
        if (!remove && holds != (sign > 0)) G32(RVA_EVENT_STATE) = EVENT_FAILED;
        return 1;
    }
    unsigned char *marks = gameBase + RVA_SQUARE_EVENT_A;
    int any = 0;
    for (size_t i = 0; i < SQUARE_EVENT_A_SIZE; i++) {
        if (!marks[i]) continue;
        int holds = corpseEventSquare(squareCorpses((int)(i / MAP_COLUMN_SQUARES), (int)(i % MAP_COLUMN_SQUARES)), mask, custom, count, remove);
        if (!remove && holds != (sign > 0)) marks[i] = 0;
        else any = 1;
    }
    if (!remove && !any) G32(RVA_EVENT_STATE) = EVENT_FAILED;
    return 1;
}

// ---- Custom kinds in saves: CRPS ------------------------------------------------------------------
#define CUSTOM_CORPSES_VERSION 1

static int customCorpseCellWritten(int i, int turn) {
    const CustomCorpseCell *cell = &customCorpseCells[i];
    if (!cell->key || cell->count <= 0) return 0;
    int square = (cell->key - 1) / CUSTOM_CORPSE_KINDS_MAX;
    return !turn || (changedCorpseSquareBits && changedCorpseSquareBits[square / 8] & 1 << square % 8);
}

static void addCustomCorpsesChunk(SaveList *list) {
    int turn = saveTailPlayer >= 0;
    if (!customCorpseKindCount || (turn && netRoleNow() != NET_ROLE_CLIENT)) return;
    fitAllCustomCorpses();
    if (turn && !changedCorpseSquareCount) return;
    SaveBuffer chunk = {0};
    saveBufferInt(&chunk, CUSTOM_CORPSES_VERSION);
    saveBufferInt(&chunk, customCorpseKindCount);
    for (int k = 0; k < customCorpseKindCount; k++) {
        saveBufferString(&chunk, customCorpseKinds[k].name);
        saveBufferInt(&chunk, customCorpseKinds[k].base);
    }
    saveBufferInt(&chunk, turn ? changedCorpseSquareCount : 0);
    for (int i = 0; turn && i < changedCorpseSquareCount; i++) saveBufferInt(&chunk, changedCorpseSquares[i]);
    int cells = 0;
    for (int i = 0; i < customCorpseCellCapacity; i++) cells += customCorpseCellWritten(i, turn);
    saveBufferInt(&chunk, cells);
    for (int i = 0; i < customCorpseCellCapacity; i++) {
        if (!customCorpseCellWritten(i, turn)) continue;
        const CustomCorpseCell *cell = &customCorpseCells[i];
        saveBufferInt(&chunk, (cell->key - 1) / CUSTOM_CORPSE_KINDS_MAX);
        saveBufferInt(&chunk, (cell->key - 1) % CUSTOM_CORPSE_KINDS_MAX);
        saveBufferInt(&chunk, cell->count);
    }
    writeLog("custom_corpses_written", "file=%s kinds=%d changed=%d cells=%d bytes=%zu", turn ? "turn" : "save", customCorpseKindCount,
             turn ? changedCorpseSquareCount : 0, cells, chunk.size);
    if (turn) forgetChangedCorpseSquares();
    saveListAdd(list, SAVE_TAG_CRPS, &chunk);
}

static int turnFileReadPlayer(void);

static void useCustomCorpsesChunk(const SaveChunk *chunk, SaveFile kind) {
    int turn = kind == SAVE_FILE_TURN;
    if (turn && !chunk) return;
    if (turn && (netRoleNow() != NET_ROLE_HOST || turnFileReadPlayer() < 0)) {
        writeLogWarning("custom_corpses_refused", "reason=not_the_host_read");
        return;
    }
    if (!turn) clearCustomCorpses();
    if (!chunk) return;
    SaveCursor c = {chunk->payload, chunk->payload + chunk->size, 0};
    int version = saveCursorInt(&c), kinds = saveCursorInt(&c);
    if (c.failed || version != CUSTOM_CORPSES_VERSION || kinds < 0 || kinds > CUSTOM_CORPSE_KINDS_MAX) {
        writeLogWarning("custom_corpses_read", "reason=%s version=%d", c.failed || kinds < 0 || kinds > CUSTOM_CORPSE_KINDS_MAX ? "bad_chunk" : "other_version", version);
        return;
    }
    int index[CUSTOM_CORPSE_KINDS_MAX], declaredBase[CUSTOM_CORPSE_KINDS_MAX], dropped = 0;
    for (int i = 0; i < kinds; i++) {
        char name[SAVE_STRING_MAX + 1];
        saveCursorString(&c, name, sizeof name);
        int base = saveCursorInt(&c);
        if (c.failed || base < CORPSE_BASE_BYPASS || base >= SQUARE_CORPSE_KINDS || !name[0] || strlen(name) >= CUSTOM_CORPSE_NAME_SIZE) {
            writeLogWarning("custom_corpses_read", "reason=bad_kind at=%d", i);
            if (!turn) clearCustomCorpses();
            return;
        }
        int k = addCustomCorpseKind(name, base);
        index[i] = k;
        if (k < 0) {
            dropped++;
            writeLogWarning("custom_corpses_dropped", "reason=too_many_kinds name=%.63s max=%d", name, CUSTOM_CORPSE_KINDS_MAX);
            continue;
        }
        declaredBase[i] = customCorpseKindDeclared(k) ? customCorpseKinds[k].base : base;
        if (!turn) customCorpseKinds[k].base = base;
        else if (base != customCorpseKinds[k].base) index[i] = -1;
    }
    int squares = MAP_COLUMNS_MAX * MAP_COLUMN_SQUARES, changed = saveCursorInt(&c);
    if (!c.failed && (changed < 0 || changed > squares)) c.failed = 1;
    for (int i = 0; !c.failed && i < changed; i++) {
        int square = saveCursorInt(&c);
        if (c.failed || square < 0 || square >= squares) {
            c.failed = 1;
            break;
        }
        for (int k = 0; turn && k < customCorpseKindCount; k++) {
            CustomCorpseCell *cell = customCorpseCell(square, k, 0);
            if (cell) cell->count = 0;
        }
    }
    int cells = saveCursorInt(&c), read = 0;
    for (int i = 0; !c.failed && i < cells; i++) {
        int square = saveCursorInt(&c), at = saveCursorInt(&c), count = saveCursorInt(&c);
        if (c.failed || square < 0 || square >= squares || at < 0 || at >= kinds || count < 0 || count > CORPSE_COUNT_MAX) {
            c.failed = 1;
            break;
        }
        if (index[at] < 0) continue;
        CustomCorpseCell *cell = customCorpseCell(square, index[at], 1);
        if (cell) cell->count = (short)count, read++;
    }
    for (int i = 0; !turn && i < kinds; i++)
        if (index[i] >= 0) moveCustomCorpseBase(index[i], declaredBase[i]);
    fitAllCustomCorpses();
    if (!turn) forgetChangedCorpseSquares();
    writeLog("custom_corpses_read", "file=%s result=%s kinds=%d changed=%d cells=%d dropped=%d player=%d", SAVE_FILE_NAMES[kind],
             c.failed || cells < 0 ? "bad_chunk" : "read", kinds, changed, read, dropped, turn ? turnFileReadPlayer() : -1);
}

static u64 (*nextCustomCorpsesNewWorld)(u64);
static u64 customCorpsesNewWorldHook(u64 mode) {
    clearCustomCorpses();
    return nextCustomCorpsesNewWorld(mode);
}

// ---- The calls: a cast, its raises and putcorpses, and a unit's corpse ------------------------------
typedef long long (*ExecuteRitualFn)(int, int, int, int, int, int);
typedef int (*RaiseFn)(int);
typedef const char *(*SquareTextFn)(int, int);
typedef void (*LeaveCorpseFn)(int);
typedef char *(*SenseDeadTextFn)(int, int, int, int *);
static ExecuteRitualFn executeRitualGame;
static RaiseFn raiseDeadGame, raiseAnimalsGame;
static SquareTextFn squareTextGame;
static LeaveCorpseFn leaveCorpseGame;
static SenseDeadTextFn senseDeadTextGame;
static int (*corpseRandom)(int), (*corpseOpenDie)(int), (*corpseMindless)(int);
static int (*corpseAbilityTotal)(int, int, int, char *, int, int *);
static int (*corpseSummon)(int, const char *, int, int, int);
static void (*corpseSendSquare)(int, int, int);
static uint64_t (*corpseProgressText)(int, const char *, ...);

static int unitOwnerIsHuman(int unit) {
    if (unit < 0 || unit >= UNIT_TABLE_MAX) return 0;
    int owner = (short)G16(RVA_UNIT_PLAYER + (size_t)unit * UNIT_SIZE);
    if (owner < 0 || owner >= playerSlotCount()) return 0;
    short controller;
    memcpy(&controller, playerSlotAddress(RVA_SLOT_CONTROLLER, owner, 2), 2);
    return controller == CONTROLLER_HUMAN || controller == CONTROLLER_REMOTE_HUMAN;
}

static CorpseCast *currentCorpseCast(void) {
    return corpseCastDepth > 0 && corpseCastDepth <= CORPSE_CASTS_MAX ? &corpseCasts[corpseCastDepth - 1] : NULL;
}

static int gameSquareRoot(int n) {
    if (n < 1) return 0;
    int i = 1;
    while (i < 40000 && (long long)i * i <= n) i++;
    return i < 40000 || (long long)i * i > n ? i - 1 : 40000;
}

static void addInsanity(int unit, int amount) {
    int last = G32(RVA_LAST_COMMANDER), commander = -1;
    for (int c = 0; c <= last && c < COMMANDER_TABLE_MAX && commander < 0; c++)
        if (G32(RVA_COMMANDERS + (size_t)c * COMMANDER_SIZE) == unit) commander = c;
    if (unit < 0 || commander < 0 || corpseMindless(unit)) return;
    int resist = corpseAbilityTotal(unit, ABILITY_VOIDSANITY, 1, NULL, 0, NULL);
    int add = amount > 0 && resist != 0 ? (amount - resist > 0 ? amount - resist : 0) : amount;
    short *insanity = (short *)(gameBase + RVA_COMMANDER_FIELD_10 + (size_t)commander * COMMANDER_SIZE);
    int value = amount == INSANITY_HALVED ? *insanity / 2 : (short)(*insanity + add);
    *insanity = (short)(value < 0 ? 0 : value > INSANITY_MAX ? INSANITY_MAX : value);
}

static void runRaiseCorpses(const CorpseCast *cast) {
    for (int i = 0; i < corpseRuleCount; i++) {
        const CorpseRule *r = &corpseRules[i];
        if (r->type != CORPSE_RULE_RAISE_CORPSES || r->ritual != cast->ritual) continue;
        int unit = cast->unit;
        if (unit < 0 || unit >= UNIT_TABLE_MAX) return;
        int x = G32(RVA_UNIT_X + (size_t)unit * UNIT_SIZE), y = G32(RVA_UNIT_X + 4 + (size_t)unit * UNIT_SIZE);
        short *corpses = squareCorpses(x, y);
        int mask = cast->human ? r->mask : CORPSE_MASK_ALL, custom = cast->human ? r->custom : -1;
        int n = corpses ? countSelectedCorpses(corpses, mask, custom) : 0;
        if (n < 1) {
            corpseProgressText(4000, "CoE5SE: Ritual failed due to no suitable corpses found.");
            continue;
        }
        int raised = corpseRandom(n + 1);
        if (n - raised > 0) raised += corpseRandom(n - raised) + 1;
        takeSelectedCorpses(corpses, mask, custom, raised);
        corpseSendSquare((short)G16(RVA_UNIT_PLAYER + (size_t)unit * UNIT_SIZE), x, y);
        corpseSummon(unit, r->monster, RAISE_SUMMON_CONTROL, raised, RAISE_SUMMON_FAIL_PLAYER);
        int root = gameSquareRoot(raised);
        addInsanity(unit, root + 1 + corpseOpenDie(root * 2));
    }
}

static ExecuteRitualFn nextCorpseExecute;
static long long corpseExecuteRitual(int unit, int ritual, int x, int y, int percent, int pay) {
    ExecuteRitualFn executeRitualNext = nextCorpseExecute ? nextCorpseExecute : executeRitualGame;
    if (!corpseRuleCount) return executeRitualNext(unit, ritual, x, y, percent, pay);
    int depth = corpseCastDepth++;
    if (depth < CORPSE_CASTS_MAX) corpseCasts[depth] = (CorpseCast){ritual, unit, unitOwnerIsHuman(unit), percent, 0, 0, 0};
    long long result = executeRitualNext(unit, ritual, x, y, percent, pay);
    corpseCastDepth--;
    if (depth < CORPSE_CASTS_MAX && result != -1) runRaiseCorpses(&corpseCasts[depth]);
    return result;
}

static int raiseWithKinds(RaiseFn game, int commander, int mask) {
    if (commander < 0 || commander > G32(RVA_LAST_COMMANDER) || commander >= COMMANDER_TABLE_MAX) return game(commander);
    int unit = G32(RVA_COMMANDERS + (size_t)commander * COMMANDER_SIZE);
    if (unit < 0 || unit >= UNIT_TABLE_MAX) return game(commander);
    int x = G32(RVA_UNIT_X + (size_t)unit * UNIT_SIZE), y = G32(RVA_UNIT_X + 4 + (size_t)unit * UNIT_SIZE);
    short *corpses = squareCorpses(x, y), aside[SQUARE_CORPSE_KINDS] = {0};
    if (!corpses) return game(commander);
    fitCustomCorpses(corpses);
    for (int k = 0; k < SQUARE_CORPSE_KINDS; k++)
        if (!(mask & 1 << k)) aside[k] = corpses[k], corpses[k] = 0;
    int raised = game(commander), anyAside = 0;
    for (int k = 0; k < SQUARE_CORPSE_KINDS; k++) anyAside |= aside[k], addCorpses(corpses, k, aside[k]);
    fitCustomCorpses(corpses);
    if (anyAside) corpseSendSquare((short)G16(RVA_UNIT_PLAYER + (size_t)unit * UNIT_SIZE), x, y);
    return raised;
}

static short *commanderCorpses(int commander) {
    if (commander < 0 || commander > G32(RVA_LAST_COMMANDER) || commander >= COMMANDER_TABLE_MAX) return NULL;
    int unit = G32(RVA_COMMANDERS + (size_t)commander * COMMANDER_SIZE);
    if (unit < 0 || unit >= UNIT_TABLE_MAX) return NULL;
    return squareCorpses(G32(RVA_UNIT_X + (size_t)unit * UNIT_SIZE), G32(RVA_UNIT_X + 4 + (size_t)unit * UNIT_SIZE));
}

static int corpseRaise(RaiseFn game, int commander, int type, int *seen) {
    CorpseCast *cast = currentCorpseCast();
    int occurrence = cast ? (*seen)++ : -1;
    const CorpseRule *r = cast && cast->human ? corpseRuleFor(cast->ritual, type, occurrence) : NULL;
    if (r) return raiseWithKinds(game, commander, r->mask);
    short *corpses = commanderCorpses(commander);
    fitCustomCorpses(corpses);
    int raised = game(commander);
    fitCustomCorpses(corpses);
    return raised;
}

static int corpseRaiseDead(int commander) {
    CorpseCast *cast = currentCorpseCast();
    return corpseRaise(raiseDeadGame, commander, CORPSE_RULE_RAISE_DEAD, cast ? &cast->raiseDeads : NULL);
}

static int corpseRaiseAnimals(int commander) {
    CorpseCast *cast = currentCorpseCast();
    return corpseRaise(raiseAnimalsGame, commander, CORPSE_RULE_RAISE_ANIMALS, cast ? &cast->raiseAnimals : NULL);
}

static const char *corpsePutText(int x, int y) {
    CorpseCast *cast = currentCorpseCast();
    int occurrence = cast ? cast->puts++ : -1;
    const CorpseRule *r = cast && cast->human ? corpseRuleFor(cast->ritual, CORPSE_RULE_PUT, occurrence) : NULL;
    short *corpses = r ? squareCorpses(x, y) : NULL;
    if (corpses) {
        long long value = ritualEffectValueAt(cast->ritual, RITUAL_FX_PUT_CORPSES, occurrence);
        long long n = value >= PUT_CORPSES_SCALED ? (value - PUT_CORPSES_SCALED) * cast->percent / 100 : value;
        int moved = n <= 0 ? 0 : n < corpses[CORPSE_KIND_HUMAN] ? (int)n : corpses[CORPSE_KIND_HUMAN];
        fitCustomCorpses(corpses);
        corpses[CORPSE_KIND_HUMAN] = (short)(corpses[CORPSE_KIND_HUMAN] - moved);
        if (r->custom >= 0) addCustomCorpses(corpses, r->custom, moved);
        else addCorpses(corpses, onlyCorpseKind(r->mask), moved);
    }
    return squareTextGame(x, y);
}

static void corpseLeave(int unit) {
    int monster = unit >= 0 && unit < UNIT_TABLE_MAX ? (short)G16(RVA_UNIT_MONSTER + (size_t)unit * UNIT_SIZE) : -1;
    int kind = monster >= 0 && monster < monsterCorpseKindCount ? monsterCorpseKinds[monster] - 1 : -1;
    short *corpses = unit >= 0 && unit < UNIT_TABLE_MAX
                         ? squareCorpses(G32(RVA_UNIT_X + (size_t)unit * UNIT_SIZE), G32(RVA_UNIT_X + 4 + (size_t)unit * UNIT_SIZE))
                         : NULL;
    fitCustomCorpses(corpses);
    if (kind < 0) {
        leaveCorpseGame(unit);
        return;
    }
    if (kind == CORPSE_KIND_LEAVES_NONE) return;
    if (!corpses) return;
    if (kind >= CORPSE_STORED_CUSTOM - 1) addCustomCorpses(corpses, kind - (CORPSE_STORED_CUSTOM - 1), 1);
    else addCorpses(corpses, kind, 1);
}

static const char *senseDeadShareWords(int n, int total) {
    if (n <= 0 || total <= 0) return NULL;
    if ((long long)n * 100 >= (long long)total * 95) return "Almost all";
    if ((long long)n * 2 > total) return "Most";
    if ((long long)n * 5 > total) return "Some";
    return NULL;
}

static char *corpseSenseDeadText(int commander, int x, int y, int *band) {
    short *corpses = squareCorpses(x, y);
    if (!corpses || !customCorpseCellUsed) return senseDeadTextGame(commander, x, y, band);
    fitCustomCorpses(corpses);
    int square = corpseSquareIndex(corpses), hidden = 0, best = -1, most = 0;
    for (int k = 0; k < customCorpseKindCount; k++) {
        int n = customCorpseCount(square, k);
        if (customCorpseKinds[k].base == CORPSE_BASE_BYPASS) hidden += n;
        if (n > most) most = n, best = k;
    }
    short kept = corpses[CORPSE_KIND_HUMAN];
    int lent = hidden < CORPSE_COUNT_MAX - kept ? hidden : CORPSE_COUNT_MAX - kept;
    corpses[CORPSE_KIND_HUMAN] = (short)(kept + lent);
    int total = countCorpses(corpses, CORPSE_MASK_ALL);
    char *text = senseDeadTextGame(commander, x, y, band);
    corpses[CORPSE_KIND_HUMAN] = kept;
    const char *share = text && total >= 3 && best >= 0 ? senseDeadShareWords(most, total) : NULL;
    if (!share) return text;
    size_t used = strnlen(text, SENSEDEAD_TEXT_SIZE);
    char line[SENSEDEAD_TEXT_SIZE];
    int n = snprintf(line, sizeof line, "%sCoE5SE: %s of the dead seem to be %s.", used && text[used - 1] != ' ' ? " " : "", share,
                     customCorpseKinds[best].name);
    if (n > 0 && used + (size_t)n < SENSEDEAD_TEXT_SIZE) memcpy(text + used, line, (size_t)n + 1);
    return text;
}

typedef struct {
    unsigned int callRva, targetRva;
    void *hook;
    const char *name;
} CorpseCall;

#ifndef __APPLE__
#define CORPSE_STUB_SIZE 0x20
static int pointCorpseCall(const CorpseCall *c, int index) {
    static unsigned char *stubs;
    if (!stubs) {
        if (!(stubs = allocNear(gameBase + RVA_TEXT_SECTION))) {
            writeLog("hook_skipped", "site=%s reason=no_near_page", c->name);
            return 0;
        }
        makePagesReadExecute(stubs, 0x1000);
    }
    return redirectCall(c->callRva, c->targetRva, stubs + CORPSE_STUB_SIZE * index, c->hook, c->name, NULL);
}
#else
static int pointCorpseCall(const CorpseCall *c, int index) {
    (void)index;
    if (blTarget(gameBase + c->callRva) != gameBase + c->targetRva) {
        writeLog("hook_skipped", "site=%s reason=not_the_expected_call va=0x%llx", c->name, c->callRva + 0x100000000ULL);
        return 0;
    }
    static void *hooks[8];
    static unsigned char *trampolines[8];
    int t = 0;
    while (t < 8 && hooks[t] && hooks[t] != c->hook) t++;
    if (t == 8) {
        writeLog("hook_skipped", "site=%s reason=no_padding", c->name);
        return 0;
    }
    if (!hooks[t]) trampolines[t] = newTrampoline(c->hook), hooks[t] = trampolines[t] ? c->hook : NULL;
    uint32_t word = trampolines[t] ? blTo(gameBase + c->callRva, trampolines[t]) : 0;
    if (trampolines[t] && writeGameCode(gameBase + c->callRva, &word, 4)) return 1;
    writeLog("hook_skipped", "site=%s reason=%s", c->name, trampolines[t] ? "code_unwritable" : "no_padding");
    return 0;
}
#endif

static void corpseKindsInit(void) {
    executeRitualGame = GAME_FN(ExecuteRitualFn, RVA_EXECUTE_RITUAL);
    raiseDeadGame = GAME_FN(RaiseFn, RVA_RAISE_DEAD);
    raiseAnimalsGame = GAME_FN(RaiseFn, RVA_RAISE_ANIMALS);
    squareTextGame = GAME_FN(SquareTextFn, RVA_SQUARE_TEXT);
    leaveCorpseGame = GAME_FN(LeaveCorpseFn, RVA_UNIT_LEAVE_CORPSE);
    senseDeadTextGame = GAME_FN(SenseDeadTextFn, RVA_SENSEDEAD_TEXT);
    corpseRandom = GAME_FN(int (*)(int), RVA_RANDOM);
    corpseOpenDie = GAME_FN(int (*)(int), RVA_RANDOM_OPEN_DIE);
    corpseMindless = GAME_FN(int (*)(int), RVA_UNIT_MINDLESS);
    corpseAbilityTotal = GAME_FN(int (*)(int, int, int, char *, int, int *), RVA_UNIT_ABILITY_TOTAL);
    corpseSummon = GAME_FN(int (*)(int, const char *, int, int, int), RVA_RITUAL_SUMMON_UNITS);
    corpseSendSquare = GAME_FN(void (*)(int, int, int), RVA_NET_SEND_SQUARE);
    corpseProgressText = GAME_FN(uint64_t (*)(int, const char *, ...), RVA_PROGRESS_TEXT);
    int words = apiAddModCommand("corpsekind", corpsekindLine) + apiAddModCommand("raisecorpses", raisecorpsesLine) +
                apiAddModCommand("newcorpsekind", newcorpsekindLine);
    static const CorpseCall CALLS[] = {
        {RVA_DAMAGE_LEAVE_CORPSE_CALL, RVA_UNIT_LEAVE_CORPSE, (void *)corpseLeave, "damage_leave_corpse"},
        {RVA_HP_LOSS_LEAVE_CORPSE_CALL, RVA_UNIT_LEAVE_CORPSE, (void *)corpseLeave, "hp_loss_leave_corpse"},
        {RVA_RITUAL_RAISE_DEAD_CALL, RVA_RAISE_DEAD, (void *)corpseRaiseDead, "ritual_raise_dead"},
        {RVA_RITUAL_RAISE_ANIMALS_CALL, RVA_RAISE_ANIMALS, (void *)corpseRaiseAnimals, "ritual_raise_animals"},
        {RVA_RITUAL_PUT_CORPSES_TEXT_CALL, RVA_SQUARE_TEXT, (void *)corpsePutText, "ritual_put_corpses"},
        {RVA_PANEL_SENSEDEAD_TEXT_CALL, RVA_SENSEDEAD_TEXT, (void *)corpseSenseDeadText, "panel_sensedead_text"},
        {RVA_RITUAL_SENSEDEAD_TEXT_CALL, RVA_SENSEDEAD_TEXT, (void *)corpseSenseDeadText, "ritual_sensedead_text"},
    };
    int count = (int)(sizeof CALLS / sizeof CALLS[0]), calls = 0, cast = 0, newWorld = 0;
    if (checkGameFunction("corpse_kinds_check", "")) {
        for (int i = 0; i < count; i++) calls += pointCorpseCall(&CALLS[i], i);
        newWorld = wrapSite(SITE_PLAY_GAME_NEW_WORLD, (void *)customCorpsesNewWorldHook, (void **)&nextCustomCorpsesNewWorld);
        cast = wrapSite(SITE_EXECUTE_RITUAL, (void *)corpseExecuteRitual, (void **)&nextCorpseExecute);
    }
    writeResultLog(words == 3 && calls == count && cast && newWorld, "corpse_kinds_installed", "words=%d of 3 calls=%d of %d cast=%d new_world=%d",
                   words, calls, count, cast, newWorld);
}
