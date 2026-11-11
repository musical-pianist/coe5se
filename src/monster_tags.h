#pragma once

#define MONSTER_TAG_NAME_SIZE  32
#define MONSTER_KILL_VALUE     999
#define DMGTYPE_POLYMORPH      15
#define DMGTYPE_PETRIFY        24

enum { MONSTER_RES_KILL = 1, MONSTER_RES_POLYMORPH = 2, MONSTER_RES_PETRIFY = 4, MONSTER_RES_FEEBLE = 8 };
#define AFFLICTION_FEEBLEMINDED  (1ULL << 25)

typedef struct {
    int monster, tag;
} MonsterTagMark;

static struct {
    char (*names)[MONSTER_TAG_NAME_SIZE];
    int nameCount, nameCapacity;
    MonsterTagMark *marks;
    int markCount, markCapacity;
    unsigned char *res;
    int resSize;
    unsigned char *itemRes;
    int itemResSize;
} monsterTags;

static int monsterTagId(const char *name, int add) {
    for (int i = 0; i < monsterTags.nameCount; i++)
        if (!strcmp(monsterTags.names[i], name)) return i + 1;
    if (!add) return 0;
    if (!growList((void **)&monsterTags.names, &monsterTags.nameCapacity, monsterTags.nameCount + 1, sizeof *monsterTags.names))
        return -1;
    snprintf(monsterTags.names[monsterTags.nameCount], MONSTER_TAG_NAME_SIZE, "%s", name);
    return ++monsterTags.nameCount;
}

static int monsterHasTag(int monster, int tag) {
    for (int i = 0; i < monsterTags.markCount; i++)
        if (monsterTags.marks[i].monster == monster && monsterTags.marks[i].tag == tag) return 1;
    return 0;
}

static int unitMonsterNumber(int unit) {
    if (unit < 0 || unit >= UNIT_TABLE_MAX) return -1;
    return G16(RVA_UNIT_MONSTER + (size_t)unit * UNIT_SIZE);
}

static int unitHasTag(int unit, int tag) {
    int monster = unitMonsterNumber(unit);
    return monster >= 0 && monsterHasTag(monster, tag);
}

static int monsterResBits(int monster) {
    return monster >= 0 && monster < monsterTags.resSize ? monsterTags.res[monster] : 0;
}

static int itemResBits(int item) {
    return item >= 0 && item < monsterTags.itemResSize ? monsterTags.itemRes[item] : 0;
}

static int unitResBits(int unit) {
    int monster = unitMonsterNumber(unit), bits = monsterResBits(monster);
    if (monster < 0 || !monsterTags.itemResSize) return bits;
    for (int i = 0; i < UNIT_ITEM_SLOTS; i++)
        bits |= itemResBits((short)G16(RVA_UNIT_ITEMS + (size_t)unit * UNIT_SIZE + (size_t)i * 2));
    return bits;
}

static int weaponDamageType(int weapon) {
    if (weapon < 0 || weapon >= tableRecords(TABLE_WEAPON)) return -1;
    return *(int *)(tableAddress(TABLE_WEAPON) + (size_t)weapon * WEAPON_SIZE + WEAPON_DMGTYPE);
}

static int immunityStopsHit(int unit, int weapon, unsigned long long value) {
    int bits = unitResBits(unit);
    if (!bits) return 0;
    if ((bits & MONSTER_RES_KILL) && value == MONSTER_KILL_VALUE) return 1;
    int type = (bits & (MONSTER_RES_POLYMORPH | MONSTER_RES_PETRIFY)) ? weaponDamageType(weapon) : -1;
    return ((bits & MONSTER_RES_POLYMORPH) && type == DMGTYPE_POLYMORPH) || ((bits & MONSTER_RES_PETRIFY) && type == DMGTYPE_PETRIFY);
}

static int readTagName(const char **p, char *out, size_t size) {
    skipSpaces(p);
    if (**p != '"') return 0;
    const char *end = strchr(*p + 1, '"');
    if (!end || end == *p + 1) return 0;
    size_t len = (size_t)(end - (*p + 1));
    if (len >= size) return -1;
    memcpy(out, *p + 1, len);
    out[len] = 0;
    *p = end + 1;
    return (int)len;
}

static int readOnOff(const char *p) {
    skipSpaces(&p);
    if (isLineEnd(p) || (p[0] == '1' && isLineEnd(p + 1))) return 1;
    if (p[0] == '0' && isLineEnd(p + 1)) return 0;
    return -1;
}

static void removeMonsterTagMarks(int monster, int tag) {
    int kept = 0;
    for (int i = 0; i < monsterTags.markCount; i++)
        if (monsterTags.marks[i].monster != monster || (tag != -1 && monsterTags.marks[i].tag != tag)) monsterTags.marks[kept++] = monsterTags.marks[i];
    monsterTags.markCount = kept;
}

static int addMonsterTagMark(int monster, int tag) {
    if (!growList((void **)&monsterTags.marks, &monsterTags.markCapacity, monsterTags.markCount + 1, sizeof *monsterTags.marks))
        return 0;
    monsterTags.marks[monsterTags.markCount++] = (MonsterTagMark){monster, tag};
    return 1;
}

static void tagLine(const char *args, int mod, int level) {
    if (level != 2) return;
    const char *p = args;
    char name[MONSTER_TAG_NAME_SIZE];
    int len = readTagName(&p, name, sizeof name), monster = G32(RVA_MONSTER_CURRENT), on = len > 0 ? readOnOff(p) : -1;
    if (!monsterRecord(monster))
        refuseC5mLine("tag", spellModName(mod), "no_monster", "line=%d", G32(RVA_MOD_LINE));
    else if (len < 0)
        refuseC5mLine("tag", spellModName(mod), "too_long", "line=%d max=%d", G32(RVA_MOD_LINE), MONSTER_TAG_NAME_SIZE - 1);
    else if (!len)
        refuseC5mLine("tag", spellModName(mod), "no_name", "line=%d text=%.40s", G32(RVA_MOD_LINE), args);
    else if (on < 0)
        refuseC5mLine("tag", spellModName(mod), "bad_value", "line=%d text=%.40s", G32(RVA_MOD_LINE), args);
    else {
        int tag = monsterTagId(name, on);
        if (tag < 0) {
            refuseC5mLine("tag", spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
            return;
        }
        if (!on) {
            removeMonsterTagMarks(monster, tag);
            return;
        }
        if (monsterHasTag(monster, tag)) return;
        if (!addMonsterTagMark(monster, tag)) refuseC5mLine("tag", spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
    }
}

static int growMonsterRes(void) {
    int records = tableRecords(TABLE_MONSTER);
    if (monsterTags.resSize >= records) return 1;
    unsigned char *grown = realloc(monsterTags.res, (size_t)records);
    if (!grown) return 0;
    memset(grown + monsterTags.resSize, 0, (size_t)(records - monsterTags.resSize));
    monsterTags.res = grown;
    monsterTags.resSize = records;
    return 1;
}

static int growItemRes(void) {
    int records = tableRecords(TABLE_ITEM);
    if (monsterTags.itemResSize >= records) return 1;
    unsigned char *grown = realloc(monsterTags.itemRes, (size_t)records);
    if (!grown) return 0;
    memset(grown + monsterTags.itemResSize, 0, (size_t)(records - monsterTags.itemResSize));
    monsterTags.itemRes = grown;
    monsterTags.itemResSize = records;
    return 1;
}

static void immunityLine(const char *word, int bit, const char *args, int mod, int level) {
    if (level != 2) return;
    int monster = G32(RVA_MONSTER_CURRENT), item = G32(RVA_ITEM_CURRENT), on = !isFlagOff(args);
    int isMonster = monsterRecord(monster) != NULL, isItem = item >= 0 && item < tableRecords(TABLE_ITEM);
    if (!isMonster && !isItem) {
        refuseC5mLine(word, spellModName(mod), "no_monster_or_item", "line=%d", G32(RVA_MOD_LINE));
        return;
    }
    if ((isMonster && !growMonsterRes()) || (isItem && !growItemRes())) {
        refuseC5mLine(word, spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
        return;
    }
    if (isMonster && monster < monsterTags.resSize)
        monsterTags.res[monster] = (unsigned char)(on ? monsterTags.res[monster] | bit : monsterTags.res[monster] & ~bit);
    if (isItem)
        monsterTags.itemRes[item] = (unsigned char)(on ? monsterTags.itemRes[item] | bit : monsterTags.itemRes[item] & ~bit);
}

static void killresLine(const char *args, int mod, int level) {
    immunityLine("killres", MONSTER_RES_KILL, args, mod, level);
}
static void polymorphresLine(const char *args, int mod, int level) {
    immunityLine("polymorphres", MONSTER_RES_POLYMORPH, args, mod, level);
}
static void petrifyresLine(const char *args, int mod, int level) {
    immunityLine("petrifyres", MONSTER_RES_PETRIFY, args, mod, level);
}
static void feebleresLine(const char *args, int mod, int level) {
    immunityLine("feebleres", MONSTER_RES_FEEBLE, args, mod, level);
}

static unsigned long long woundBitsAfterImmunity(int unit, unsigned long long bits) {
    return (unitResBits(unit) & MONSTER_RES_FEEBLE) ? bits & ~AFFLICTION_FEEBLEMINDED : bits;
}

static int copystatsCarrySource(const char *line, int level) {
    if (level != 2 || !spellWordIs(line, strcspn(line, " \t\r\n"), "copystats")) return -1;
    if (!featureOn(FEATURE_WEAPON_TRAITS) && !featureOn(FEATURE_CORPSE_KINDS) && !featureOn(FEATURE_FACTION_WORDS)) return -1;
    int target = G32(RVA_MONSTER_CURRENT);
    if (!monsterRecord(target)) return -1;
    char text[OFFSET_NAME_TEXT_MAX] = "";
    const char *quote = strchr(line, '"');
    if (quote) readSpellQuoted(&quote, text, sizeof text);
    int source = monsterByName(text);
    return source == target || source > G32(RVA_MONSTER_COUNT) ? -1 : source;
}

static void carryCopiedMonsterWords(int source, int mod) {
    int target = G32(RVA_MONSTER_CURRENT), bits = monsterResBits(source), ok = 1;
    removeMonsterTagMarks(target, -1);
    int marks = monsterTags.markCount;
    for (int i = 0; i < marks && ok; i++)
        if (monsterTags.marks[i].monster == source) ok = addMonsterTagMark(target, monsterTags.marks[i].tag);
    if (bits && !growMonsterRes()) ok = 0;
    if (target < monsterTags.resSize) monsterTags.res[target] = (unsigned char)bits;
    if (!copyMonsterCorpseKind(source, target)) ok = 0;
    if (!copyMonsterFactionRefs(source, target)) ok = 0;
    if (!ok) refuseC5mLine("copystats", spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
}

static int copystatsItemCarrySource(const char *line, int level) {
    if (level != 2 || !spellWordIs(line, strcspn(line, " \t\r\n"), "copystats")) return -1;
    int target = G32(RVA_ITEM_CURRENT), last = G32(RVA_ITEM_COUNT);
    if (monsterRecord(G32(RVA_MONSTER_CURRENT)) || target < 0 || target >= tableRecords(TABLE_ITEM)) return -1;
    char text[OFFSET_NAME_TEXT_MAX] = "";
    const char *quote = strchr(line, '"');
    if (quote) readSpellQuoted(&quote, text, sizeof text);
    if (!text[0]) return -1;
    for (int i = 0; i <= last && i < tableRecords(TABLE_ITEM); i++)
        if (recordNameIs(tableAddress(TABLE_ITEM) + (size_t)i * ITEM_SIZE, ITEM_NAME_SIZE, text)) return i == target ? -1 : i;
    return -1;
}

static void carryCopiedItemWords(int source, int mod) {
    int target = G32(RVA_ITEM_CURRENT), bits = itemResBits(source);
    if (bits && !growItemRes()) refuseC5mLine("copystats", spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
    if (target < monsterTags.itemResSize) monsterTags.itemRes[target] = (unsigned char)bits;
}

static void clearspecLineHook(const char *line, int level) {
    if (level != 2 || !spellWordIs(line, strcspn(line, " \t\r\n"), "clearspec")) return;
    if (!featureOn(FEATURE_WEAPON_TRAITS) && !featureOn(FEATURE_CORPSE_KINDS)) return;
    int monster = G32(RVA_MONSTER_CURRENT);
    if (!monsterRecord(monster)) return;
    removeMonsterTagMarks(monster, -1);
    if (monster < monsterTags.resSize) monsterTags.res[monster] = 0;
    storeMonsterCorpseKind(monster, 0);
}

enum { ITEM_WORD_CLEARSPEC = 1, ITEM_WORD_CLEARMOVE, ITEM_WORD_CLEARWEAPONS, ITEM_WORD_MAKEAFF, ITEM_WORD_MAKEBEN, ITEM_WORD_USEABLE,
       ITEM_WORD_CONSUMABLE, ITEM_WORD_EXTRALIFE, ITEM_WORD_EYE, ITEM_WORD_HEART,
       ITEM_WORD_USEBEANSTALK, ITEM_WORD_USEPIT, ITEM_WORD_USEBREAKSEAL, ITEM_WORD_CUREONEAFF, ITEM_WORD_NEWSPELL1, ITEM_WORD_NEWSPELL2, ITEM_WORD_NEWSPELL3,
       ITEM_WORD_USEREVEAL, ITEM_WORD_USETELEPORT, ITEM_WORD_USEPLANETRAVEL, ITEM_WORD_USEONLYIN };
static const char *const itemBlockWords[] = {"clearspec", "clearmove", "clearweapons", "makeaff", "makeben", "useableitem",
                                              "consumableitem", "extralife", "eyeitem", "heartitem",
                                              "usebeanstalk", "usepit", "usebreakseal", "cureoneaff", "newspell1", "newspell2",
                                              "newspell3", "usereveal", "useteleport", "useplanetravel",
                                              "useonlyin"};
static int itemBlockWord(const char *line, size_t len) {
    for (int i = 0; i < (int)(sizeof itemBlockWords / sizeof *itemBlockWords); i++) {
        if (len == strlen(itemBlockWords[i]) && !strncmp(line, itemBlockWords[i], len)) return i + 1;
    }
    return 0;
}

static int itemWordLineHook(const char *line, size_t len, int mod, int level) {
    int word = level == 2 ? itemBlockWord(line, len) : 0;
    if (!word) return 0;
    if (G32(RVA_MONSTER_CURRENT) >= 0 || (word == ITEM_WORD_CLEARSPEC && G32(RVA_TERRAIN_CURRENT) >= 0)) return 0;
    int item = G32(RVA_ITEM_CURRENT);
    if (item < 0 || item >= tableRecords(TABLE_ITEM)) return 0;
    unsigned char *record = tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE;
    if (word == ITEM_WORD_MAKEAFF || word == ITEM_WORD_MAKEBEN) {
        unsigned long long bits = (unsigned long long)gameFirstNumber(line + len), have;
        unsigned char *field = record + (word == ITEM_WORD_MAKEAFF ? ITEM_AFFLICTIONS : ITEM_BLESSINGS);
        memcpy(&have, field, sizeof have);
        have |= bits;
        memcpy(field, &have, sizeof have);
        writeLog("item_word_added", "word=%.7s bits=0x%llx item=%d mod=%d", line, bits, item, mod);
        return 1;
    }
    if (word >= ITEM_WORD_USEABLE) {
        static const int abilities[] = {ITEM_ABILITY_USEABLE, ITEM_ABILITY_CONSUMABLE, ITEM_ABILITY_EXTRA_LIFE, ITEM_ABILITY_EYE,
                                        ITEM_ABILITY_HEART, ITEM_ABILITY_BEANSTALK, ITEM_ABILITY_PIT, ITEM_ABILITY_BREAK_SEAL,
                                        ITEM_ABILITY_CURE_AFFLICTION,
                                        ITEM_ABILITY_NEW_SPELL1, ITEM_ABILITY_NEW_SPELL2, ITEM_ABILITY_NEW_SPELL3,
                                        ITEM_ABILITY_REVEAL, ITEM_ABILITY_TELEPORT, ITEM_ABILITY_PLANE_TRAVEL,
                                        ITEM_ABILITY_USE_ONLY_IN};
        int ability = abilities[word - ITEM_WORD_USEABLE];
        int value = word >= ITEM_WORD_CUREONEAFF ? (int)gameFirstNumber(line + len) : !isFlagOff(line + len);
        if (word == ITEM_WORD_HEART && value) {
            long long n = gameFirstNumber(line + len);
            value = n > 0 ? (int)n : 1;
        }
        if (!setAbilityPair((int *)(record + ITEM_ABILITIES), ITEM_ABILITY_PAIRS, ability, value, 0)) {
            refuseC5mLine(itemBlockWords[word - 1], spellModName(mod), "too_many_abilities",
                          "line=%d item=%.48s max=%d", G32(RVA_MOD_LINE), (const char *)record, ITEM_ABILITY_PAIRS);
            return 1;
        }
        writeLog("item_word_added", "word=%.*s value=%d item=%d mod=%d", (int)len, line, value, item, mod);
        return 1;
    }
    if (word == ITEM_WORD_CLEARMOVE) {
        memset(record + ITEM_MOVE_FLAGS, 0, sizeof(unsigned long long));
    } else if (word == ITEM_WORD_CLEARWEAPONS) {
        int none = -1;
        memcpy(record + ITEM_WEAPON, &none, sizeof none);
    } else {
        memset(record + (RVA_ITEM_FLAGS - RVA_ITEM_TABLE), 0, sizeof(unsigned long long));
        memset(record + ITEM_AFFLICTIONS, 0, sizeof(unsigned long long));
        memset(record + ITEM_BLESSINGS, 0, sizeof(unsigned long long));
        memset(record + ITEM_ABILITIES, 0, sizeof(int));
        memset(record + ITEM_COMBAT_WORDS, 0, ITEM_COMBAT_WORDS_SIZE);
        if (item < monsterTags.itemResSize) monsterTags.itemRes[item] = 0;
    }
    writeLog("item_words_cleared", "word=%.*s item=%d mod=%d", (int)len, line, item, mod);
    return 1;
}

static int refusedItemSpellPathLine(const char *line, size_t len, int mod, int level) {
    int word = level == 2 ? itemBlockWord(line, len) : 0;
    if (word < ITEM_WORD_NEWSPELL1 || word > ITEM_WORD_NEWSPELL3 || !strchr(line + len, '"')) return 0;
    int item = G32(RVA_ITEM_CURRENT);
    if (G32(RVA_MONSTER_CURRENT) >= 0 || item < 0 || item >= tableRecords(TABLE_ITEM)) return 0;
    refuseC5mLine(itemBlockWords[word - 1], spellModName(mod), "path_on_item", "line=%d item=%d text=%.40s", G32(RVA_MOD_LINE), item, line + len);
    return 1;
}

static int declinedItemWordLine(const char *line, size_t len, int mod, int level) {
    static int inItem, blockMod = -1, blockLevel = -1;
    if (mod != blockMod || level != blockLevel) inItem = 0;
    blockMod = mod;
    blockLevel = level;
    if ((len == 7 && !strncmp(line, "newitem", len)) || (len == 10 && !strncmp(line, "selectitem", len))) inItem = 1;
    else if (isSpellBlockEnder(line, len)) inItem = 0;
    int word = itemBlockWord(line, len);
    return inItem && level == 2 && word && word < ITEM_WORD_CUREONEAFF;
}

static void monsterTagsBeforePass(int level) {
    if (level != 2) return;
    monsterTags.nameCount = 0;
    monsterTags.markCount = 0;
    if (monsterTags.res) memset(monsterTags.res, 0, (size_t)monsterTags.resSize);
    if (monsterTags.itemRes) memset(monsterTags.itemRes, 0, (size_t)monsterTags.itemResSize);
}

static int monsterTagsInit(void) {
    return apiAddModCommand("tag", tagLine) + apiAddModCommand("killres", killresLine) +
           apiAddModCommand("polymorphres", polymorphresLine) + apiAddModCommand("petrifyres", petrifyresLine) +
           apiAddModCommand("feebleres", feebleresLine);
}
