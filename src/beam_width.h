#pragma once

#define BEAM_WIDTH_WORD   "beamwidth"
#define BEAM_WIDTH_MAX    15
#define AREA_LEVELS       12
#define AREA_CELLS_MAX    0x7800
#define AREA_LEVEL_STRIDE 0x3c000
#define BATTLE_ROWS       20
#define BEAM_LINES_BIT    0x80
#define BEAM_LINES_WORD   "lines"

static struct {
    unsigned char *width;
    int size;
} beamWidths;

static int (*beamUnitAt)(int x, int y);

typedef struct {
    short x, y;
    signed char lane;
} BeamCell;
static struct {
    int unit, weapon, width, acrossRows, lines, count;
    BeamCell cells[AREA_CELLS_MAX];
} beamLast;

typedef struct {
    int weapon, toX, toY;
    unsigned char width, acrossRows, lines;
} BeamFly;
static BeamFly beamFly[FLY_WEAPONS_MAX];

static int beamLaneOffset(int lane) { return lane % 2 ? (lane + 1) / 2 : -(lane / 2); }

static void beamWidthBeforePass(int level) {
    if (level != 2 || !beamWidths.width) return;
    memset(beamWidths.width, 0, (size_t)beamWidths.size);
}

static int beamWidthOf(int weapon) {
    int w = weapon >= 0 && weapon < beamWidths.size ? beamWidths.width[weapon] & ~BEAM_LINES_BIT : 0;
    return w > 1 ? w : 1;
}

static int beamLinesOf(int weapon) { return weapon >= 0 && weapon < beamWidths.size && (beamWidths.width[weapon] & BEAM_LINES_BIT); }

static void beamwidthLine(const char *args, int mod, int level) {
    if (level != 2) return;
    const char *p = args;
    char word[16];
    int weapon = G32(RVA_WEAPON_CURRENT), len = readWord(&p, word, sizeof word), records = tableRecords(TABLE_WEAPON);
    char *end = NULL;
    long n = len > 0 && (size_t)len < sizeof word ? strtol(word, &end, 10) : 0;
    skipSpaces(&p);
    int lines = 0;
    if (!isLineEnd(p)) {
        char style[16];
        int styleLen = readWord(&p, style, sizeof style);
        lines = styleLen > 0 && (size_t)styleLen < sizeof style && !strcmp(style, BEAM_LINES_WORD) ? 1 : -1;
        skipSpaces(&p);
    }
    if (weapon < 0 || weapon >= records) {
        refuseC5mLine(BEAM_WIDTH_WORD, spellModName(mod), "no_weapon", "line=%d", G32(RVA_MOD_LINE));
        return;
    }
    if (!end || *end || n < 1 || n > BEAM_WIDTH_MAX || lines < 0 || !isLineEnd(p)) {
        refuseC5mLine(BEAM_WIDTH_WORD, spellModName(mod), "bad_value", "line=%d text=%.40s max=%d", G32(RVA_MOD_LINE), args, BEAM_WIDTH_MAX);
        return;
    }
    if (beamWidths.size != records) {
        unsigned char *w = realloc(beamWidths.width, (size_t)records);
        if (!w) {
            refuseC5mLine(BEAM_WIDTH_WORD, spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
            return;
        }
        if (records > beamWidths.size) memset(w + beamWidths.size, 0, (size_t)(records - beamWidths.size));
        beamWidths.width = w;
        beamWidths.size = records;
    }
    beamWidths.width[weapon] = (unsigned char)(n | (lines ? BEAM_LINES_BIT : 0));
    writeLog("beam_width_set", "mod=%s weapon=%d width=%ld lines=%d line=%d", spellModName(mod), weapon, n, lines, G32(RVA_MOD_LINE));
}

static int *areaLast(int level) { return (int *)GAME_PTR(RVA_AREA_LAST) + level; }
static int *areaCell(int level, int i) { return (int *)(GAME_PTR(RVA_AREA_CELLS) + (size_t)level * AREA_LEVEL_STRIDE) + 2 * i; }

static int areaListed(int level, int x, int y) {
    for (int i = 0; i <= *areaLast(level); i++)
        if (areaCell(level, i)[0] == x && areaCell(level, i)[1] == y) return 1;
    return 0;
}

static int addBeamLanes(int unit, int aoe, int level, int weapon, int count) {
    int width = beamWidthOf(weapon), band = aoe / 1000;
    beamLast.count = 0;
    if (width < 2 || (band != 1 && band != 5) || level < 0 || level >= AREA_LEVELS) return count;
    int n = *areaLast(level) + 1;
    if (n < 1 || n > AREA_CELLS_MAX) return count;
    int first[2] = {areaCell(level, 0)[0], areaCell(level, 0)[1]}, last[2] = {areaCell(level, n - 1)[0], areaCell(level, n - 1)[1]};
    int dx = last[0] - first[0], dy = last[1] - first[1];
    int acrossRows = (dx < 0 ? -dx : dx) >= (dy < 0 ? -dy : dy);
    int battleWidth = G32(RVA_BATTLE_WIDTH), added = 0;
    beamLast.unit = unit, beamLast.weapon = weapon, beamLast.width = width, beamLast.acrossRows = acrossRows, beamLast.lines = beamLinesOf(weapon);
    for (int i = 0; i < n; i++) beamLast.cells[i] = (BeamCell){(short)areaCell(level, i)[0], (short)areaCell(level, i)[1], 0};
    beamLast.count = n;
    for (int lane = 1; lane < width; lane++) {
        int offset = beamLaneOffset(lane);
        for (int i = 0; i < n; i++) {
            int x = areaCell(level, i)[0] + (acrossRows ? 0 : offset), y = areaCell(level, i)[1] + (acrossRows ? offset : 0);
            if (x < 0 || y < 0 || x >= battleWidth || y >= BATTLE_ROWS || areaListed(level, x, y)) continue;
            if (band == 1 && unit >= 0 && beamUnitAt(x, y) == unit) continue;
            int at = *areaLast(level) + 1;
            if (at >= AREA_CELLS_MAX) break;
            areaCell(level, at)[0] = x;
            areaCell(level, at)[1] = y;
            *areaLast(level) = at;
            beamLast.cells[beamLast.count++] = (BeamCell){(short)x, (short)y, (signed char)offset};
            added++;
        }
    }
    writeLog("beam_lanes_added", "unit=%d weapon=%d aoe=%d width=%d level=%d game=%d added=%d", unit, weapon, aoe, width, level, n, added);
    return *areaLast(level) + 1;
}

static int *flyField(int i, unsigned field) { return (int *)(GAME_PTR(RVA_FLY_WEAPONS) + (size_t)i * FLY_WEAPON_SIZE + field); }

static void beamFlyLaunched(int attacker, int weapon, int x, int y) {
    if (!beamLast.count || weapon != beamLast.weapon || attacker != beamLast.unit) return;
    int c = 0;
    while (c < beamLast.count && (beamLast.cells[c].x != x || beamLast.cells[c].y != y)) c++;
    if (c == beamLast.count) return;
    int lane = beamLast.cells[c].lane;
    for (int i = G32(RVA_FLY_WEAPONS_LAST); i >= 0; i--) {
        if (i >= FLY_WEAPONS_MAX || *flyField(i, FLY_WEAPON_ATTACKER) != attacker || *flyField(i, FLY_WEAPON_WEAPON) != weapon ||
            *flyField(i, FLY_WEAPON_TO_X) != 2 * x + 1 || *flyField(i, FLY_WEAPON_TO_Y) != 2 * y + 1 || *flyField(i, FLY_WEAPON_PROGRESS))
            continue;
        if (lane) {
            *flyField(i, FLY_WEAPON_MODE) = 0;
            beamFly[i].width = 0;
        } else {
            beamFly[i] = (BeamFly){weapon, 2 * x + 1, 2 * y + 1, (unsigned char)beamLast.width, (unsigned char)beamLast.acrossRows,
                                   (unsigned char)beamLast.lines};
        }
        return;
    }
}

#define BEAM_TRAIL_SLOTS     512
#define BEAM_TRAIL_SIZE_PER  4.0f

static void beamFlyDrawLanes(int index, void (*draw)(int index)) {
    const BeamFly *b = &beamFly[index];
    unsigned fromField = b->acrossRows ? FLY_WEAPON_FROM_Y : FLY_WEAPON_FROM_X, toField = b->acrossRows ? FLY_WEAPON_TO_Y : FLY_WEAPON_TO_X;
    int from = *flyField(index, fromField), to = *flyField(index, toField);
    for (int lane = 1; lane < b->width; lane++) {
        int shift = 2 * beamLaneOffset(lane);
        *flyField(index, fromField) = from + shift;
        *flyField(index, toField) = to + shift;
        draw(index);
    }
    *flyField(index, fromField) = from;
    *flyField(index, toField) = to;
}

static void beamFlyDraw(int index, void (*draw)(int index)) {
    int wide = index >= 0 && index < FLY_WEAPONS_MAX && beamFly[index].width >= 2;
    if (wide && (*flyField(index, FLY_WEAPON_WEAPON) != beamFly[index].weapon || *flyField(index, FLY_WEAPON_TO_X) != beamFly[index].toX ||
                 *flyField(index, FLY_WEAPON_TO_Y) != beamFly[index].toY))
        wide = beamFly[index].width = 0;
    if (!wide) {
        draw(index);
        return;
    }
    if (beamFly[index].lines) {
        draw(index);
        beamFlyDrawLanes(index, draw);
        return;
    }
    int from = G32(RVA_PARTICLE_FREE_HINT);
    if (from < 0) from = 0;
    unsigned char *particles = GAME_PTR(RVA_PARTICLES);
    unsigned char wasFree[BEAM_TRAIL_SLOTS];
    int slots = from + BEAM_TRAIL_SLOTS <= PARTICLES_MAX ? BEAM_TRAIL_SLOTS : PARTICLES_MAX > from ? PARTICLES_MAX - from : 0;
    for (int i = 0; i < slots; i++) wasFree[i] = *(unsigned short *)(particles + (size_t)(from + i) * PARTICLE_SIZE + PARTICLE_FLAGS) == 0xffff;
    draw(index);
    int to = G32(RVA_PARTICLE_FREE_HINT) - from;
    if (to > slots) to = slots;
    float grow = BEAM_TRAIL_SIZE_PER * (float)beamFly[index].width;
    for (int i = 0; i < to; i++) {
        unsigned char *p = particles + (size_t)(from + i) * PARTICLE_SIZE;
        if (wasFree[i] && *(unsigned short *)(p + PARTICLE_FLAGS) != 0xffff) *(float *)(p + PARTICLE_SIZE_PX) *= grow;
    }
}

static void beamWidthTextLine(int weapon) {
    int width = beamWidthOf(weapon);
    if (width < 2) return;
    char line[48];
    snprintf(line, sizeof line, "(beam %d squares wide)", width);
    textButton(0, line);
}

// ---- The game's side -----------------------------------------------------------------------------
typedef int (*AreaCellsFn)(int unit, int x, int y, int aoe, int level);
static int gameBeamUnitAt(int x, int y) { return GAME_FN(int (*)(int, int), RVA_BATTLE_UNIT_AT)(x, y); }

typedef int (*AddFlyWeaponFn)(int attacker, int weapon, unsigned long long dmg, int type, int fromX, int fromY, int toX, int toY, int n);
static int beamAddFlyWeapon(int attacker, int weapon, unsigned long long dmg, int type, int fromX, int fromY, int toX, int toY, int n) {
    int ok = GAME_FN(AddFlyWeaponFn, RVA_ADD_FLY_WEAPON)(attacker, weapon, dmg, type, fromX, fromY, toX, toY, n);
    if (ok) beamFlyLaunched(attacker, weapon, toX, toY);
    return ok;
}
static void gameFlyWeaponDrawFx(int index) { GAME_FN(void (*)(int), RVA_FLY_WEAPON_DRAW_FX)(index); }
static void beamFlyWeaponDrawFx(int index) { beamFlyDraw(index, gameFlyWeaponDrawFx); }

static int beamAreaCells(int unit, int x, int y, int aoe, int level, int weapon) {
    int count = GAME_FN(AreaCellsFn, RVA_WEAPON_AREA_CELLS)(unit, x, y, aoe, level);
    return addBeamLanes(unit, aoe, level, weapon, count);
}

#ifndef __aarch64__
static uintptr_t *beamRspSlots;
enum { BEAM_SLOT_SPELL, BEAM_SLOT_ATTACK, BEAM_SLOT_HIT };
static int frameInt(int slot, unsigned offset) { return *(const int *)(beamRspSlots[slot] + 8 + offset); }
static int beamSpellAreaHook(int unit, int x, int y, int aoe, int level) {
    return beamAreaCells(unit, x, y, aoe, level, frameInt(BEAM_SLOT_SPELL, FRAME_DOATTACK_SPELL_WEAPON));
}
static int beamAttackAreaHook(int unit, int x, int y, int aoe, int level) {
    return beamAreaCells(unit, x, y, aoe, level, frameInt(BEAM_SLOT_ATTACK, FRAME_DOATTACK_WEAPON));
}
static int beamHitAreaHook(int unit, int x, int y, int aoe, int level) {
    return beamAreaCells(unit, x, y, aoe, level, frameInt(BEAM_SLOT_HIT, FRAME_WEAPON_ATTACK_WEAPON));
}

#define BEAM_STUB_SIZE 0x20
static int installBeamCalls(void) {
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION), *slots = stubs ? allocNear(stubs) : NULL;
    if (!slots) {
        writeLog("hook_skipped", "site=beam_width reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    beamRspSlots = (uintptr_t *)slots;
    static const unsigned int FLY_CALLS[] = {RVA_DOATTACK_FLY_CALL_A, RVA_DOATTACK_FLY_CALL_B, RVA_DOATTACK_FLY_CALL_C,
                                             RVA_DOATTACK_FLY_CALL_D, RVA_DOATTACK_FLY_CALL_E, RVA_DOATTACK_FLY_CALL_F};
    static const unsigned int DRAW_CALLS[] = {RVA_FLY_STEP_DRAW_CALL_A, RVA_FLY_STEP_DRAW_CALL_B, RVA_FLY_STEP_DRAW_CALL_C};
    int fly = 0, draws = 0;
    for (size_t i = 0; i < sizeof FLY_CALLS / sizeof FLY_CALLS[0]; i++)
        fly += redirectCall(FLY_CALLS[i], RVA_ADD_FLY_WEAPON, stubs + 3 * BEAM_STUB_SIZE, (void *)beamAddFlyWeapon, "doattack_fly", NULL);
    for (size_t i = 0; i < sizeof DRAW_CALLS / sizeof DRAW_CALLS[0]; i++)
        draws += redirectCall(DRAW_CALLS[i], RVA_FLY_WEAPON_DRAW_FX, stubs + 4 * BEAM_STUB_SIZE, (void *)beamFlyWeaponDrawFx, "fly_step_draw", NULL);
    writeResultLog(fly == 6 && draws == 3, "beam_trail_installed", "launches=%d of 6 draws=%d of 3", fly, draws);
    return redirectCall(RVA_DOATTACK_SPELL_AREA_CALL, RVA_WEAPON_AREA_CELLS, stubs, (void *)beamSpellAreaHook, "doattack_spell_area",
                        &beamRspSlots[BEAM_SLOT_SPELL]) +
           redirectCall(RVA_DOATTACK_AREA_CALL, RVA_WEAPON_AREA_CELLS, stubs + BEAM_STUB_SIZE, (void *)beamAttackAreaHook, "doattack_area",
                        &beamRspSlots[BEAM_SLOT_ATTACK]) +
           redirectCall(RVA_WEAPON_ATTACK_AREA_CALL, RVA_WEAPON_AREA_CELLS, stubs + 2 * BEAM_STUB_SIZE, (void *)beamHitAreaHook,
                        "weapon_attack_area", &beamRspSlots[BEAM_SLOT_HIT]);
}
#else
// @platform-split beam_width: Mac's stubs read the weapon from the caller's frame in asm; Windows' hooks read it through the stub's saved rsp
int coe5seBeamAreaCells(int unit, int x, int y, int aoe, int level, int weapon);
__attribute__((used)) int coe5seBeamAreaCells(int unit, int x, int y, int aoe, int level, int weapon) {
    return beamAreaCells(unit, x, y, aoe, level, weapon);
}
__asm__(".text\n.p2align 2\n_coe5seBeamSpellAreaStub:\n"
        "  ldr x5, [sp, #0xb8]\n"
        "  ldr w5, [x5]\n"
        "  b _coe5seBeamAreaCells\n"
        ".p2align 2\n_coe5seBeamAttackAreaStub:\n"
        "  ldr w5, [sp, #0x108]\n"
        "  b _coe5seBeamAreaCells\n"
        ".p2align 2\n_coe5seBeamHitAreaStub:\n"
        "  ldr w5, [sp, #0x168]\n"
        "  b _coe5seBeamAreaCells\n");
extern const unsigned char coe5seBeamSpellAreaStub[] __asm__("_coe5seBeamSpellAreaStub");
extern const unsigned char coe5seBeamAttackAreaStub[] __asm__("_coe5seBeamAttackAreaStub");
extern const unsigned char coe5seBeamHitAreaStub[] __asm__("_coe5seBeamHitAreaStub");
_Static_assert(FRAME_DOATTACK_SPELL_WEAPON == 0xb8 && FRAME_DOATTACK_WEAPON == 0x108 && FRAME_WEAPON_ATTACK_WEAPON == 0x168,
               "the stubs' immediates are the frame entries' Mac columns");

static int pointBeamCall(unsigned int callRva, const void *stub, const char *name) {
    if (blTarget(gameBase + callRva) != gameBase + RVA_WEAPON_AREA_CELLS) {
        writeLog("hook_skipped", "site=%s reason=not_the_expected_call va=0x%llx", name, callRva + 0x100000000ULL);
        return 0;
    }
    unsigned char *trampoline = newTrampoline((void *)stub);
    uint32_t word = trampoline ? blTo(gameBase + callRva, trampoline) : 0;
    if (trampoline && writeGameCode(gameBase + callRva, &word, 4)) return 1;
    writeLog("hook_skipped", "site=%s reason=%s", name, trampoline ? "code_unwritable" : "no_padding");
    return 0;
}

static int installBeamCalls(void) {
    int fly = macRedirectCalls(RVA_ADD_FLY_WEAPON, (void *)beamAddFlyWeapon, "doattack_fly"),
        draws = macRedirectCalls(RVA_FLY_WEAPON_DRAW_FX, (void *)beamFlyWeaponDrawFx, "fly_step_draw");
    writeResultLog(fly == 7 && draws == 3, "beam_trail_installed", "launches=%d of 7 draws=%d of 3", fly, draws);
    return pointBeamCall(RVA_DOATTACK_SPELL_AREA_CALL, coe5seBeamSpellAreaStub, "doattack_spell_area") +
           pointBeamCall(RVA_DOATTACK_AREA_CALL, coe5seBeamAttackAreaStub, "doattack_area") +
           pointBeamCall(RVA_WEAPON_ATTACK_AREA_CALL, coe5seBeamHitAreaStub, "weapon_attack_area");
}
#endif

static void beamWidthInit(void) {
    beamUnitAt = gameBeamUnitAt;
    int words = apiAddModCommand("beamwidth", beamwidthLine);
    int calls = installBeamCalls();
    writeResultLog(words == 1 && calls == 3, "beam_width_installed", "words=%d of 1 calls=%d of 3", words, calls);
}
