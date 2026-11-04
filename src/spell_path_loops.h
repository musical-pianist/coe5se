#define IN_PLACE_PATH_COUNT   128
#define UNIT_KNOWN_SPELLS     60
#define UNIT_MEMORIZED_SPELLS 30
#define PATH_SPELL_END        (-999)
#define PATH_FORTUNE          18
#define LEARN_TRIES           5000
#define UNMEMORIZE_TRIES      1000
#define WEAPON_TYPE_RAISE     0x12
#define MAX_HP_FOR_DISPLACE   50
#define LIBRARY_ROW_LEVEL     1000
#define LIBRARY_MAX_KNOWN     60

#define PATH_SPELL_NAME_AT    0x00
#define PATH_SPELL_PATH_AT    0x08
#define PATH_SPELL_LEVEL_AT   0x0c
#define PATH_SPELL_WEAPON_AT  0x10
#define PATH_SPELL_DMG_AT     0x18
#define PATH_SPELL_FLAGS_AT   0x20
#define WEAPON_RANGE_TYPE_AT  0x40
#define WEAPON_RANGE_AT       0x44
#define WEAPON_TYPE_AT        0x4c
#define WEAPON_VALUE_AT       0x50
#define WEAPON_STRIKES_AT     0x58
#define WEAPON_NEXT_AT        0x90
#define SPELL_FLAG_SLOW       0x04

static struct {
    int (*unitPathLevel)(int unit, int path);
    int (*unitSpellWeaponLevel)(int unit, int path);
    int (*unitAbilityTotal)(int unit, int ability, int add, char *out, int outFlag, int *outValue);
    int (*randomSpell)(int path, int level, int flags);
    int (*addKnownSpell)(int unit, int spell);
    int (*memorizeSpell)(int unit, int spell);
    int (*unmemorizeSpell)(int unit, int spell);
    int (*findSpell)(const char *name);
    int (*random)(int n);
    int (*unitMaxHp)(int unit);
    int (*unitHasFlag4)(int unit);
    int (*countMemorizedOfType)(int unit, int path, int weaponType);
    int (*countKnownSpells)(int unit, int path, int level);
    int (*countSpells)(int path, int level);
    int (*unitOwner)(int unit);
    int (*unitX)(int unit);
    int (*unitY)(int unit);
    int (*libraryLevel)(int player, int x, int y);
    void (*textButton)(int id, const char *fmt, ...);
    void (*disabledButton)(int id, const char *fmt, ...);
    void (*tooltip)(const char *fmt, ...);
    void (*listGap)(float gap);
    int (*layoutNew)(void);
    void (*learnStartSpells)(int unit);
    int (*chooseMemorized)(int unit);
    void (*sectionBegin)(int frame);
    void (*sectionEnd)(void);
    void (*rowBegin)(int end);
    void (*columnX)(float x);
    void (*rowStripe)(int odd, int width);
    void (*titleCell)(int id, const char *text);
    void (*headerText)(int id, const char *fmt, ...);
    void (*textCell)(int id, const char *text);
    void (*spellNameButton)(int id, float width, const char *text);
    void (*optionTooltip)(const char *text);
    int (*memorizeAllowance)(int unit, int path);
    int (*countMemorizedSpells)(int unit, int path);
    int (*memorizedSlot)(int unit, int spell);
    int (*spellRangeBonus)(int unit, int attackType, int weapon);
    void (*damageText)(int type, unsigned long long value, int dmg, const char *suffix);
    void (*formatAreaShape)(int strikes, char *out, int size);
    unsigned char *knownSpells;
    unsigned char *memorizedSpells;
    const unsigned char *spells;
    int spellRowBase;
    const unsigned char *weapons;
    const char **pathNames;
    int pathCount;
} pathGame;

static short *unitKnownSpells(int unit) { return (short *)(void *)(pathGame.knownSpells + (size_t)unit * UNIT_SIZE); }
static short *unitMemorizedSpells(int unit) { return (short *)(void *)(pathGame.memorizedSpells + (size_t)unit * UNIT_SIZE); }

static int spellField(int spell, int at) {
    int value;
    memcpy(&value, pathGame.spells + (size_t)spell * SPELL_SIZE + at, sizeof value);
    return value;
}

static int countMemorizedOfPath(int unit, int path) {
    const short *memorized = unitMemorizedSpells(unit);
    int count = 0;
    for (int i = 0; i < UNIT_MEMORIZED_SPELLS && memorized[i] >= 0; i++)
        if (path == -1 || spellField(memorized[i], PATH_SPELL_PATH_AT) == path) count++;
    return count;
}

static int knownSpellSlot(int unit, int spell) {
    const short *known = unitKnownSpells(unit);
    for (int i = 0; i < UNIT_KNOWN_SPELLS && known[i] >= 0; i++)
        if (known[i] == spell) return i;
    return -1;
}

// ---- learn_start_spells --------------------------------------------------------------------------
static void learnStartSpellsPaths(int unit, int from, int to) {
    for (int path = from; path < to; path++) {
        int attack = pathGame.unitSpellWeaponLevel(unit, path);
        if (attack <= 0) continue;
        int level = pathGame.unitPathLevel(unit, path);
        if (level < attack) level = attack;
        for (int spellLevel = 1; spellLevel <= level; spellLevel++) {
            int wanted = pathGame.unitAbilityTotal(unit, spellLevel + 0x22, 1, NULL, 0, NULL) + 2 - pathGame.countKnownSpells(unit, path, spellLevel);
            if (wanted <= 0) continue;
            int spell = pathGame.randomSpell(path, spellLevel, 2);
            if (spell < 0) spell = pathGame.randomSpell(path, spellLevel, 0);
            if (pathGame.addKnownSpell(unit, spell) >= 0) wanted--;
            int left = path == PATH_FORTUNE ? 0 : wanted;
            for (int tries = 0; left > 0 && tries < LEARN_TRIES;) {
                spell = pathGame.randomSpell(path, spellLevel, 0);
                tries++;
                if (pathGame.addKnownSpell(unit, spell) >= 0) left--;
            }
        }
    }
}

static void learnStartSpellsHook(int unit) {
    noteSpellPathsNetworkGame("learn_start_spells");
    pathGame.learnStartSpells(unit);
    learnStartSpellsPaths(unit, IN_PLACE_PATH_COUNT, pathGame.pathCount);
}

// ---- choose_memorized ----------------------------------------------------------------------------
static const char *const RAISE_SPELLS[] = {"Army of the Dead", "Dance Macabre", "Raise Longdead", "Reanimation"};

static int memorizeKnownByName(int unit, int path, const char *name, int *count) {
    int spell = pathGame.findSpell(name);
    if (spell < 0 || knownSpellSlot(unit, spell) < 0) return 0;
    pathGame.memorizeSpell(unit, spell);
    *count = countMemorizedOfPath(unit, path);
    return 1;
}

static int chooseMemorizedPaths(int unit, int from, int to, int result) {
    if (unit < 0) return result;
    short *memorized = unitMemorizedSpells(unit);
    for (int path = from; path < to; path++) {
        int pathLevel = pathGame.unitPathLevel(unit, path);
        for (int i = 0; i < UNIT_MEMORIZED_SPELLS && memorized[i] >= 0;) {
            int spell = memorized[i], spellPath = spellField(spell, PATH_SPELL_PATH_AT);
            if (spellPath < 0 || spellField(spell, PATH_SPELL_LEVEL_AT) > pathGame.unitPathLevel(unit, spellPath) ||
                knownSpellSlot(unit, spell) < 0) {
                pathGame.unmemorizeSpell(unit, spell);
                result = 1;
                i = 0;
                continue;
            }
            i++;
        }
        int count = countMemorizedOfPath(unit, path), wanted = pathGame.unitPathLevel(unit, path);
        wanted = wanted > 0 ? wanted + 1 : 0;
        if (count < wanted) {
            if (pathGame.unitMaxHp(unit) > MAX_HP_FOR_DISPLACE && memorizeKnownByName(unit, path, "Displace Self", &count)) result = -1;
            for (size_t r = 0; r < sizeof RAISE_SPELLS / sizeof RAISE_SPELLS[0]; r++)
                if (count < wanted && pathGame.unitHasFlag4(unit) && pathGame.countMemorizedOfType(unit, -1, WEAPON_TYPE_RAISE) <= 0 &&
                    memorizeKnownByName(unit, path, RAISE_SPELLS[r], &count))
                    result = -1;
        }
        int before = count, lowest = pathLevel;
        for (int tries = 0; count < wanted && tries < LEARN_TRIES;) {
            const short *known = unitKnownSpells(unit);
            int matching = 0;
            for (int i = 0; i < UNIT_KNOWN_SPELLS && known[i] >= 0; i++)
                if (path == -1 || spellField(known[i], PATH_SPELL_PATH_AT) == path) matching++;
            if (matching > 0) {
                int pick = pathGame.random(matching);
                for (int i = 0; i < UNIT_KNOWN_SPELLS && known[i] >= 0; i++) {
                    int spell = known[i];
                    if (path != -1 && spellField(spell, PATH_SPELL_PATH_AT) != path) continue;
                    if (--pick >= 0) continue;
                    int level = spellField(spell, PATH_SPELL_LEVEL_AT);
                    if (level <= pathLevel && level >= lowest) {
                        pathGame.memorizeSpell(unit, spell);
                        count = countMemorizedOfPath(unit, path);
                        if (count > before) result = -1;
                    }
                    break;
                }
            }
            tries++;
            if (tries % 100 == 0) lowest--;
        }
        for (int tries = 0; count > wanted && tries < UNMEMORIZE_TRIES; tries++) {
            int matching = countMemorizedOfPath(unit, path), spell = -1;
            if (matching > 0) {
                int pick = pathGame.random(matching);
                for (int i = 0; i < UNIT_MEMORIZED_SPELLS && memorized[i] >= 0; i++) {
                    if (path != -1 && spellField(memorized[i], PATH_SPELL_PATH_AT) != path) continue;
                    if (--pick < 0) {
                        spell = memorized[i];
                        break;
                    }
                }
            }
            pathGame.unmemorizeSpell(unit, spell);
            result = 1;
            count = countMemorizedOfPath(unit, path);
        }
    }
    return result;
}

static int chooseMemorizedHook(int unit) {
    noteSpellPathsNetworkGame("choose_memorized");
    int result = pathGame.chooseMemorized(unit);
    return chooseMemorizedPaths(unit, IN_PLACE_PATH_COUNT, pathGame.pathCount, result);
}

// ---- The library's rows --------------------------------------------------------------------------
static const char *const LIBRARY_STUDY[3] = {"Study %s spells of level 1 (%d gold)", "Study %s spells of level 2 (%d gold)",
                                            "Study %s spells of level 3 (%d gold)"};
static const char *const LIBRARY_LOCKED[3] = {NULL, "A library of at least level 2 is required for this",
                                             "A library of at least level 3 is required for this"};
#define LIBRARY_NO_MORE "No more spells available to learn"
static const int LIBRARY_GOLD[3] = {25, 75, 225};

static int libraryUnit = -1;

static void libraryRowsPaths(int unit, int from, int to) {
    int libraryLevel = pathGame.libraryLevel(pathGame.unitOwner(unit), pathGame.unitX(unit), pathGame.unitY(unit));
    unsigned divisor = libraryLevel >= 4 ? 2 : 1;
    int known = pathGame.countKnownSpells(unit, -1, -1);
    for (int path = from; path < to; path++) {
        int pathLevel = pathGame.unitPathLevel(unit, path);
        if (pathLevel <= 0) continue;
        const char *name = pathGame.pathNames[path];
        for (int level = 1; level <= 3 && level <= pathLevel; level++) {
            int all = pathGame.countSpells(path, level), have = pathGame.countKnownSpells(unit, path, level);
            unsigned gold = (unsigned)LIBRARY_GOLD[level - 1] / divisor;
            if (level > 1 && libraryLevel < level) {
                pathGame.disabledButton(0, LIBRARY_STUDY[level - 1], name, gold);
                pathGame.tooltip(LIBRARY_LOCKED[level - 1]);
            } else if (have < all && known < LIBRARY_MAX_KNOWN) {
                pathGame.textButton(path + LIBRARY_ROW_LEVEL * level, LIBRARY_STUDY[level - 1], name, gold);
            } else {
                pathGame.disabledButton(0, LIBRARY_STUDY[level - 1], name, gold);
                pathGame.tooltip(LIBRARY_NO_MORE);
            }
        }
    }
}

static int libraryKnownCountHook(int unit, int path, int level) {
    libraryUnit = unit;
    return pathGame.countKnownSpells(unit, path, level);
}

static int libraryRowsEndHook(void) {
    if (libraryUnit >= 0) libraryRowsPaths(libraryUnit, IN_PLACE_PATH_COUNT, pathGame.pathCount);
    libraryUnit = -1;
    return pathGame.layoutNew();
}

// ---- The stats screen's "Known <path> Spells" boxes ----------------------------------------------
typedef struct {
    float range;
    float level;
    float damage;
    float special;
    int stripe;
    int unit;
    int skipped;
} KnownBoxColumns;

#define KNOWN_BOX_GAP 4.0f

static int weaponField(int weapon, int at) {
    int value;
    memcpy(&value, pathGame.weapons + (size_t)weapon * WEAPON_SIZE + at, sizeof value);
    return value;
}

static void knownSpellRow(int unit, int spell, int odd, const KnownBoxColumns *c) {
    char text[100], range[100] = "", cell[10], tip[500] = "";
    int weapon = spellField(spell, PATH_SPELL_WEAPON_AT);
    pathGame.rowBegin(0);
    pathGame.rowStripe(odd, c->stripe);
    snprintf(text, sizeof text, pathGame.memorizedSlot(unit, spell) < 0 ? "  %s" : "* %s",
             *(const char *const *)(const void *)(pathGame.spells + (size_t)spell * SPELL_SIZE + PATH_SPELL_NAME_AT));
    pathGame.spellNameButton(spell + pathGame.spellRowBase, c->level, text);
    pathGame.columnX(c->level);
    pathGame.textButton(0, "%d", spellField(spell, PATH_SPELL_LEVEL_AT));
    if (weapon >= 0) {
        int bonus = pathGame.spellRangeBonus(unit, 3, weapon), rangeType = weaponField(weapon, WEAPON_RANGE_TYPE_AT);
        pathGame.columnX(c->range);
        if (bonus) snprintf(range, sizeof range, "(+%d)", bonus);
        if (rangeType == 5 || rangeType == 6) {
            snprintf(cell, sizeof cell, "E");
            snprintf(tip, sizeof tip, "Targets the edge of the battlefield.");
        } else if (rangeType == 8) {
            snprintf(cell, sizeof cell, "R");
            snprintf(tip, sizeof tip, "Targets rear enemy ranks.");
        } else {
            snprintf(cell, sizeof cell, "%d%.8s", weaponField(weapon, WEAPON_RANGE_AT), range);
        }
        pathGame.textCell(0, cell);
        if (tip[0]) pathGame.optionTooltip(tip);
        pathGame.columnX(c->damage);
        unsigned long long value;
        memcpy(&value, pathGame.weapons + (size_t)weapon * WEAPON_SIZE + WEAPON_VALUE_AT, sizeof value);
        pathGame.damageText(weaponField(weapon, WEAPON_TYPE_AT), value, spellField(spell, PATH_SPELL_DMG_AT),
                            weaponField(weapon, WEAPON_NEXT_AT) == -1 ? "" : "*");
        pathGame.columnX(c->special);
        int strikes = weaponField(weapon, WEAPON_STRIKES_AT), any = 0;
        if (strikes != 1) {
            if (strikes < 0) {
                pathGame.textButton(0, "%d strikes", -strikes);
                any = 1;
            } else if (weaponField(weapon, WEAPON_TYPE_AT) != WEAPON_TYPE_RAISE) {
                pathGame.formatAreaShape(strikes, text, sizeof text);
                pathGame.textCell(0, text);
                any = 1;
            }
        }
        if (spellField(spell, PATH_SPELL_FLAGS_AT) & SPELL_FLAG_SLOW) {
            if (any) pathGame.textButton(0, ", ");
            pathGame.textButton(0, "slow to cast");
        }
    }
    pathGame.rowBegin(1);
}

static void knownSpellsBox(int unit, int path, const KnownBoxColumns *c, float gap) {
    char title[500];
    pathGame.sectionBegin(1);
    pathGame.listGap(gap);
    pathGame.rowBegin(0);
    snprintf(title, sizeof title, "Known %s Spells", pathGame.pathNames[path]);
    pathGame.titleCell(0, title);
    pathGame.tooltip("List of the known spells in this school of magic. The spells marked with a * are currently memorized and will "
                     "be used in battles.");
    pathGame.columnX(c->level);
    pathGame.headerText(0, "Level");
    pathGame.tooltip("A level 2 mage is able to cast one level 2 spell or two level 1 spells per combat round. It is not possible to "
                     "cast spells of higher level than the mage.");
    pathGame.columnX(c->range);
    pathGame.headerText(0, "Range");
    pathGame.tooltip("The spell cannot target units that are further away than this number of squares.");
    pathGame.columnX(c->damage);
    pathGame.headerText(0, "Damage");
    pathGame.tooltip("The basic damage of the spell, right click on the spell name to see more details.");
    pathGame.columnX(c->special);
    int allowance = pathGame.memorizeAllowance(unit, path);
    pathGame.headerText(0, "Special (memorized: %d/%d)", pathGame.countMemorizedSpells(unit, path), allowance);
    pathGame.tooltip("A mage must memorize his full allowance of memorized spells. The spells used in battle will be selected from the "
                     "memorized spells only.");
    pathGame.rowBegin(1);
    const short *known = unitKnownSpells(unit);
    for (int pass = 1; pass <= 3; pass++) {
        for (int i = 0; i < UNIT_KNOWN_SPELLS && known[i] >= 0; i++) {
            int spell = known[i], level = spellField(spell, PATH_SPELL_LEVEL_AT);
            if (spellField(spell, PATH_SPELL_PATH_AT) != path) continue;
            if (level == pass || (pass == 3 && level > 3) || (pass == 1 && level < 1)) knownSpellRow(unit, spell, ((i + 1) & 1) + 1, c);
        }
    }
    pathGame.sectionEnd();
}

static void knownSpellsBoxesPaths(int unit, int from, int to, const KnownBoxColumns *c, float gap) {
    for (int path = from; path < to; path++)
        if (pathGame.countKnownSpells(unit, path, -1) > 0 && pathGame.unitPathLevel(unit, path) > 0) knownSpellsBox(unit, path, c, gap);
}

static KnownBoxColumns *knownBoxColumns;

static int statsKnownEndHook(void) {
    KnownBoxColumns *c = knownBoxColumns;
    if (!c->skipped) knownSpellsBoxesPaths(c->unit, IN_PLACE_PATH_COUNT, pathGame.pathCount, c, KNOWN_BOX_GAP);
    c->skipped = 0;
    return pathGame.layoutNew();
}

static void emitBytes(unsigned char **p, const void *bytes, int count) {
    memcpy(*p, bytes, (size_t)count);
    *p += count;
}

static void emitRel32(unsigned char **p, const void *target, int after) {
    int rel = (int)((const unsigned char *)target - (*p + 4 + after));
    emitBytes(p, &rel, 4);
}

static int writeColumnStub(unsigned char *stub, KnownBoxColumns *columns, void *hook, int unitAt, int stripeAt) {
    if (unitAt < 0 || unitAt + 8 > 0x7f || stripeAt < 0 || stripeAt + 8 > 0x7f) return 0;
    static const struct { unsigned char bytes[5]; int size; } MOVSS[4] = {
        {{0xf3, 0x0f, 0x11, 0x35}, 4},
        {{0xf3, 0x44, 0x0f, 0x11, 0x0d}, 5},
        {{0xf3, 0x0f, 0x11, 0x3d}, 4},
        {{0xf3, 0x44, 0x0f, 0x11, 0x05}, 5},
    };
    const float *floats[4] = {&columns->level, &columns->range, &columns->damage, &columns->special};
    unsigned char *p = stub;
    for (int i = 0; i < 4; i++) {
        emitBytes(&p, MOVSS[i].bytes, MOVSS[i].size);
        emitRel32(&p, floats[i], 0);
    }
    const struct { int offset; const int *field; } INTS[2] = {{unitAt, &columns->unit}, {stripeAt, &columns->stripe}};
    for (int i = 0; i < 2; i++) {
        const unsigned char load[4] = {0x8b, 0x44, 0x24, (unsigned char)(INTS[i].offset + 8)};
        emitBytes(&p, load, 4);
        emitBytes(&p, "\x89\x05", 2);
        emitRel32(&p, INTS[i].field, 0);
    }
    emitBytes(&p, "\xff\x25\0\0\0\0", 6);
    emitBytes(&p, &hook, 8);
    return (int)(p - stub);
}

static int writeSkipStub(unsigned char *stub, int *skipped, const unsigned char *target) {
    unsigned char *p = stub;
    emitBytes(&p, "\xc7\x05", 2);
    emitRel32(&p, skipped, 4);
    emitBytes(&p, "\x01\0\0\0", 4);
    emitBytes(&p, "\xe9", 1);
    emitRel32(&p, target, 0);
    return (int)(p - stub);
}

#ifndef __APPLE__
// ---- Windows: the patch --------------------------------------------------------------------------
typedef struct { unsigned int rva, targetRva; void *hook; const char *name; } PathLoopCall;

static const PathLoopCall PATH_LOOP_CALLS[] = {
    {RVA_LEARN_START_SPELLS_CALL,     RVA_LEARN_START_SPELLS,  (void *)learnStartSpellsHook,  "learn_start_spells_1"},
    {RVA_LEARN_START_SPELLS_CALL_2,   RVA_LEARN_START_SPELLS,  (void *)learnStartSpellsHook,  "learn_start_spells_2"},
    {RVA_LEARN_START_SPELLS_CALL_3,   RVA_LEARN_START_SPELLS,  (void *)learnStartSpellsHook,  "learn_start_spells_3"},
    {RVA_LEARN_START_SPELLS_CALL_4,   RVA_LEARN_START_SPELLS,  (void *)learnStartSpellsHook,  "learn_start_spells_4"},
    {RVA_LEARN_START_SPELLS_CALL_5,   RVA_LEARN_START_SPELLS,  (void *)learnStartSpellsHook,  "learn_start_spells_5"},
    {RVA_LEARN_START_SPELLS_CALL_6,   RVA_LEARN_START_SPELLS,  (void *)learnStartSpellsHook,  "learn_start_spells_6"},
    {RVA_CHOOSE_MEMORIZED_CALL,       RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_1"},
    {RVA_CHOOSE_MEMORIZED_CALL_2,     RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_2"},
    {RVA_CHOOSE_MEMORIZED_CALL_3,     RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_3"},
    {RVA_CHOOSE_MEMORIZED_CALL_4,     RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_4"},
    {RVA_CHOOSE_MEMORIZED_CALL_5,     RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_5"},
    {RVA_CHOOSE_MEMORIZED_CALL_6,     RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_6"},
    {RVA_CHOOSE_MEMORIZED_CALL_7,     RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_7"},
    {RVA_CHOOSE_MEMORIZED_CALL_8,     RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_8"},
    {RVA_CHOOSE_MEMORIZED_CALL_9,     RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_9"},
    {RVA_CHOOSE_MEMORIZED_JUMP,       RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_jump_1"},
    {RVA_CHOOSE_MEMORIZED_JUMP_2,     RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_jump_2"},
    {RVA_CHOOSE_MEMORIZED_CALL_10,    RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_10"},
    {RVA_CHOOSE_MEMORIZED_CALL_11,    RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_11"},
    {RVA_CHOOSE_MEMORIZED_CALL_12,    RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_12"},
    {RVA_CHOOSE_MEMORIZED_CALL_13,    RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_13"},
    {RVA_CHOOSE_MEMORIZED_CALL_14,    RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_14"},
    {RVA_CHOOSE_MEMORIZED_CALL_15,    RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_15"},
    {RVA_CHOOSE_MEMORIZED_CALL_16,    RVA_CHOOSE_MEMORIZED,    (void *)chooseMemorizedHook,   "choose_memorized_16"},
    {RVA_LIBRARY_KNOWN_COUNT_CALL,    RVA_COUNT_KNOWN_SPELLS,  (void *)libraryKnownCountHook, "library_known_count"},
    {RVA_LIBRARY_ROWS_END_LAYOUT_CALL, RVA_LAYOUT_NEW,         (void *)libraryRowsEndHook,    "library_rows_end"},
    {RVA_STATS_KNOWN_END_LAYOUT_CALL, RVA_LAYOUT_NEW,          (void *)statsKnownEndHook,     "stats_known_end"},
};
#define PATH_LOOP_CALL_COUNT ((int)(sizeof PATH_LOOP_CALLS / sizeof PATH_LOOP_CALLS[0]))
#define PATH_LOOP_SITE_COUNT (PATH_LOOP_CALL_COUNT + 4)
#define PATH_LOOP_STUB_SIZE  16
#define COLUMN_STUB_AT       0x800
#define SKIP_STUB_AT         0x900
#define COLUMNS_AT           0xc00
_Static_assert(PATH_LOOP_CALL_COUNT * PATH_LOOP_STUB_SIZE <= COLUMN_STUB_AT, "the plain stubs end before the column stub");

static const unsigned char *liveWeaponTable(void) {
    const unsigned char *at = gameBase + RVA_FIND_WEAPON_TABLE_REF;
    if (at[0] != 0x48 || at[1] != 0x8d || at[2] != 0x2d) return NULL;
    int rel;
    memcpy(&rel, at + 3, 4);
    return at + 7 + rel;
}

static const unsigned char *liveSpellTable(void) {
    const unsigned char *at = gameBase + RVA_FIND_SPELL_TABLE_REF;
    if (at[0] != 0x48 || at[1] != 0x8d || at[2] != 0x2d) return NULL;
    int rel;
    memcpy(&rel, at + 3, 4);
    return at + 7 + rel;
}

static int liveSpellRowBase(void) {
    const unsigned char *at = gameBase + RVA_STATS_SPELL_ROW_ID;
    if (at[0] != 0x81 || at[1] != 0xc3) return 0;
    int base;
    memcpy(&base, at + 2, 4);
    return base;
}

static void setPathGameFunctions(const char **names) {
    pathGame.unitPathLevel = GAME_FN(int (*)(int, int), RVA_UNIT_PATH_LEVEL);
    pathGame.unitSpellWeaponLevel = GAME_FN(int (*)(int, int), RVA_UNIT_SPELL_WEAPON_LEVEL);
    pathGame.unitAbilityTotal = GAME_FN(int (*)(int, int, int, char *, int, int *), RVA_UNIT_ABILITY_TOTAL);
    pathGame.randomSpell = GAME_FN(int (*)(int, int, int), RVA_RANDOM_SPELL);
    pathGame.addKnownSpell = GAME_FN(int (*)(int, int), RVA_ADD_KNOWN_SPELL);
    pathGame.memorizeSpell = GAME_FN(int (*)(int, int), RVA_MEMORIZE_SPELL);
    pathGame.unmemorizeSpell = GAME_FN(int (*)(int, int), RVA_UNMEMORIZE_SPELL);
    pathGame.findSpell = GAME_FN(int (*)(const char *), RVA_FIND_SPELL);
    pathGame.random = GAME_FN(int (*)(int), RVA_RANDOM);
    pathGame.unitMaxHp = GAME_FN(int (*)(int), RVA_UNIT_MAX_HP);
    pathGame.unitHasFlag4 = GAME_FN(int (*)(int), RVA_UNIT_HAS_FLAG_4);
    pathGame.countMemorizedOfType = GAME_FN(int (*)(int, int, int), RVA_COUNT_MEMORIZED_OF_TYPE);
    pathGame.countKnownSpells = GAME_FN(int (*)(int, int, int), RVA_COUNT_KNOWN_SPELLS);
    pathGame.countSpells = GAME_FN(int (*)(int, int), RVA_COUNT_SPELLS);
    pathGame.unitOwner = GAME_FN(int (*)(int), RVA_UNIT_OWNER);
    pathGame.unitX = GAME_FN(int (*)(int), RVA_GET_UNIT_X);
    pathGame.unitY = GAME_FN(int (*)(int), RVA_GET_UNIT_Y);
    pathGame.libraryLevel = GAME_FN(int (*)(int, int, int), RVA_LIBRARY_LEVEL);
    pathGame.textButton = GAME_FN(void (*)(int, const char *, ...), RVA_TEXT_BUTTON);
    pathGame.disabledButton = GAME_FN(void (*)(int, const char *, ...), RVA_DISABLED_BUTTON);
    pathGame.tooltip = GAME_FN(void (*)(const char *, ...), RVA_TOOLTIP);
    pathGame.listGap = GAME_FN(void (*)(float), RVA_LIST_GAP);
    pathGame.layoutNew = GAME_FN(int (*)(void), RVA_LAYOUT_NEW);
    pathGame.learnStartSpells = GAME_FN(void (*)(int), RVA_LEARN_START_SPELLS);
    pathGame.chooseMemorized = GAME_FN(int (*)(int), RVA_CHOOSE_MEMORIZED);
    pathGame.sectionBegin = GAME_FN(void (*)(int), RVA_SECTION_BEGIN);
    pathGame.sectionEnd = GAME_FN(void (*)(void), RVA_SECTION_END);
    pathGame.rowBegin = GAME_FN(void (*)(int), RVA_ROW_BEGIN);
    pathGame.columnX = GAME_FN(void (*)(float), RVA_COLUMN_X);
    pathGame.rowStripe = GAME_FN(void (*)(int, int), RVA_ROW_STRIPE);
    pathGame.titleCell = GAME_FN(void (*)(int, const char *), RVA_TITLE_CELL);
    pathGame.headerText = GAME_FN(void (*)(int, const char *, ...), RVA_HEADER_TEXT);
    pathGame.textCell = GAME_FN(void (*)(int, const char *), RVA_TEXT_CELL);
    pathGame.spellNameButton = GAME_FN(void (*)(int, float, const char *), RVA_SPELL_NAME_BUTTON);
    pathGame.optionTooltip = GAME_FN(void (*)(const char *), RVA_OPTION_TOOLTIP);
    pathGame.memorizeAllowance = GAME_FN(int (*)(int, int), RVA_MEMORIZE_ALLOWANCE);
    pathGame.countMemorizedSpells = GAME_FN(int (*)(int, int), RVA_COUNT_MEMORIZED_SPELLS);
    pathGame.memorizedSlot = GAME_FN(int (*)(int, int), RVA_MEMORIZED_SLOT);
    pathGame.spellRangeBonus = GAME_FN(int (*)(int, int, int), RVA_SPELL_RANGE_BONUS);
    pathGame.damageText = GAME_FN(void (*)(int, unsigned long long, int, const char *), RVA_DAMAGE_TEXT);
    pathGame.formatAreaShape = GAME_FN(void (*)(int, char *, int), RVA_FORMAT_AREA_SHAPE);
    pathGame.knownSpells = gameBase + RVA_UNIT_KNOWN_SPELLS;
    pathGame.memorizedSpells = gameBase + RVA_UNIT_MEMORIZED_SPELLS;
    pathGame.spells = liveSpellTable();
    pathGame.weapons = liveWeaponTable();
    pathGame.spellRowBase = liveSpellRowBase();
    pathGame.pathNames = names;
    pathGame.pathCount = SPELL_PATH_COUNT;
}



static int queueSpellPathLoopWrites(unsigned char *stubs) {
    int matched = 0;
    for (int i = 0; i < PATH_LOOP_CALL_COUNT; i++) {
        const PathLoopCall *c = &PATH_LOOP_CALLS[i];
        unsigned char *call = gameBase + c->rva, *stub = stubs + PATH_LOOP_STUB_SIZE * i;
        int rel;
        memcpy(&rel, call + 1, 4);
        long long toStub = stub - (call + 5);
        if ((call[0] != 0xe8 && call[0] != 0xe9) || call + 5 + rel != gameBase + c->targetRva || toStub < INT32_MIN || toStub > INT32_MAX) {
            writeLogError("spell_path_loops_site_mismatch", "reason=bytes_differ group=call name=%s rva=0x%x", c->name, c->rva);
            continue;
        }
        if (c->hook == (void *)statsKnownEndHook) {
            stub = stubs + COLUMN_STUB_AT;
            toStub = stub - (call + 5);
            knownBoxColumns = (KnownBoxColumns *)(void *)(stubs + COLUMNS_AT);
            if (!writeColumnStub(stub, knownBoxColumns, c->hook, FRAME_STATS_SCREEN_UNIT, FRAME_STATS_SCREEN_STRIPE)) {
                writeLogError("spell_path_loops_site_mismatch", "reason=out_of_reach group=call name=%s rva=0x%x frame_out_of_reach=1", c->name, c->rva);
                continue;
            }
        } else {
            stub[0] = 0xff; stub[1] = 0x25; memset(stub + 2, 0, 4);
            memcpy(stub + 6, &c->hook, 8);
        }
        int out = (int)toStub;
        queuePathWrite(call + 1, &out, 4);
        matched++;
    }
    unsigned char *skip = gameBase + RVA_STATS_KNOWN_BOXES_SKIP, *skipStub = stubs + SKIP_STUB_AT;
    int skipRel;
    memcpy(&skipRel, skip + 2, 4);
    long long toSkipStub = skipStub - (skip + 6), skipBack = (skip + 6 + skipRel) - (skipStub + 15);
    if (skip[0] != 0x0f || skip[1] != 0x84 || toSkipStub < INT32_MIN || toSkipStub > INT32_MAX || skipBack < INT32_MIN || skipBack > INT32_MAX) {
        writeLogError("spell_path_loops_site_mismatch", "reason=bytes_differ group=branch name=stats_boxes_skip rva=0x%x", RVA_STATS_KNOWN_BOXES_SKIP);
    } else {
        writeSkipStub(skipStub, &((KnownBoxColumns *)(void *)(stubs + COLUMNS_AT))->skipped, skip + 6 + skipRel);
        int out = (int)toSkipStub;
        queuePathWrite(skip + 2, &out, 4);
        matched++;
    }
    if (liveSpellTable()) matched++;
    else writeLogError("spell_path_loops_site_mismatch", "reason=bytes_differ group=read name=spell_table rva=0x%x", RVA_FIND_SPELL_TABLE_REF);
    if (liveWeaponTable()) matched++;
    else writeLogError("spell_path_loops_site_mismatch", "reason=bytes_differ group=read name=weapon_table rva=0x%x", RVA_FIND_WEAPON_TABLE_REF);
    if (liveSpellRowBase() > 0) matched++;
    else writeLogError("spell_path_loops_site_mismatch", "reason=bytes_differ group=read name=spell_row_id rva=0x%x", RVA_STATS_SPELL_ROW_ID);
    return matched;
}
#endif
