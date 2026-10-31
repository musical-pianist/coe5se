#pragma once

#define NO_EXPLODE_WORD        "noexplode"
#define NO_EXPLODE_PARTS_MAX   64
#define NO_EXPLODE_PICKS       8
#define SUMMON_PART_SEPARATOR  " & "
#define NO_EXPLODE_ALL_PARTS   ~0ULL

typedef struct {
    int ritual, line;
    unsigned long long parts;
} NoExplodeString;

static struct {
    unsigned char *weapons;
    int weaponCount;
    NoExplodeString *strings;
    int stringCount, stringCapacity;
    int stringRitual;
    int rangeMod, rangeLine, rangeOldWeapon;
    struct {
        char *copy;
        const char *line;
    } picks[NO_EXPLODE_PICKS];
    int nextPick;
    unsigned long long summonParts;
    int summonPart;
} noExplode = {.stringRitual = -1, .rangeMod = -1};

static int (*noExplodeRandom)(int n);
static int (*noExplodeOpenDie)(int n);
static int (*noExplodeOpenDice)(int sides, int count);
typedef int (*SummonStringFn)(int caster, int percent, int player, int x, int y, char *string, int defctrl, int reliability,
                              int countPercent, int add, int failplayer);
static SummonStringFn noExplodeSummonString;
static void (*noExplodeTrace)(const char *format, ...);

static int plainDice(int sides, int count) {
    if (sides < 1 || count < 1) return 0;
    int sum = 0;
    for (int i = 0; i < count; i++) sum += noExplodeRandom(sides) + 1;
    return sum;
}

static int noExplodeWeaponDie(int n, int weapon) {
    if (weapon >= 0 && weapon < noExplode.weaponCount && noExplode.weapons[weapon]) return plainDice(n, 1);
    return noExplodeOpenDie(n);
}

static int summonParts(const char *text, unsigned long long *dice) {
    int parts = 0;
    *dice = 0;
    for (const char *part = text;; parts++) {
        const char *end = strstr(part, SUMMON_PART_SEPARATOR), *star = strchr(part, '*');
        size_t len = end ? (size_t)(end - part) : strlen(part);
        if (part[0] == '(' && memchr(part, ')', len)) {
            size_t skip = (size_t)((const char *)memchr(part, ')', len) - part) + 1;
            part += skip;
            len -= skip;
        }
        if (star && star >= part && star < part + len && memchr(part, 'd', (size_t)(star - part)) && parts < NO_EXPLODE_PARTS_MAX)
            *dice |= 1ULL << parts;
        if (!end) return parts + 1;
        part = end + sizeof SUMMON_PART_SEPARATOR - 1;
    }
}

static int lastRitualString(int ritual) {
    const char *strings = (const char *)ritualWordRecord(ritual) + RITUAL_STRINGS;
    int line = -1;
    while (line + 1 < RITUAL_STRINGS_MAX && strings[(size_t)(line + 1) * RITUAL_STRING_SIZE]) line++;
    return line;
}

static unsigned long long noExplodeLineParts(const char *line) {
    for (int i = 0; i < noExplode.stringCount; i++) {
        const NoExplodeString *s = &noExplode.strings[i];
        if ((const char *)ritualWordRecord(s->ritual) + RITUAL_STRINGS + (size_t)s->line * RITUAL_STRING_SIZE == line) return s->parts;
    }
    return 0;
}

static int addNoExplodeWeapon(int weapon) {
    int records = tableRecords(TABLE_WEAPON);
    if (noExplode.weaponCount != records) {
        unsigned char *weapons = realloc(noExplode.weapons, (size_t)records);
        if (!weapons) return 0;
        if (records > noExplode.weaponCount) memset(weapons + noExplode.weaponCount, 0, (size_t)(records - noExplode.weaponCount));
        noExplode.weapons = weapons;
        noExplode.weaponCount = records;
    }
    noExplode.weapons[weapon] = 1;
    return 1;
}

static int addNoExplodeString(int ritual, int line, unsigned long long parts) {
    for (int i = 0; i < noExplode.stringCount; i++)
        if (noExplode.strings[i].ritual == ritual && noExplode.strings[i].line == line) {
            noExplode.strings[i].parts |= parts;
            return 1;
        }
    if (!growList((void **)&noExplode.strings, &noExplode.stringCapacity, noExplode.stringCount + 1, sizeof *noExplode.strings)) return 0;
    noExplode.strings[noExplode.stringCount++] = (NoExplodeString){ritual, line, parts};
    return 1;
}

static void noexplodeStringLine(const char *args, int mod, int ritual) {
    int line = lastRitualString(ritual);
    if (ritual != noExplode.stringRitual || line < 0) {
        refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "no_string", "line=%d ritual=%.48s", G32(RVA_MOD_LINE),
                      (const char *)ritualWordRecord(ritual));
        return;
    }
    const char *text = (const char *)ritualWordRecord(ritual) + RITUAL_STRINGS + (size_t)line * RITUAL_STRING_SIZE, *p = args;
    unsigned long long dice, parts = 0;
    int count = summonParts(text, &dice);
    long long n;
    skipSpaces(&p);
    if (isLineEnd(p)) parts = dice;
    while (!isLineEnd(p)) {
        if (!readSpellNumber(&p, &n) || n < 1 || n > count) {
            refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "bad_part", "line=%d text=%.40s parts=%d", G32(RVA_MOD_LINE), args, count);
            return;
        }
        if (!(dice >> (n - 1) & 1)) {
            refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "no_dice", "line=%d part=%lld string=%.60s", G32(RVA_MOD_LINE), n, text);
            return;
        }
        parts |= 1ULL << (n - 1);
        skipSpaces(&p);
    }
    if (!parts) refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "no_dice", "line=%d string=%.60s", G32(RVA_MOD_LINE), text);
    else if (!addNoExplodeString(ritual, line, parts))
        refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
}

static void noexplodeRangeLine(int start, int mod) {
    if (start && noExplode.rangeMod >= 0) {
        refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "already_started", "line=%d start=%d", G32(RVA_MOD_LINE), noExplode.rangeLine);
    } else if (start) {
        noExplode.rangeMod = mod;
        noExplode.rangeLine = G32(RVA_MOD_LINE);
        noExplode.rangeOldWeapon = G32(RVA_WEAPON_CURRENT);
    } else if (noExplode.rangeMod < 0) {
        refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "no_start", "line=%d", G32(RVA_MOD_LINE));
    } else {
        noExplode.rangeMod = -1;
    }
}

static void endOpenNoExplodeRange(void) {
    if (noExplode.rangeMod < 0) return;
    refuseC5mLine(NO_EXPLODE_WORD, spellModName(noExplode.rangeMod), "no_end", "start=%d", noExplode.rangeLine);
    noExplode.rangeMod = -1;
}

static void noexplodeLine(const char *args, int mod, int level) {
    if (level != 2) return;
    char word[8];
    const char *p = args;
    int len = readWord(&p, word, sizeof word);
    if ((len == 5 && !strcmp(word, "start")) || (len == 3 && !strcmp(word, "end"))) {
        if (isLineEnd(p)) noexplodeRangeLine(word[0] == 's', mod);
        else refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "bad_value", "line=%d text=%.40s", G32(RVA_MOD_LINE), args);
        return;
    }
    int weapon = G32(RVA_WEAPON_CURRENT), ritual = G32(RVA_RITUAL_CURRENT);
    if (weapon >= 0 && weapon < tableRecords(TABLE_WEAPON)) {
        if (!isLineEnd(args))
            refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "bad_value", "line=%d text=%.40s", G32(RVA_MOD_LINE), args);
        else if (!addNoExplodeWeapon(weapon))
            refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
    } else if (ritual >= 0 && ritual < ritualSlotCount()) {
        noexplodeStringLine(args, mod, ritual);
    } else {
        refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "no_weapon_or_ritual", "line=%d", G32(RVA_MOD_LINE));
    }
}

static void noExplodeLineHook(const char *line, size_t len, int mod, int level) {
    if (level != 2) return;
    if (noExplode.rangeMod >= 0 && mod != noExplode.rangeMod) endOpenNoExplodeRange();
    int weapon = G32(RVA_WEAPON_CURRENT), ritual = G32(RVA_RITUAL_CURRENT), adding = len == sizeof "addstring" - 1 && !strncmp(line, "addstring", len);
    if (noExplode.rangeMod >= 0) {
        if (weapon != noExplode.rangeOldWeapon) noExplode.rangeOldWeapon = -2;
        if (weapon >= 0 && weapon < tableRecords(TABLE_WEAPON) && noExplode.rangeOldWeapon == -2 && !addNoExplodeWeapon(weapon))
            refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
        int next = ritual >= 0 && ritual < ritualSlotCount() ? lastRitualString(ritual) + 1 : RITUAL_STRINGS_MAX;
        if (adding && next < RITUAL_STRINGS_MAX && !addNoExplodeString(ritual, next, NO_EXPLODE_ALL_PARTS))
            refuseC5mLine(NO_EXPLODE_WORD, spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
    }
    if (adding) {
        noExplode.stringRitual = ritual;
        return;
    }
    if (len != sizeof "clearwspec" - 1 || strncmp(line, "clearwspec", len)) return;
    if (weapon >= 0 && weapon < noExplode.weaponCount) noExplode.weapons[weapon] = 0;
}

static void noExplodeBeforePass(int level) {
    if (level != 2) return;
    if (noExplode.weapons) memset(noExplode.weapons, 0, (size_t)noExplode.weaponCount);
    noExplode.stringCount = 0;
    noExplode.stringRitual = -1;
    noExplode.rangeMod = -1;
}

static void noExplodeAfterPass(int level) {
    if (level != 2) return;
    endOpenNoExplodeRange();
}

// ---- The game's side -----------------------------------------------------------------------------
static void notePickedString(char *copy, const char *line) {
    for (int i = 0; i < NO_EXPLODE_PICKS; i++)
        if (noExplode.picks[i].copy == copy) {
            noExplode.picks[i].line = line;
            return;
        }
    noExplode.picks[noExplode.nextPick].copy = copy;
    noExplode.picks[noExplode.nextPick].line = line;
    noExplode.nextPick = (noExplode.nextPick + 1) % NO_EXPLODE_PICKS;
}

#ifdef __aarch64__
// @platform-split ritual_string_copy: the cast copies its string with __strlcpy_chk on Mac, the game's own strlcpy on Windows
typedef size_t (*StringCopyFn)(char *copy, const char *line, size_t size, size_t copySize);
static StringCopyFn noExplodeCopy;
static size_t pickedStringCopyHook(char *copy, const char *line, size_t size, size_t copySize) {
    notePickedString(copy, line);
    return noExplodeCopy(copy, line, size, copySize);
}
#else
// @platform-split ritual_string_copy: the cast copies its string with __strlcpy_chk on Mac, the game's own strlcpy on Windows
typedef int (*StringCopyFn)(char *copy, const char *line, int size);
static StringCopyFn noExplodeCopy;
static int pickedStringCopyHook(char *copy, const char *line, int size) {
    notePickedString(copy, line);
    return noExplodeCopy(copy, line, size);
}
#endif

static int noExplodeSummon(int caster, int percent, int player, int x, int y, char *string, int defctrl, int reliability,
                           int countPercent, int add, int failplayer) {
    unsigned long long outerParts = noExplode.summonParts;
    int outerPart = noExplode.summonPart;
    noExplode.summonParts = 0;
    for (int i = 0; i < NO_EXPLODE_PICKS; i++)
        if (noExplode.picks[i].copy == string) noExplode.summonParts = noExplodeLineParts(noExplode.picks[i].line);
    noExplode.summonPart = 0;
    int made = noExplodeSummonString(caster, percent, player, x, y, string, defctrl, reliability, countPercent, add, failplayer);
    noExplode.summonParts = outerParts;
    noExplode.summonPart = outerPart;
    return made;
}

static void noExplodePartTrace(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int last = va_arg(ap, int);
    const char *part = va_arg(ap, const char *);
    va_end(ap);
    noExplode.summonPart++;
    noExplodeTrace(format, last, part);
}

static int noExplodeSummonDice(int sides, int count) {
    int part = noExplode.summonPart;
    if (part >= 1 && part <= NO_EXPLODE_PARTS_MAX && (noExplode.summonParts >> (part - 1) & 1)) return plainDice(sides, count);
    return noExplodeOpenDice(sides, count);
}

typedef struct {
    unsigned int callRva, targetRva;
    void *hook;
    const char *name;
} NoExplodeCall;

#ifndef __aarch64__
#define NO_EXPLODE_STUB_SIZE 0x20
static int pointWeaponDieCall(unsigned char *stub) {
    unsigned char *site = GAME_PTR(RVA_WEAPON_HIT_DAMAGE_DIE_CALL);
    int rel;
    memcpy(&rel, site + 1, 4);
    if (site[0] != 0xe8 || site + 5 + rel != GAME_PTR(RVA_DIE_OPEN_ENDED)) {
        writeLog("hook_skipped", "site=weapon_damage_die reason=not_the_expected_call address=%p", (void *)site);
        return 0;
    }
    void *hook = (void *)noExplodeWeaponDie;
    unsigned char code[17] = {0x8b, 0x55, 0x60, 0xff, 0x25};
    memcpy(code + 9, &hook, 8);
    rel = (int)(stub - (site + 5));
    if (!writeGameCode(stub, code, sizeof code) || !writeGameCode(site + 1, &rel, 4)) {
        writeLog("hook_skipped", "site=weapon_damage_die reason=code_unwritable address=%p", (void *)site);
        return 0;
    }
    return 1;
}

static int installNoExplodeCalls(const NoExplodeCall *calls, int count) {
    unsigned char *stubs = allocNear(GAME_PTR(RVA_WEAPON_HIT_DAMAGE_DIE_CALL));
    if (!stubs) {
        writeLog("hook_skipped", "site=no_explode reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    int ok = pointWeaponDieCall(stubs);
    for (int i = 0; i < count; i++)
        ok += redirectCall(calls[i].callRva, calls[i].targetRva, stubs + NO_EXPLODE_STUB_SIZE * (i + 1), calls[i].hook, calls[i].name, NULL);
    return ok;
}
#else
int coe5seNoExplodeWeaponDie(int n, int weapon);
__attribute__((used)) int coe5seNoExplodeWeaponDie(int n, int weapon) { return noExplodeWeaponDie(n, weapon); }
__asm__(".text\n.p2align 2\n_coe5seNoExplodeWeaponDieStub:\n"
        "  ldur w1, [x29, #-0x90]\n"
        "  b _coe5seNoExplodeWeaponDie\n");
extern const unsigned char coe5seNoExplodeWeaponDieStub[] __asm__("_coe5seNoExplodeWeaponDieStub");

static int pointNoExplodeCall(const NoExplodeCall *c) {
    if (blTarget(gameBase + c->callRva) != gameBase + c->targetRva) {
        writeLog("hook_skipped", "site=%s reason=not_the_expected_call va=0x%llx", c->name, c->callRva + 0x100000000ULL);
        return 0;
    }
    unsigned char *trampoline = newTrampoline(c->hook);
    uint32_t word = trampoline ? blTo(gameBase + c->callRva, trampoline) : 0;
    if (trampoline && writeGameCode(gameBase + c->callRva, &word, 4)) return 1;
    writeLog("hook_skipped", "site=%s reason=%s", c->name, trampoline ? "code_unwritable" : "no_padding");
    return 0;
}

static int installNoExplodeCalls(const NoExplodeCall *calls, int count) {
    NoExplodeCall weapon = {RVA_WEAPON_HIT_DAMAGE_DIE_CALL, RVA_DIE_OPEN_ENDED, (void *)coe5seNoExplodeWeaponDieStub, "weapon_damage_die"};
    int ok = pointNoExplodeCall(&weapon);
    for (int i = 0; i < count; i++) ok += pointNoExplodeCall(&calls[i]);
    return ok;
}
#endif

static void noExplodeInit(void) {
    noExplodeRandom = GAME_FN(int (*)(int), RVA_RANDOM);
    noExplodeOpenDie = GAME_FN(int (*)(int), RVA_DIE_OPEN_ENDED);
    noExplodeOpenDice = GAME_FN(int (*)(int, int), RVA_RANDOM_OPEN_DICE);
    noExplodeSummonString = GAME_FN(SummonStringFn, RVA_RITUAL_SUMMON_STRING);
    noExplodeTrace = GAME_FN(void (*)(const char *, ...), RVA_TRACE_C);
    noExplodeCopy = GAME_FN(StringCopyFn, RVA_STRING_COPY);
    int words = apiAddModCommand("noexplode", noexplodeLine);
    static const NoExplodeCall CALLS[] = {
        {RVA_RITUAL_STRING_COPY_CALL, RVA_STRING_COPY, (void *)pickedStringCopyHook, "ritual_string_copy"},
        {RVA_RITUAL_SUMMON_CALL, RVA_RITUAL_SUMMON_STRING, (void *)noExplodeSummon, "ritual_summon"},
        {RVA_RITUAL_FARSUMMON_CALL, RVA_RITUAL_SUMMON_STRING, (void *)noExplodeSummon, "ritual_farsummon"},
        {RVA_RITUAL_SUMMON_PART_TRACE_CALL, RVA_TRACE_C, (void *)noExplodePartTrace, "ritual_summon_part"},
        {RVA_RITUAL_SUMMON_DICE_CALL, RVA_RANDOM_OPEN_DICE, (void *)noExplodeSummonDice, "ritual_summon_dice"},
    };
    int count = (int)(sizeof CALLS / sizeof CALLS[0]), calls = 0;
    if (checkGameFunction("no_explode_check", "")) calls = installNoExplodeCalls(CALLS, count);
    writeLog("no_explode_installed", "words=%d of 1 calls=%d of %d", words, calls, count + 1);
}
