#pragma once
#include "record_words.h"

#define MONSTER_FLAG_SMALL          (1ULL << 13)
#define MONSTER_FLAG_COMBAT_CASTER  (1ULL << 19)

typedef struct {
    const char *word;
    unsigned long long bit;
} MonsterFlagWord;

static const MonsterFlagWord GAME_FLAG_WORDS[] = {
    {"lucky", 1ULL << 48},         {"diseaseres", 1ULL << 18},    {"human", 1ULL << 3},          {"undead", 1ULL << 4},
    {"demonic", 1ULL << 33},       {"animal", 1ULL << 11},        {"inanimate", 1ULL << 45},     {"holy", 1ULL << 14},
    {"immortal", 1ULL << 20},      {"acutesenses", 1ULL << 16},   {"spiritsight", 1ULL << 5},    {"badsight", 1ULL << 30},
    {"stealth", 1ULL << 17},       {"foreststealth", 1ULL << 44}, {"invisible", 1ULL << 22},     {"sleepres", 1ULL << 6},
    {"charmres", 1ULL << 7},       {"bluntres", 1ULL << 23},      {"slashres", 1ULL << 24},      {"pierceres", 1ULL << 25},
    {"ethereal", 1ULL << 26},      {"shield", 1ULL << 27},        {"largeshield", 1ULL << 28},   {"magicshield", 1ULL << 31},
    {"noheal", 1ULL << 8},         {"berserker", 1ULL << 40},     {"fastheal", 1ULL << 9},       {"noleader", 1ULL << 39},
    {"regeneration", 1ULL << 10},  {"allitemslots", 1ULL << 35},  {"weaponslots", 1ULL << 36},   {"noslots", 1ULL << 43},
    {"nobootslots", 1ULL << 47},   {"nonmaginvul", 1ULL << 49},   {"swallowres", 1ULL << 50},    {"banishsurv", 1ULL << 51},
    {"size1x1", 1ULL << 52},       {"size2x2", 1ULL << 53},       {"unaging", 1ULL << 54},       {"coldblood", 1ULL << 55},
    {"snowstealth", 1ULL << 56},   {"clumsy", 1ULL << 57},        {"nozoc", 1ULL << 58},         {"unimportant", 1ULL << 59},
    {"rearpos", 1ULL << 61},       {"frontpos", 1ULL << 60},      {"nocombat", 1ULL << 34},      {"treelook", 1ULL << 37},
    {"female", 1ULL << 38},        {"desertstealth", 1ULL << 41}, {"tiny", 1ULL << 12},          {"invul", 1ULL << 32},
    {"curseditem", 1ULL << 46},
};

static const MonsterFlagWord *gameFlagWord(const char *line, size_t len) {
    for (size_t i = 0; i < sizeof GAME_FLAG_WORDS / sizeof GAME_FLAG_WORDS[0]; i++)
        if (strlen(GAME_FLAG_WORDS[i].word) == len && !strncmp(GAME_FLAG_WORDS[i].word, line, len)) return &GAME_FLAG_WORDS[i];
    return NULL;
}

static int isFlagOff(const char *args) {
    skipSpaces(&args);
    return args[0] == '0' && isLineEnd(args + 1);
}

static int setMonsterFlag(unsigned long long bit, int on) {
    int monster = G32(RVA_MONSTER_CURRENT), item = G32(RVA_ITEM_CURRENT), changed = 0;
    unsigned char *record = monsterRecord(monster);
    unsigned long long *flags[2] = {
        record ? (unsigned long long *)(record + (RVA_MONSTER_FLAGS - RVA_MONSTER_TABLE)) : NULL,
        item >= 0 && item < tableRecords(TABLE_ITEM)
            ? (unsigned long long *)(tableAddress(TABLE_ITEM) + (RVA_ITEM_FLAGS - RVA_ITEM_TABLE) + (size_t)item * ITEM_SIZE) : NULL,
    };
    for (int i = 0; i < 2; i++) {
        if (!flags[i]) continue;
        *flags[i] = on ? *flags[i] | bit : *flags[i] & ~bit;
        changed++;
    }
    return changed;
}

static void monsterFlagLine(const char *word, unsigned long long bit, int on, int mod) {
    if (!setMonsterFlag(bit, on))
        refuseC5mLine(word, spellModName(mod), "no_monster_or_item", "line=%d", G32(RVA_MOD_LINE));
}

static void ownFlagWordLine(const char *word, unsigned long long bit, const char *args, int mod, int level) {
    if (level != 2) return;
    monsterFlagLine(word, bit, !isFlagOff(args), mod);
}

static void smallLine(const char *args, int mod, int level) {
    ownFlagWordLine("small", MONSTER_FLAG_SMALL, args, mod, level);
}

static void combatcasterLine(const char *args, int mod, int level) {
    ownFlagWordLine("combatcaster", MONSTER_FLAG_COMBAT_CASTER, args, mod, level);
}

static int clearRecordFlagWord(unsigned char *base, int record, const char *line, size_t len) {
    if (!base || (len == sizeof "miscslots" - 1 && !strncmp(line, "miscslots", len))) return 0;
    for (size_t i = 0; i < sizeof RECORD_WORDS / sizeof RECORD_WORDS[0]; i++) {
        const RecordWord *row = &RECORD_WORDS[i];
        if (row->record != record || strlen(row->word) != len || strncmp(row->word, line, len)) continue;
        if (row->kind != WORD_FLAG) return 0;
        unsigned long long flags = 0;
        memcpy(&flags, base + row->offset, row->size);
        flags &= ~row->value;
        memcpy(base + row->offset, &flags, row->size);
        return 1;
    }
    return 0;
}

static int flagOffLineHook(const char *line, size_t len, int mod, int level) {
    if (level != 2) return 0;
    if (!isFlagOff(line + len)) return 0;
    const MonsterFlagWord *flag = gameFlagWord(line, len);
    int monster = G32(RVA_MONSTER_CURRENT), item = G32(RVA_ITEM_CURRENT), weapon = G32(RVA_WEAPON_CURRENT), terrain = G32(RVA_TERRAIN_CURRENT), ritual = G32(RVA_RITUAL_CURRENT);
    unsigned char *monsterAt = monsterRecord(monster);
    unsigned char *itemAt = item >= 0 && item < tableRecords(TABLE_ITEM) ? tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE : NULL;
    if (flag && (monsterAt || itemAt)) {
        setMonsterFlag(flag->bit, 0);
        return 1;
    }
    int taken = clearRecordFlagWord(monsterAt, RECORD_MONSTER, line, len) + clearRecordFlagWord(itemAt, RECORD_ITEM, line, len) +
                clearRecordFlagWord(weapon >= 0 && weapon < tableRecords(TABLE_WEAPON) ? tableAddress(TABLE_WEAPON) + (size_t)weapon * WEAPON_SIZE : NULL, RECORD_WEAPON, line, len) +
                clearRecordFlagWord(terrain >= 0 && terrain < TERRAIN_TABLE_MAX ? GAME_PTR(RVA_TERRAIN_TABLE) + (size_t)terrain * TERRAIN_SIZE : NULL, RECORD_TERRAIN, line, len) +
                clearRecordFlagWord(ritual >= 0 && ritual < ritualSlotCount() ? gameBase + ritualTableRva() + (size_t)ritual * RITUAL_SIZE : NULL, RECORD_RITUAL, line, len);
    if (taken) return 1;
    if (!flag) return 0;
    monsterFlagLine(flag->word, flag->bit, 0, mod);
    return 1;
}

static int declinedFlagOffLine(const char *line) {
    size_t len = strcspn(line, " \t");
    return gameFlagWord(line, len) && isFlagOff(line + len);
}

static void monsterFlagWordsInit(void) {
    int words = apiAddModCommand("small", smallLine) + apiAddModCommand("combatcaster", combatcasterLine);
    writeResultLog(words == 2, "monster_flag_words_installed", "words=%d of 2", words);
}
