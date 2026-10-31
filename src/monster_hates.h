#pragma once

#define HATES_WORD  "hates"
#define HATED_WORD  "hated"

#define HATED_KIND_NUMBER_MAX  6

static int hatedKindByName(const char *name, size_t len) {
    const char *const *names = (const char *const *)GAME_PTR(RVA_HATED_KIND_NAMES);
    for (int kind = 1; kind <= HATED_KIND_NUMBER_MAX; kind++)
        if (names[kind] && strlen(names[kind]) == len && !strncmp(names[kind], name, len)) return kind;
    return 0;
}

static int readHatedKind(const char *args) {
    const char *p = args;
    skipSpaces(&p);
    if (*p == '"') {
        const char *end = strchr(p + 1, '"');
        if (!end || !isLineEnd(end + 1)) return -1;
        int kind = hatedKindByName(p + 1, (size_t)(end - p - 1));
        return kind ? kind : -1;
    }
    int n;
    if (!readInt(&p, &n) || !isLineEnd(p) || n < 0 || n > HATED_KIND_NUMBER_MAX) return -1;
    return n;
}

static int setAbilityPair(int *pairs, int count, int ability, int value, int reuseZero) {
    int end = 0;
    while (end < count && pairs[2 * end]) {
        if (pairs[2 * end] == ability) {
            pairs[2 * end + 1] = value;
            return 1;
        }
        end++;
    }
    if (!value) return 1;
    for (int i = 0; reuseZero && i < end; i++)
        if (!pairs[2 * i + 1]) {
            pairs[2 * i] = ability;
            pairs[2 * i + 1] = value;
            return 1;
        }
    if (end >= count) return 0;
    pairs[2 * end] = ability;
    pairs[2 * end + 1] = value;
    if (end + 1 < count) pairs[2 * end + 2] = 0;
    return 1;
}

static void hatesWordLine(const char *word, int ability, const char *args, int mod, int level) {
    if (level != 2) return;
    int kind = readHatedKind(args);
    if (kind < 0) {
        refuseC5mLine(word, spellModName(mod), "bad_value", "line=%d text=%.40s min=0 max=%d", G32(RVA_MOD_LINE), args,
                      HATED_KIND_NUMBER_MAX);
        return;
    }
    int monster = G32(RVA_MONSTER_CURRENT), item = G32(RVA_ITEM_CURRENT), records = 0;
    unsigned char *record = monsterRecord(monster);
    if (record) {
        records++;
        if (!setAbilityPair((int *)(record + MONSTER_ABILITIES), MONSTER_ABILITY_PAIRS, ability, kind, 1))
            refuseC5mLine(word, spellModName(mod), "too_many_abilities", "line=%d monster=%.48s max=%d", G32(RVA_MOD_LINE),
                          (const char *)record, MONSTER_ABILITY_PAIRS);
    }
    if (item >= 0 && item < tableRecords(TABLE_ITEM)) {
        unsigned char *itemRecord = tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE;
        records++;
        if (!setAbilityPair((int *)(itemRecord + ITEM_ABILITIES), ITEM_ABILITY_PAIRS, ability, kind, 0))
            refuseC5mLine(word, spellModName(mod), "too_many_abilities", "line=%d item=%.48s max=%d", G32(RVA_MOD_LINE),
                          (const char *)itemRecord, ITEM_ABILITY_PAIRS);
    }
    if (!records) refuseC5mLine(word, spellModName(mod), "no_monster_or_item", "line=%d", G32(RVA_MOD_LINE));
}

static void hatesLine(const char *args, int mod, int level) {
    hatesWordLine(HATES_WORD, MONSTER_ABILITY_HATES, args, mod, level);
}

static void hatedLine(const char *args, int mod, int level) {
    hatesWordLine(HATED_WORD, MONSTER_ABILITY_HATED, args, mod, level);
}

static void hatesWordsInit(void) {
    int words = apiAddModCommand("hates", hatesLine) + apiAddModCommand("hated", hatedLine);
    writeLog("hates_words_installed", "words=%d of 2", words);
}
