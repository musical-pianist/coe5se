#pragma once

#define TEST_BATTLE_X 1
#define TEST_BATTLE_Y 1
#define TEST_BATTLE_TERRAIN 0
#define TEST_BATTLE_ENEMY_PLAYER 24
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
    int open, unit, commander, spell, weapon, standIn, x, y;
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

typedef struct {
    int monster;
    long long count;
} TestFieldGroup;

typedef struct {
    TestFieldGroup *groups;
    int groupCount;
    long long units;
} TestFieldSide;

typedef struct {
    lua_Integer monster, spell, standIn;
    TestFieldSide friends, enemies;
} TestFieldSpec;

static void freeTestFieldSide(TestFieldSide *side) {
    free(side->groups);
    side->groups = NULL;
    side->groupCount = 0;
    side->units = 0;
}

static const char *readTestFieldGroups(lua_State *L, int index, TestFieldSide *side, const char *badMonster) {
    freeTestFieldSide(side);
    if (!lua_istable(L, index)) return "bad_list";
    size_t n = lua_rawlen(L, index);
    if (n > (size_t)INT_MAX) return "bad_list";
    side->groups = calloc(n ? n : 1, sizeof *side->groups);
    if (!side->groups) return "out_of_memory";
    for (int i = 1; i <= (int)n; i++) {
        lua_rawgeti(L, index, i);
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            return "bad_list";
        }
        lua_rawgeti(L, -1, 1);
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            lua_getfield(L, -1, "monster");
        }
        lua_rawgeti(L, -2, 2);
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            lua_getfield(L, -2, "count");
        }
        int isMonster, isCount;
        lua_Integer monster = lua_tointegerx(L, -2, &isMonster);
        lua_Integer count = lua_isnil(L, -1) ? (isCount = 1, 1) : lua_tointegerx(L, -1, &isCount);
        lua_pop(L, 3);
        if (!isMonster || !isUsedMonster(monster)) return badMonster;
        if (!isCount || count < 1) return "bad_count";
        side->groups[side->groupCount].monster = (int)monster;
        side->groups[side->groupCount].count = count;
        side->groupCount++;
        side->units = side->units > LLONG_MAX - count ? LLONG_MAX : side->units + count;
    }
    return NULL;
}

static const char *readTestFieldSpec(lua_State *L, TestFieldSpec *spec) {
    if (!lua_istable(L, 1)) {
        spec->monster = luaL_checkinteger(L, 1);
        spec->spell = luaL_optinteger(L, 2, -1);
        lua_Integer enemy = luaL_checkinteger(L, 3), count = luaL_optinteger(L, 4, 40);
        spec->standIn = luaL_optinteger(L, 5, -1);
        if (!isUsedMonster(spec->monster)) return "bad_monster";
        if (!isUsedMonster(enemy)) return "bad_enemy";
        if (count < 1) return "bad_count";
        spec->enemies.groups = calloc(1, sizeof *spec->enemies.groups);
        if (!spec->enemies.groups) return "out_of_memory";
        spec->enemies.groups[0].monster = (int)enemy;
        spec->enemies.groups[0].count = count;
        spec->enemies.groupCount = 1;
        spec->enemies.units = count;
        return NULL;
    }
    int isNumber;
    lua_getfield(L, 1, "commander");
    spec->monster = lua_tointegerx(L, -1, &isNumber);
    lua_pop(L, 1);
    if (!isNumber || !isUsedMonster(spec->monster)) return "bad_monster";
    lua_getfield(L, 1, "spell");
    spec->spell = lua_isnil(L, -1) ? -1 : lua_tointegerx(L, -1, &isNumber);
    lua_pop(L, 1);
    if (!isNumber) return "bad_spell";
    lua_getfield(L, 1, "standIn");
    spec->standIn = lua_isnil(L, -1) ? (isNumber = 1, -1) : lua_tointegerx(L, -1, &isNumber);
    lua_pop(L, 1);
    if (!isNumber) return "bad_weapon";
    const char *reason = NULL;
    lua_getfield(L, 1, "friends");
    if (!lua_isnil(L, -1)) reason = readTestFieldGroups(L, lua_gettop(L), &spec->friends, "bad_friend");
    lua_pop(L, 1);
    if (reason) return reason;
    lua_getfield(L, 1, "enemies");
    reason = readTestFieldGroups(L, lua_gettop(L), &spec->enemies, "bad_enemy");
    lua_pop(L, 1);
    if (reason) return reason;
    return spec->enemies.units ? NULL : "no_enemies";
}

static int makeTestFieldGroups(int player, int commander, const TestFieldSide *side, int *made) {
    int n = 0;
    for (int g = 0; g < side->groupCount; g++)
        for (long long i = 0; i < side->groups[g].count; i++) {
            int unit = n < UNIT_TABLE_MAX ? GAME_FN(int (*)(int, int, int, int, int), RVA_NEW_UNIT)(testField.x, testField.y, player, commander,
                                                                                                      side->groups[g].monster)
                                          : -1;
            if (unit < 0) {
                for (int k = 0; k < n; k++) *(short *)GAME_PTR(RVA_UNIT_HP + (size_t)made[k] * UNIT_SIZE) = 0;
                return -1;
            }
            made[n++] = unit;
        }
    return n;
}

static void pushTestFieldUnits(lua_State *L, const int *units, int n) {
    lua_createtable(L, n, 0);
    for (int i = 0; i < n; i++) {
        lua_pushinteger(L, units[i]);
        lua_rawseti(L, -2, i + 1);
    }
}

static int testFieldFriendUnits[UNIT_TABLE_MAX], testFieldEnemyUnits[UNIT_TABLE_MAX];
static int testFieldFriendMade, testFieldEnemyMade;

static int openTestFieldFrom(lua_State *L, const char *call) {
    LuaScript *s = checkLuaScript(L);
    TestFieldSpec *spec = calloc(1, sizeof *spec);
    if (!spec) return luaL_error(L, "out of memory");
    const char *reason = G32(RVA_ACTIVE_PLAYER) >= 0 ? "in_game" : testField.open ? "already_open" : readTestFieldSpec(L, spec);
    lua_Integer monster = spec->monster, spell = spec->spell, standIn = spec->standIn;
    int spellWeapon = spell >= 0 && spell < spellTableRecords() ? spellSlot((int)spell)->weapon : -1;
    int standInMaker = standIn >= 0 && standIn < WEAPON_RECORDS_MOVED ? weaponMadeBy[standIn] - 1 : -1;
    if (!reason)
        reason = spell < -1 || spell >= spellTableRecords()                     ? "bad_spell"
               : standIn >= 0 && (spellWeapon < 0 || spellWeapon >= tableRecords(TABLE_WEAPON)) ? "no_spell_weapon"
               : standIn >= 0 && (standInMaker < 0 || standInMaker > G32(RVA_MOD_LAST_INDEX) || _stricmp(modName(standInMaker), s->mod))
                   ? "not_own_weapon"
               : standIn < -1 ? "bad_weapon"
                              : NULL;
    if (reason) {
        refuseLuaCall(s->mod, call, reason, "monster=%lld spell=%lld friends=%d enemies=%d", (long long)monster, (long long)spell,
                      spec->friends.groupCount, spec->enemies.groupCount);
        freeTestFieldSide(&spec->friends);
        freeTestFieldSide(&spec->enemies);
        free(spec);
        return 0;
    }
    int x = testField.x = TEST_BATTLE_X, y = testField.y = TEST_BATTLE_Y;
    unsigned char *square = testFieldSquare();
    memcpy(testField.square, square, SQUARE_SIZE);
    memset(square, 0, SQUARE_SIZE);
    *(short *)(square + (RVA_SQUARES - RVA_SQUARE_RECORDS)) = TEST_BATTLE_TERRAIN;
    clearTestBattleSquare(x, y);

    int commander = GAME_FN(int (*)(int, int, int, int), RVA_NEW_COMMANDER)(x, y, 0, (int)monster);
    int unit = commander >= 0 && commander <= G32(RVA_LAST_COMMANDER) ? G32(RVA_COMMANDERS + (size_t)commander * COMMANDER_SIZE) : -1;
    testFieldFriendMade = unit < 0 ? -1 : makeTestFieldGroups(0, commander, &spec->friends, testFieldFriendUnits);
    testFieldEnemyMade = testFieldFriendMade < 0 ? -1 : makeTestFieldGroups(TEST_BATTLE_ENEMY_PLAYER, -1, &spec->enemies, testFieldEnemyUnits);
    int friendGroups = spec->friends.groupCount, enemyGroups = spec->enemies.groupCount;
    freeTestFieldSide(&spec->friends);
    freeTestFieldSide(&spec->enemies);
    free(spec);
    if (testFieldEnemyMade < 1) {
        clearTestBattleSquare(x, y);
        memcpy(square, testField.square, SQUARE_SIZE);
        refuseLuaCall(s->mod, call, "no_free_slot", "monster=%lld friends=%d enemies=%d", (long long)monster, friendGroups, enemyGroups);
        return 0;
    }
    testField.open = 1;
    testField.unit = unit;
    testField.commander = commander;
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
    writeLog("test_field_opened", "mod=%s monster=%lld spell=%lld friends=%d enemies=%d", s->mod, (long long)monster, (long long)spell,
             testFieldFriendMade, testFieldEnemyMade);
    GAME_FN(void (*)(int, int, int), RVA_BATTLE_SETUP)(x, y, 0);
    GAME_FN(void (*)(int, int, int), RVA_BATTLE_DEPLOY)(-1, x, y);
    return 1;
}

static int lOpenTestField(lua_State *L) {
    if (!openTestFieldFrom(L, "coe5.openTestField")) {
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_pushboolean(L, 1);
    lua_createtable(L, 0, 3);
    lua_pushinteger(L, testField.unit);
    lua_setfield(L, -2, "commander");
    pushTestFieldUnits(L, testFieldFriendUnits, testFieldFriendMade);
    lua_setfield(L, -2, "friends");
    pushTestFieldUnits(L, testFieldEnemyUnits, testFieldEnemyMade);
    lua_setfield(L, -2, "enemies");
    return 2;
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

static int lAddTestFieldUnits(lua_State *L) {
    LuaScript *s = checkLuaScript(L);
    const char *side = luaL_checkstring(L, 1);
    int friends = !strcmp(side, "friends"), enemies = !strcmp(side, "enemies");
    int player = friends ? 0 : TEST_BATTLE_ENEMY_PLAYER;
    TestFieldSide groups = {0};
    const char *reason = !testField.open      ? "not_open"
                       : !friends && !enemies ? "bad_side"
                                              : readTestFieldGroups(L, 2, &groups, friends ? "bad_friend" : "bad_enemy");
    if (!reason && !groups.units) reason = "bad_count";
    int *made = friends ? testFieldFriendUnits : testFieldEnemyUnits;
    int n = reason ? 0 : makeTestFieldGroups(player, friends ? testField.commander : -1, &groups, made);
    if (!reason && n < 0) reason = "no_free_slot";
    if (reason) {
        refuseLuaCall(s->mod, "coe5.addTestFieldUnits", reason, "side=%.20s groups=%d units=%lld", side, groups.groupCount, groups.units);
        freeTestFieldSide(&groups);
        lua_pushnil(L);
        return 1;
    }
    freeTestFieldSide(&groups);
    if (friends) testFieldFriendMade = n;
    else testFieldEnemyMade = n;
    ((void (*)(int, int, int, int, int))siteEntry(SITE_BATTLE_SETUP_SIDE, GAME_PTR(RVA_BATTLE_SETUP_SIDE)))(player, testField.x, testField.y,
                                                                                                          friends ? 0 : 1, -1);
    writeLog("test_field_units_added", "mod=%s side=%s units=%d", s->mod, side, n);
    pushTestFieldUnits(L, made, n);
    return 1;
}

#define TEST_FIELD_CELL_OBJECT 4

static int testFieldTokenCovering(int x, int y, int layer, int self) {
    for (int ax = x - 2; ax <= x; ax++)
        for (int ay = y - 2; ay <= y; ay++) {
            if (ax < 0 || ay < 0) continue;
            int u = *(const int *)(battleCell(ax, ay) + layer);
            if (u < 0 || u == self || u >= UNIT_TABLE_MAX) continue;
            int w = GAME_FN(int (*)(int), RVA_UNIT_BATTLE_SIZE_2)(u), h = GAME_FN(int (*)(int), RVA_UNIT_BATTLE_SIZE)(u);
            if (ax + w > x && ay + h > y) return u;
        }
    return -1;
}

static int lMoveTestFieldUnit(lua_State *L) {
    LuaScript *s = checkLuaScript(L);
    lua_Integer unit = luaL_checkinteger(L, 1), x = luaL_checkinteger(L, 2), y = luaL_checkinteger(L, 3);
    int fromX = -1, fromY = -1;
    const char *reason = !testField.open ? "not_open" : unit < 0 || unit >= UNIT_TABLE_MAX || !unitBattleCell((int)unit, &fromX, &fromY) ? "not_on_field" : NULL;
    int w = 1, h = 1;
    if (!reason) {
        w = GAME_FN(int (*)(int), RVA_UNIT_BATTLE_SIZE_2)((int)unit);
        h = GAME_FN(int (*)(int), RVA_UNIT_BATTLE_SIZE)((int)unit);
        if (x < 0 || y < 0 || x + w > G32(RVA_BATTLE_WIDTH) || y + h > BATTLE_ROWS) reason = "off_field";
    }
    for (int cx = (int)x; !reason && cx < x + w; cx++)
        for (int cy = (int)y; !reason && cy < y + h; cy++)
            if (testFieldTokenCovering(cx, cy, 0, (int)unit) >= 0 || testFieldTokenCovering(cx, cy, TEST_FIELD_CELL_OBJECT, (int)unit) >= 0)
                reason = "blocked";
    if (reason) {
        refuseLuaCall(s->mod, "coe5.moveTestFieldUnit", reason, "unit=%lld x=%lld y=%lld", (long long)unit, (long long)x, (long long)y);
        lua_pushboolean(L, 0);
        return 1;
    }
    *(int *)battleCell(fromX, fromY) = -1;
    *(int *)battleCell((int)x, (int)y) = (int)unit;
    lua_pushboolean(L, 1);
    return 1;
}

static int lTestBattle(lua_State *L) {
    int opened = openTestFieldFrom(L, "coe5.testBattle");
    if (opened) {
        GAME_FN(void (*)(int, int), RVA_FIGHT_THE_FIGHT)(testField.x, testField.y);
        closeTestFieldNow();
    }
    lua_pushboolean(L, opened);
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
