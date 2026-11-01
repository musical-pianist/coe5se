#pragma once

#define TEST_BATTLE_X 1
#define TEST_BATTLE_Y 1
#define TEST_BATTLE_TERRAIN 0
#define TEST_BATTLE_ENEMY_PLAYER 24
#define TEST_BATTLE_ENEMIES_MAX 200
#define TEST_FIELD_FIRE_RANGE 30
#define TEST_BATTLE_DELAY 400000

static void clearTestBattleSquare(int x, int y) {
    int last = G32(RVA_UNIT_COUNT);
    for (int u = 0; u <= last && u < UNIT_TABLE_MAX; u++) {
        unsigned char *at = GAME_PTR(RVA_UNIT_X + (size_t)u * UNIT_SIZE);
        if (*(int *)at == x && *(int *)(at + 4) == y) *(short *)GAME_PTR(RVA_UNIT_HP + (size_t)u * UNIT_SIZE) = 0;
    }
}

static struct {
    int open, unit, spell, weapon, standIn, x, y;
    unsigned char square[SQUARE_SIZE];
    short army, teamA, teamB;
    int mapW, mapH, activePlayer, delay, showFlag;
    unsigned char *spellWeaponRecord, spellWeaponSaved[WEAPON_SIZE];
} testField;

static unsigned char *testFieldSquare(void) {
    return GAME_PTR(RVA_SQUARE_RECORDS + ((size_t)testField.x * MAP_COLUMN_SQUARES + testField.y) * SQUARE_SIZE);
}

static void closeTestFieldNow(void) {
    if (!testField.open) return;
    int x = testField.x, y = testField.y;
    if (testField.spellWeaponRecord) memcpy(testField.spellWeaponRecord, testField.spellWeaponSaved, WEAPON_SIZE);
    clearTestBattleSquare(x, y);
    memcpy(testFieldSquare(), testField.square, SQUARE_SIZE);
    *(short *)GAME_PTR(RVA_SQUARE_ARMY + ((size_t)x * MAP_COLUMN_SQUARES + y) * 2) = testField.army;
    *(short *)GAME_PTR(RVA_SLOT_TEAM) = testField.teamA;
    *(short *)GAME_PTR(RVA_SLOT_TEAM + (size_t)TEST_BATTLE_ENEMY_PLAYER * PLAYER_BLOCK_SIZE) = testField.teamB;
    *(int *)GAME_PTR(RVA_ACTIVE_PLAYER) = testField.activePlayer;
    *(int *)GAME_PTR(RVA_BATTLE_DELAY) = testField.delay;
    *(int *)GAME_PTR(RVA_MAP_W) = testField.mapW;
    *(int *)GAME_PTR(RVA_MAP_H) = testField.mapH;
#ifdef __APPLE__
    *GAME_PTR(RVA_TEST_GAME_FLAG) = (unsigned char)testField.showFlag;
#else
    *(int *)GAME_PTR(RVA_TEST_GAME_FLAG) = testField.showFlag;
#endif
    testField.open = 0;
    writeLog("test_field_closed", "x=%d y=%d", x, y);
}

static int lOpenTestField(lua_State *L) {
    LuaScript *s = checkLuaScript(L);
    lua_Integer monster = luaL_checkinteger(L, 1), spell = luaL_optinteger(L, 2, -1);
    lua_Integer enemy = luaL_checkinteger(L, 3), count = luaL_optinteger(L, 4, 40), standIn = luaL_optinteger(L, 5, -1);
    int spellWeapon = spell >= 0 && spell < spellTableRecords() ? spellSlot((int)spell)->weapon : -1;
    int standInMaker = standIn >= 0 && standIn < WEAPON_RECORDS_MOVED ? weaponMadeBy[standIn] - 1 : -1;
    const char *reason = G32(RVA_ACTIVE_PLAYER) >= 0                        ? "in_game"
                       : testField.open                                     ? "already_open"
                       : !isUsedMonster(monster)                            ? "bad_monster"
                       : !isUsedMonster(enemy)                              ? "bad_enemy"
                       : count < 1 || count > TEST_BATTLE_ENEMIES_MAX       ? "bad_count"
                       : spell < -1 || spell >= spellTableRecords()         ? "bad_spell"
                       : standIn >= 0 && (spellWeapon < 0 || spellWeapon >= tableRecords(TABLE_WEAPON)) ? "no_spell_weapon"
                       : standIn >= 0 && (standInMaker < 0 || standInMaker > G32(RVA_MOD_LAST_INDEX) || _stricmp(modName(standInMaker), s->mod))
                           ? "not_own_weapon"
                       : standIn < -1                                       ? "bad_weapon"
                                                                            : NULL;
    if (reason) {
        refuseLuaCall(s->mod, "coe5.openTestField", reason, "monster=%lld spell=%lld enemy=%lld count=%lld", (long long)monster,
                      (long long)spell, (long long)enemy, (long long)count);
        lua_pushboolean(L, 0);
        return 1;
    }
    int x = testField.x = TEST_BATTLE_X, y = testField.y = TEST_BATTLE_Y;
    unsigned char *square = testFieldSquare();
    memcpy(testField.square, square, SQUARE_SIZE);
    memset(square, 0, SQUARE_SIZE);
    *(short *)(square + (RVA_SQUARES - RVA_SQUARE_RECORDS)) = TEST_BATTLE_TERRAIN;
    clearTestBattleSquare(x, y);

    int commander = GAME_FN(int (*)(int, int, int, int), RVA_NEW_COMMANDER)(x, y, 0, (int)monster);
    int unit = commander >= 0 && commander <= G32(RVA_LAST_COMMANDER) ? G32(RVA_COMMANDERS + (size_t)commander * COMMANDER_SIZE) : -1;
    int made = 0;
    for (int i = 0; unit >= 0 && i < count; i++)
        made += GAME_FN(int (*)(int, int, int, int, int), RVA_NEW_UNIT)(x, y, TEST_BATTLE_ENEMY_PLAYER, -1, (int)enemy) >= 0;
    if (unit < 0 || !made) {
        clearTestBattleSquare(x, y);
        memcpy(square, testField.square, SQUARE_SIZE);
        refuseLuaCall(s->mod, "coe5.openTestField", "no_free_slot", "monster=%lld enemy=%lld", (long long)monster, (long long)enemy);
        lua_pushboolean(L, 0);
        return 1;
    }
    testField.open = 1;
    testField.unit = unit;
    testField.spell = (int)spell;
    testField.weapon = spell >= 0 ? spellWeapon : *(const int *)(monsterRecord((int)monster) + MONSTER_ATTACKS + MONSTER_ATTACK_WEAPON);
    short *teamA = (short *)GAME_PTR(RVA_SLOT_TEAM), *teamB = (short *)GAME_PTR(RVA_SLOT_TEAM + (size_t)TEST_BATTLE_ENEMY_PLAYER * PLAYER_BLOCK_SIZE);
    testField.teamA = *teamA;
    testField.teamB = *teamB;
    *teamA = 100;
    *teamB = 100 + TEST_BATTLE_ENEMY_PLAYER;
    *(short *)(square + SQUARE_ARMY_OWNER) = 0;
    *(short *)(square + SQUARE_HOLDER) = TEST_BATTLE_ENEMY_PLAYER;
    short *army = (short *)GAME_PTR(RVA_SQUARE_ARMY + ((size_t)x * MAP_COLUMN_SQUARES + y) * 2);
    testField.army = *army;
    *army = -1;
    if (spell >= 0) {
        GAME_FN(int (*)(int, int), RVA_ADD_KNOWN_SPELL)(unit, (int)spell);
        short *memorized = (short *)GAME_PTR(RVA_UNIT_MEMORIZED_SPELLS + (size_t)unit * UNIT_SIZE);
        memorized[0] = (short)spell;
        memorized[1] = -1;
    }
    testField.mapW = G32(RVA_MAP_W);
    testField.mapH = G32(RVA_MAP_H);
    if (testField.mapW <= x + 1) *(int *)GAME_PTR(RVA_MAP_W) = x + 2;
    if (testField.mapH <= y + 1) *(int *)GAME_PTR(RVA_MAP_H) = y + 2;
    testField.activePlayer = G32(RVA_ACTIVE_PLAYER);
    testField.delay = G32(RVA_BATTLE_DELAY);
#ifdef __APPLE__
    testField.showFlag = *GAME_PTR(RVA_TEST_GAME_FLAG);
    *GAME_PTR(RVA_TEST_GAME_FLAG) = 1;
#else
    testField.showFlag = G32(RVA_TEST_GAME_FLAG);
    *(int *)GAME_PTR(RVA_TEST_GAME_FLAG) = 1;
#endif
    *(int *)GAME_PTR(RVA_ACTIVE_PLAYER) = 0;
    *(int *)GAME_PTR(RVA_BATTLE_DELAY) = TEST_BATTLE_DELAY;
    *(int *)GAME_PTR(RVA_BATTLE_X) = x;
    *(int *)GAME_PTR(RVA_BATTLE_Y) = y;
    *(int *)GAME_PTR(RVA_BATTLE_PLAYER_A) = 0;
    *(int *)GAME_PTR(RVA_BATTLE_PLAYER_B) = TEST_BATTLE_ENEMY_PLAYER;
    testField.spellWeaponRecord = NULL;
    testField.standIn = (int)standIn;
    if (standIn >= 0) {
        testField.spellWeaponRecord = tableAddress(TABLE_WEAPON) + (size_t)spellWeapon * WEAPON_SIZE;
        memcpy(testField.spellWeaponSaved, testField.spellWeaponRecord, WEAPON_SIZE);
        memcpy(testField.spellWeaponRecord + WEAPON_NAME_MAX + 1, tableAddress(TABLE_WEAPON) + (size_t)standIn * WEAPON_SIZE + WEAPON_NAME_MAX + 1,
               WEAPON_SIZE - WEAPON_NAME_MAX - 1);
    }
    writeLog("test_field_opened", "mod=%s monster=%lld spell=%lld enemy=%lld count=%d", s->mod, (long long)monster, (long long)spell,
             (long long)enemy, made);
    GAME_FN(void (*)(int, int, int), RVA_BATTLE_SETUP)(x, y, 0);
    GAME_FN(void (*)(int, int, int), RVA_BATTLE_DEPLOY)(-1, x, y);
    lua_pushboolean(L, 1);
    return 1;
}

static int testFieldIsOpen(lua_State *L, const char *call) {
    if (testField.open) return 1;
    LuaScript *s = checkLuaScript(L);
    refuseLuaCall(s->mod, call, "not_open", "field=closed");
    return 0;
}

static int lDrawTestField(lua_State *L) {
    if (!testFieldIsOpen(L, "coe5.drawTestField")) return 0;
    *(int *)GAME_PTR(RVA_BATTLE_SCREEN_CARRY_US) = G32(RVA_FRAME_DELAY_MS) * 1000;
    GAME_FN(void (*)(int), RVA_BATTLE_SCREEN)(0);
    return 0;
}

static int lFireTestField(lua_State *L) {
    if (!testFieldIsOpen(L, "coe5.fireTestField")) return 0;
    int *range = NULL, oldRange = 0;
    if (testField.weapon >= 0 && testField.weapon < tableRecords(TABLE_WEAPON)) {
        range = (int *)(tableAddress(TABLE_WEAPON) + (size_t)testField.weapon * WEAPON_SIZE + WEAPON_RANGE);
        oldRange = *range;
        if (*range > 1 && *range < TEST_FIELD_FIRE_RANGE) *range = TEST_FIELD_FIRE_RANGE;
    }
    if (testField.spellWeaponRecord && testField.standIn >= 0)
        memcpy(testField.spellWeaponRecord + WEAPON_NAME_MAX + 1, tableAddress(TABLE_WEAPON) + (size_t)testField.standIn * WEAPON_SIZE + WEAPON_NAME_MAX + 1,
               WEAPON_SIZE - WEAPON_NAME_MAX - 1);
    if (testField.spell >= 0) GAME_FN(void (*)(int, int, int), RVA_DOATTACK)(testField.unit, -1, testField.spell);
    else GAME_FN(void (*)(int, int, int), RVA_DOATTACK)(testField.unit, 0, -1);
    GAME_FN(int (*)(int), RVA_FLY_WEAPONS_STEP)(-1);
    if (range) *range = oldRange;
    return 0;
}

static int lFightTestField(lua_State *L) {
    if (!testFieldIsOpen(L, "coe5.fightTestField")) return 0;
    GAME_FN(void (*)(int, int), RVA_FIGHT_THE_FIGHT)(testField.x, testField.y);
    *(int *)GAME_PTR(RVA_BATTLE_DELAY) = TEST_BATTLE_DELAY;
#ifdef __APPLE__
    *GAME_PTR(RVA_TEST_GAME_FLAG) = 1;
    *GAME_PTR(RVA_BATTLE_OVER) = 0;
#else
    *(int *)GAME_PTR(RVA_TEST_GAME_FLAG) = 1;
    *(int *)GAME_PTR(RVA_BATTLE_OVER) = 0;
#endif
    return 0;
}

static int lStopTestFight(lua_State *L) {
    if (!testFieldIsOpen(L, "coe5.stopTestFight")) return 0;
    *(int *)GAME_PTR(RVA_BATTLE_DELAY) = 0;
#ifdef __APPLE__
    *GAME_PTR(RVA_TEST_GAME_FLAG) = 0;
    *GAME_PTR(RVA_BATTLE_OVER) = 1;
#else
    *(int *)GAME_PTR(RVA_TEST_GAME_FLAG) = 0;
    *(int *)GAME_PTR(RVA_BATTLE_OVER) = 1;
#endif
    return 0;
}

static int lCloseTestField(lua_State *L) {
    (void)L;
    closeTestFieldNow();
    return 0;
}

static int lTestBattle(lua_State *L) {
    lOpenTestField(L);
    if (!lua_toboolean(L, -1)) return 1;
    GAME_FN(void (*)(int, int), RVA_FIGHT_THE_FIGHT)(testField.x, testField.y);
    closeTestFieldNow();
    return 1;
}

static int lSpellInfo(lua_State *L) {
    lua_Integer spell = luaL_checkinteger(L, 1);
    if (spell < 0 || spell >= spellTableRecords()) {
        lua_pushnil(L);
        return 1;
    }
    const SpellRecord *r = spellSlot((int)spell);
    if (!r->name || !r->name[0] || r->path < 0) {
        lua_pushnil(L);
        return 1;
    }
    lua_createtable(L, 0, 5);
    lua_pushstring(L, r->name);
    lua_setfield(L, -2, "name");
    lua_pushinteger(L, r->path);
    lua_setfield(L, -2, "path");
    lua_pushinteger(L, r->level);
    lua_setfield(L, -2, "level");
    lua_pushinteger(L, r->weapon);
    lua_setfield(L, -2, "weapon");
    lua_pushinteger(L, r->dmg);
    lua_setfield(L, -2, "dmg");
    return 1;
}

static int lLookName(lua_State *L) {
    lua_Integer look = luaL_checkinteger(L, 1);
    if (look < 0 || look >= VISUAL_FX_ROWS) {
        lua_pushnil(L);
        return 1;
    }
    const char *name = *(const char *const *)GAME_PTR(RVA_VISUAL_FX_TABLE + (size_t)look * VISUAL_FX_SIZE);
    if (name && name[0]) lua_pushstring(L, name);
    else lua_pushnil(L);
    return 1;
}
