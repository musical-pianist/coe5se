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

static int weaponDamageType(int weapon) {
    if (weapon < 0 || weapon >= tableRecords(TABLE_WEAPON)) return -1;
    return *(int *)(tableAddress(TABLE_WEAPON) + (size_t)weapon * WEAPON_SIZE + WEAPON_DMGTYPE);
}

static int immunityStopsHit(int unit, int weapon, unsigned long long value) {
    int bits = monsterResBits(unitMonsterNumber(unit));
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
            int kept = 0;
            for (int i = 0; i < monsterTags.markCount; i++)
                if (monsterTags.marks[i].monster != monster || monsterTags.marks[i].tag != tag) monsterTags.marks[kept++] = monsterTags.marks[i];
            monsterTags.markCount = kept;
            return;
        }
        if (monsterHasTag(monster, tag)) return;
        if (!growList((void **)&monsterTags.marks, &monsterTags.markCapacity, monsterTags.markCount + 1, sizeof *monsterTags.marks)) {
            refuseC5mLine("tag", spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
            return;
        }
        monsterTags.marks[monsterTags.markCount++] = (MonsterTagMark){monster, tag};
    }
}

static void immunityLine(const char *word, int bit, const char *args, int mod, int level) {
    if (level != 2) return;
    int monster = G32(RVA_MONSTER_CURRENT), on = readOnOff(args), records = tableRecords(TABLE_MONSTER);
    if (!monsterRecord(monster)) {
        refuseC5mLine(word, spellModName(mod), "no_monster", "line=%d", G32(RVA_MOD_LINE));
        return;
    }
    if (on < 0) {
        refuseC5mLine(word, spellModName(mod), "bad_value", "line=%d text=%.40s", G32(RVA_MOD_LINE), args);
        return;
    }
    if (monsterTags.resSize < records) {
        unsigned char *grown = realloc(monsterTags.res, (size_t)records);
        if (!grown) {
            refuseC5mLine(word, spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
            return;
        }
        memset(grown + monsterTags.resSize, 0, (size_t)(records - monsterTags.resSize));
        monsterTags.res = grown;
        monsterTags.resSize = records;
    }
    if (monster < monsterTags.resSize)
        monsterTags.res[monster] = (unsigned char)(on ? monsterTags.res[monster] | bit : monsterTags.res[monster] & ~bit);
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
    return (monsterResBits(unitMonsterNumber(unit)) & MONSTER_RES_FEEBLE) ? bits & ~AFFLICTION_FEEBLEMINDED : bits;
}

static void monsterTagsBeforePass(int level) {
    if (level != 2) return;
    monsterTags.nameCount = 0;
    monsterTags.markCount = 0;
    if (monsterTags.res) memset(monsterTags.res, 0, (size_t)monsterTags.resSize);
}

static int monsterTagsInit(void) {
    return apiAddModCommand("tag", tagLine) + apiAddModCommand("killres", killresLine) +
           apiAddModCommand("polymorphres", polymorphresLine) + apiAddModCommand("petrifyres", petrifyresLine) +
           apiAddModCommand("feebleres", feebleresLine);
}
