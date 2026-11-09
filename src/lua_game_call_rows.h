#pragma once

#define LUA_GAME_CALLS(X) \
 \
    X(fixPortals, WINDOWS_ONLY_CALL(RVA_FIX_PORTALS), "", 'i') \
    X(planeSameSpot, BOTH_BUILDS_CALL(RVA_PLANE_SAME_SPOT), "l&&", 'o') \
    X(planesAlike, MAC_COPY_CALL(RVA_PLANES_ALIKE, planesAlikeCopy), "ll", 'B') \
    X(planeAbilitySum, BOTH_BUILDS_CALL(RVA_PLANE_ABILITY_SUM), "La", 'i') \
    X(portalsLinkedTo, BOTH_BUILDS_CALL(RVA_PORTALS_LINKED_TO), "LE", 'i') \
    X(countPlayerPortals, BOTH_BUILDS_CALL(RVA_COUNT_PLAYER_PORTALS), "PEi", 'i') \
    X(countTerrainOnPlane, BOTH_BUILDS_CALL(RVA_COUNT_TERRAIN_ON_PLANE), PER_BUILD("-t-0L", "tL"), 'i') \
    X(planeOrderStep, MAC_COPY_CALL(RVA_PLANE_ORDER_STEP, planeOrderStepCopy), "li", 'i') \
    X(planeShiftUnit, WINDOWS_ONLY_CALL(RVA_PLANESHIFT_UNIT), "uli", 'v') \
    X(newPortalPair, BOTH_BUILDS_CALL(RVA_NEW_PORTAL_PAIR), "xyxyi", 'v') \
    X(revealPortalExits, BOTH_BUILDS_CALL(RVA_REVEAL_PORTAL_EXITS), "pi", 'i') \
    X(planeRiseToElysium, WINDOWS_ONLY_CALL(RVA_PLANE_SPAWN_CHANCE), "liin", 'v') \
    X(viewPlane, BOTH_BUILDS_CALL(RVA_VIEW_PLANE), "l", 'v') \
 \
    X(randomTeleport, BOTH_BUILDS_CALL(RVA_RANDOM_TELEPORT), "uii", PER_BUILD('b', 'v')) \
    X(popHorrors, BOTH_BUILDS_CALL(RVA_POP_HORRORS), "i", 'v') \
    X(removeBadFollowers, WINDOWS_ONLY_CALL(RVA_REMOVE_BAD_FOLLOWERS), "", 'v') \
    X(unitPlane, MAC_COPY_CALL(RVA_UNIT_PLANE, unitPlaneCopy), "u", 'i') \
    X(unitFireResistance, BOTH_BUILDS_CALL(RVA_UNIT_RESISTANCE_1D1280), "u", 'i') \
    X(unitColdResistance, BOTH_BUILDS_CALL(RVA_UNIT_COLD_RESISTANCE), "u", 'i') \
    X(unitShockResistance, BOTH_BUILDS_CALL(RVA_UNIT_SHOCK_RESISTANCE), "u", 'i') \
    X(unitPoisonResistance, BOTH_BUILDS_CALL(RVA_UNIT_POISON_RESISTANCE), "u", 'i') \
    X(unitAcidResistance, MAC_COPY_CALL(RVA_UNIT_ACID_RESISTANCE, unitAcidResistanceCopy), "u", 'i') \
    X(unitMorale, BOTH_BUILDS_CALL(RVA_UNIT_MORALE), "u", 'i') \
    X(unitBattleRank, BOTH_BUILDS_CALL(RVA_UNIT_BATTLE_RANK), "u", 'i') \
    X(unitBattleSlowness, MAC_COPY_CALL(RVA_UNIT_BATTLE_SLOWNESS, unitBattleSlownessCopy), "u", 'i') \
    X(unitBattleFast, MAC_COPY_CALL(RVA_UNIT_BATTLE_FAST, unitBattleFastCopy), "u", 'B') \
    X(unitIncapacitated, MAC_COPY_CALL(RVA_UNIT_INCAPACITATED, unitIncapacitatedCopy), "u", 'B') \
    X(unitSpeedClass, BOTH_BUILDS_CALL(RVA_UNIT_SPEED_CLASS), "u", 'i') \
    X(unitFlightType, BOTH_BUILDS_CALL(RVA_UNIT_FLIGHT_KIND), "u", 'k') \
    X(unitFlightKind, BOTH_BUILDS_CALL(RVA_UNIT_FLIGHT_KIND), "u", 'k') \
    X(unitHasWaterMove, BOTH_BUILDS_CALL(RVA_UNIT_HAS_WATER_MOVE), "u", 'B') \
    X(unitNoLand, WINDOWS_ONLY_CALL(RVA_UNIT_NOLAND), "u", 'B') \
    X(unitMindless, BOTH_BUILDS_CALL(RVA_UNIT_MINDLESS), "u", 'B') \
    X(unitRegenPercent, BOTH_BUILDS_CALL(RVA_UNIT_REGEN_PERCENT), "u", 'i') \
    X(unitAfflictionCount, MAC_COPY_CALL(RVA_UNIT_AFFLICTION_COUNT, unitAfflictionCountCopy), "u", 'i') \
    X(unitHasSpellAttack, WINDOWS_ONLY_CALL(RVA_UNIT_HAS_SPELL_ATTACK), "u", 'B') \
    X(unitIsUndead, MAC_COPY_CALL(RVA_UNIT_IS_UNDEAD, unitIsUndeadCopy), "u", 'B') \
    X(unitIsDemon, MAC_COPY_CALL(RVA_UNIT_IS_DEMON, unitIsDemonCopy), "u", 'B') \
    X(unitAbilitySum, WINDOWS_ONLY_CALL(RVA_UNIT_ABILITY_SUM), "uai", 'i') \
    X(monsterHasRanged, WINDOWS_ONLY_CALL(RVA_MONSTER_HAS_RANGED), "m", 'B') \
    X(monsterNameType, BOTH_BUILDS_CALL(RVA_MONSTER_NAME_TYPE), "m", 'i') \
    X(commanderLedCount, BOTH_BUILDS_CALL(RVA_COMMANDER_LED_COUNT), "c", 'i') \
    X(commanderApLeft, BOTH_BUILDS_CALL(RVA_COMMANDER_AP_LEFT), "c", 'i') \
    X(commanderMayAct, BOTH_BUILDS_CALL(RVA_COMMANDER_MAY_ACT), "c", 'B') \
    X(armyBurnValue, WINDOWS_ONLY_CALL(RVA_ARMY_BURN_VALUE), "c", 'i') \
    X(armyAbilityTotal, BOTH_BUILDS_CALL(RVA_ARMY_ABILITY_TOTAL), "caii", 'i') \
    X(itemAbility, WINDOWS_ONLY_CALL(RVA_ITEM_ABILITY), "Ta", 'i') \
    X(itemAbilityNth, BOTH_BUILDS_CALL(RVA_ITEM_ABILITY_NTH), "Tai", 'i') \
    X(countFreeCommanders, WINDOWS_ONLY_CALL(RVA_COUNT_FREE_COMMANDERS), "pi", 'i') \
    X(squareHasSeenUnit, MAC_COPY_CALL(RVA_SQUARE_HAS_SEEN_UNIT, squareHasSeenUnitCopy), "pxy", 'B') \
    X(squareSideStrength, BOTH_BUILDS_CALL(RVA_SQUARE_SIDE_STRENGTH), "Pxyi", 'i') \
    X(squareMoveCost, BOTH_BUILDS_CALL(RVA_SQUARE_MOVE_COST), "cxyi", 'i') \
    X(weaponRangeFor, BOTH_BUILDS_CALL(RVA_WEAPON_RANGE_FOR), "uiw", 'i') \
    X(unitLovesSquare, BOTH_BUILDS_CALL(RVA_INDEP_LOVED_TERRAIN), "uxy", 'B') \
    X(unitLikesSquare, BOTH_BUILDS_CALL(RVA_INDEP_LIKED_TERRAIN), "uxy", 'B') \
    X(unitHatesSquare, BOTH_BUILDS_CALL(RVA_INDEP_HATED_TERRAIN), "uxy", 'B') \
    X(wandererAt, BOTH_BUILDS_CALL(RVA_WANDERER_AT), "xy", 'B') \
    X(teleportUnit, WINDOWS_ONLY_CALL(RVA_TELEPORT_UNIT), "uxy", 'v') \
    X(unitAddXp, WINDOWS_ONLY_CALL(RVA_UNIT_ADD_XP), "ui", 'v') \
    X(unitRemoveItem, WINDOWS_ONLY_CALL(RVA_UNIT_REMOVE_ITEM), "uT", 'i') \
    X(damageUnitPlain, WINDOWS_ONLY_CALL(RVA_DAMAGE_UNIT_PLAIN), "ui", 'v') \
    X(squareHold, WINDOWS_ONLY_CALL(RVA_SET_HOLDIT), "pxy", 'i') \
    X(spendAllApAt, BOTH_BUILDS_CALL(RVA_SPEND_ALL_AP_AT), "Pxy", 'v') \
    X(commanderUseItems, WINDOWS_ONLY_CALL(RVA_COMMANDER_USE_ITEMS), "c", 'i') \
    X(killUnit, WINDOWS_ONLY_CALL(RVA_KILL_UNIT_999), "u", 'v') \
    X(giveItem, BOTH_BUILDS_CALL(RVA_GIVE_ITEM_BY_NAME), "us", 'v') \
    X(takeItem, BOTH_BUILDS_CALL(RVA_TAKE_ITEM_BY_NAME), "us", 'v') \
    X(giveRandomItem, BOTH_BUILDS_CALL(RVA_GIVE_RANDOM_ITEM), "uiii", 'v') \
 \
    X(moveEnchants, WINDOWS_ONLY_CALL(RVA_MOVE_ENCHANTS), "xyxy", 'i') \
    X(swapLocations, BOTH_BUILDS_CALL(RVA_LOCATION_SWAP), "xyxy", PER_BUILD('i', 'v')) \
    X(moveGems, WINDOWS_ONLY_CALL(RVA_MOVE_GEMS), "xyxy", 'v') \
    X(ritualEffectFirst, WINDOWS_ONLY_CALL(RVA_RITUAL_FX_FIRST), "ra", 'i') \
    X(ritualEffectOr, WINDOWS_ONLY_CALL(RVA_RITUAL_FX_FIRST_OR), "rai", 'i') \
    X(ritualPointless, BOTH_BUILDS_CALL(RVA_RITUAL_POINTLESS), "cr", 'B') \
    X(ritualCastableAnywhere, BOTH_BUILDS_CALL(RVA_RITUAL_CASTABLE_ANYWHERE), "cr", 'B') \
    X(slotBlessingOf, MAC_COPY_CALL(RVA_PLAYER_BLESSING_VALUE, slotBlessingOfCopy), "pa", 'i') \
    X(magicPathWeight, MAC_COPY_CALL(RVA_PATH_STRENGTH_WEIGHT, spellPathWeightCopy), "i", 'i') \
    X(spellPathWeight, MAC_COPY_CALL(RVA_PATH_STRENGTH_WEIGHT, spellPathWeightCopy), "i", 'i') \
    X(unitPathBonus, BOTH_BUILDS_CALL(RVA_UNIT_PATH_BONUS), "ui", 'i') \
    X(unitSpellWeaponLevel, BOTH_BUILDS_CALL(RVA_UNIT_SPELL_WEAPON_LEVEL), "ui", 'i') \
    X(memorizeAllowance, MAC_COPY_CALL(RVA_MEMORIZE_ALLOWANCE, memorizeAllowanceCopy), "ui", 'i') \
    X(memorizedSlot, BOTH_BUILDS_CALL(RVA_MEMORIZED_SLOT), "uS", 'i') \
    X(randomSpell, BOTH_BUILDS_CALL(RVA_RANDOM_SPELL), "iii", 'i') \
    X(randomSpellNotKnown, BOTH_BUILDS_CALL(RVA_RANDOM_SPELL_NOT_KNOWN), "uii", 'i') \
    X(countKnownSpells, BOTH_BUILDS_CALL(RVA_COUNT_KNOWN_SPELLS), "uii", 'i') \
    X(countMemorizedSpells, BOTH_BUILDS_CALL(RVA_COUNT_MEMORIZED_SPELLS), "ui", 'i') \
    X(countMemorizedOfType, MAC_COPY_CALL(RVA_COUNT_MEMORIZED_OF_TYPE, countMemorizedOfTypeCopy), "uii", 'i') \
    X(countSpells, MAC_COPY_CALL(RVA_COUNT_SPELLS, countSpellsCopy), "ii", 'i') \
    X(unitFirstMagicPath, BOTH_BUILDS_CALL(RVA_UNIT_FIRST_SPELL_PATH), "u", 'i') \
    X(unitFirstSpellPath, BOTH_BUILDS_CALL(RVA_UNIT_FIRST_SPELL_PATH), "u", 'i') \
    X(randomMemorizedSpell, BOTH_BUILDS_CALL(RVA_RANDOM_MEMORIZED_SPELL), "ui", 'i') \
    X(itemCastableSpell, BOTH_BUILDS_CALL(RVA_ITEM_CASTABLE_SPELL), "u", 'i') \
    X(spellUseful, BOTH_BUILDS_CALL(RVA_SPELL_USEFUL), "uS0", 'b') \
    X(isSpellAttack, MAC_COPY_CALL(RVA_IS_SPELL_ATTACK, isSpellAttackCopy), "i", 'B') \
    X(findSpell, MAC_COPY_CALL(RVA_FIND_SPELL, findSpellCopy), "s", 'i') \
    X(findRitual, MAC_COPY_CALL(RVA_FIND_RITUAL, findRitualCopy), "si", 'i') \
    X(randomRitual, MAC_COPY_CALL(RVA_RANDOM_RITUAL, randomRitualCopy), "ii", 'i') \
    X(ritualMayLearn, BOTH_BUILDS_CALL(RVA_RITUAL_MAY_LEARN), "cr", 'B') \
    X(commanderKnowsRitual, MAC_COPY_CALL(RVA_COMMANDER_KNOWS_RITUAL, commanderKnowsRitualCopy), "cr", 'B') \
    X(memorizeSpell, BOTH_BUILDS_CALL(RVA_MEMORIZE_SPELL), "uS", 'i') \
    X(unmemorizeSpell, WINDOWS_ONLY_CALL(RVA_UNMEMORIZE_SPELL), "uS", 'i') \
    X(chooseMemorized, BOTH_BUILDS_CALL(RVA_CHOOSE_MEMORIZED), "u", 'v') \
    X(repickMemorized, WINDOWS_ONLY_CALL(RVA_UNIT_REPICK_MEMORIZED), "u", 'v') \
    X(learnRandomSpell, BOTH_BUILDS_CALL(RVA_LEARN_RANDOM_SPELL), "uii", 'i') \
    X(learnStartSpells, BOTH_BUILDS_CALL(RVA_LEARN_START_SPELLS), "u", 'v') \
    X(commanderLearnRitual, WINDOWS_ONLY_CALL(RVA_COMMANDER_LEARN_RITUAL), "cr", 'i') \
    X(learnLevelRituals, BOTH_BUILDS_CALL(RVA_LEARN_LEVEL_RITUALS), "cii", 'v') \
    X(getStartRituals, BOTH_BUILDS_CALL(RVA_GET_START_RITUALS), "c", PER_BUILD('i', 'v')) \
    X(removeEnchant, BOTH_BUILDS_CALL(RVA_REMOVE_ENCHANT), "iixy", 'i') \
    X(bolsterBlessing, BOTH_BUILDS_CALL(RVA_BOLSTER_BLESSING), "pii", 'v') \
    X(addPlayerBlessing, BOTH_BUILDS_CALL(RVA_ADD_PLAYER_BLESSING), "paii", 'v') \
 \
    X(worldNews, BOTH_BUILDS_CALL(RVA_WORLD_NEWS), "s", 'v') \
    X(killSquare, BOTH_BUILDS_CALL(RVA_EVENT_KILL_SQUARE), "xy", 'v') \
    X(squareShownName, BOTH_BUILDS_CALL(RVA_SQUARE_SHOWN_NAME), "pxy", 'z') \
 \
    X(playerHasCommander, MAC_COPY_CALL(RVA_PLAYER_HAS_COMMANDER, playerHasCommanderCopy), "p", 'B') \
    X(playerHasCitadel, BOTH_BUILDS_CALL(RVA_PLAYER_HAS_CITADEL), "p", 'B') \
    X(playerClassAbility, MAC_COPY_CALL(RVA_PLAYER_CLASS_ABILITY, playerClassAbilityCopy), "pa", 'i') \
    X(canOwnSquare, BOTH_BUILDS_CALL(RVA_CAN_OWN_SQUARE), "Pxy", 'B') \
    X(startPlacementScore, BOTH_BUILDS_CALL(RVA_START_PLACEMENT_SCORE), "pxyO", 'i') \
    X(startLikesFilter, BOTH_BUILDS_CALL(RVA_START_LIKES_FILTER), "pxy", 'B') \
    X(playerIncomeByType, BOTH_BUILDS_CALL(RVA_PLAYER_INCOME_BY_TYPE), "pi", 'i') \
    X(ironBonusPercent, MAC_COPY_CALL(RVA_IRON_BONUS_PERCENT, ironBonusPercentCopy), "p", 'i') \
    X(goldBonusPercent, WINDOWS_ONLY_CALL(RVA_GOLD_BONUS_PERCENT), "p", 'i') \
    X(squareTeamAbilitySum, BOTH_BUILDS_CALL(RVA_SQUARE_TEAM_ABILITY_SUM), "pxya", 'i') \
    X(squareUnitsAbility, BOTH_BUILDS_CALL(RVA_SQUARE_UNITS_ABILITY), "Pxya0", 'i') \
    X(trollForestSpawn, WINDOWS_ONLY_CALL(RVA_TROLL_FOREST_SPAWN), "xy", 'v') \
    X(toggleAutoRecruit, WINDOWS_ONLY_CALL(RVA_TOGGLE_AUTO_RECRUIT), "pxym", 'i') \
    X(changeLoyalty, BOTH_BUILDS_CALL(RVA_CHANGE_LOYALTY), "xysi", 'i') \
    X(squareIncome, BOTH_BUILDS_CALL(RVA_SQUARE_INCOME), "pxyi", 'i') \
    X(squareGoldIncome, WINDOWS_ONLY_CALL(RVA_SQUARE_GOLD_INCOME), "pxy", 'i') \
    X(squareLifeforce, WINDOWS_ONLY_CALL(RVA_SQUARE_LIFEFORCE_AT), "xy", 'i') \
    X(tradeBonusPercent, WINDOWS_ONLY_CALL(RVA_TRADE_BONUS_PERCENT), "p", 'i') \
    X(illusionTerrain, BOTH_BUILDS_CALL(RVA_ILLUSION_TERRAIN), "pxyO", 'i') \
    X(ruinSquare, BOTH_BUILDS_CALL(RVA_RUIN_SQUARE), "pxy", 'v') \
    X(digTunnelSquare, BOTH_BUILDS_CALL(RVA_DIG_TUNNEL_SQUARE), "pxy", PER_BUILD('i', 'v')) \
    X(alterTerrain, BOTH_BUILDS_CALL(RVA_ALTER_TERRAIN), "pxyiii", 'i') \
    X(destroyGateway, WINDOWS_ONLY_CALL(RVA_DESTROY_GATEWAY), "pxyi", 'i') \
    X(conscription, WINDOWS_ONLY_CALL(RVA_CONSCRIPTION), "p", 'v') \
    X(priestFreespawn, WINDOWS_ONLY_CALL(RVA_PRIEST_FREESPAWN), "p", 'v') \
    X(inquisitionSpawn, BOTH_BUILDS_CALL(RVA_INQUISITION_SPAWN), "p", 'v') \
    X(resetIncomeCaches, BOTH_BUILDS_CALL(RVA_RESET_INCOME_CACHES), "", 'v') \
 \
    X(squareIsSky, BOTH_BUILDS_CALL(RVA_SQUARE_IS_SKY), "xy", 'b') \
    X(terrainBurnableAt, MAC_COPY_CALL(RVA_TERRAIN_BURNABLE_AT, terrainBurnableAtCopy), "xy", 'b') \
    X(terrainRuinForm, BOTH_BUILDS_CALL(RVA_TERRAIN_RUIN_FORM), "t", 'i') \
    X(squareIsSouth, WINDOWS_ONLY_CALL(RVA_SQUARE_SOUTH), "xy", 'b') \
    X(squareIsNorth, MAC_COPY_CALL(RVA_SQUARE_NORTH, squareIsNorthCopy), "xy", 'b') \
    X(countWaterNear, BOTH_BUILDS_CALL(RVA_COUNT_WATER_NEAR), "xy", 'i') \
    X(countLandNear, BOTH_BUILDS_CALL(RVA_COUNT_LAND_NEAR), "xy", 'i') \
    X(squareDefaultGround, BOTH_BUILDS_CALL(RVA_SQUARE_DEFAULT_GROUND), "xy", 'i') \
    X(squareIsCoast, BOTH_BUILDS_CALL(RVA_SQUARE_IS_COAST), "xy", 'b') \
    X(squareEffectiveTemp, BOTH_BUILDS_CALL(RVA_SQUARE_EFFECTIVE_TEMP), "xy", 'i') \
    X(swapSquares, BOTH_BUILDS_CALL(RVA_SWAP_SQUARES), "xyxy", PER_BUILD('i', 'v')) \
    X(teleportSquare, WINDOWS_ONLY_CALL(RVA_TELEPORT_SQUARE), "xyxy", 'v') \
 \
    X(unitMayBeOnLand, BOTH_BUILDS_CALL(RVA_UNIT_MAY_BE_ON_LAND), "u", 'b') \
    X(unitMagicResistance, BOTH_BUILDS_CALL(RVA_UNIT_MR), "u", 'i') \
    X(unitIsHuge, BOTH_BUILDS_CALL(RVA_UNIT_IS_HUGE), "u", 'b') \
    X(unitLifeless, WINDOWS_ONLY_CALL(RVA_UNIT_LIFELESS), "u", 'b') \
    X(unitPrimable, BOTH_BUILDS_CALL(RVA_UNIT_PRIMABLE), "u", 'b') \
    X(unitRemoveTempAbility, WINDOWS_ONLY_CALL(RVA_UNIT_REMOVE_TEMP_ABILITY), "ua", 'b') \
    X(checkFollowerLinks, BOTH_BUILDS_CALL(RVA_CHECK_FOLLOWER_LINKS), "c", 'v') \
    X(orderBurnForest, BOTH_BUILDS_CALL(RVA_ORDER_BURN_FOREST), PER_BUILD("0c", "c"), 'v') \
    X(orderChopForest, BOTH_BUILDS_CALL(RVA_ORDER_CHOP_FOREST), PER_BUILD("0c", "c"), 'v') \
    X(orderSlaveHunt, BOTH_BUILDS_CALL(RVA_ORDER_SLAVE_HUNT), "c", 'v') \
    X(orderEatVillage, BOTH_BUILDS_CALL(RVA_ORDER_EAT_VILLAGE), "c", 'v') \
    X(useSpecialSite, BOTH_BUILDS_CALL(RVA_USE_SPECIAL_SITE), "xyc", 'v') \
 \
    X(aiThreatNear, WINDOWS_ONLY_CALL(RVA_AI_THREAT_NEAR), "pxyi", 'B') \
    X(aiCountAllyTargetsNear, MAC_COPY_CALL(RVA_AI_COUNT_ALLY_TARGETS_NEAR, aiCountAllyTargetsNearCopy), "pxyci", 'i') \
    X(aiUpdateStrengthMemory, BOTH_BUILDS_CALL(RVA_AI_UPDATE_STRENGTH_MEMORY), "p", 'v') \
    X(aiMakeGuardsAt, BOTH_BUILDS_CALL(RVA_AI_MAKE_GUARDS_AT), "pxyi", 'v') \
    X(aiMakeDefenders, WINDOWS_ONLY_CALL(RVA_AI_MAKE_DEFENDERS), "p", 'v') \
    X(aiCountTroops, BOTH_BUILDS_CALL(RVA_AI_COUNT_TROOPS), "pxyOOO", 'w') \
    X(aiUnitStays, WINDOWS_ONLY_CALL(RVA_AI_UNIT_STAYS), "u", 'B') \
    X(aiNearestTerrain, RVA_AI_NEAREST_TREQ, "ixyPOO", 'i') \
    X(aiRitualMaster, WINDOWS_ONLY_CALL(RVA_AI_RITUAL_MASTER), "pa", 'i') \
    X(aiCommanderBoatPlaces, WINDOWS_ONLY_CALL(RVA_AI_COMMANDER_BOAT_PLACES), "c", 'i') \
    X(aiShipDropCrew, WINDOWS_ONLY_CALL(RVA_AI_SHIP_DROP_CREW), "c", 'B') \
    X(aiShipTakeBoat, WINDOWS_ONLY_CALL(RVA_AI_SHIP_TAKE_BOAT), "c", 'B') \
    X(aiShipTakeCrew, WINDOWS_ONLY_CALL(RVA_AI_SHIP_TAKE_CREW), "c", 'B') \
    X(aiGroupBoatTotal, WINDOWS_ONLY_CALL(RVA_AI_GROUP_BOAT_TOTAL), "c", 'i') \
    X(aiPortsBoatTotals, WINDOWS_ONLY_CALL(RVA_AI_PORTS_BOAT_TOTALS), "pO", 'i') \
    X(aiShipsAt, WINDOWS_ONLY_CALL(RVA_AI_SHIPS_AT), "pxy", 'i') \
    X(aiCountPlanners, WINDOWS_ONLY_CALL(RVA_AI_COUNT_PLANNERS), "pr", 'i') \
    X(aiPlanSquareBonus, WINDOWS_ONLY_CALL(RVA_AI_PLAN_SQUARE_BONUS), "cxy", 'i') \
    X(aiRedistributeItems, WINDOWS_ONLY_CALL(RVA_AI_REDISTRIBUTE_ITEMS), "pxy", 'v') \
    X(aiShips, WINDOWS_ONLY_CALL(RVA_AI_SHIPS), "c", 'i') \
    X(aiConquerStep, WINDOWS_ONLY_CALL(RVA_AI_CONQUER_STEP), "ciOO", 'i') \
    X(aiExploreStep, WINDOWS_ONLY_CALL(RVA_AI_EXPLORE_STEP), "ciOO", 'i') \
    X(aiFarExploreStep, BOTH_BUILDS_CALL(RVA_AI_FAR_EXPLORE_STEP), "ciOO", 'i') \
    X(aiRallyStep, WINDOWS_ONLY_CALL(RVA_AI_RALLY_STEP), "ciOO", 'i') \
    X(aiFriendlyMoveStep, WINDOWS_ONLY_CALL(RVA_AI_FRIENDLY_MOVE_STEP), "ciOO", 'i') \
 \
    X(battleUnitAt, BOTH_BUILDS_CALL(RVA_BATTLE_UNIT_AT), "!XY", 'i') \
    X(battleFearAt, WINDOWS_ONLY_CALL(RVA_BATTLE_FEAR_AT), "!XYp", 'i') \
    X(battleLocalLeadership, WINDOWS_ONLY_CALL(RVA_BATTLE_LOCAL_LEADERSHIP), "!XYp", 'i') \
    X(battleCountRank, BOTH_BUILDS_CALL(RVA_BATTLE_COUNT_RANK), "!pii", 'i') \
    X(battleWinner, MAC_COPY_CALL(RVA_BATTLE_WINNER, battleWinnerCopy), "!", 'i') \
    X(battleLineOfSight, WINDOWS_ONLY_CALL(RVA_BATTLE_LINE_OF_SIGHT), "!uu", 'b') \
    X(battleInRange, WINDOWS_ONLY_CALL(RVA_BATTLE_IN_RANGE), "!uuii", 'b') \
    X(unitInContact, WINDOWS_ONLY_CALL(RVA_UNIT_IN_CONTACT), "!u", 'B') \
    X(battleCanStand, BOTH_BUILDS_CALL(RVA_BATTLE_CAN_STAND), "!uXYi", 'b') \
    X(battleUnitDistance, WINDOWS_ONLY_CALL(RVA_BATTLE_UNIT_DISTANCE), "!uuO", 'i') \
    X(battleNearestEnemy, WINDOWS_ONLY_CALL(RVA_BATTLE_NEAREST_ENEMY), "!uO", 'i') \
    X(battleEnemyExtent, MAC_COPY_CALL(RVA_BATTLE_ENEMY_EXTENT, battleEnemyExtentCopy), "!pOO", 'w') \
    X(battleSquareAbility, WINDOWS_ONLY_CALL(RVA_BATTLE_SQUARE_ABILITY), "!ua0", 'i') \
    X(battleWallCover, BOTH_BUILDS_CALL(RVA_BATTLE_WALL_COVER), "!ui", 'i') \
    X(battleAdvanceRank, BOTH_BUILDS_CALL(RVA_BATTLE_ADVANCE_RANK), "!u", 'i') \
 \
    X(findUnitAt, BOTH_BUILDS_CALL(RVA_FIND_UNIT_AT), "Pxys", 'i') \
    X(findItemHolderAt, BOTH_BUILDS_CALL(RVA_FIND_ITEM_HOLDER_AT), "Pxys", 'i') \
    X(randomSquareOfTerrain, BOTH_BUILDS_CALL(RVA_RANDOM_SQUARE_OF_TERRAIN), "LiOO", 'i') \
    X(groundFromNeighbours, BOTH_BUILDS_CALL(RVA_GROUND_FROM_NEIGHBOURS), "xy", 'i') \
 \
    X(unitRandomAffliction, BOTH_BUILDS_CALL(RVA_UNIT_RANDOM_AFFLICTION), "u", 'v') \
    X(addAffliction, BOTH_BUILDS_CALL(RVA_ADD_AFFLICTION), "uU", 'v') \
    X(revealTerrainOnPlane, WINDOWS_ONLY_CALL(RVA_REVEAL_TERRAIN_ON_PLANE), "pLi", 'i') \
    X(nearTerrainSquare, BOTH_BUILDS_CALL(RVA_NEAR_TERRAIN_SQUARE), PER_BUILD("LixyOO", "lixyOO"), 'i') \
 \
    X(putRareSquare, WINDOWS_ONLY_CALL(RVA_PUT_RARE_SQUARE), "xy", 'v') \
    X(groveReversion, WINDOWS_ONLY_CALL(RVA_GROVE_REVERSION), "xy", 'v') \
    X(revealDisc, BOTH_BUILDS_CALL(RVA_REVEAL_DISC), "pxyfii", 'v') \
    X(setSeenDisc, BOTH_BUILDS_CALL(RVA_SET_SEEN_DISC), "Pxyfi", 'v') \
 \
    X(playSample, BOTH_BUILDS_CALL(RVA_PLAY_SAMPLE), "ii", 'v') \
    X(autoName, BOTH_BUILDS_CALL(RVA_AUTO_NAME), "NZ", 'z') \
    X(confirm, BOTH_BUILDS_CALL(RVA_CONFIRM), PER_BUILD("%s", "%.s"), 'b') \
 \
    X(squareSideWater, WINDOWS_ONLY_CALL(RVA_MAPGEN_SIDE_WATER), "xy", 'b') \
    X(squareIsWet, MAC_COPY_CALL(RVA_GEN_SQUARE_IS_WET, squareIsWetCopy), "xy", 'b') \
    X(wetNeighbours, BOTH_BUILDS_CALL(RVA_GEN_WET_NEIGHBOURS), "xy", 'i') \
    X(nearestTerrainDistance, BOTH_BUILDS_CALL(RVA_MAPGEN_NEAREST_TERRAIN), "xyiiii", 'i') \
    X(countTerrainCode, MAC_COPY_CALL(RVA_MAPGEN_COUNT_TERRAIN, countTerrainCodeCopy), "li", 'i') \
 \
    X(openIncomeDetail, WINDOWS_ONLY_CALL(RVA_INCOME_DETAIL_SCREEN), "~pR", 'v') \
    X(openPlaneInfo, BOTH_BUILDS_CALL(RVA_PLANE_INFO_SCREEN), "~l", 'v') \
    X(openSquareInfo, BOTH_BUILDS_CALL(RVA_SQUARE_INFO), "~pxyi", 'v') \
    X(openCeremonies, WINDOWS_ONLY_CALL(RVA_CEREMONY_SCREEN), "~c", 'v') \
    X(openRitualScreen, BOTH_BUILDS_CALL(RVA_RITUAL_SCREEN), "~u", 'v') \
    X(openLibrary, BOTH_BUILDS_CALL(RVA_LIBRARY_SCREEN), "~c", 'v') \
    X(openChopBurn, BOTH_BUILDS_CALL(RVA_CHOP_BURN_SCREEN), PER_BUILD("~", "~^"), 'v') \
    X(openSlaveHunt, BOTH_BUILDS_CALL(RVA_SLAVE_HUNT_SCREEN), PER_BUILD("~", "~^"), 'v') \
    X(openArmyMenu, BOTH_BUILDS_CALL(RVA_ARMY_COMMANDS_MENU), "~c", 'v') \
    X(openWeaponInfo, WINDOWS_ONLY_CALL(RVA_WEAPON_INFO_WINDOW), "~w0", 'v') \
    X(openItemInfo, WINDOWS_ONLY_CALL(RVA_ITEM_INFO_SCREEN), "~T", 'v') \
    X(monsterPicker, WINDOWS_ONLY_CALL(RVA_MONSTER_PICKER), "~iia", 'i') \
    X(classPicker, BOTH_BUILDS_CALL(RVA_CLASS_PICKER), "@~", 'i') \
    X(countCommandersWithAp, BOTH_BUILDS_CALL(RVA_COUNT_COMMANDERS_WITH_AP), "p", 'i') \
    X(selectedCommanderDone, WINDOWS_ONLY_CALL(RVA_COMMANDER_SET_DONE), "", 'v') \
    X(selectNextCommander, BOTH_BUILDS_CALL(RVA_SELECT_NEXT_COMMANDER), "", 'v') \
    X(selectPrevCommander, WINDOWS_ONLY_CALL(RVA_SELECT_PREV_COMMANDER), "", 'v') \
 \
    X(countUnitsAt, BOTH_BUILDS_CALL(RVA_COUNT_UNITS_AT), "Pxys", 'i') \
    X(countUnits, BOTH_BUILDS_CALL(RVA_COUNT_UNITS), "PLs", 'i') \
    X(nearestUnitAbilityIs, WINDOWS_ONLY_CALL(RVA_NEAREST_UNIT_ABILITY_IS), "Lxyaii", 'i') \
    X(armyAtForPlayer, MAC_COPY_CALL(RVA_ARMY_AT_FOR_PLAYER, armyAtForPlayerCopy), "Pxy", 'b') \
    X(commanderTheName, MAC_COPY_CALL(RVA_COMMANDER_THE_NAME, commanderTheName), "c", 'z') \
    X(playerRitualFlags, WINDOWS_ONLY_CALL(RVA_PLAYER_RITUAL_FLAGS), "p", 'i') \
    X(countSlotsWithController, MAC_COPY_CALL(RVA_COUNT_SLOTS_WITH_CONTROLLER, countSlotsWithControllerCopy), "i", 'i') \
    X(findClass, BOTH_BUILDS_CALL(RVA_FIND_CLASS), "s", 'i') \
    X(weaponExtra, MAC_COPY_CALL(RVA_WEAPON_EXTRA, weaponExtraCopy), "wi", 'i') \
    X(afflictionImmune, BOTH_BUILDS_CALL(RVA_AFFLICTION_IMMUNE), "uU", 'b') \
    X(soullessForm, BOTH_BUILDS_CALL(RVA_SOULLESS_FORM), "u", 'i') \
 \
    X(addBenefit, WINDOWS_ONLY_CALL(RVA_ADD_BENEFIT), "uU", 'v') \
    X(heroArrival, WINDOWS_ONLY_CALL(RVA_HERO_ARRIVAL), "p", 'v') \
    X(cleanSquare, BOTH_BUILDS_CALL(RVA_CLEAN_SQUARE), "xyi", 'v') \
    X(initSquareGems, BOTH_BUILDS_CALL(RVA_INIT_SQUARE_GEMS), "xy", 'v') \
    X(initSquareCorpses, BOTH_BUILDS_CALL(RVA_INIT_SQUARE_CORPSES), "xy", 'v') \
    X(runHatreds, WINDOWS_ONLY_CALL(RVA_RUN_HATREDS), "", 'v') \
    X(sandstormCloudUpdate, WINDOWS_ONLY_CALL(RVA_SANDSTORM_CLOUD_UPDATE), "", 'v') \
    X(startTune, BOTH_BUILDS_CALL(RVA_START_TUNE), "ii", 'v') \
    X(numberBox, BOTH_BUILDS_CALL(RVA_NUMBER_BOX), PER_BUILD("s%s", "s%.s"), 'i') \
 \
    X(battleObjectAt, BOTH_BUILDS_CALL(RVA_BATTLE_UNIT_AT_2), "!XY0", 'i') \
    X(battlePathfind, BOTH_BUILDS_CALL(RVA_BATTLE_PATHFIND), "!uXYOO", 'i') \
    X(battleKillerMove, WINDOWS_ONLY_CALL(RVA_BATTLE_KILLER_MOVE), "!uOO", 'i') \
 \
    X(planeIsReachable, MAC_COPY_CALL(RVA_PLANE_IS_REACHABLE, planeIsReachableCopy), "lxy", 'B') \
    X(destroyEnchantsAt, BOTH_BUILDS_CALL(RVA_ENCHANTS_DESTROYED_AT), "pxy", 'v') \
    X(stormSpawn, WINDOWS_ONLY_CALL(RVA_STORM_SPAWN), "xy", 'v') \
    X(wildSpawn, WINDOWS_ONLY_CALL(RVA_WILD_SPAWN), "xy", 'v') \
    X(revelsSpawn, WINDOWS_ONLY_CALL(RVA_REVELS_SPAWN), "xy", 'v') \
    X(forestShepherdSpawn, WINDOWS_ONLY_CALL(RVA_FOREST_SHEPHERD_SPAWN), "xy", 'v') \
 \
    X(setSquareOwner, WINDOWS_ONLY_CALL(RVA_SQUARE_SET_OWNER), "xyp", 'v') \
    X(findAfflictedUnitAt, BOTH_BUILDS_CALL(RVA_FIND_AFFLICTED_UNIT_AT), "PxyiUU", 'i') \
    X(classAbilityNth, BOTH_BUILDS_CALL(RVA_CLASS_ABILITY_NTH), "Cai", 'i') \
    X(countSideUnits, WINDOWS_ONLY_CALL(RVA_COUNT_SIDE_UNITS), "pxy", 'i') \
    X(loneHumanNow, BOTH_BUILDS_CALL(RVA_BEFORE_GAME_START), "", 'i') \

