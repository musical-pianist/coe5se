#pragma once

typedef enum { LUA_CLASS_CONTENT, LUA_CLASS_PERSONAL, LUA_CLASS_OFF } LuaClass;

// ---- The sites a personal script may wrap -----------------------------------------------------
#define PERSONAL_SITES(X) \
    X(SITE_ARMY_WAIT) X(SITE_BATTLE_DRAW) X(SITE_BATTLE_WAIT) X(SITE_FILE_BROWSER_HOVERED) X(SITE_FILE_BROWSER_WAIT) \
    X(SITE_FILE_BROWSER_ROW_DATE) X(SITE_GAME_MENU_SAVING_SECTION_END) X(SITE_GAME_MENU_WAIT) X(SITE_GAME_OVER_WAIT) \
    X(SITE_KEYBOARD_SHORTCUTS_WAIT) X(SITE_MAP_DRAW) X(SITE_MAP_MENU_BUTTON_TEXT) X(SITE_MAP_SCREEN_WAIT) X(SITE_MESSAGES_WAIT) \
    X(SITE_MESSAGE_BOX_WAIT) X(SITE_MOD_BANNER_DRAW) X(SITE_RECRUIT_WAIT) X(SITE_RITUAL_SCREEN_ROW) X(SITE_SQUARE_INFO_BUTTON_ROW) \
    X(SITE_SQUARE_INFO_SCREEN) X(SITE_SQUARE_INFO_WAIT) X(SITE_TOOLTIP_LINE_DRAW) X(SITE_TOP_BAR_INCOME) X(SITE_UNIT_LIST_WAIT) \
    X(SITE_VIEW_ARMY_WAIT) X(SITE_WARN_WAIT)

static int isPersonalSite(CallSite site) {
#define PERSONAL_CASE(s) case s:
    switch (site) {
    PERSONAL_SITES(PERSONAL_CASE)
        return 1;
    default:
        return 0;
    }
#undef PERSONAL_CASE
}

// ---- The calls a personal script may make ---------------------------------------------------------
static const char *const PERSONAL_CALLS[] = {
    "coe5.actionKey", "coe5.addActionKey", "coe5.addCommanderLine", "coe5.addGameMenuLine", "coe5.addMainMenuButton",
    "coe5.addModCommand", "coe5.addScreen", "coe5.addTraitLine", "coe5.afflictionImmune", "coe5.aiActions",
    "coe5.aiCommanderBoatPlaces", "coe5.aiCountAllyTargetsNear", "coe5.aiCountPlanners", "coe5.aiCountTroops",
    "coe5.aiGroupBoatTotal", "coe5.aiItemUseRating", "coe5.aiItemValue", "coe5.aiNearestTerrain",
    "coe5.aiPlanSquareBonus", "coe5.aiPortsBoatTotals", "coe5.aiRitualBad", "coe5.aiRitualBadTime",
    "coe5.aiRitualMaster", "coe5.aiShipsAt", "coe5.aiThreatNear", "coe5.aiUnitStays", "coe5.armies",
    "coe5.armyAbilityTotal", "coe5.armyAtForPlayer", "coe5.armyBurnValue", "coe5.battleAdvanceRank",
    "coe5.battleCanStand", "coe5.battleCountRank", "coe5.battleEnemyExtent", "coe5.battleFearAt", "coe5.battleInRange",
    "coe5.battleKillerMove", "coe5.battleLineOfSight", "coe5.battleLocalLeadership", "coe5.battleNearestEnemy",
    "coe5.battleObjectAt", "coe5.battlePathfind", "coe5.battleSquareAbility", "coe5.battleUnitAt",
    "coe5.battleUnitDistance", "coe5.battleWallCover", "coe5.battleWinner", "coe5.builtinOfferChance",
    "coe5.builtinOffers", "coe5.canOwnSquare", "coe5.centerMap", "coe5.changedBy", "coe5.chooseLocation",
    "coe5.classAbilityNth", "coe5.classPicker", "coe5.classRecruits", "coe5.commanderApLeft",
    "coe5.commanderKnowsRitual", "coe5.commanderLedCount", "coe5.commanderMayAct", "coe5.commanderTheName",
    "coe5.confirm", "coe5.copyText", "coe5.countCommandersWithAp", "coe5.countFreeCommanders", "coe5.countKnownSpells",
    "coe5.countLandNear", "coe5.countMemorizedOfType", "coe5.countMemorizedSpells", "coe5.countPlayerPortals",
    "coe5.countSideUnits", "coe5.countSlotsWithController", "coe5.countSpells", "coe5.countTerrainCode",
    "coe5.countTerrainOnPlane", "coe5.countUnits", "coe5.countUnitsAt", "coe5.countWaterNear", "coe5.dataFolder",
    "coe5.deleteIni", "coe5.factionSlot", "coe5.findAfflictedUnitAt", "coe5.findClass", "coe5.findItemHolderAt",
    "coe5.findMonster", "coe5.findRitual", "coe5.findSpell", "coe5.findUnitAt", "coe5.get", "coe5.goldBonusPercent",
    "coe5.groundFromNeighbours", "coe5.guildCount", "coe5.hideMainMenuButton", "coe5.illusionTerrain",
    "coe5.ironBonusPercent", "coe5.isSpellAttack", "coe5.itemAbility", "coe5.itemAbilityNth", "coe5.itemCastableSpell",
    "coe5.listIni", "coe5.localPlayer", "coe5.log", "coe5.loneHumanNow", "coe5.lookName", "coe5.magicPathWeight", "coe5.mapView",
    "coe5.mayEnterSquare", "coe5.memorizeAllowance", "coe5.memorizedSlot", "coe5.monsterHasRanged",
    "coe5.monsterNameType", "coe5.monsterPicker", "coe5.monsterVariant", "coe5.nearTerrainSquare",
    "coe5.nearestTerrainDistance", "coe5.nearestUnitAbilityIs", "coe5.netRole", "coe5.numberBox", "coe5.observe",
    "coe5.openArmyMenu", "coe5.openCeremonies", "coe5.openChopBurn", "coe5.openIncomeDetail", "coe5.openItemInfo",
    "coe5.openLibrary", "coe5.openPlaneInfo", "coe5.openRitualScreen", "coe5.openSlaveHunt", "coe5.openSquareInfo",
    "coe5.openWeaponInfo", "coe5.planeAbilitySum", "coe5.planeIndex", "coe5.planeIsReachable", "coe5.planeOrderStep",
    "coe5.planeSameSpot", "coe5.planesAlike", "coe5.playSample", "coe5.playerClassAbility", "coe5.playerHasCitadel",
    "coe5.playerHasCommander", "coe5.playerIncomeByType", "coe5.playerRitualFlags", "coe5.portalExits",
    "coe5.portalsLinkedTo", "coe5.readIni", "coe5.rebindKey", "coe5.recruitSquares", "coe5.resetIncomeCaches",
    "coe5.ritualCastableAnywhere", "coe5.ritualEffectFirst", "coe5.ritualEffectOr", "coe5.ritualMayLearn",
    "coe5.ritualPointless", "coe5.route", "coe5.selectNextCommander", "coe5.selectPrevCommander", "coe5.set",
    "coe5.setting", "coe5.shopOffers", "coe5.showMessageBox", "coe5.slotBlessingOf", "coe5.soullessForm",
    "coe5.spellInfo", "coe5.spellPathWeight", "coe5.spellUseful", "coe5.squareDefaultGround",
    "coe5.squareEffectiveTemp", "coe5.squareGoldIncome", "coe5.squareHasSeenUnit", "coe5.squareIncome",
    "coe5.squareIsCoast", "coe5.squareIsNorth", "coe5.squareIsSky", "coe5.squareIsSouth", "coe5.squareIsWet",
    "coe5.squareLifeforce", "coe5.squareMoveCost", "coe5.squareShownName", "coe5.squareSideStrength",
    "coe5.squareSideWater", "coe5.squareTeamAbilitySum", "coe5.squareUnits", "coe5.squareUnitsAbility",
    "coe5.startLikesFilter", "coe5.startPlacementScore", "coe5.startTune", "coe5.terrainBurnableAt",
    "coe5.terrainMatch", "coe5.terrainRuinForm", "coe5.tradeBonusPercent", "coe5.turn", "coe5.ui.bindEscape",
    "coe5.ui.bindKey", "coe5.ui.button", "coe5.ui.buttonRow", "coe5.ui.checkbox", "coe5.ui.choice", "coe5.ui.columnX",
    "coe5.ui.drawLine", "coe5.ui.drawRect", "coe5.ui.drawResourceIcon", "coe5.ui.drawText", "coe5.ui.listBegin",
    "coe5.ui.listEnd", "coe5.ui.listRow", "coe5.ui.monsterRow", "coe5.ui.nameBox", "coe5.ui.resourceIcon",
    "coe5.ui.rowBegin", "coe5.ui.rowEnd", "coe5.ui.screenBegin", "coe5.ui.screenMusic", "coe5.ui.screenWidth",
    "coe5.ui.sectionBegin", "coe5.ui.sectionEnd", "coe5.ui.sectionHeader", "coe5.ui.setupScreenWidth", "coe5.ui.text",
    "coe5.ui.textWidth", "coe5.ui.tooltip", "coe5.ui.wait", "coe5.unitAbilitySum", "coe5.unitAcidResistance",
    "coe5.unitAfflictionCount", "coe5.unitBattleFast", "coe5.unitBattleRank", "coe5.unitBattleSlowness",
    "coe5.unitColdResistance", "coe5.unitFireResistance", "coe5.unitFirstMagicPath", "coe5.unitFirstSpellPath", "coe5.unitFlightKind", "coe5.unitFlightType",
    "coe5.unitHasSpellAttack", "coe5.unitHasWaterMove", "coe5.unitHatesSquare", "coe5.unitInContact",
    "coe5.unitIncapacitated", "coe5.unitIsDemon", "coe5.unitIsHuge", "coe5.unitIsUndead", "coe5.unitLifeless",
    "coe5.unitLikesSquare", "coe5.unitLovesSquare", "coe5.unitMagicResistance", "coe5.unitMayBeOnLand",
    "coe5.unitMindless", "coe5.unitMorale", "coe5.unitNoLand", "coe5.unitPathBonus", "coe5.unitPlane",
    "coe5.unitPoisonResistance", "coe5.unitPrimable", "coe5.unitRegenPercent", "coe5.unitShockResistance",
    "coe5.unitSpeedClass", "coe5.unitSpellWeaponLevel", "coe5.viewArmy", "coe5.viewPlane", "coe5.wandererAt",
    "coe5.weaponExtra", "coe5.weaponRangeFor", "coe5.wetNeighbours", "coe5.wrap", "coe5.writeIni", "require",
};
#define PERSONAL_CALL_COUNT ((int)(sizeof PERSONAL_CALLS / sizeof PERSONAL_CALLS[0]))

static int isPersonalCall(const char *name) {
    int low = 0, high = PERSONAL_CALL_COUNT - 1;
    while (low <= high) {
        int middle = (low + high) / 2, order = strcmp(name, PERSONAL_CALLS[middle]);
        if (!order) return 1;
        if (order < 0) high = middle - 1;
        else low = middle + 1;
    }
    return 0;
}

static int isPersonalSetValue(const char *name) {
    static const char *const PERSONAL_SET_VALUES[] = {"bannerFade", "battleGrid", "fullscreen", "hoveredId", "mouseButtons"};
    for (size_t i = 0; i < sizeof PERSONAL_SET_VALUES / sizeof PERSONAL_SET_VALUES[0]; i++)
        if (!strcmp(name, PERSONAL_SET_VALUES[i])) return 1;
    return 0;
}

// ---- The calls that hold the game until someone answers -------------------------------------------
typedef enum { LUA_BLOCKING_NONE, LUA_BLOCKING_NIL, LUA_BLOCKING_FALSE } LuaBlockingCall;

static LuaBlockingCall luaBlockingCallOf(const char *name) {
    static const char *const SCREENS[] = {
        "coe5.chooseLocation", "coe5.classPicker", "coe5.commanderUseItems", "coe5.monsterPicker", "coe5.numberBox", "coe5.openArmyMenu",
        "coe5.openCeremonies", "coe5.openChopBurn", "coe5.openIncomeDetail", "coe5.openItemInfo", "coe5.openLibrary", "coe5.openPlaneInfo",
        "coe5.openRitualScreen", "coe5.openSlaveHunt", "coe5.openSquareInfo", "coe5.openWeaponInfo", "coe5.recruitScreen", "coe5.viewArmy",
        "coe5.viewPlane",
    };
    if (!strcmp(name, "coe5.showMessageBox") || !strcmp(name, "coe5.confirm")) return LUA_BLOCKING_FALSE;
    if (!strncmp(name, "coe5.ui.", 8)) return LUA_BLOCKING_NIL;
    for (size_t i = 0; i < sizeof SCREENS / sizeof SCREENS[0]; i++)
        if (!strcmp(name, SCREENS[i])) return LUA_BLOCKING_NIL;
    return LUA_BLOCKING_NONE;
}

static int luaCallWaitsForPlayer(const char *name) {
    if (!strncmp(name, "coe5.ui.", 8)) return !strcmp(name, "coe5.ui.screenBegin") || !strcmp(name, "coe5.ui.wait");
    return luaBlockingCallOf(name) != LUA_BLOCKING_NONE;
}

// ---- The decisions ------------------------------------------------------------------------------
static int luaStartsFor(NetRole role, LuaClass cls) {
    switch (role) {
    case NET_ROLE_SINGLE: return 1;
    case NET_ROLE_HOST: return cls == LUA_CLASS_CONTENT;
    case NET_ROLE_CLIENT: return cls != LUA_CLASS_OFF;
    default: return 0;
    }
}

static int luaSiteOpenFor(NetRole role, LuaClass cls, CallSite site, int loading, int replay) {
    switch (role) {
    case NET_ROLE_SINGLE: return 1;
    case NET_ROLE_HOST: return cls == LUA_CLASS_CONTENT;
    case NET_ROLE_CLIENT:
        if (loading) return 0;
        return cls == LUA_CLASS_PERSONAL ? isPersonalSite(site) : cls == LUA_CLASS_CONTENT && !replay;
    default: return 0;
    }
}

static int luaCallbackOpenFor(NetRole role, LuaClass cls, int loading, int replay) {
    switch (role) {
    case NET_ROLE_SINGLE: return 1;
    case NET_ROLE_HOST: return cls == LUA_CLASS_CONTENT;
    case NET_ROLE_CLIENT: return !loading && (cls == LUA_CLASS_PERSONAL || (cls == LUA_CLASS_CONTENT && !replay));
    default: return 0;
    }
}

static int luaGameLogicOpenFor(NetRole role, LuaClass cls, int loading, int inTurn) {
    return role == NET_ROLE_SINGLE || (role == NET_ROLE_HOST && cls == LUA_CLASS_CONTENT) ||
           (role == NET_ROLE_CLIENT && cls == LUA_CLASS_CONTENT && !loading && inTurn);
}

static int luaModCommandOpenFor(NetRole role, LuaClass cls, int level) {
    switch (role) {
    case NET_ROLE_SINGLE: return 1;
    case NET_ROLE_HOST: return cls == LUA_CLASS_CONTENT;
    case NET_ROLE_CLIENT: return level == 2 && cls != LUA_CLASS_OFF;
    default: return 0;
    }
}

static const char *luaCallRefusalFor(NetRole role, LuaClass cls, const char *name, int inTurn) {
    if (role == NET_ROLE_SINGLE) return NULL;
    if (role == NET_ROLE_OFFICIAL) return "official_game";
    if (cls == LUA_CLASS_OFF) return "nomultiplayer";
    if (cls == LUA_CLASS_PERSONAL) {
        if (role == NET_ROLE_HOST) return "personal_on_host";
        if (!isPersonalCall(name)) return "personal_mod";
        return !inTurn && luaCallWaitsForPlayer(name) ? "wait_screen" : NULL;
    }
    if (luaBlockingCallOf(name) == LUA_BLOCKING_NONE) return NULL;
    if (role == NET_ROLE_HOST) return "lan_host";
    return inTurn ? NULL : "content_on_client";
}

// ---- What a content script's write on a client reaches ------------------------------------------
typedef enum { CLIENT_WRITE_STAYS, CLIENT_WRITE_TRAVELS, CLIENT_WRITE_LOST } ClientWriteKind;
typedef enum { WRITE_OWNER_NONE, WRITE_OWNER_PLAYER, WRITE_OWNER_UNIT, WRITE_OWNER_COMMANDER } ClientWriteOwner;
typedef struct { const char *name; ClientWriteKind kind; ClientWriteOwner owner; int arg; } ClientWrite;

#define WRITE_TRAVELS(name, owner, arg) {name, CLIENT_WRITE_TRAVELS, WRITE_OWNER_##owner, arg}
#define WRITE_LOST(name) {name, CLIENT_WRITE_LOST, WRITE_OWNER_NONE, 0}
#define WRITE_STAYS(name) {name, CLIENT_WRITE_STAYS, WRITE_OWNER_NONE, 0}
static const ClientWrite CLIENT_WRITE_CALLS[] = {
    WRITE_TRAVELS("coe5.addAffliction", UNIT, 1), WRITE_TRAVELS("coe5.addBenefit", UNIT, 1), WRITE_LOST("coe5.addPlayerBlessing"),
    WRITE_TRAVELS("coe5.alterTerrain", NONE, 0), WRITE_LOST("coe5.bolsterBlessing"), WRITE_LOST("coe5.changeLoyalty"),
    WRITE_TRAVELS("coe5.checkFollowerLinks", COMMANDER, 1), WRITE_TRAVELS("coe5.chooseMemorized", UNIT, 1), WRITE_LOST("coe5.cleanSquare"),
    WRITE_TRAVELS("coe5.commanderLearnRitual", COMMANDER, 1), WRITE_TRAVELS("coe5.commanderUseItems", COMMANDER, 1),
    WRITE_TRAVELS("coe5.damageUnitPlain", UNIT, 1), WRITE_LOST("coe5.destroyEnchantsAt"), WRITE_TRAVELS("coe5.destroyGateway", NONE, 0),
    WRITE_TRAVELS("coe5.digTunnelSquare", NONE, 0), WRITE_LOST("coe5.fixPortals"), WRITE_TRAVELS("coe5.getStartRituals", COMMANDER, 1),
    WRITE_TRAVELS("coe5.giveItem", UNIT, 1), WRITE_TRAVELS("coe5.giveRandomItem", UNIT, 1), WRITE_LOST("coe5.initSquareCorpses"),
    WRITE_LOST("coe5.initSquareGems"), WRITE_LOST("coe5.killSquare"), WRITE_LOST("coe5.killUnit"),
    WRITE_TRAVELS("coe5.learnLevelRituals", COMMANDER, 1), WRITE_TRAVELS("coe5.learnRandomSpell", UNIT, 1),
    WRITE_TRAVELS("coe5.learnStartSpells", UNIT, 1), WRITE_TRAVELS("coe5.memorizeSpell", UNIT, 1), WRITE_LOST("coe5.moveEnchants"),
    WRITE_LOST("coe5.moveGems"), WRITE_LOST("coe5.newPortalPair"), WRITE_TRAVELS("coe5.newUnits", PLAYER, 1),
    WRITE_TRAVELS("coe5.orderBurnForest", COMMANDER, 1), WRITE_TRAVELS("coe5.orderChopForest", COMMANDER, 1),
    WRITE_TRAVELS("coe5.orderEatVillage", COMMANDER, 1), WRITE_TRAVELS("coe5.orderSlaveHunt", COMMANDER, 1),
    WRITE_TRAVELS("coe5.planeShiftUnit", UNIT, 1), WRITE_LOST("coe5.putRareSquare"), WRITE_TRAVELS("coe5.randomTeleport", UNIT, 1),
    WRITE_TRAVELS("coe5.removeBadFollowers", NONE, 0), WRITE_TRAVELS("coe5.removeEnchant", NONE, 0),
    WRITE_TRAVELS("coe5.repickMemorized", UNIT, 1), WRITE_TRAVELS("coe5.revealDisc", NONE, 0), WRITE_LOST("coe5.revealMap"),
    WRITE_LOST("coe5.revealPortalExits"), WRITE_TRAVELS("coe5.revealTerrainOnPlane", NONE, 0), WRITE_TRAVELS("coe5.ruinSquare", NONE, 0),
    WRITE_TRAVELS("coe5.selectedCommanderDone", NONE, 0), WRITE_LOST("coe5.sendMessage"), WRITE_LOST("coe5.setSeenDisc"),
    WRITE_LOST("coe5.setSquareOwner"), WRITE_TRAVELS("coe5.spawnCommander", PLAYER, 1), WRITE_TRAVELS("coe5.spawnUnit", PLAYER, 1),
    WRITE_TRAVELS("coe5.spendAllApAt", PLAYER, 1), WRITE_TRAVELS("coe5.squareHold", PLAYER, 1), WRITE_TRAVELS("coe5.swapLocations", NONE, 0),
    WRITE_LOST("coe5.swapSquares"), WRITE_TRAVELS("coe5.takeItem", UNIT, 1), WRITE_TRAVELS("coe5.teleportCommander", COMMANDER, 1),
    WRITE_TRAVELS("coe5.teleportSquare", NONE, 0), WRITE_TRAVELS("coe5.teleportUnit", UNIT, 1), WRITE_TRAVELS("coe5.unitAddXp", UNIT, 1),
    WRITE_TRAVELS("coe5.unitRandomAffliction", UNIT, 1), WRITE_TRAVELS("coe5.unitRemoveItem", UNIT, 1),
    WRITE_TRAVELS("coe5.unitRemoveTempAbility", UNIT, 1), WRITE_TRAVELS("coe5.unmemorizeSpell", UNIT, 1),
    WRITE_TRAVELS("coe5.useSpecialSite", COMMANDER, 3), WRITE_LOST("coe5.worldNews"),
};
static const ClientWrite CLIENT_SET_VALUES[] = {
    WRITE_STAYS("aiMoveDelay"), WRITE_STAYS("bannerFade"), WRITE_STAYS("battleGrid"), WRITE_STAYS("bugChecksOff"), WRITE_STAYS("ceremonyCost"),
    WRITE_STAYS("cliMapH"), WRITE_STAYS("cliMapW"), WRITE_STAYS("fullscreen"), WRITE_STAYS("hoveredId"), WRITE_STAYS("mouseButtons"),
    WRITE_STAYS("newWorldUniqueRandom"), WRITE_TRAVELS("slotAi", PLAYER, 2), WRITE_TRAVELS("slotResource", PLAYER, 2),
    WRITE_TRAVELS("slotTradeType", PLAYER, 2), WRITE_STAYS("startDefault"), WRITE_STAYS("traceLevel"), WRITE_STAYS("uniqueRandomDefault"),
    WRITE_TRAVELS("unitApUsed", UNIT, 2), WRITE_STAYS("weapon"), WRITE_STAYS("wilderDefault"),
};
#undef WRITE_TRAVELS
#undef WRITE_LOST
#undef WRITE_STAYS
#define CLIENT_WRITE_CALL_COUNT ((int)(sizeof CLIENT_WRITE_CALLS / sizeof CLIENT_WRITE_CALLS[0]))
#define CLIENT_SET_VALUE_COUNT ((int)(sizeof CLIENT_SET_VALUES / sizeof CLIENT_SET_VALUES[0]))

static const ClientWrite *findClientWrite(const ClientWrite *rows, int count, const char *name) {
    int low = 0, high = count - 1;
    while (low <= high) {
        int middle = (low + high) / 2, order = strcmp(name, rows[middle].name);
        if (!order) return &rows[middle];
        if (order < 0) high = middle - 1;
        else low = middle + 1;
    }
    return NULL;
}

static ClientWrite clientWriteOf(const char *name, const char *setValue) {
    if (!strcmp(name, "coe5.set")) {
        const ClientWrite *w = findClientWrite(CLIENT_SET_VALUES, CLIENT_SET_VALUE_COUNT, setValue ? setValue : "");
        return w ? *w : (ClientWrite){setValue, CLIENT_WRITE_LOST, WRITE_OWNER_NONE, 0};
    }
    const ClientWrite *w = findClientWrite(CLIENT_WRITE_CALLS, CLIENT_WRITE_CALL_COUNT, name);
    return w ? *w : (ClientWrite){name, CLIENT_WRITE_STAYS, WRITE_OWNER_NONE, 0};
}

static const char *clientWriteRefusalFor(ClientWriteKind kind, int inTurn, int own) {
    if (kind == CLIENT_WRITE_STAYS) return NULL;
    if (kind == CLIENT_WRITE_LOST) return "client_write_lost";
    if (!inTurn) return "client_not_acting";
    return own == 0 ? "not_own_player" : NULL;
}

static int keyPickCountsFor(NetRole role, int fromPersonal) {
    return role == NET_ROLE_SINGLE || (role == NET_ROLE_CLIENT && fromPersonal);
}

// ---- Now ----------------------------------------------------------------------------------------
static int modIsPersonal(const char *modName);
static int modIsNoMultiplayer(const char *modName);

static LuaClass modClassNow(const char *name) {
    return modIsNoMultiplayer(name) ? LUA_CLASS_OFF : modIsPersonal(name) ? LUA_CLASS_PERSONAL : LUA_CLASS_CONTENT;
}

static int netClientLoading;
static unsigned netClientLoads;
static int netClientInTurn;
static int netClientGameOverSeen;

static void clearNetServerFlag(void) {
#ifdef __APPLE__
    G8(RVA_NET_SERVER) = 0;
#else
    G32(RVA_NET_SERVER) = 0;
#endif
}

static int netShutsGameCalls(void) {
    NetRole role = netRoleNow();
    return role == NET_ROLE_CLIENT || role == NET_ROLE_OFFICIAL;
}
