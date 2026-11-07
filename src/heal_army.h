#pragma once

#define HEAL_ARMY_WORD      "healarmy"
#define HEAL_ARMY_BYPASS_WORD "bypassnoheal"
#define HEAL_ARMY_DISEASED  1ULL
#define HEAL_RATE_FAST      2
#define RESIST_NOHEAL_BIT   (1ULL << 8)
#define RESIST_FASTHEAL_BIT (1ULL << 9)
#define ABILITY_FALSELIFE   0x1e9

typedef struct {
    int slot;
    int bypassNoheal;
    WeaponTraitRule trait;
} HealArmyRule;

static struct {
    HealArmyRule *rules;
    int count, capacity;
} healArmy;

static int (*healArmyPathLevel)(int unit, int path);
static unsigned long long (*healArmyUnitState)(int unit);
static int (*healArmyMaxHp)(int unit);
static int (*healArmyFalselife)(int unit);

static void healArmyReset(void) { healArmy.count = 0; }

static void healArmyBlockStored(int slot) {
    for (int i = 0; i < healArmy.count; i++)
        if (!healArmy.rules[i].slot) healArmy.rules[i].slot = slot;
}

static void healArmyDropPending(void) {
    int n = 0;
    for (int i = 0; i < healArmy.count; i++)
        if (healArmy.rules[i].slot) healArmy.rules[n++] = healArmy.rules[i];
    healArmy.count = n;
}

static int addHealArmyRule(int kind, unsigned long long value, const char *word, int bypassNoheal) {
    int slot = spellBlock.slot;
    for (int i = 0; i < healArmy.count; i++) {
        HealArmyRule *r = &healArmy.rules[i];
        if (r->slot == slot && r->trait.kind == kind && r->trait.value == value) {
            r->bypassNoheal |= bypassNoheal;
            return 1;
        }
    }
    if (!growList((void **)&healArmy.rules, &healArmy.capacity, healArmy.count + 1, sizeof *healArmy.rules)) return 0;
    HealArmyRule *r = &healArmy.rules[healArmy.count++];
    memset(r, 0, sizeof *r);
    r->slot = slot;
    r->bypassNoheal = bypassNoheal;
    r->trait.kind = kind;
    r->trait.value = value;
    snprintf(r->trait.word, sizeof r->trait.word, "%s", word);
    return 1;
}

static int healArmyWordTrait(const char *line, size_t len, char *trait, size_t size) {
    static const char PRE[] = "heal", POST[] = "army";
    size_t pre = sizeof PRE - 1, post = sizeof POST - 1;
    if (len <= pre + post || strncmp(line, PRE, pre) || strncmp(line + len - post, POST, post)) return 0;
    if ((len == sizeof "heallivingarmy" - 1 && !strncmp(line, "heallivingarmy", len)) ||
        (len == sizeof "healhumanarmy" - 1 && !strncmp(line, "healhumanarmy", len)))
        return 0;
    size_t n = len - pre - post;
    if (n >= size) n = size - 1;
    memcpy(trait, line + pre, n);
    trait[n] = 0;
    return 1;
}

static int readHealArmyBypass(const char *p) {
    char word[16];
    skipSpaces(&p);
    if (isLineEnd(p)) return 0;
    int len = readWord(&p, word, sizeof word);
    skipSpaces(&p);
    return len > 0 && (size_t)len < sizeof word && !strcmp(word, HEAL_ARMY_BYPASS_WORD) && isLineEnd(p) ? 1 : -1;
}

static void healArmyRuleLine(const char *word, const char *trait, const char *p, int mod, int level) {
    if (!spellBlockTakes(word, mod, level)) return;
    int kind, line = G32(RVA_MOD_LINE);
    unsigned long long value;
    char name[MONSTER_TAG_NAME_SIZE];
    const char *wordShown = trait;
    if (!trait[0]) {
        refuseC5mLine(word, spellModName(mod), "no_trait", "line=%d", line);
        noteSpellProblem(mod, line, "%s needs a trait", word);
        return;
    }
    if (!strcmp(trait, "tag")) {
        int len = readTagName(&p, name, sizeof name), tag = len > 0 ? monsterTagId(name, 1) : 0;
        if (len <= 0 || tag < 0) {
            refuseC5mLine(word, spellModName(mod), len < 0 ? "too_long" : tag < 0 ? "out_of_memory" : "no_name", "line=%d", line);
            noteSpellProblem(mod, line, "%s tag needs a name in quotes", word);
            return;
        }
        kind = WEAPON_TRAIT_TAG;
        value = (unsigned long long)tag;
        wordShown = name;
    } else if (!findWeaponTrait(trait, &kind, &value)) {
        refuseC5mLine(word, spellModName(mod), "unknown_trait", "line=%d trait=%.40s", line, trait);
        noteSpellProblem(mod, line, "%.40s isn't a monster trait", trait);
        return;
    }
    int bypassNoheal = readHealArmyBypass(p);
    if (bypassNoheal < 0) {
        refuseC5mLine(word, spellModName(mod), "bad_value", "line=%d text=%.40s", line, p);
        noteSpellProblem(mod, line, "only " HEAL_ARMY_BYPASS_WORD " can follow %s", word);
        return;
    }
    if (!addHealArmyRule(kind, value, wordShown, bypassNoheal)) {
        refuseC5mLine(word, spellModName(mod), "out_of_memory", "line=%d", line);
        return;
    }
    writeLog("heal_army_rule", "mod=%s slot=%d trait=%s bypassnoheal=%d line=%d", spellModName(mod), spellBlock.slot, wordShown, bypassNoheal, line);
}

static void healarmyLine(const char *args, int mod, int level) {
    char trait[WEAPON_TRAIT_WORD_SIZE];
    const char *p = args;
    int len = readWord(&p, trait, sizeof trait);
    if (len < 0 || (size_t)len >= sizeof trait) trait[0] = 'x', trait[1] = 0;
    healArmyRuleLine(HEAL_ARMY_WORD, len > 0 ? trait : "", p, mod, level);
}

static int healArmyLineHook(const char *line, size_t len, const char *args, int mod, int level) {
    char trait[WEAPON_TRAIT_WORD_SIZE + 1], word[64];
    if (!healArmyWordTrait(line, len, trait, sizeof trait)) return 0;
    snprintf(word, sizeof word, "%.*s", (int)(len < sizeof word ? len : sizeof word - 1), line);
    if (strlen(trait) >= WEAPON_TRAIT_WORD_SIZE) trait[0] = 'x', trait[1] = 0;
    healArmyRuleLine(word, trait, args, mod, level);
    return 1;
}

static int healArmyCasterHas(int caster, int slot) {
    const short *known = (const short *)GAME_PTR(RVA_UNIT_KNOWN_SPELLS + (size_t)caster * UNIT_SIZE);
    for (int k = 0; k < UNIT_KNOWN_SPELLS_MAX && known[k] >= 0; k++) {
        if (known[k] != slot) continue;
        const SpellRecord *s = spellSlot(slot);
        return s->path >= 0 && healArmyPathLevel(caster, s->path) >= s->level;
    }
    return 0;
}

static int healArmyHealsFast(int unit, int rate) {
    if (!healArmy.count || rate >= HEAL_RATE_FAST || unit < 0 || unit >= UNIT_TABLE_MAX) return 0;
    size_t at = (size_t)unit * UNIT_SIZE;
    if ((short)G16(RVA_UNIT_HP + at) >= healArmyMaxHp(unit) || (healArmyUnitState(unit) & HEAL_ARMY_DISEASED)) return 0;
    int match = 0;
    for (int i = 0; i < healArmy.count && !match; i++)
        if (healArmy.rules[i].slot && (rate >= 1 || healArmy.rules[i].bypassNoheal) && unitHasWeaponTrait(unit, &healArmy.rules[i].trait)) match = 1;
    if (!match) return 0;
    int x = G32(RVA_UNIT_X + at), y = G32(RVA_UNIT_X + 4 + at), player = (short)G16(RVA_UNIT_PLAYER + at), last = G32(RVA_UNIT_COUNT);
    for (int c = 0; c <= last && c < UNIT_TABLE_MAX; c++) {
        size_t cat = (size_t)c * UNIT_SIZE;
        if (G32(RVA_UNIT_X + cat) != x || G32(RVA_UNIT_X + 4 + cat) != y || (short)G16(RVA_UNIT_HP + cat) <= 0) continue;
        if (!playerSameTeam(player, (short)G16(RVA_UNIT_PLAYER + cat))) continue;
        for (int i = 0; i < healArmy.count; i++) {
            const HealArmyRule *r = &healArmy.rules[i];
            if (!r->slot || (rate < 1 && !r->bypassNoheal) || !unitHasWeaponTrait(unit, &r->trait) || !healArmyCasterHas(c, r->slot)) continue;
            writeLog("heal_army_heals", "unit=%d caster=%d spell=%d trait=%s rate=%d", unit, c, r->slot, r->trait.word, rate);
            return 1;
        }
    }
    return 0;
}

static void healArmyInfoLines(int spell) {
    char line[160];
    for (int i = 0; i < healArmy.count; i++) {
        const HealArmyRule *r = &healArmy.rules[i];
        if (r->slot != spell || !spell) continue;
        snprintf(line, sizeof line, "Knowing this spell grants increased healing rate for all %s beings in the army%s.", r->trait.word,
                 r->bypassNoheal ? ", even never healing ones" : "");
        textButton(0, line);
        listGap(4.0f);
    }
}

// ---- The game's side -----------------------------------------------------------------------------
static int gameHealArmyPathLevel(int unit, int path) { return GAME_FN(int (*)(int, int), RVA_UNIT_PATH_LEVEL)(unit, path); }
static unsigned long long gameHealArmyUnitState(int unit) { return GAME_FN(unsigned long long (*)(int), RVA_UNIT_STATE_FLAGS)(unit); }
static int gameHealArmyMaxHp(int unit) { return GAME_FN(int (*)(int), RVA_UNIT_MAX_HP)(unit); }
static int gameHealArmyFalselife(int unit) {
    return GAME_FN(int (*)(int, int, int, char *, int, int *), RVA_UNIT_ABILITY_TOTAL)(unit, ABILITY_FALSELIFE, 1, NULL, 0, NULL);
}

static void healArmyFlagLinesHook(int spell) {
    GAME_FN(void (*)(int), RVA_SPELL_FLAG_LINES)(spell);
    healArmyInfoLines(spell);
}

#ifndef __APPLE__
static int healArmyHealRateHook(int unit) {
    int rate = GAME_FN(int (*)(int), RVA_UNIT_HEAL_RATE)(unit);
    return healArmyHealsFast(unit, rate) ? HEAL_RATE_FAST : rate;
}

#define HEAL_ARMY_STUB_SIZE 0x20
static int installHealArmyCalls(void) {
    static const struct {
        unsigned int callRva, targetRva;
        void *hook;
        const char *name;
    } CALLS[] = {
        {RVA_UPKEEP_HEAL_RATE_CALL, RVA_UNIT_HEAL_RATE, (void *)healArmyHealRateHook, "upkeep_heal_rate"},
        {RVA_SPELL_FLAG_LINES_CALL_A, RVA_SPELL_FLAG_LINES, (void *)healArmyFlagLinesHook, "spell_flag_lines_a"},
        {RVA_SPELL_FLAG_LINES_CALL_B, RVA_SPELL_FLAG_LINES, (void *)healArmyFlagLinesHook, "spell_flag_lines_b"},
        {RVA_SPELL_FLAG_LINES_CALL_C, RVA_SPELL_FLAG_LINES, (void *)healArmyFlagLinesHook, "spell_flag_lines_c"},
    };
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION);
    if (!stubs) {
        writeLog("hook_skipped", "site=heal_army reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    int ok = 0;
    for (int i = 0; i < (int)(sizeof CALLS / sizeof CALLS[0]); i++)
        ok += redirectCall(CALLS[i].callRva, CALLS[i].targetRva, stubs + HEAL_ARMY_STUB_SIZE * i, CALLS[i].hook, CALLS[i].name, NULL);
    return ok == 4;
}
#else
// @platform-split heal_army: Mac's upkeep inlines RVA_UNIT_HEAL_RATE, so its RVA_UNIT_RESISTANCES call answers instead
static unsigned long long healArmyResistancesHook(int unit) {
    unsigned long long res = GAME_FN(unsigned long long (*)(int), RVA_UNIT_RESISTANCES)(unit);
    int rate = (res & RESIST_NOHEAL_BIT) && healArmyFalselife(unit) < 1 ? 0 : (res & RESIST_FASTHEAL_BIT) ? HEAL_RATE_FAST : 1;
    return healArmyHealsFast(unit, rate) ? (res & ~RESIST_NOHEAL_BIT) | RESIST_FASTHEAL_BIT : res;
}

static int installHealArmyCalls(void) {
    unsigned char *call = gameBase + RVA_UPKEEP_HEAL_RESIST_CALL;
    if (blTarget(call) != gameBase + RVA_UNIT_RESISTANCES) {
        writeLog("hook_skipped", "site=upkeep_heal_rate reason=not_the_expected_call va=0x%llx", RVA_UPKEEP_HEAL_RESIST_CALL + 0x100000000ULL);
        return 0;
    }
    unsigned char *trampoline = newTrampoline((void *)healArmyResistancesHook);
    uint32_t word = trampoline ? blTo(call, trampoline) : 0;
    if (!trampoline || !writeGameCode(call, &word, 4)) {
        writeLog("hook_skipped", "site=upkeep_heal_rate reason=%s", trampoline ? "code_unwritable" : "no_padding");
        return 0;
    }
    return macRedirectCalls(RVA_SPELL_FLAG_LINES, (void *)healArmyFlagLinesHook, "spell_flag_lines") > 0;
}
#endif

static void healArmyInit(void) {
    healArmyPathLevel = gameHealArmyPathLevel;
    healArmyUnitState = gameHealArmyUnitState;
    healArmyMaxHp = gameHealArmyMaxHp;
    healArmyFalselife = gameHealArmyFalselife;
    if (!weaponTraitUnitFlags) {
        weaponTraitUnitFlags = gameUnitResistances;
        weaponTraitUnitMoves = gameUnitMoveFlags;
        weaponTraitUnitAbility = gameUnitAbility;
    }
    int words = apiAddModCommand("healarmy", healarmyLine);
    int calls = installHealArmyCalls();
    writeResultLog(words == 1 && calls, "heal_army_installed", "words=%d of 1 calls=%d", words, calls);
}
