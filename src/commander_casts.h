#pragma once

#define CASTERMAXCAST_WORD       "castermaxcast"
#define COMMANDER_CAST_MAX       0x7d00
#define COMMANDER_CASTS_VERSION  1
#define RITUAL_FX_RESRESTRICT_ID 912
#define RITUAL_FX_BENRESTRICT_ID 924

static short *ritualCasterMaxCasts;
static int casterMaxCastLines;

typedef struct {
    int unit, ritual, count;
} CommanderCast;
static CommanderCast *commanderCasts;
static int commanderCastCount, commanderCastCapacity;

static int ritualCasterMaxCast(int ritual) {
    if (!ritualCasterMaxCasts || ritual < 0 || ritual >= RITUAL_SLOT_CAP) return 0;
    return ritualCasterMaxCasts[ritual];
}

static int unitRitualCasts(int unit, int ritual) {
    for (int i = 0; i < commanderCastCount; i++)
        if (commanderCasts[i].unit == unit && commanderCasts[i].ritual == ritual) return commanderCasts[i].count;
    return 0;
}

static int setUnitRitualCasts(int unit, int ritual, int count) {
    for (int i = 0; i < commanderCastCount; i++) {
        if (commanderCasts[i].unit != unit || commanderCasts[i].ritual != ritual) continue;
        if (count > 0) commanderCasts[i].count = count;
        else commanderCasts[i] = commanderCasts[--commanderCastCount];
        return 1;
    }
    if (count <= 0) return 1;
    if (!growList((void **)&commanderCasts, &commanderCastCapacity, commanderCastCount + 1, sizeof commanderCasts[0])) return 0;
    commanderCasts[commanderCastCount++] = (CommanderCast){unit, ritual, count};
    return 1;
}

static void dropCommanderCasts(int (*keep)(int unit, int arg), int arg) {
    int kept = 0;
    for (int i = 0; i < commanderCastCount; i++)
        if (keep(commanderCasts[i].unit, arg)) commanderCasts[kept++] = commanderCasts[i];
    commanderCastCount = kept;
}

static int keepOtherUnit(int unit, int other) { return unit != other; }
static int unitPlayerOf(int unit) {
    return unit >= 0 && unit < UNIT_TABLE_MAX ? (short)G16(RVA_UNIT_PLAYER + (size_t)unit * UNIT_SIZE) : -1;
}
static int keepOtherPlayersUnit(int unit, int player) { return unitPlayerOf(unit) != player; }

// ---- The word -----------------------------------------------------------------------------------
static void castermaxcastLine(const char *args, int mod, int level) {
    if (level != 2) return;
    const char *p = args;
    long long n = -1;
    if (!readSpellNumber(&p, &n) || (skipSpaces(&p), !isLineEnd(p)) || n < 0 || n > COMMANDER_CAST_MAX) {
        refuseC5mLine(CASTERMAXCAST_WORD, spellModName(mod), "bad_value", "line=%d text=%.40s min=0 max=%d", G32(RVA_MOD_LINE), args,
                      COMMANDER_CAST_MAX);
        return;
    }
    int ritual = G32(RVA_RITUAL_CURRENT);
    if (ritual < 0 || ritual >= ritualSlotCount()) {
        refuseC5mLine(CASTERMAXCAST_WORD, spellModName(mod), "no_ritual", "line=%d", G32(RVA_MOD_LINE));
        return;
    }
    if (!ritualCasterMaxCasts && !(ritualCasterMaxCasts = calloc(RITUAL_SLOT_CAP, sizeof ritualCasterMaxCasts[0]))) {
        refuseC5mLine(CASTERMAXCAST_WORD, spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
        return;
    }
    static unsigned char *set;
    if (!set && !(set = calloc(RITUAL_SLOT_CAP, 1))) {
        refuseC5mLine(CASTERMAXCAST_WORD, spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
        return;
    }
    if (casterMaxCastLines == 0) memset(set, 0, RITUAL_SLOT_CAP);
    casterMaxCastLines++;
    if (set[ritual]) {
        refuseC5mLine(CASTERMAXCAST_WORD, spellModName(mod), "already_set", "line=%d ritual=%.48s kept=%d", G32(RVA_MOD_LINE),
                      (const char *)ritualWordRecord(ritual), ritualCasterMaxCasts[ritual]);
        return;
    }
    set[ritual] = 1;
    ritualCasterMaxCasts[ritual] = (short)n;
}

static int copyCasterMaxCast(int source, int target) {
    if (!ritualCasterMaxCasts || source < 0 || source >= RITUAL_SLOT_CAP || target < 0 || target >= RITUAL_SLOT_CAP) return 1;
    ritualCasterMaxCasts[target] = ritualCasterMaxCasts[source];
    return 1;
}

static void commanderCastsBeforePass(int level) {
    if (level != 2) return;
    if (ritualCasterMaxCasts) memset(ritualCasterMaxCasts, 0, RITUAL_SLOT_CAP * sizeof ritualCasterMaxCasts[0]);
    casterMaxCastLines = 0;
}

// ---- The cast check -----------------------------------------------------------------------------
static int commanderCastEffect(int ritual, int id) {
    const unsigned char *record = ritualWordRecord(ritual) + RITUAL_FX;
    for (int s = 0; s < RITUAL_FX_MAX; s++) {
        int at = *(const int *)(record + (size_t)s * RITUAL_FX_SIZE);
        if (at < 1) break;
        if (at == id) return (int)*(const long long *)(record + (size_t)s * RITUAL_FX_SIZE + RITUAL_FX_VALUE);
    }
    return 0;
}

static unsigned long long (*commanderCastResistances)(int), (*commanderCastBenefits)(int);
typedef int (*RitualCanCastFn)(int commander, int ritual);
static RitualCanCastFn commanderCastGameCheck;

static int commanderCastEarlierRuleFails(int unit, int ritual) {
    int res = commanderCastEffect(ritual, RITUAL_FX_RESRESTRICT_ID), ben = commanderCastEffect(ritual, RITUAL_FX_BENRESTRICT_ID);
    if (res && ((unsigned long long)(long long)res & commanderCastResistances(unit))) return 1;
    if (ben && ((unsigned long long)(long long)ben & commanderCastBenefits(unit))) return 1;
    return 0;
}

static int commanderCastCheck(int commander, int ritual, int result) {
    int limit = ritualCasterMaxCast(ritual);
    if (limit < 1 || result == RITUAL_REASON_MAXCAST || result == RITUAL_REASON_IMMORTAL) return result;
    if (commander < 0 || commander >= COMMANDER_TABLE_MAX) return result;
    int unit = G32(RVA_COMMANDERS + (size_t)commander * COMMANDER_SIZE);
    if (unit < 0 || unit >= UNIT_TABLE_MAX || unitRitualCasts(unit, ritual) < limit) return result;
    if (result == -1 && commanderCastEarlierRuleFails(unit, ritual)) return result;
    return RITUAL_REASON_MAXCAST;
}

static RitualCanCastFn nextCommanderCastScreenCheck;

static int commanderCastScreenCheck(int commander, int ritual) {
    return commanderCastCheck(commander, ritual, nextCommanderCastScreenCheck(commander, ritual));
}

static int commanderCastOtherCheck(int commander, int ritual) {
    return commanderCastCheck(commander, ritual, commanderCastGameCheck(commander, ritual));
}

// ---- The count ----------------------------------------------------------------------------------
typedef long long (*CommanderCastExecuteFn)(int, int, int, int, int, int);
static CommanderCastExecuteFn nextCommanderCastExecute;

static int playerRitualCasts(int player, int ritual) {
    if (player < 0 || player >= PLAYER_SLOTS || ritual < 0 || ritual >= ritualSlotCount()) return -1;
    return *(const short *)(castCountsBase() + (size_t)player * PLAYER_BLOCK_SIZE + (size_t)ritual * 2);
}

static long long commanderCastExecute(int unit, int ritual, int x, int y, int percent, int pay) {
    int player = unitPlayerOf(unit), before = ritualCasterMaxCast(ritual) > 0 ? playerRitualCasts(player, ritual) : -1;
    long long result = nextCommanderCastExecute(unit, ritual, x, y, percent, pay);
    if (before < 0 || playerRitualCasts(player, ritual) <= before) return result;
    int count = unitRitualCasts(unit, ritual);
    if (count < COMMANDER_CAST_MAX && !setUnitRitualCasts(unit, ritual, count + 1))
        writeLogError("commander_cast_lost", "reason=out_of_memory unit=%d ritual=%d", unit, ritual);
    return result;
}

static int commanderCastFights;
static void commanderCastsFightsBegin(const CallInfo *call) { (void)call, commanderCastFights++; }
static void commanderCastsFightsEnd(const CallInfo *call) { (void)call, commanderCastFights -= commanderCastFights > 0; }

static void commanderCastsNewUnit(const CallInfo *call) {
    int unit = (int)call->result, commander = (int)call->args[3];
    if (unit < 0 || commander == -2 || commander == -3 || commanderCastFights) return;
    dropCommanderCasts(keepOtherUnit, unit);
}

static u64 (*nextCommanderCastsNewWorld)(u64);
static u64 commanderCastsNewWorld(u64 mode) {
    commanderCastCount = 0;
    return nextCommanderCastsNewWorld(mode);
}

// ---- In saves: CMXC -------------------------------------------------------------------------------
static int turnFileReadPlayer(void);

static int commanderCastWritten(const CommanderCast *e) {
    return saveTailPlayer < 0 || unitPlayerOf(e->unit) == saveTailPlayer;
}

static void addCommanderCastsChunk(SaveList *list) {
    int turn = saveTailPlayer >= 0;
    if (turn && netRoleNow() != NET_ROLE_CLIENT) return;
    if (!commanderCastCount && (turn ? !casterMaxCastLines : 1)) return;
    SaveBuffer chunk = {0};
    saveBufferInt(&chunk, COMMANDER_CASTS_VERSION);
    int entries = 0;
    for (int i = 0; i < commanderCastCount; i++) entries += commanderCastWritten(&commanderCasts[i]);
    saveBufferInt(&chunk, entries);
    for (int i = 0; i < commanderCastCount; i++) {
        if (!commanderCastWritten(&commanderCasts[i])) continue;
        saveBufferInt(&chunk, commanderCasts[i].unit);
        saveBufferInt(&chunk, commanderCasts[i].ritual);
        saveBufferInt(&chunk, commanderCasts[i].count);
    }
    writeLog("commander_casts_written", "file=%s entries=%d bytes=%zu", turn ? "turn" : "save", entries, chunk.size);
    saveListAdd(list, SAVE_TAG_CMXC, &chunk);
}

static void useCommanderCastsChunk(const SaveChunk *chunk, SaveFile kind) {
    int turn = kind == SAVE_FILE_TURN, player = turn ? turnFileReadPlayer() : -1;
    if (turn && !chunk) return;
    if (turn && (netRoleNow() != NET_ROLE_HOST || player < 0)) {
        writeLogWarning("commander_casts_refused", "reason=not_the_host_read");
        return;
    }
    if (!turn) commanderCastCount = 0;
    if (!chunk) return;
    SaveCursor c = {chunk->payload, chunk->payload + chunk->size, 0};
    int version = saveCursorInt(&c), entries = saveCursorInt(&c);
    if (c.failed || version != COMMANDER_CASTS_VERSION || entries < 0 || entries > chunk->size / 12) {
        writeLogWarning("commander_casts_read", "file=%s reason=%s version=%d", SAVE_FILE_NAMES[kind],
                        !c.failed && version != COMMANDER_CASTS_VERSION ? "other_version" : "bad_chunk", version);
        return;
    }
    if (turn) dropCommanderCasts(keepOtherPlayersUnit, player);
    int read = 0, skipped = 0;
    for (int i = 0; i < entries; i++) {
        int unit = saveCursorInt(&c), ritual = saveCursorInt(&c), count = saveCursorInt(&c);
        if (c.failed) break;
        if (unit < 0 || unit >= UNIT_TABLE_MAX || ritual < 0 || ritual >= ritualSlotCount() || count < 1 || count > COMMANDER_CAST_MAX ||
            (turn && unitPlayerOf(unit) != player)) {
            skipped++;
            continue;
        }
        if (setUnitRitualCasts(unit, ritual, count)) read++;
        else skipped++;
    }
    if (c.failed) writeLogWarning("commander_casts_read", "file=%s reason=bad_chunk read=%d", SAVE_FILE_NAMES[kind], read);
    else if (skipped) writeLogWarning("commander_casts_read", "file=%s reason=bad_entries read=%d skipped=%d player=%d", SAVE_FILE_NAMES[kind], read, skipped, player);
    else writeLog("commander_casts_read", "file=%s read=%d player=%d", SAVE_FILE_NAMES[kind], read, player);
}

// ---- The calls ----------------------------------------------------------------------------------
#ifdef __APPLE__
static int macRedirectCalls(unsigned int rva, void *hook, const char *name);

static int pointCommanderCastChecks(void) { return macRedirectCalls(RVA_RITUAL_CAN_CAST, (void *)commanderCastOtherCheck, "ritual_can_cast") == 8; }
#else
#define COMMANDER_CAST_STUB_SIZE 0x20

static int pointCommanderCastChecks(void) {
    static const struct {
        unsigned int callRva;
        const char *name;
    } CALLS[] = {
        {RVA_AI_RITUAL_CAN_CAST_CALL_A, "ai_apprspam_can_cast"}, {RVA_AI_RITUAL_CAN_CAST_CALL_B, "ai_plan_can_cast"},
        {RVA_AI_RITUAL_CAN_CAST_CALL_C, "ai_has_plan_can_cast"},   {RVA_AI_RITUAL_CAN_CAST_CALL_D, "ai_always_can_cast"},
        {RVA_REPEAT_RITUAL_CAN_CAST_CALL, "repeat_ritual_can_cast"},
    };
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION);
    if (!stubs) {
        writeLog("hook_skipped", "site=ritual_can_cast reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    int ok = 0, count = (int)(sizeof CALLS / sizeof CALLS[0]);
    for (int i = 0; i < count; i++)
        ok += redirectCall(CALLS[i].callRva, RVA_RITUAL_CAN_CAST, stubs + COMMANDER_CAST_STUB_SIZE * i, (void *)commanderCastOtherCheck,
                           CALLS[i].name, NULL);
    return ok == count;
}
#endif

static void commanderCastsInit(void) {
    int word = apiAddModCommand("castermaxcast", castermaxcastLine), screen = 0, others = 0, cast = 0, newUnit = 0, newWorld = 0;
    commanderCastResistances = GAME_FN(unsigned long long (*)(int), RVA_UNIT_RESISTANCES);
    commanderCastBenefits = GAME_FN(unsigned long long (*)(int), RVA_UNIT_BENEFITS);
    commanderCastGameCheck = GAME_FN(RitualCanCastFn, RVA_RITUAL_CAN_CAST);
    if (checkGameFunction("commander_casts_check", "")) {
        screen = wrapSite(SITE_RITUAL_SCREEN_CAN_CAST, (void *)commanderCastScreenCheck, (void **)&nextCommanderCastScreenCheck);
        others = screen && pointCommanderCastChecks();
        cast = wrapSite(SITE_EXECUTE_RITUAL, (void *)commanderCastExecute, (void **)&nextCommanderCastExecute);
        newUnit = apiObserve(SITE_NEW_UNIT, NULL, commanderCastsNewUnit) &&
                  apiObserve(SITE_DO_ALL_FIGHTS, commanderCastsFightsBegin, commanderCastsFightsEnd);
        newWorld = wrapSite(SITE_PLAY_GAME_NEW_WORLD, (void *)commanderCastsNewWorld, (void **)&nextCommanderCastsNewWorld);
    }
    writeResultLog(word && screen && others && cast && newUnit && newWorld, "commander_casts_installed",
                   "word=%d screen=%d others=%d cast=%d new_unit=%d new_world=%d", word, screen, others, cast, newUnit, newWorld);
}
