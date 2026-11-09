#pragma once

enum { KILL_REANIMATE, KILL_MUTATE };

typedef struct {
    int weapon, word, mask, custom, monster, line;
    char mod[64], name[MONSTER_NAME_SIZE];
} KillRule;

static KillRule *killRules;
static int killRuleCount, killRuleCapacity;
static int pendingMutateUnit = -1, pendingMutateMonster = -1;
static int (*soullessFormGame)(int unit);
static void (*nextKillTransform)(int unit, int monster, int attacker);

static const char *const KILL_WORDS[] = {"reanimate", "mutate"};

static int killRuleTakesCorpse(const KillRule *r, int left) {
    if (!r->mask && r->custom < 0) return 1;
    if (left < 0 || left == CORPSE_KIND_LEAVES_NONE) return 0;
    if (left > CORPSE_KIND_LEAVES_NONE) {
        int c = left - (CORPSE_STORED_CUSTOM - 1);
        if (r->custom >= 0) return r->custom == c;
        int base = c < customCorpseKindCount ? customCorpseKinds[c].base : CORPSE_BASE_BYPASS;
        return base >= 0 && (r->mask & 1 << base);
    }
    return r->custom < 0 && (r->mask & 1 << left);
}

static const KillRule *killRuleFor(int weapon, int word, int left) {
    const KillRule *any = NULL;
    for (int i = 0; i < killRuleCount; i++) {
        const KillRule *r = &killRules[i];
        if (r->weapon != weapon || r->word != word || r->monster < 0) continue;
        if (!r->mask && r->custom < 0) {
            if (!any) any = r;
        } else if (killRuleTakesCorpse(r, left)) return r;
    }
    return any;
}

static int killVictimCorpse(int unit) { return unit == lastCorpseUnit ? lastCorpseLeft : -1; }

static int reanimateForm(int unit, int weapon) {
    int form = soullessFormGame(unit);
    if (form < 0 || !killRuleCount) return form;
    const KillRule *r = killRuleFor(weapon, KILL_REANIMATE, killVictimCorpse(unit));
    return r ? r->monster : form;
}

static const unsigned char *mutateLogRecord(int weapon, int target) {
    const KillRule *r = killRuleCount ? killRuleFor(weapon, KILL_MUTATE, -1) : NULL;
    pendingMutateUnit = r ? target : -1;
    pendingMutateMonster = r ? r->monster : -1;
    return r ? monsterRecord(r->monster) : NULL;
}

static void killTransform(int unit, int monster, int attacker) {
    if (unit == pendingMutateUnit && pendingMutateMonster >= 0) monster = pendingMutateMonster;
    pendingMutateUnit = -1;
    nextKillTransform(unit, monster, attacker);
}

// ---- The words -------------------------------------------------------------------------------------
static void clearKillRules(int weapon) {
    int kept = 0;
    for (int i = 0; i < killRuleCount; i++)
        if (killRules[i].weapon != weapon) killRules[kept++] = killRules[i];
    killRuleCount = kept;
}

static int readKillQuoted(const char **p, char *out, size_t size) {
    if (**p != '"') return 0;
    const char *end = strchr(*p + 1, '"');
    size_t n = end ? (size_t)(end - *p - 1) : 0;
    if (!end || !n || n >= size) return 0;
    memcpy(out, *p + 1, n), out[n] = '\0', *p = end + 1;
    return 1;
}

static const char *killWordLineHook(const char *line, size_t len, int mod, int level) {
    if (level != 2) return line;
    if (len == sizeof "clearwspec" - 1 && !strncmp(line, "clearwspec", len)) {
        clearKillRules(G32(RVA_WEAPON_CURRENT));
        return line;
    }
    int word = len == 9 && !strncmp(line, "reanimate", len) ? KILL_REANIMATE : len == 6 && !strncmp(line, "mutate", len) ? KILL_MUTATE : -1;
    if (word < 0) return line;
    const char *p = line + len, *text = p;
    skipSpaces(&p);
    if (isLineEnd(p) || *p == '-' || (*p >= '0' && *p <= '9')) return line;
    int mask = 0, custom = -1, read = 1;
    char name[MONSTER_NAME_SIZE] = "";
    const char *start = p;
    if (*p != '"' || (readKillQuoted(&p, name, sizeof name) && (skipSpaces(&p), *p == '"'))) {
        p = start;
        read = readCorpseKind(&p, &mask, &custom);
        if (read < 0) {
            refuseC5mLine(KILL_WORDS[word], spellModName(mod), "unknown_corpse_kind", "line=%d name=%.63s", G32(RVA_MOD_LINE), unknownCorpseKindName);
            return NULL;
        }
        skipSpaces(&p);
        if (!read || word == KILL_MUTATE) {
            refuseC5mLine(KILL_WORDS[word], spellModName(mod), "bad_kind", "line=%d text=%.60s", G32(RVA_MOD_LINE), text);
            return NULL;
        }
        read = readKillQuoted(&p, name, sizeof name);
    } else read = name[0] != '\0';
    skipSpaces(&p);
    if (!read || !isLineEnd(p)) {
        refuseC5mLine(KILL_WORDS[word], spellModName(mod), "bad_value", "line=%d text=%.60s", G32(RVA_MOD_LINE), text);
        return NULL;
    }
    int weapon = G32(RVA_WEAPON_CURRENT);
    if (weapon < 0 || weapon >= tableRecords(TABLE_WEAPON)) {
        refuseC5mLine(KILL_WORDS[word], spellModName(mod), "no_weapon", "line=%d", G32(RVA_MOD_LINE));
        return NULL;
    }
    KillRule *r = NULL;
    for (int i = 0; i < killRuleCount && !r; i++)
        if (killRules[i].weapon == weapon && killRules[i].word == word && !killRules[i].mask && killRules[i].custom < 0 && !mask && custom < 0) r = &killRules[i];
    if (!r && !growList((void **)&killRules, &killRuleCapacity, killRuleCount + 1, sizeof *killRules)) {
        refuseC5mLine(KILL_WORDS[word], spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
        return NULL;
    }
    if (!r) r = &killRules[killRuleCount++];
    *r = (KillRule){.weapon = weapon, .word = word, .mask = mask, .custom = custom, .monster = -1, .line = G32(RVA_MOD_LINE)};
    snprintf(r->mod, sizeof r->mod, "%s", spellModName(mod));
    snprintf(r->name, sizeof r->name, "%s", name);
    return KILL_WORDS[word];
}

static void killWordsBeforePass(int level) {
    if (level != 2) return;
    killRuleCount = 0;
    pendingMutateUnit = -1;
}

static void killWordsAfterPass(int level) {
    if (level != 2) return;
    for (int i = 0; i < killRuleCount; i++) {
        KillRule *r = &killRules[i];
        r->monster = monsterByName(r->name);
        if (r->monster < 0)
            refuseC5mLine(KILL_WORDS[r->word], r->mod, "unknown_monster", "line=%d monster=%.48s", r->line, r->name);
    }
}

// ---- The calls -------------------------------------------------------------------------------------
#ifndef __aarch64__
static uintptr_t *killRspSlots;
enum { KILL_SLOT_FORM, KILL_SLOT_LOG };
static int killFrameWeapon(int slot) {
    return (int)(*(const long long *)(killRspSlots[slot] + 8 + FRAME_WEAPON_HIT_WEAPON) / WEAPON_SIZE);
}
static int reanimateFormHook(int unit) { return reanimateForm(unit, killFrameWeapon(KILL_SLOT_FORM)); }
static void mutateLogHook(int attacker, int target, int kind, const char *fmt, const char *name, const unsigned char *record) {
    const unsigned char *named = mutateLogRecord(killFrameWeapon(KILL_SLOT_LOG), target);
    GAME_FN(void (*)(int, int, int, const char *, ...), RVA_BATTLE_LOG)(attacker, target, kind, fmt, name, named ? named : record);
}

#define KILL_STUB_SIZE 0x20
static int installKillCalls(void) {
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION), *slots = stubs ? allocNear(stubs) : NULL;
    if (!slots) {
        writeLog("hook_skipped", "site=kill_transforms reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    killRspSlots = (uintptr_t *)slots;
    return redirectCall(RVA_REANIMATE_FORM_CALL_A, RVA_SOULLESS_FORM, stubs, (void *)reanimateFormHook, "reanimate_form_a",
                        &killRspSlots[KILL_SLOT_FORM]) +
           redirectCall(RVA_REANIMATE_FORM_CALL_B, RVA_SOULLESS_FORM, stubs, (void *)reanimateFormHook, "reanimate_form_b",
                        &killRspSlots[KILL_SLOT_FORM]) +
           redirectCall(RVA_MUTATE_LOG_CALL, RVA_BATTLE_LOG, stubs + KILL_STUB_SIZE, (void *)mutateLogHook, "mutate_log",
                        &killRspSlots[KILL_SLOT_LOG]);
}
#else
// @platform-split kill_transforms: Mac's stubs read the weapon from the caller's frame in asm; Windows' hooks read it through the stub's saved rsp
int coe5seReanimateForm(int unit, int weapon);
__attribute__((used)) int coe5seReanimateForm(int unit, int weapon) { return reanimateForm(unit, weapon); }
const unsigned char *coe5seMutateLogRecord(int weapon, int target);
__attribute__((used)) const unsigned char *coe5seMutateLogRecord(int weapon, int target) { return mutateLogRecord(weapon, target); }
__attribute__((used)) void *coe5seKillBattleLog;
__asm__(".text\n.p2align 2\n_coe5seReanimateFormStub:\n"
        "  ldr w1, [sp, #0xd0]\n"
        "  b _coe5seReanimateForm\n"
        ".p2align 2\n_coe5seMutateLogStub:\n"
        "  sub sp, sp, #0x40\n"
        "  stp x0, x1, [sp]\n"
        "  stp x2, x3, [sp, #0x10]\n"
        "  stp x29, x30, [sp, #0x20]\n"
        "  ldr w0, [sp, #0x110]\n"
        "  ldr w1, [sp, #8]\n"
        "  bl _coe5seMutateLogRecord\n"
        "  mov x9, x0\n"
        "  ldp x29, x30, [sp, #0x20]\n"
        "  ldp x2, x3, [sp, #0x10]\n"
        "  ldp x0, x1, [sp]\n"
        "  add sp, sp, #0x40\n"
        "  cbz x9, 1f\n"
        "  str x9, [sp, #8]\n"
        "1:\n"
        "  adrp x16, _coe5seKillBattleLog@PAGE\n"
        "  ldr x16, [x16, _coe5seKillBattleLog@PAGEOFF]\n"
        "  br x16\n");
extern const unsigned char coe5seReanimateFormStub[] __asm__("_coe5seReanimateFormStub");
extern const unsigned char coe5seMutateLogStub[] __asm__("_coe5seMutateLogStub");
_Static_assert(FRAME_WEAPON_HIT_WEAPON == 0xd0, "the stubs' immediates are FRAME_WEAPON_HIT_WEAPON's Mac column");

static int pointKillCall(unsigned int callRva, unsigned int targetRva, const void *stub, const char *name) {
    if (blTarget(gameBase + callRva) != gameBase + targetRva) {
        writeLog("hook_skipped", "site=%s reason=not_the_expected_call va=0x%llx", name, callRva + 0x100000000ULL);
        return 0;
    }
    unsigned char *trampoline = newTrampoline((void *)stub);
    uint32_t word = trampoline ? blTo(gameBase + callRva, trampoline) : 0;
    if (trampoline && writeGameCode(gameBase + callRva, &word, 4)) return 1;
    writeLog("hook_skipped", "site=%s reason=%s", name, trampoline ? "code_unwritable" : "no_padding");
    return 0;
}

static int installKillCalls(void) {
    coe5seKillBattleLog = gameBase + RVA_BATTLE_LOG;
    return pointKillCall(RVA_REANIMATE_FORM_CALL_A, RVA_SOULLESS_FORM, coe5seReanimateFormStub, "reanimate_form_a") +
           pointKillCall(RVA_REANIMATE_FORM_CALL_B, RVA_SOULLESS_FORM, coe5seReanimateFormStub, "reanimate_form_b") +
           pointKillCall(RVA_MUTATE_LOG_CALL, RVA_BATTLE_LOG, coe5seMutateLogStub, "mutate_log");
}
#endif

static void killTransformsInit(void) {
    soullessFormGame = GAME_FN(int (*)(int), RVA_SOULLESS_FORM);
    int calls = 0, transform = 0;
    if (checkGameFunction("kill_transforms_check", "")) {
        calls = installKillCalls();
        transform = wrapSite(SITE_BATTLE_TRANSFORM, (void *)killTransform, (void **)&nextKillTransform);
    }
    writeResultLog(calls == 3 && transform, "kill_transforms_installed", "calls=%d of 3 transform=%d", calls, transform);
}
