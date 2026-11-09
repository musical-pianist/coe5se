---@meta
-- What CoE5SE gives a mod's Lua script, written for your editor. CoE5SE never runs this file: it runs
-- only <ModName>.lua, <ModName>_commands.lua, and the files they require. Copy it next to your
-- script, and VS Code or Cursor with the Lua extension (LuaLS) completes coe5.* as you type and shows
-- these notes on hover.
--
-- How a script runs: CoE5SE loads it on the first main menu frame, after every DLL has started,
-- and runs its top-level code once. Only an enabled mod's files ever run. A mod that adds mod
-- commands keeps its coe5.addModCommand calls and their handlers in <ModName>_commands.lua, which
-- loads first, in the same Lua state, before the startup mod pass reads a line; <ModName>.lua sees
-- what it defines, and require("<ModName>_commands") returns what it returned. If the commands file
-- stops with an error, <ModName>.lua never loads. The two files exist only so the commands are
-- added before the game reads any mod's content. A later CoE5SE may load <ModName>.lua early too,
-- right after the commands file, so write neither to depend on when the other loads. Of the coe5
-- functions, a commands file may call only coe5.addModCommand and coe5.log while it loads; any
-- other coe5 call returns nil (lua_refused reason=commands_file), while require, print, warn, and
-- Lua's libraries work as usual. It has Lua's base library (without load, dofile, loadfile, and
-- collectgarbage, and with CoE5SE's own require, below), string, table, math, utf8, and coroutine;
-- there is no io, os, or package, and its only files are the ones it requires from its own mod's
-- folder and its own data files (coe5.readIni and the rest). print and warn
-- write lua_print and lua_warning lines to coe5mod.log, as coe5.log does; coe5.log's and print's
-- lines only while the player has CoE5SE's script_log setting on (off by default). One call into
-- the script may run 20,000,000 Lua steps, counting the coroutines it resumes, its __gc
-- finalizers, and library loops (table.move, concat, unpack, sort, insert, remove, and string
-- find, match, gmatch, gsub: one step per element, byte, or match step), and 10,000 steps for each
-- coe5.route call (about 2,000 routes in one call). The count starts again
-- when the game draws a frame (coe5.ui.wait, a next() that ran one, or a call that comes after
-- one, even inside the script's own wrap). A call that spends it stops
-- the script for good: pcall, a __close handler, or a new coroutine can't catch the stop, and
-- xpcall's handler doesn't run for it (lua_refused reason=step_limit). A script's Lua state may
-- hold 1.5 GB, and all scripts' together 6 GB: past the first the script gets "not enough memory"
-- and is stopped the same way (memory_cap); past the second the script holding the most is
-- (memory_total). A coe5 call made outside a
-- call into the script (a __gc finalizer while CoE5SE closes a stopped script) returns nil
-- (reason=not_running).
--
-- Cheat sheet:
--   Run another file of your mod   local safety = require("gatherer.safety")   (gatherer/safety.lua in your mod's folder)
--   Log a line                     coe5.log("hello")
--   Add a main menu button         coe5.addMainMenuButton("My Mod", openScreen)
--   ... right after Mods           coe5.addMainMenuButton("My Mod", openScreen, coe5.mainMenu.mods)
--   Add a Game Menu line           coe5.addGameMenuLine{label = "Resign now", onClick = function() return coe5.gameMenu.resign end}
--   ... in its Settings section    coe5.addGameMenuLine{label = "My Options", onClick = openOptions, section = coe5.gameMenuSection.settings}
--   ... in a section of its own    coe5.addGameMenuLine{label = "Resources", onClick = openResources, section = "Cheats"}
--   End the game, start a new one  coe5.restartGame()   (in a Game Menu line's onClick)
--   Pick a save, end, and load it  coe5.loadGame()      (in a Game Menu line's onClick)
--   Read or change a game value    coe5.get("society"), coe5.set("society", 3)
--   Read a player slot's value     coe5.get("slotName", 0)
--   Give a player 10 Gold          coe5.set("slotResource", p, 0, coe5.get("slotResource", p, 0) + 10)
--   Run code around a game call    coe5.wrap("PLAY_GAME_NEW_WORLD", function(next, mode) return next(mode) end)
--   ... outside other scripts' wraps  coe5.wrap("PLAY_GAME_NEW_WORLD", function(next, mode) return next(mode) end, 100)
--   Which machine, in a LAN game   coe5.netRole() == coe5.net.client; coe5.localPlayer()   (nil while a client waits for its turn)
--   Keep state that lasts          coe5.gameData.count = (coe5.gameData.count or 0) + 1   (in saves and LAN turns; globals don't last)
--   A player's own state           coe5.playerData[coe5.localPlayer()].asked = true   (a LAN client's reaches the host)
--   Watch a call you may not change  coe5.observe("MONTH_START", nil, function(result, kind, text) coe5.log(coe5.turn()) end)
--   Send players a message         coe5.sendMessage(coe5.message.everyHuman, "Hello", "Welcome back.")
--   Show a box the player closes   coe5.showMessageBox("Sealed Gate", "The gate is sealed.")   (at once, a human's turn only)
--   A game number by name          coe5.controller.human, coe5.gameOverMode.plain, ... (the tables at the end)
--   Read a setting your .c5se has  coe5.setting("welcome")   (on/off: true or false; a choice: its index from 0; a key: its key value)
--   Add a screen to your settings page  coe5.addScreen("Open My Mod", openScreen)
--   Change a key of the game's own coe5.rebindKey(coe5.keyAction.mapEndTurn, string.byte("k"))
--   Hide the Read Tip button       coe5.hideMainMenuButton(coe5.mainMenu.readTip, function() return true end)
--   Keep data in a file            coe5.writeIni("last", {Player1 = {Class = 3}}), coe5.readIni("last").Player1.Class
--   A battle with no game          coe5.testBattle(coe5.findMonster("Old Wizard"), -1, coe5.findMonster("Spearman"), 40)
--   Mixed groups on a test field   local ok, units = coe5.openTestField{commander = wizard, enemies = {{spearman, 5}, {wolf, 4}}}
--   Where a unit stands in battle  coe5.get("unitBattleX", unit), coe5.get("unitBattleY", unit)
--   Text to the clipboard          coe5.copyText("newweapon \"Laser\"\n")
--   Text as a new mod              coe5.saveNewMod("My Laser", "newweapon \"Laser\"\ndmg 12\n")
--   Change your own weapon         coe5.set("weapon", w, "dmg", 12), w a weapon your mod made
--   Build a screen                 coe5.ui.screenBegin("My Mod") ... coe5.ui.wait()
--   A row with a monster's sprite  coe5.ui.monsterRow(20, coe5.findMonster("Spearman"), "Spearman", "Recruit")
--   A commander's rituals          coe5.get("commanderRitual", c, 0), coe5.get("ritualString", r, 0)   (nil past the last)
--   What a ritual needs            coe5.get("ritualCostType", r, 0), coe5.get("ritualTerrainName", r), coe5.get("ritualEffect", r, 0)
--   Can a caster pay a cost        coe5.get("commanderCostAvailable", c, coe5.get("ritualCostType", r, 0)) >= amount
--   Time something                 local start = coe5.get("ticksMs") ... coe5.get("ticksMs") - start   (milliseconds)
--   A class's recruits             coe5.get("classRecruitName", coe5.get("slotClass", p), 0)   (nil past the last)
--   Make a unit on a square        coe5.spawnUnit(p, coe5.monsterVariant(coe5.findMonster("Spearman")), x, y)
--   The square under the pointer   coe5.get("mapPointerX"), coe5.get("mapPointerY")   (in a MAP_SCREEN_WAIT wrap, after next)
--   Keep the map from a click      coe5.set("mouseButtons", 0)   (the same wrap)
--   Click on text drawn over the map  coe5.wrap("MAP_SCREEN_WAIT", function(next, b, m) local id = next(b, m) local x, y = coe5.get("uiPointerX"), coe5.get("uiPointerY") if coe5.get("mouseButtons") == 1 and x >= 10 and x < 240 and y >= 300 and y < 336 then coe5.set("mouseButtons", 0) return 0 end return id end)   (Windows)
--   Send an AI explorer elsewhere  coe5.wrap("AI_EXPLORE", function(next, c, comstr) local s, x, y = next(c, comstr) return s, x, y end)
--   A square's plane and portal    coe5.get("squarePlane", x, y), coe5.get("squarePortal", x, y), coe5.portalExits(x, y)
--   Seal a portal                  coe5.onPortalCrossing(function(c) if c.portal == 1234 then return false end end)   (humans, the AI and its routes)
--   Tint a plane's map             coe5.setPlaneTint("Inferno", 1, 0.5, 0, 0.2), coe5.clearPlaneTint("Inferno"); local r, g, b, s = coe5.get("planeTint", 4)
--   Draw a mod terrain like a game one  coe5.setTerrainLooks{terrain = "Dungeon Wall", ground = "Cave Wall", picture = "./wall.tga"}   (at load)
--   A battle map for a terrain     coe5.addBattleMap{file = "./maps/moria_halls.coem", terrains = {"Moria Deeps"}, weight = 20}   (at load)
--   A terrain's battle plants      coe5.setBattleScatter{terrain = "Moria Deeps", objects = {{"Mithril Vein", 20}}}   (at load)
--   Add an independent faction     coe5.addFaction{name = "Ash Court", like = "Inferno", home = "Ash Plane", color = {0.8, 0.3, 0}}   (at load; coe5.factionSlot("Ash Court") in the game)
--   Add a mod command              coe5.addModCommand("mymod_gold", function(args, mod, level) ... end)   (in <ModName>_commands.lua only)
--   Change a square as the world is built  coe5.wrap("PLAY_GAME_NEW_WORLD", function(next, mode) local r = next(mode) coe5.set("squareTerrain", x, y, t) coe5.set("squareName", x, y, "Old Tower") return r end)
--   A commander's square           local u = coe5.get("commanderUnit", c); coe5.get("unitX", u), coe5.get("unitY", u)
--   Can an AI commander get there  coe5.route(c, x, y)   (nil for no route; only inside an AI site's wrap)
--   Reveal the map for a turn      coe5.revealMap(coe5.get("activePlayer"))   (one plane: coe5.revealMap(p, coe5.planeIndex("Inferno")))
--   A square's army, unit by unit  for _, u in ipairs(coe5.squareUnits(x, y)) do local s = coe5.get("unitStrength", coe5.get("unitOwner", u), u) end
--   Does a player see a unit        coe5.get("unitVisibleTo", u, p)   (stealth, invisibility, spiritsight; squareSeen for the fog)
--   Write the AI's trace lines only  coe5.set("traceLevel", 2); coe5.keepTraceLines({"ailog", "aiship"})
--   Keep an AI from an attack      coe5.wrap("AI_CONQUER", function(next, c, comstr) local s, x, y = next(c, comstr) if risky(x, y) then return 0 end return s, x, y end)
--   Move a resting AI commander    coe5.wrap("AI_RALLY", function(next, c, comstr) local s, x, y = next(c, comstr) if s <= 0 then return 1, hx, hy end return s, x, y end)
--   Drop an AI caster's ritual plan  coe5.scrapRitualPlan(c)   (only inside an AI site, AI_TURN's observers included)
--   Free one AI unit from its commander  coe5.releaseFollower(u)   (only inside an AI site, a unit of the moving AI)
--   Move one AI unit to another commander  coe5.transferFollower(u, c)   (same, both of the moving AI on one square)
--   Plan a ritual an AI caster knows  coe5.planRitual(c, coe5.get("commanderRitual", c, 0))   (same; the commander has no plan)
--   A player's income next turn    coe5.get("slotIncome", p, 3)   (Fungus)
--   That income as its AI gets it  coe5.get("slotAiIncome", p, 0)   (Gold; a played AI is human meanwhile)
--   And what its trade moves       coe5.get("slotAiIncome", p, 0) + coe5.get("slotAiTrade", p, 0)   (the gold its next turn start brings)
--   Play an AI player's turn       coe5.observe("HUMAN_TURN", nil, function(_, p) coe5.runAiTurn(p) end)   (the game's AI moves what the player left)
--   Which mods changed an answer   coe5.wrap("AI_CONQUER", function(next, c, comstr) local s, x, y = next(c, comstr) for _, ch in ipairs(coe5.changedBy()) do coe5.log(ch.mod) end return s, x, y end, 2000000000)
--   Move the map to a square       coe5.centerMap(x, y)   (its plane too; Windows)
--   Build a world to look at       coe5.startPreviewWorld{plane = "Agartha", seed = 7, mapSize = coe5.mapSize.medium, society = 0, seats = {0, 0}}   (from the main menu; Windows)
--   Show an army's battlefield view coe5.viewArmy(x, y)   (the game's right-click view; Windows)
--   See what another player sees   coe5.showSight(seat, p) ... coe5.restoreSight(seat)   (a human seat; Windows)
--   Every battle's report to a seat coe5.reportBattlesTo(seat)   (with Battle reports on; Windows)
--   A played AI's Recruit buy      coe5.wrap("RECRUIT_WAIT", function(next, b, m) local r = coe5.recruitScreen() ... coe5.aiBuyOffer(r.player, offer, r.x, r.y) return 0 end)
--   What a terrain pays            coe5.get("terrainIncome", coe5.get("squareTerrain", x, y), 3)
--   Does a square count for a terrain  coe5.terrainMatch(coe5.get("ritualTerrain", r), x, y)
--   Keep the AI from planning a ritual  coe5.wrap("AI_RITUAL_CANDIDATE", function(next, c, ritual) if blocked(c, ritual) then return false end return next(c, ritual) end)
--   A player's recruitment offers  for _, o in ipairs(coe5.shopOffers(p)) do coe5.log(o.name .. " " .. o.gold) end
--   Buy for the AI yourself        coe5.wrap("AI_RECRUIT", function(next, p) local sq = coe5.recruitSquares(p, offer)[1] if sq then coe5.aiBuyOffer(p, offer, sq.x, sq.y) end end)   (skipping next skips the game's buying)
--   Change a built-in offer        for _, o in ipairs(coe5.builtinOffers()) do if o.name == "Sage" then coe5.replaceBuiltinOffer(o.id, {gold = 20}) end end
--   Add a monthly offer            coe5.addShopOffer{name = "Spearman", count = 10, gold = 60, chance = function(p) return coe5.guildCount(p, "Mercenaries Guild") * 10 end}
--   Leave troops for another AI commander  coe5.wrap("AI_GATHER_TROOPS", function(next, c) if not leader(c) then next(c) end end)
--   Keep an AI commander from being picked up  coe5.wrap("AI_PICK_UP_LONER", function(next, c, unit) if busy(unit) then return 0 end return next(c, unit) end)
--   Split AI armies of 30 units or more  coe5.wrap("AI_SPLIT_ARMY", function(next, c, minUnits) next(c, 30) end)
--   Route onto an army with a group's strength  coe5.wrap("AI_MOVE_COMSTR", function(next, c) return next(c) + helpers(c) end)
--   Let the AI cross frozen water  coe5.wrap("AI_DANGEROUS_STEP", function(next, c, dx, dy) if frozenAhead(c, dx, dy) then return 0 end return next(c, dx, dy) end)
--   Two guards where the AI leaves one  coe5.wrap("AI_MAKE_GUARDS", function(next, p, x, y, n) next(p, x, y, n * 2) end)
--   Cast an AI ritual at a square you pick  coe5.wrap("AI_RITUAL_TARGET", function(next, c, ritual) local found, x, y = next(c, ritual) if found == 1 then x, y = better(c, ritual, x, y) end return found, x, y end)
--   No random villages on a terrain  coe5.wrap("DECORATE_SQUARE", function(next, x, y) if coe5.get("squareTerrain", x, y) ~= t then next(x, y) end end)
--   A random enemy, the AI's way   coe5.aiRandomEnemy(p)   (-1 for none)
--   The commander a unit follows   coe5.get("unitCommander", u)   (-1 for none)
--   Tell every human player        coe5.worldNews("The Ring has been found")   (Windows only for now)
--   The same spot on another plane coe5.planeSameSpot(2, x, y)
--   Ask the player for a square    local x, y = coe5.chooseLocation("Where shall the Ring go?", p, x0, y0, 0)   (Windows only for now)
--   Ask yes or no                  if coe5.confirm("Destroy the Ring?") then ... end   (Windows only for now)
--   Ask for a number               local n = coe5.numberBox("Tribute", "How much gold?")   (Windows only for now)
--   Open one of the game's screens coe5.openIncomeDetail(p, 0), coe5.openPlaneInfo(4), coe5.openRitualScreen(unit)   (Windows only for now)
--   Where the AI would go next     local score, x, y = coe5.aiConquerStep(c, strength)   (Windows only for now)
--   React to Horrors coming        coe5.observe("POP_HORRORS", function(_, chance) coe5.log("chance " .. chance) end)   (Windows only for now)
--   Every army a player sees now   for _, a in ipairs(coe5.armies(p)) do coe5.log(a.x .. "," .. a.y .. " " .. a.owner .. " " .. a.units) end
--   Does a monster fly             coe5.get("monsterMoveFlags", coe5.get("unitMonster", u)) & 1 ~= 0   (unitMoveFlags adds its items)
--   A monster's ability by its mod command  coe5.get("monsterAbility", m, "aihold"), coe5.get("unitAbility", u, "mr")
--   A record by its mod commands   coe5.get("monster", m, "hp"), coe5.get("terrain", t, "citadel"), coe5.get("class", c, "startplane")
--   Can an AI army take a sea square  coe5.get("commanderMoveFlags", c) & 192 ~= 0
--   A caster's ritual level        coe5.get("commanderRitualLevel", c)   (its school: commanderRitpow)
--   A square's library level       coe5.get("squareLibraryLevel", x, y)
--   Did the player press Space on a commander  coe5.get("commanderDone", c)   ("Commander is done for this turn")
--   Block or add to a Voice of El ceremony  coe5.wrap("CEREMONY_PERFORM", function(next, ceremony, c, target) if coe5.get("ceremonyName", ceremony) ~= "Call the Inquisition" then next(ceremony, c, target) end end)
--   Change a ceremony's cost       coe5.wrap("CEREMONY_COSTS", function(next, c) next(c) coe5.set("ceremonyCost", n, 30) end)   (n: the ceremony whose ceremonyName you matched)
--   Keep a seal from breaking      coe5.wrap("SEAL_BREAK", function(next, slot, seal) if seal ~= 3 then return next(slot, seal) end coe5.set("eventVar", 300, 1) end)
--   A line in the map editor's toolbox  local id = coe5.addMapEditorLine{label = "Show grid", onClick = function() on = true end}
--   A line in the commander's orders  coe5.addCommanderLine{label = "Rally", icon = 0x75, onClick = function() local c = coe5.get("selectedCommander") end}
--   Teleport a commander's army    coe5.teleportCommander(c, x, y)   (coe5.mayEnterSquare(c, x, y): could it walk there)
--   Draw over the map editor's map  coe5.wrap("MAP_EDITOR_DRAW", function(next) local v = coe5.mapView() coe5.ui.drawLine(v.originX, 0, v.originX, 600, 1, 1, 1, 0.3) return next() end)
--   End the turn from a script     coe5.wrap("MAP_SCREEN_WAIT", function(next, b, m) local id = next(b, m) if skipping then return coe5.mapAction.endTurn end return id end)
--   Close the Messages screen      coe5.wrap("MESSAGES_WAIT", function(next, b, m) if skipping then return coe5.box.messagesOk end return next(b, m) end)
--   Answer the End Turn Warning    coe5.wrap("MESSAGE_BOX_WAIT", function(next, b, m) if skipping and coe5.get("messageBoxText"):find("AP left") then return coe5.box.firstButton end return next(b, m) end)   (Windows)
--   Who bought for an AI           for _, a in ipairs(coe5.aiActions(p)) do coe5.log(a.mod .. " " .. a.call) end
--   Draw over the world map        coe5.wrap("MAP_DRAW", function(next) coe5.ui.drawRect(10, 400, 200, 60, 0, 0, 0, 0.6) coe5.ui.drawText(16, 440, 12, "Hermann", 1, 1, 1, 1) return next() end)   (Windows)
--   Change the top bar's income    coe5.wrap("TOP_BAR_INCOME", function(next, p, r, income) next(p, r, income) return coe5.get("slotAiIncome", p, r) end)   (Windows)
--   A table row on a screen        coe5.ui.rowBegin(30, 400, 0.6, 0.5, 0.2, 0.4) coe5.ui.text(30, "Hermann") coe5.ui.columnX(200) coe5.ui.text(30, "275 (+54)") coe5.ui.rowEnd()
--   A button on a screen           coe5.ui.button(32, "Generate", 300, 28, 0.16, 0.38, 0.66)
--   A resource's icon in a row     coe5.ui.rowBegin(31, 400) coe5.ui.resourceIcon(31, 0) coe5.ui.tooltip("Gold") coe5.ui.text(31, "275") coe5.ui.rowEnd()
--   Panels over a preview world    coe5.setPreviewDocks({{x = 0, y = 0, w = 220, h = 574}})   then uiWheel, uiPointerX, mouseButtons in a MAP_SCREEN_WAIT wrap, coe5.ui.textWidth in MAP_DRAW, coe5.setPreviewKeyField(true) then coe5.takePreviewFieldKeys() after next for a text field   (Windows)
--   A resource's icon on the map   coe5.wrap("MAP_DRAW", function(next) coe5.ui.drawResourceIcon(16, 420, 12, 5) coe5.ui.drawText(30, 420, 12, "31", 1, 1, 1, 1) return next() end)   (Windows)
--   Replace a seal's event         coe5.wrap("SEAL_EVENT", function(next, slot, seal) if seal ~= 3 then return next(slot, seal) end coe5.set("eventVar", 300, 1) end)
--
-- When CoE5SE refuses a call, the call returns false or coe5.message.notAdded (each note below
-- says which), and coe5mod.log says why with reason=<word>: api_refused (mod=<your mod>) for calls
-- that go through CoE5SE's plugin API, lua_refused for the rest. Search the log for "_refused".
--
-- A wrong argument type, an unknown name, a read-only value, or a call that runs too long is an
-- error instead: logged as lua_error, it stops the script until the game restarts. Its functions
-- don't run again, and its wraps pass the game's calls straight on (a wrap that errors gives the
-- game what next returned). A call of the script still running further out (a wrap waiting in
-- next) finishes, but adds no wrap, observer, or callback (lua_refused reason=script_stopped), and
-- CoE5SE turns setupAutomated off if the script left it on. Put a risky part in pcall to keep the script running (the step limit
-- is the one error pcall can't catch). An error names
-- the place as <ModName>.lua:<line> (or a required file's gatherer/safety.lua:<line>), never the
-- file's full path.
--
-- No table fills up: any number of mods can add buttons, Game Menu lines, and wraps.

---Runs `<name>.lua` from your mod's own folder, the one holding <ModName>.lua, and returns what it
---returns (true when it returns nothing). Each dot is a subfolder: require("gatherer.safety") runs
---gatherer/safety.lua. A file runs once, in your script's Lua state (its globals, step budget, and
---memory), and gets its name as `...`; a later require of the same name returns the first one's
---value. It works at load time and later. A name is 1 to 200 bytes of letters, digits, and _, a
---single dot between folders, in the case the files are stored in (on Windows too). Refused, with
---lua_refused call=require and an error to the caller: any other name (empty_name, name_too_long,
---bad_character, bad_name, reserved_name for a Windows device name such as con), a file or folder
---in another case (wrong_case), a link or junction on the way (link), and your own <ModName>
---(main_script). A missing file and a loop (a file that requires itself through others) are errors
---too, and a file of bytecode doesn't load. There is no package table.
---@param name string
---@return any
function require(name) end

---The game calls a script can run code around with coe5.wrap. Each note gives the function you
---pass and what `next` returns. A "frame" site runs once per frame of its screen, and its `next`
---returns the id clicked in that frame (0 for none). Lines a script draws with coe5.ui.text inside
---a wrap go onto the game's own screen. A site marked "Windows only" isn't found on the Mac build
---yet: coe5.wrap returns false there.
---@alias coe5.Site
---| "MAIN_MENU_WAIT"            # fn(next, block, mode): each frame of the main menu. next returns the clicked id (coe5.mainMenu).
---| "MAIN_MENU_PLAY_GAME"       # fn(next, server): Start New Game or Load Game (server 0 in single player). next returns after the Game Over screen, or at once if the player cancels setup. Not called when the player backs out of a loaded save whose spells changed.
---| "PLAY_GAME_NEW_WORLD"       # fn(next, mode): a new game's setup and world. Mode coe5.newGame.normal, .network, or .skipParticipants. next returns 0 on Cancel. Not called for a loaded game.
---| "NEW_WORLD_MAP_SCREEN"      # fn(next): Random Map Settings. next returns coe5.newGame.mapScreenOk on Ok, 0 on Cancel.
---| "NEW_WORLD_INIT_PARTICIPANTS"  # fn(next, arg): the default single-player slots, set before Setup Participants draws; after next returns, the slot values are what the screen opens with. Pass arg on unchanged. next returns nothing.
---| "NEW_WORLD_CREATING_WORLD"  # fn(next, kind, text): "Creating World", before the classes are rolled. next(kind, text) shows the text; other text is shown as it is (a % in it isn't read as a format). next returns nothing.
---| "MAP_SETTINGS_WAIT"         # fn(next, block, mode): each frame of Random Map Settings. next returns the clicked id; coe5.newGame.mapSettingsOk is Ok.
---| "PARTICIPANTS_WAIT"         # fn(next, block, mode): each frame of Setup Participants. next returns the clicked id; coe5.newGame.participantsOk is Ok.
---| "PARTICIPANTS_DIFFICULTY_WAIT"  # fn(next, block, mode): each frame of Setup Participants' AI difficulty popup. next returns the clicked id. "newWorldUniqueRandom" is nil here (the popup is a call of its own).
---| "PARTICIPANTS_BUTTON_ROW"   # fn(next, style): starts Setup Participants' Ok row. Lines drawn before next go above the row; lines drawn after it go into the row, above Ok. next returns an integer. On Mac CoE5SE makes the row itself (the game has no call for it there), and next works the same.
---| "GAME_MENU_SAVING_SECTION_END"  # fn(next): closes the Game Menu's "Saving & Exiting" section. Lines drawn before next go at the end of the section. next returns an integer.
---| "GAME_MENU_WAIT"            # fn(next, block, mode): each frame of the in-game Game Menu (F10). next returns the clicked id; coe5.gameMenu.resign resigns. CoE5SE's own wrapper, which runs the Game Menu lines, runs first.
---| "RITUAL_SCREEN_ROW"         # fn(next, id, a, b, name, costType1, cost1, costType2, cost2, costType3, cost3, costType4, cost4, hover, apCost, castable, mirrorMonster, mirrorText): one row of the ritual screen. id is the slot + coe5.ritual.rowCastable, or + coe5.ritual.rowGreyed when greyed out; hover is the reason text, and the costs are already scaled for the caster; pass a and b on unchanged. hover and mirrorText may be nil; name may not. In next, a mirrorMonster past the last monster or a costType above 50 is refused (lua_refused reason=out_of_range) and the game's own value kept. next returns nothing the game uses (nil).
---| "MAP_SCREEN_WAIT"           # fn(next, block, mode): each frame of the map screen, after the game has bound its shortcuts. next returns the clicked id; coe5.mapAction.gameMenu opens the Game Menu. A commander's button is 20000 + its index (negated too); an id for a commander past the game's last becomes 0 (lua_refused, reason=commander_past_table). After next, "mapPointerX" and "mapPointerY" hold the square under the pointer and "mouseButtons" the frame's buttons (set it to 0 to keep a click from the map). A line drawn here isn't shown: the map lays out its lines before this wait.
---| "MAP_MENU_BUTTON_TEXT"      # fn(next, text): the map's "(F10) Menu" button text. next(text) draws it; pass other text to draw that. next returns nothing the game uses (nil).
---| "SQUARE_INFO_SCREEN"        # fn(next, player, x, y, mode): the square info window, "(i) terrain info", for square (x, y). In next, a player that isn't a slot (0-31) is refused (lua_refused reason=out_of_range) and the game's own kept. next returns nothing the game uses (nil).
---| "SQUARE_INFO_BUTTON_ROW"    # fn(next, style): starts the square info window's Ok row. Lines drawn before next go above the row. next returns an integer. On Mac CoE5SE makes the row itself (the game has no call for it there), and next works the same.
---| "SQUARE_INFO_WAIT"          # fn(next, block, mode): each frame of the square info window. next returns the clicked id; the window uses coe5.screen.squareInfoOk and 2.
---| "LOAD_GAME_BROWSER"         # fn(next, title): Load Game's save list (the game's file browser, which other screens use too). next returns 1 when a save was picked, 0 on Cancel. The browser's other arguments go on unchanged.
---| "FILE_BROWSER_ROW_DATE"     # fn(next, id, text): one row's date column in the file browser. Row n's id is coe5.screen.fileBrowserRowFirst + n; the row is 450 wide. next(id, text) draws it; draw more after it with coe5.ui.columnX and coe5.ui.text. next returns an integer.
---| "FILE_BROWSER_WAIT"         # fn(next, block, mode): each frame of the file browser. Returning coe5.screen.fileBrowserDelete deletes the row the "hoveredId" value names (the browser reads it right after the wait), after the game's own question. A row id past the browser's last listed row becomes 0, no click (lua_refused, reason=row_past_list or hovered_row_past_list).
---| "FILE_BROWSER_HOVERED"      # Windows only until CoE5 5.41, which inlined it as Mac always had: on neither build now, so coe5.wrap returns false. The browser reads "hoveredId" right after its wait; set it in a FILE_BROWSER_WAIT wrap to pick the row to delete.
---| "MOD_BANNER_DRAW"           # fn(next, path, x, y, w, h, alpha): one enabled mod's banner in the column on the right of the menu screens. x is the screen width - 210, y the screen height - 60 - 60 * row, w 200. Skip next to hide the banner. Pass path to next as it came: another path is refused (lua_refused reason=path_changed) and the game's own banner drawn. next returns nothing.
---| "GAME_OVER_WAIT"            # fn(next, block, mode): each frame of the Game Over screen. The game passes coe5.gameOverMode.fog (fog and falling particles; mode 8 draws them too); any other mode clears to black, and coe5.gameOverMode.plain draws nothing behind the screen. next returns the clicked id.
---| "ARMY_WAIT"                 # fn(next, block, mode): each frame of Transfer troops. next returns the clicked id.
---| "BATTLE_WAIT"               # fn(next, block, mode): each frame of a battle's real-time loop. next returns the clicked id.
---| "MAP_EDITOR_WAIT"           # fn(next, block, mode): each frame of the map editor. next returns the clicked id (coe5.mapEditor, or a coe5.addMapEditorLine id). A key bound with coe5.ui.bindKey before next counts for that frame, toolbox hidden or not.
---| "MAP_EDITOR"                # fn(next): the whole map editor, opened from the Mods screen's Start Map Editor or the --mapeditor launch option; next returns when the player quits it. With --mapeditor the editor opens before the main menu: the scripts load just before it, and the game exits when it closes.
---| "MAP_EDITOR_DRAW"           # fn(next): once per frame of the map editor's map, after the map's squares and before the toolbox, the plane buttons, and the hover box. Draw with coe5.ui.drawLine, drawText and drawRect: before next lands over the map and under the toolbox, after next over everything but the hover box. coe5.mapView says where each square is. next returns nothing.
---| "MAP_DRAW"                  # fn(next): once per frame of the world map in a player's turn, after the squares and the units and before the top bar, the commander panel, and the hover box. Draw with coe5.ui.drawLine, drawText and drawRect: before next lands over the map and under the game's panels, after next over everything but the hover box. coe5.mapView says where each square is. next returns nothing. Windows only for now: on Mac coe5.wrap returns false.
---| "BATTLE_DRAW"               # fn(next): once per frame of a battle's screen, after the battlefield and its units and before the battle's GUI. Draw with coe5.ui.drawLine, drawText and drawRect here; after next, over everything. Take a click on what you drew in a BATTLE_WAIT wrap, after next, with uiPointerX, uiPointerY and mouseButtons.
---| "MESSAGES_WAIT"             # fn(next, block, mode): each frame of the Messages screen, as a human turn starts (View messages shares it). next returns the clicked id; return coe5.box.messagesOk to close it, as the player's Ok does.
---| "WARN_WAIT"                 # fn(next, block, mode): each frame of the game's warning box ("<name> has been eliminated" and the like). next returns the clicked id; return coe5.box.warnOk to press its Ok.
---| "MESSAGE_BOX_WAIT"          # fn(next, block, mode): each frame of the game's message box (the End Turn Warning, and any question the game asks with one or two buttons). next returns the clicked id, 0 for none; return coe5.box.firstButton, secondButton or cancel to answer it as the player would. Its text reads as messageBoxText meanwhile. Windows only for now: on Mac coe5.wrap returns false.
---| "TOP_BAR_INCOME"            # fn(next, player, resource, income): the "+income" the map's top bar draws after one of the player's resources (0 Gold to 13 Lifeforce, as slotResource; not the gems or the trade points). Return the number to draw; next returns the game's. Only the drawn number changes: the income the game pays, which the AI reads, stays the game's. Windows only for now: on Mac coe5.wrap returns false.
---| "BATTLE_EDITOR_WAIT"        # fn(next, block, mode): each frame of the battle map editor. next returns the clicked id.
---| "RECRUIT_WAIT"              # fn(next, block, mode): each frame of Recruit. next returns the clicked id.
---| "VIEW_ARMY_WAIT"            # fn(next, block, mode): each frame of an army on the battlefield, before a battle. next returns the clicked id.
---| "UNIT_LIST_WAIT"            # fn(next, block, mode): each frame of a battle's unit list (F1). next returns the clicked id. A unit's row is 100000 + its unit index (negated too); an id for a unit past the game's last becomes 0 (lua_refused, reason=unit_past_table).
---| "KEYBOARD_SHORTCUTS_WAIT"   # fn(next, block, mode): each frame of the Keyboard Shortcuts screen. next returns the clicked id.
---| "TOOLTIP_LINE_DRAW"         # fn(next, text, index, r, g, b, alpha): one line of the hover text box at the bottom of the screen, on every frame it shows. text is the line as the game cut the hover text to the box's width, index its place (0 the top line; the game draws them from the last up), and r, g, b, alpha its colour, 0 to 1 (the game's white, 1 1 1 1). next(text, index, r, g, b, alpha) draws it in the colour you pass. Pass text and index as they came: other ones are refused (lua_refused reason=text_changed or index_changed) and the line's own kept. next returns nothing.
---| "AI_EXPLORE"                # fn(next, commander, comstr): an AI player's exploration target for one commander, during its moves; comstr is the commander's strength. next(commander, comstr) returns the game's choice: score, x, y (x and y coe5.aiMove.noSquare for none). Return score, x, y to send the commander to any square of the world, on any plane (the game routes it, through portals too); return score alone to keep next's square; return nothing to keep next's choice. The game runs this only while the commander's earlier steps (rally, friendly move, conquest) scored below coe5.aiMove.exploreBelow, and keeps the square only when the score is above their best (commanderAiMoveKind is coe5.aiMove.target when one of them picked a square, at commanderAiTargetX and Y); a score of coe5.aiMove.farExploreBelow or more also skips the game's far exploration. A square off the world or on no plane, a score above 0 with no square, or values that aren't whole numbers are refused (lua_refused reason=off_map, no_plane, no_square, not_a_score, not_a_square, out_of_range) and next's choice kept; another commander in next is refused (commander_changed).
---| "AI_RALLY"                  # fn(next, commander, comstr): the AI's rally step for one commander, the first of its steps: a square of the player's where its other commanders' armies could join this one. Takes and returns what AI_EXPLORE does, with the same refusals; the game keeps the square when the score is above 0, and the move is then coe5.aiMove.target.
---| "AI_FRIENDLY_MOVE"          # fn(next, commander, comstr): the AI's move to a square a teammate owns, after AI_RALLY; the game drops a square where an enemy army near it is much stronger than comstr. Kept when the score is above the rally's; as AI_RALLY otherwise.
---| "AI_CONQUER"                # fn(next, commander, comstr): the AI's conquest step, after AI_FRIENDLY_MOVE: a square to attack or take, among those the player has seen; -999 for none. Kept when the score is above the earlier steps' best; as AI_RALLY otherwise.
---| "AI_FAR_EXPLORE"            # fn(next, commander, comstr): the AI's far exploration, after AI_EXPLORE, run only while the best score is below coe5.aiMove.farExploreBelow: an unseen square anywhere. Kept when the score is above the best, and the move is then coe5.aiMove.explore. As AI_RALLY otherwise.
---| "AI_RITUAL_CANDIDATE"       # fn(next, commander, ritual): one candidate of the AI's ritual plan choice for a commander with no plan (commanderRitualPlan 0), after the game's income, "rare", "never plan this" and other data reasons passed it. next(commander, ritual) runs the game's pointless check of it (the corpses and resources the ritual needs at the caster's square) and returns true when the ritual isn't pointless, false when it is. Return false to veto the candidate (the game drops it as "pointless" and draws the next); return true or nothing to keep it, which leaves the decision to the game: a kept candidate always gets next's answer, called for you if the wrap didn't, so a wrap can veto but never plan what the game drops. A kept candidate is not yet planned: after the wrap the game still checks it is not bad for the AI, can be cast, has the ritual's terrain seen near enough, and meets its time and location rules, and drops it if any fails. Another value is refused (lua_refused reason=not_a_boolean) and keeps; another commander or ritual in next is refused (commander_changed, ritual_changed). A wrap that errors keeps. coe5.scrapRitualPlan works inside it.
---| "AI_RECRUIT"               # fn(next, player): an AI player's recruiting step, once a turn after AI_TURN and before its commanders move. next(player) runs the game's own buying: from the player's offers (coe5.shopOffers) at its citadels, keeping gold for the game's save target (10 turns of gold income, at most 250). Skip next and the game buys nothing this turn. coe5.aiBuyOffer buys inside it, before or after next. next returns nothing.
---| "AI_GATHER_TROOPS"         # fn(next, commander): the AI's troop pick-up for one commander, before each of its move steps: next(commander) takes the free troops on its square that may join it, leaving a share there (20% on a citadel, 60% with under 2 citadels), or, with none free, leaves some of its own ranged troops behind. Don't call next to skip it: the commander takes and leaves nothing this step, and the troops stay free for another commander there. Another commander in next is refused (commander_changed). A wrap that errors runs it. coe5.scrapRitualPlan works inside it.
---| "AI_PICK_UP_LONER"          # fn(next, commander, unit): the AI's pick-up of one lone commander (unit is its unit) into the army of commander, a caster of a higher ritual level on its square, after the game's checks passed it: from then on the lone commander and its followers follow commander and never move on their own again. next(commander, unit) joins them and returns how many units joined; don't call it (return 0) to keep the lone commander free. Another commander or unit in next is refused (commander_changed, unit_changed). A wrap that errors runs it. coe5.scrapRitualPlan works inside it.
---| "AI_SPLIT_ARMY"             # fn(next, commander, minUnits): the AI's split of a commander's army, at the end of its routine before its moves, in 25% of the calls: with minUnits units or more following it (the game's own is 240), the best commander that follows it leaves with some of its troops (each troop of strength up to 100 to 299 with a 10% chance, in two passes, until the larger of a fifth and 40 to 99 units went). minUnits is the game's minimum for this call; next(commander, minUnits) runs the split with the minimum given, 1 or more, for this call only (lua_refused reason=out_of_range keeps the game's). Don't call next to skip it. No commander follows another unless the AI picked it up (AI_PICK_UP_LONER), so with none nothing splits. Another commander in next is refused (commander_changed). A wrap that errors runs it. coe5.scrapRitualPlan works inside it.
---| "AI_MOVE_COMSTR"            # fn(next, commander): the strength the AI commander's route takes for one move step, right after AI_GATHER_TROOPS and before the game routes it to the square its steps kept. next(commander) returns the game's own, the strength of the commander's army. The route won't end on a square whose army is stronger than the strength allows (times 100, divided by aiAttackDivOther, aiAttackDivAi or aiAttackDivHuman by that army's owner, less 15, aiAttackMargin's value), so return a larger one, such as several commanders' strength together, to let it route onto a stronger army; the square itself was picked by the steps. Returns a whole number from -1 to 21474836, as coe5.route's comstr; another value is refused (lua_refused reason=not_a_number or out_of_range) and next's kept. Return nothing for next's. Another commander in next is refused (commander_changed). coe5.route works inside it. A wrap that errors gives the game's.
---| "AI_DANGEROUS_STEP"         # fn(next, commander, dx, dy): whether an AI commander's step from its square by (dx, dy) is refused ("aborted dangerous move"), in the AI's move loop and in the independents' wandering moves: the Void off its plane, frozen water in Winter or Spring with 2 or fewer turns of the season left, a burning army (burnforest 100 or more) into burnable terrain while its fire resistance is under 100, Dispersing Clouds without flying, the sky or water while Fatigued. It reads the commander's unit only. next(commander, dx, dy) returns the game's answer, 1 to refuse and 0 to let the step go; return either. Another commander or step in next is refused (commander_changed, step_changed). A wrap that errors gives the game's.
---| "AI_MAKE_GUARDS"            # fn(next, player, x, y, n): n of the player's units on square (x, y) become guards, which never leave it; x and y are -1 for the player's units anywhere: at game start for each player's army (-1, -1, n 4), and in the AI's turn on each square it defends with an Enchanted Gate, Mirror Wall, Wall of Torment or Murderous Mist (n 1). next(player, x, y, n) makes them; pass another n (1 or more) for this call, or don't call it for none. Another player or square in next is refused (player_changed, square_changed), and an n below 1 (out_of_range) keeps the game's. A wrap that errors runs it.
---| "AI_RITUAL_TARGET"          # fn(next, commander, ritual): the square an AI commander casts its planned ritual at, from up to 25,000 random squares of Elysium, the Sky and Agartha: a seen square with an enemy army for an aitarg 1 ritual, a seen square with 5 corpses or more for aitarg 2, the first square for any other. next(commander, ritual) returns 1, x, y for a square found, 0 for none (the plan waits). Return 1, x, y for a square on a plane, or 0 for none; 1 with no square, a square off the map or off every plane, or another value is refused (lua_refused reason=no_square, off_map, no_plane, out_of_range, not_a_score, not_a_square) and next's kept. Return nothing for next's. Another commander or ritual in next is refused (commander_changed, ritual_changed). A wrap that errors gives the game's.
---| "DECORATE_SQUARE"           # fn(next, x, y): one square's random special terrain by its terrain (villages, farms, mines, ports; towers, castles and ancient ruins only on Elysium): in world building, a map file's random squares, and the map editor's Create Fancy Terrain. next(x, y) places them; don't call it to keep the square as it is. Another square in next is refused (square_changed). A wrap that errors runs it.
---| "WEAPON_HIT"                # fn(next, attacker, target, weapon, damage, value, dmgtype): one hit's damage in a battle, after the game's MR checks and its weapon flags, before its double-damage rules. value is the spell's or monster attack's dmg (999 on the kill spells; an affliction's mask, low 32 bits); dmgtype is the weapon's damage type, and a type the game doesn't have takes its plain damage. Return the damage the hit does: 0 makes the hit and its effect do nothing. next returns CoE5SE's answer (killres, polymorphres, petrifyres, doesntaffect and the rest). Runs on every hit: keep it quick.
---| "BATTLE_TARGET_CHECK"       # fn(next, unit, type, weapon, value, target): whether a target is worth a spell or a mind weapon to the battle AI. next returns the game's and CoE5SE's answer, coe5.target.useful or below; return coe5.target.notAffected to make casters pass the target over. A target the game picks itself, at random, isn't asked.
---| "BATTLE_SPELL_PICK"         # fn(next, unit, path): the spell a caster's spell attack casts in a battle, picked at random among its memorized spells of that path; the attack asks again, up to 50 times, until the spell has a target. Return another spell (any spell the table holds, known or not) or -1 for none; nothing for next's. Another number is refused (not_a_spell, not_a_number) and next's kept; another unit or path in next is refused (unit_changed, path_changed). A wrap that errors gives the game's.
---| "CEREMONY_PERFORM"          # fn(next, ceremony, commander, target): one of the Voice of El's ceremonies, a player's (the Ceremonies screen, a map order) or the AI's. ceremony is the game's number for it: match it by coe5.get("ceremonyName", ceremony), never by the number. target is the player a Start Crusade goes against, -1 for every other ceremony. next(ceremony, commander, target) is the game's own: it checks the rank, limits and cost (and does nothing when one fails), charges the cost and action points, and does the ceremony. Skip next and none of it happens: no cost, no action points, no effect, and no message. Code after next adds effects of your own. Another ceremony, commander or target in next is refused (ceremony_changed, commander_changed, target_changed) and the game's kept. A wrap that errors runs it.
---| "CEREMONY_COSTS"            # fn(next, commander): the game works out every ceremony's cost for a priest of El: at the top of the Ceremonies screen, before a map order's ceremony, and before the AI's. After next(commander), coe5.get("ceremonyCost", ceremony) holds each one and coe5.set("ceremonyCost", ceremony, value) changes it: the screen shows that cost, the ceremony checks and charges it, and the AI weighs it, until the next time the game works them out. The game multiplies some: Break Seal's by the seal's number (1 for the first), Bolster Devotion's by one more than the player's earlier Bolster Devotions, Increase Divine Favor's by the level it raises the favor to, Convert Settlement's by a fifth of the square's gold income (its action points too). Each is gold, or relics for Break Seal, Bolster Devotion and Increase Divine Favor. Another commander in next is refused (commander_changed).
---| "SEAL_BREAK"                # fn(next, slot, seal): one of the Seven Seals breaking, by a player's Break Seal ceremony or an item, slot that player; seal is the seal it breaks, 1 to 7. Inside the wrap the seals broken are still seal - 1; next(slot, seal) breaks it, as the game does. Skip next and the seal stays unbroken, so none of what follows from it happens (its event, SEAL_EVENT; El's minions for the Voice of El players get no stronger), and the next Break Seal ceremony breaks the same seal again. The ceremony's cost and action points are spent either way. To break the seal but replace its event, wrap SEAL_EVENT instead. The seals the game breaks on its own at a month's end ("Cardinal Intervenes", "Heavenly Surge of Power") don't come here. Another slot or seal in next is refused (slot_changed, seal_changed). A wrap that errors breaks it.
---| "SEAL_EVENT"                # fn(next, slot, seal): a broken seal's event, at a month's end: the month after it broke, one seal a month in order when several broke, whoever broke it (a ceremony, an item, or the game at a month's end), once per seal. seal is 1 to 7; the game's events: none for the first two, an Apocalyptic Rider for the third to the sixth ("The apocalypse is coming closer"), the Apocalypse for the seventh. slot is always -1: the game keeps no record of who broke a seal. next(slot, seal) lets the game run its event, which it does once every wrap has returned (so code after next runs before the event); next returns nothing. Skip next to replace the event with your own: the seal stays broken, its event counts as done and never comes back, and the next seal can be broken as before. What the wrap returns isn't read. Another slot or seal in next is refused (slot_changed, seal_changed). A wrap that errors runs it.

---The game calls a script can watch with coe5.observe but not change: each decides or applies game
---content. before(...) gets a copy of the arguments before the call, and after(result, ...) the
---call's result and the arguments after it (result nil where the call has none). What they return
---doesn't reach the game.
---@alias coe5.ObservedSite
---| "RITUAL_SCREEN_LOCATION_CHECK"  # (commander, slot, a, b): the ritual screen's location check for one row, right before the row is drawn; a and b are -999. A result of 0 or below is a reason code.
---| "RITUAL_SCREEN_CAN_CAST"    # (commander, slot): the ritual screen's cast check for one row. A result of 0 or below is a reason (coe5.ritual.reasonMirror, ...).
---| "GAME_START"                # (a, b): once before the first turn of a new or loaded game; both 0 in single player. After a load, "turn" holds the saved month. No result.
---| "MONTH_START"               # (kind, text): the "New Month" text, after the game's monthly update; "turn" already holds the new month. The result is nil.
---| "PLAYER_TURN_START"         # (player, quick): one player's start-of-turn step, humans, AI and independents (slots 24-30) alike; quick is 1 for AI and independents. No result.
---| "HUMAN_TURN"                # (player): a human player's turn; after runs when they end it. The result is nil.
---| "CLIENT_TURN"               # (player): a LAN client's turn, once its turn file has loaded; after runs when they end it. The result is nil.
---| "AI_TURN"                   # (player): an AI player's moves. No result.
---| "PLAYER_TURN_END"           # (player): the first end-of-turn step after any player has moved.
---| "GAME_OVER"                 # (winner): the game over screens (not in autorun games). No result.
---| "GAME_SAVE"                 # (path, name): the Game Menu's Save and both autosaves. A result above 0 means the game was saved; after can write the mod's own data next to it.
---| "GAME_LOAD"                 # (path, a): Load Game, and a load from the command line. GAME_START follows.
---| "AFTER_GAME_TURN"           # (): the month's world update at its end, before MONTH_START: the season changes, the world's events run, units drown or crash-land, and the squares are cleaned up. No result.
---| "DO_ALL_FIGHTS"             # (): every battle on the map is fought; three times after each round of players' moves, and twice in the month's update. No result.
---| "MOVE_ALL_INDEPENDENTS"     # (player): an independent slot's armies move (24 to 30); for the Inferno, Hades and the Celestial after their own steps. No result.
---| "CONQUER_SQUARE"            # (x, y, player): a player takes the square: an army that took it (world population included), the month's update, a ritual, or a unit's own step. The result is nil.
---| "FIGHT_THE_FIGHT"           # (x, y): one battle on the square, its rounds to a winner; the field is built and both sides deployed before it. The result is the winner's player, -1 for none, -2 when no side is left.
---| "BATTLE_ROUND"              # (): a battle round starts (battleRound holds it), before starvation, movement and attacks; a replay's rounds too (battleReplay). No result.
---| "DAMAGE_UNIT"               # (unit, damage): damage comes off a unit's hp (below 0 heals); its death runs inside when the hp falls to 0 or below. In battle and out of it; a replay's too (battleReplay); not a meteor's kills. The result is nil.
---| "DOATTACK"                  # (unit, slot, spell): one unit's attack in a battle round: an attack slot with spell -1, or slot -1 with the spell an item casts. Damage it deals reaches DAMAGE_UNIT inside it. The result is nil.
---| "EXECUTE_RITUAL"            # (unit, ritual, x, y, percent, pay): a ritual is cast: the Rituals screen (x -999: the caster picks the target), the AI, repeated rituals, a cast by name, world building on Elysium. The result is -1 when the cast was refused.
---| "NEW_UNIT"                  # (x, y, player, commander, monster): a unit is made on square (x, y) for the player, under the commander (-1 none): recruits, summons, every populate's units, battle summons and splits. The result is the new unit, -1 when the unit table is full.
---| "REMOVE_UNIT"               # (unit): a unit leaves the game without dying: the event killunit at 9999 and above, killsquare, a mirror image's end, units the game clears. A death by damage is DAMAGE_UNIT's. The result is nil.
---| "TURN_UNIT_UPKEEP"          # (player): a player's units' step near the end of its turn start: damage on terrain, disease, ability spawns, sinners, colonies, terraforming, promotions. The result is nil.
---| "SQUARE_UPKEEP"             # (player, -1, x, y): one square with an army of the player's team: at its turn start, and after a commander's step there. The second value is always -1. The result is nil.
---| "RECRUIT_BUY"               # (player, offer, x, y): a human player's buy of shop offer `offer` at square (x, y): the Recruit screen and the auto-recruit. Its units reach NEW_UNIT inside it. The result is 1.
---| "PROMOTE_UNIT"              # (owner, unit, monster, n): a unit becomes `monster`: the event actions promote and promoteunits, a ritual's promotion, a promotion by experience. The result is nil.
---| "MASTERY_TRANSFORM"         # (commander): the ritual of mastery and the library's mastery: the commander's new form and the spells its gained levels teach. The result is nil.
---| "BATTLE_SETUP"              # (x, y, mode): a battlefield is built for square (x, y): each battle and replay (mode 0), and the backdrop of the army screens, rituals, summons and the battle map editor. The result is nil.
---| "BATTLE_SETUP_SIDE"         # (player, x, y, side, filter): one side's units are placed on the battlefield, the defender's (side 1) first. The result is nil.
---| "BATTLE_SETUP_AMBUSH"       # (player, x, y, side, filter): one side's units are placed for an ambush. The result is nil.
---| "BATTLE_ADVANCE_UNIT"       # (unit, ambush): one unit's move in a battle round: fleeing, confused, burrowed, flying, holding by rank, or a step toward the enemy; a fast unit moves twice. The result is nil.
---| "BATTLE_TURN_EFFECTS"       # (unit, aftermath): a battle round's effects on a unit: regeneration, decay, damage over time; `aftermath` is 0 in the rounds and nonzero after the fight. The result is nil.
---| "BATTLE_TRANSFORM"          # (unit, monster, attacker): a hit turns `unit` into `monster` on the attacker's side: mutate, replicate, ghoulify. The result is nil.
---| "FOUND_ITEM"                # (x, y, item): a commander on square (x, y) finds `item`: a battle's loot, a conquered square's, a dead unit's. The result is nil.
---| "REMOVE_DEAD_PLAYERS"       # (): the game takes out every player with no citadel or commander left ("has been eliminated"): after each round of moves, in the month's update, after battles, when a square is taken. Which players went shows in the slots before and after. No result.
---| "CREATE_CITADEL"            # (player, x, y): a player's citadel is made at square (x, y) as the game places the player. The result is nil.
---| "INQUISITION_SPAWN"         # (player): the Inquisition's army arrives for the player (Call the Inquisition). The result is nil.
---| "ANIMAL_SUMMONING"          # (commander, value): a commander's animal summoning, the animals by its square's terrain; `value` is the ritual effect's. The result is nil.
---| "ORDER_SLAVE_HUNT"          # (commander): a commander's slave hunt on its square: the map's order, the AI's, and the repeated rituals'. The result is nil.
---| "ORDER_EAT_VILLAGE"         # (commander): a commander eats the village on its square: the map's order and the AI's. The result is nil.
---| "UPDATE_VOID"                # (): The month's redraw of the Void: each of its squares goes unseen by every player and gets its terrain drawn again. Windows only for now. The result is nil.
---| "VOID_SHUFFLE"               # (): The month's shuffle of the Void: fifteen swaps of two random squares. Windows only for now. The result is nil.
---| "PLANESHIFT_UNIT"            # (unit, plane, noSight): A unit moves to the same spot on another plane. Windows only for now. The result is nil.
---| "NEW_PORTAL_PAIR"            # (x1, y1, x2, y2, twoWay): Two squares are linked with a new portal number: startplane's home portal, the Strange Portals, and the rituals that open a portal. Windows only for now. The result is nil.
---| "PLANE_DISCOVERED_PASS"      # (): The month's check of which vanilla planes a human player has seen enough of, which sets planeFlags bit 0. Windows only for now. The result is nil.
---| "MOVE_INFERNO_PLAYER"        # (): The Inferno's move each month, slot 28's armies included. Windows only for now. The result is nil.
---| "MOVE_HADES_PLAYER"          # (): Hades' move each month, slot 27's armies included. Windows only for now. The result is nil.
---| "MOVE_CELESTIAL_PLAYER"      # (): The Celestial Plane's move each month, slot 29's armies included. Windows only for now. The result is nil.
---| "POPULATE_CELESTIAL"         # (): World generation places the Celestial Plane's independents. Windows only for now. The result is nil.
---| "POPULATE_SKY"               # (): World generation places the Sky's independents. The result is nil.
---| "POPULATE_AGARTHA"           # (): World generation places Agartha's independents. Windows only for now. The result is nil.
---| "POPULATE_INFERNO"           # (): World generation places the Inferno's independents. The result is nil.
---| "POPULATE_HADES"             # (): World generation places Hades' independents. The result is nil.
---| "POPULATE_AZTLAN"            # (): World generation places Aztlan's independents. The result is nil.
---| "POPULATE_PRIMAL"            # (): World generation places the Primal Plane's independents. The result is nil.
---| "POPULATE_ELEMENTAL"         # (): World generation places the Elemental Planes' independents. The result is nil.
---| "PLANE_RISE_TO_ELYSIUM"         # (plane, percent, n, monster): A plane sends units of a monster up to Elysium, as Hades sends its Dispossessed Spirits. Windows only for now. The result is nil.
---| "ENLARGE_PLANE"              # (plane, w, h, dx, dy, terrain): World generation grows the last plane by 4 squares each way, the new frame of the terrain. Windows only for now. The result is nil.
---| "GEN_FLOOD_PLANE"            # (plane): World generation floods a plane from its wet squares, Agartha's among them; the result is how many squares it flooded.
---| "ADDPLANE"                   # (name, w, h): A plane is placed beside the map, from world generation or a map file; the result is its number.
---| "LOAD_MAP_PLANE"             # (path, name, plane): A plane's lines are read from a map file; the result is 0 when the file can't be opened.
---| "UNIT_ADD_XP"                # (unit, xp): A unit gains experience. Windows only for now. The result is nil.
---| "USE_SPECIAL_SITE"           # (x, y, commander): A commander uses the special site on a square. The result is nil.
---| "LONER_STEP"                 # (unit): One random step of a loner by the terrain it prefers. Windows only for now. The result is nil.
---| "REMOVE_TEMP_MEN"            # (): The units a battle made temporary are removed. Windows only for now. The result is nil.
---| "MOVE_WANDERING_MONSTERS"    # (): The active player's wandering monsters move. Windows only for now. The result is nil.
---| "MOVE_PLAYER_STUPIDS"        # (): The active player's units that have no sense of their own move. Windows only for now. The result is nil.
---| "OVERRUN_CHECK"              # (player, x, y): The check for an overrun on a contested square.
---| "KILL_UNIT_999"              # (unit): A unit dies of 999 damage in an overrun or an elimination. Windows only for now. The result is nil.
---| "DAMAGE_UNIT_PLAIN"          # (unit, damage): A unit loses Hit Points with none of a hit's checks: the damage DAMAGE_UNIT doesn't see, a meteor's kills among it. Windows only for now. The result is nil.
---| "CONTINUE_MOVE_ORDERS"       # (player, commander): A player's stored move orders carry on, or one commander's; the result is how many moved.
---| "HUMAN_MULTI_MOVE"           # (commander, x, y, mode): A human player's commander walks a route.
---| "GROUP_MOVE"                 # (group, x, y): The selected group of commanders moves a step.
---| "MOVE_ORDER_ROUTE"           # (commander, x, y): A human player's move order to a square.
---| "UNIT_REMOVE_ITEM"           # (unit, item): An item leaves a unit's hands as the AI hands items on; the result is -1 when the unit didn't hold it. Windows only for now.
---| "MEMORIZE_SPELL"             # (unit, spell): A unit memorizes a spell. Windows only for now. The result is nil.
---| "LEARN_RANDOM_SPELL"         # (unit, path, level): A unit learns a random spell: from a library, a ritual or a summoning.
---| "GET_START_RITUALS"          # (commander): A commander learns the rituals its schools start it with: a new commander or a promotion. The result is nil.
---| "COMMANDER_LEARN_RITUAL"     # (commander, ritual): A commander learns a ritual. Windows only for now. The result is nil.
---| "BOLSTER_BLESSING"           # (player, kind, show): A player gets a new blessing effect: Bolster Devotion among others. The result is nil.
---| "ADD_PLAYER_BLESSING"        # (player, ability, add, cap): A player's blessing of an ability rises. The result is nil.
---| "REMOVE_ENCHANT"             # (caster, type, x, y): An active ritual effect ends.
---| "RITUAL_PAY_COSTS"           # (player, commander, ritual, percent, show): A ritual's costs are paid. The result is nil.
---| "RITUAL_FAR_SUMMON_UNITS"    # (caster, monster, count, player, x, y): A ritual summons units on a far square; the result is the first unit.
---| "PRIEST_FREESPAWN"           # (player): A Voice of El player's chance of free units by the seals broken. Windows only for now. The result is nil.
---| "EVENT_KILL_SQUARE"          # (x, y): The event word killsquare runs. Windows only for now. The result is nil.
---| "GIVE_ITEM_BY_NAME"          # (unit, item): An item is given by string: the event word newitem and rituals. The result is nil.
---| "TAKE_ITEM_BY_NAME"          # (unit, item): The event word removeitem runs. The result is nil.
---| "GIVE_RANDOM_ITEM"           # (unit, kind, percent, level): A random item is given at a percent chance: the event words randitem and randmisc and more. Windows only for now. The result is nil.
---| "SQUAREEVENT_START"          # (): A squareevent starts: every square is marked and the event's player set. Windows only for now. The result is nil.
---| "TERRAIN_PRESENT_RESCAN"     # (): The game rescans which terrains are present after a terrain change. Windows only for now. The result is nil.
---| "SHOP_OFFER_MESSAGE"         # (player): A player gets the message for the newest shop entry: a Recruitment Offer or a Magic Item for Sale. The result is nil.
---| "WORLD_FIRE_UPDATE"          # (): The month's update of burning squares. Windows only for now. The result is nil.
---| "EVENT_VAR_CHANGED"          # (variable): A ritual effect wrote an event variable. Windows only for now. The result is nil.
---| "CONSCRIPTION"               # (player): The Baron's conscription, the first month of Summer. Windows only for now. The result is nil.
---| "GAME_STATISTICS"            # (): Each player's row of the score graphs is written for the turn. The result is nil.
---| "ALTER_TERRAIN"              # (player, x, y, from, from2, to): A square's terrain changes: every way a square changes through the game's alterterrain. Windows only for now.
---| "RUIN_SQUARE"                # (player, x, y): A square's terrain becomes its ruined form. The result is nil.
---| "DESTROY_GATEWAY"            # (player, x, y, flag): A Gateway is destroyed. Windows only for now.
---| "DIG_TUNNEL_SQUARE"          # (player, x, y): A Cave Wall is dug out; the result is 1 when it was. Windows only for now.
---| "MONTH_SQUARE_RESET"         # (): The month's reset of every square's recruited mark. Windows only for now. The result is nil.
---| "ROLL_CLASS"                 # (slot, unique): A random class is rolled for a slot. Windows only for now. The result is nil.
---| "RESET_INCOME_CACHES"        # (): Every player's income is marked to be counted again. Windows only for now. The result is nil.
---| "TEAM_START_LOCATIONS"       # (): World generation picks the teams' start squares ("Creating World"). Windows only for now. The result is nil.
---| "TRIM_TEAM_LOCATIONS"        # (): World generation trims the map around the start squares ("Trimming World"). Windows only for now. The result is nil.
---| "SWAP_TEAM_LOCATIONS"        # (): World generation swaps start squares for a better score. Windows only for now. The result is nil.
---| "CREATE_CITADELS_STEP"       # (): World generation places the citadels ("Creating Citadels"). Windows only for now. The result is nil.
---| "FIX_PORTALS"                # (): Each portal is marked active or not. Windows only for now.
---| "VOID_FLUCTUATIONS"          # (): The Void's terrain fluctuates. Windows only for now. The result is nil.
---| "PLANESHIFT_COMMANDER"       # (commander, plane): A commander's army planeshifts; the units that can't go are removed. Windows only for now. The result is nil.
---| "MIST_SPREAD"                # (): The month's spread of mist. Windows only for now.
---| "TEMPERATURE_CHANGE"         # (): Squares change terrain by their temperature. On Mac the result is nil.
---| "SPECIAL_SITE_POPULATE"              # (): Each special site's monsters come out. Windows only for now. The result is nil.
---| "UNIT_SWALLOW"               # (swallower, victim): A unit swallows another. The result is nil.
---| "UNIT_UNSWALLOW"             # (swallower): Swallowed units come out, the swallower's or, with -1, everyone's. The result is nil.
---| "LIFEFORCE_DRAIN_UNIT"       # (attacker, unit, amount): A unit is drained of lifeforce; the result is what it gave.
---| "UNIT_CATCH_FIRE"            # (a, b): A unit catches fire from nearby flames.
---| "UNIT_CRUSH"                 # (unit): A unit crushes another in battle. The result is nil.
---| "REMOVE_BAD_FOLLOWERS"       # (): Followers whose commander can't lead them go free. Windows only for now. The result is nil.
---| "OVERPOP_CULL"               # (): The game counts its units against its limit, and past 97,000 kills free units to make room. Windows only for now.
---| "MONKEY_CLOAK"               # (player): Mirror Cloaking disguises a player's units again. Windows only for now. The result is nil.
---| "RECYCLE_UNITS"              # (): Dead unit records are freed for reuse. Windows only for now.
---| "RANDOM_TELEPORT"            # (unit, terrain, followers): A unit teleports to a random square of a terrain code. On Mac the result is nil.
---| "POP_HORRORS"                # (chance): Horrors come for the units marked by them. The result is nil.
---| "KILL_TERRAIN_UNITS"         # (): A battle's terrain units are removed. Windows only for now. The result is nil.
---| "KILL_SUNKEN_SHIPS"          # (x, y): A sea battle's sunken ships are removed. Windows only for now. The result is nil.
---| "CREATE_BOATS"               # (): A sea battle's boats are made. Windows only for now. The result is nil.
---| "BATTLE_TRAPS"               # (): A siege's enchantment traps go off. Windows only for now. The result is nil.
---| "BATTLE_ENCHANT_TRAPS"       # (): A siege counts its trap enchantments. Windows only for now. The result is nil.
---| "BATTLE_SUMMON_STRING"       # (): A battle spell summons by string. Windows only for now. The result is nil.
---| "PAY_SPELL_LIFEFORCE"        # (): A battle spell pays its lifeforce. Windows only for now. The result is nil.
---| "WORLD_NEWS"                 # (text): A World News line goes out to every human player. Windows only for now. The result is nil.
---| "SEASON_NEWS"                # (season): The season changes, and World News says so. Windows only for now. The result is nil.
---| "CHECK_GAME_WON"             # (): The turn's check for a winner. Windows only for now. The result is nil.
---| "SPY_CAUGHT"                 # (): A spying unit is found and killed. The result is nil.
---| "TOGGLE_AUTO_RECRUIT"        # (player, x, y, monster): A player's auto recruit of a monster on a square switches. Windows only for now.
---| "CHANGE_LOYALTY"             # (x, y, monster, fromOwner): Units on a square turn to slot 27, Hades. Windows only for now.
---| "ALTER_TERRAIN_RECT"         # (x, y): A block of squares around a square changes terrain. The result is nil.
---| "RESPAWN_VOID"               # (): The Void's monsters come back. Windows only for now. The result is nil.
---| "BATTLE_SIEGE_UNIT"          # (unit): A unit acts against a siege's walls. The result is nil.
---| "GATHER_SHIPS"               # (): A sea battle's ships gather. Windows only for now. The result is nil.
---| "ADVANCE_SHIPS"              # (): A sea battle's ships advance. Windows only for now. The result is nil.
---| "MOVE_ENCHANTS"              # (x1, y1, x2, y2): Active ritual effects move from one square to another; the result is how many. Windows only for now.
---| "MOVE_GEMS"                  # (x1, y1, x2, y2): A square's gems move to another. Windows only for now. The result is nil.
---| "TAINTED_RITUAL_EFFECT"      # (commander, taint): The taint of a ritual cast under tainted magic reaches its caster. The result is nil.
---| "LOCATION_SWAP"              # (x1, y1, x2, y2): The units on two squares swap places. On Mac the result is nil.
---| "APE_POSSESSION"             # (a, b): A ritual possesses bodies from the square's corpses. On Mac the result is nil.
---| "RITUAL_ASK_HEMISPHERE"      # (): The screen asking where a ritual falls.
---| "GET_TITHES"                 # (): The Tithe of El is collected. Windows only for now. The result is nil.
---| "ELIMINATE_PLAYER"           # (player): A player is removed from the game. Windows only for now. The result is nil.
---| "FIND_START_SQUARE"          # (player): World generation picks a player's start square. The result is nil.
---| "TROLL_FOREST_SPAWN"         # (x, y): The troll forest spawns on a square. Windows only for now. The result is nil.
---| "AI_MAKE_DEFENDERS"          # (player): An AI player leaves guards after its moves. Windows only for now. The result is nil.
---| "AI_REDISTRIBUTE_ITEMS"      # (player, x, y): An AI player hands items around a square. Windows only for now. The result is nil.
---| "AI_SHIPS"                   # (commander): The AI's ship handling for one commander. Windows only for now.
---| "AI_SHIP_DROP_CREW"          # (commander): An AI commander drops one ship's crew member. Windows only for now.
---| "AI_SHIP_TAKE_BOAT"          # (commander): An AI commander links a free ship. Windows only for now.
---| "AI_SHIP_TAKE_CREW"          # (commander): An AI commander links a free sailor. Windows only for now.
---| "CHECK_FOLLOWER_LINKS"       # (commander): The followers of a dead commander go free. The result is nil.
---| "STEP_BURN_FOREST"           # (player, x, y): A burning unit's step sets its square alight. Windows only for now. The result is nil.
---| "ORDER_BURN_FOREST"          # (unused, commander): A commander burns the forest on its square. Windows only for now. The result is nil.
---| "ORDER_CHOP_FOREST"          # (unused, commander): A commander chops the forest on its square. Windows only for now. The result is nil.
---| "LIFEFORCE_DRAIN_SQUARE"     # (player, x, y, amount): A square's lifeforce is drained. The result is nil.
---| "SPEND_ALL_AP_AT"            # (player, x, y): A square's units spend their action points. The result is nil.
---| "SWAP_SQUARES"               # (x1, y1, x2, y2): Two squares trade records, as the Void's shuffle does. On Mac the result is nil.
---| "TELEPORT_UNIT"              # (unit, x, y): One unit teleports to a square. Windows only for now. The result is nil.
---| "UNIT_CLEAR_BENEFITS"        # (unit): A unit's benefits end. The result is nil.
---| "GAME_MENU"                  # (): The Game Menu opens. Windows only for now. The result is nil.
---| "ARMY_COMMANDS_MENU"         # (commander): The army screen's menu opens. The result is nil.
---| "CEREMONY_SCREEN"            # (commander): The Ceremonies screen opens for a priest. Windows only for now. The result is nil.
---| "INCOME_DETAIL_SCREEN"       # (player, resource): The map's income popup opens for a resource. Windows only for now. The result is nil.
---| "BATTLE_CREATE_TREES"        # (x, y): A battle's field gets its trees, bushes and boulders by the square. Windows only for now. The result is nil.
---| "BATTLE_DEPLOY"              # (filter, x, y): Both sides deploy for a battle. Windows only for now. The result is nil.
---| "BATTLE_FIGHT_MOVE"          # (unit, x, y, mode, mark): A unit moves on the field. The result is nil.
---| "BATTLE_FLY_MOVE"            # (unit): A flier jumps on the field. The result is nil.
---| "BATTLE_FLEE"                # (unit): A unit flees the battlefield. Windows only for now. The result is nil.
---| "BATTLE_FEAR_CHECK"          # (unit): A unit's round's fear check. Windows only for now. The result is nil.
---| "BATTLE_CLOUD_STEP"          # (unit): The clouds a unit leaves after its advance. The result is nil.
---| "BATTLE_ENSNARE_CHECK"       # (unit): A held unit's grip, net and vines before an attack. The result is nil.
---| "BATTLE_ENSNARE_RELEASE"     # (unit): The holds on a unit wear off after an attack. The result is nil.
---| "BATTLE_WALL_ENCHANTS"       # (): A siege's wall enchantments are placed on the wall cells. The result is nil.
---| "BATTLE_WALL_EFFECTS"        # (): Each round's wall and field effects: Wall of Fire, Lightning and more. The result is nil.
---| "BATTLE_START_SETUP"         # (): The start of a fight: mirror images, sleepers on snow and more. Windows only for now. The result is nil.
---| "BATTLE_TUNNEL_MOVE"         # (unit): A unit burrows with tunnelmove. The result is nil.
---| "BATTLE_BANISH"              # (unit): A unit is banished from the field. Windows only for now. The result is nil.
---| "BATTLE_DEPLOY_UNIT"         # (unit, x, y): A unit is put on the battlefield. The result is nil.
---| "BATTLE_AFTER_FIGHT_UPKEEP"  # (x, y): After a fight on a square: tagalong commanders, survivors' experience, charge refills. Windows only for now. The result is nil.
---| "BATTLE_AFTERMATH_RETURN"    # (x, y, winner): Burrowed and fled units come back on the square for the winner's team. Windows only for now. The result is nil.
---| "BATTLE_MR_ROLL"             # (unit): A unit's magic resistance roll against a weapon. Windows only for now. The result is nil.
---| "BATTLE_SUMMON_WEAPON"       # (): A weapon that summons by its name runs. Windows only for now. The result is nil.
---| "BATTLE_GATE_SWAP"           # (): A siege's gate takes the square's monster. The result is nil.
---| "BATTLE_SPREAD_COLLAPSE"     # (): A collapse spreads to the units around a battle cell. The result is nil.
---| "UNIT_CURE_AFFLICTION"       # (unit): One of a unit's afflictions is cured. The result is nil.
---| "UNIT_HP_SHAPE_CHANGE"       # (unit): A unit keeps its share of Hit Points across a shape change. The result is nil.
---| "BERSERK_END"                # (unit): A unit stops being berserk. Windows only for now. The result is nil.
---| "UNIT_RANDOM_AFFLICTION"     # (unit): A unit gets a random affliction. The result is nil.
---| "REVEAL_TERRAIN_ON_PLANE"    # (player, plane, terrain): A player sees every square of a terrain code on a plane. Windows only for now.
---| "NEAR_TERRAIN_SQUARE"        # (plane, terrain, x, y): The game looks for a square of a terrain code near a square. The result is nil.
---| "PLAY_SAMPLE"                # (sample, change): The game plays a sound. Windows only for now. The result is nil.
---| "GROVE_REVERSION"            # (x, y): A grove's monthly step. Windows only for now. The result is nil.
---| "PUT_RARE_SQUARE"            # (x, y): World generation puts a rare terrain on a square. Windows only for now. The result is nil.
---| "RANDOM_ITEM_PICK"           # (a, b, rarePercent, c, d): The game draws a random item. Windows only for now.
---| "CREATE_RANDOM_MAP"          # (): World generation builds the random map. The result is nil.
---| "WORLD_PART_TERRAIN"         # (): World generation's polar pass. Windows only for now. The result is nil.
---| "MAPGEN_REMOVE_SMALL_ISLANDS" # (): World generation fills in small islands of sea. Windows only for now. The result is nil.
---| "MAPGEN_RIVER_CROSSINGS"     # (): World generation places the fords and bridges. Windows only for now.
---| "MAPGEN_ANCIENT_FOREST"      # (): World generation places one Ancient Forest. Windows only for now.
---| "MAPGEN_CLEAR_AREA"          # (x, y): World generation clears the area around the Capital. Windows only for now. The result is nil.
---| "MAPGEN_ADJUST_TEMPERATURE"  # (): World generation adjusts each square's temperature by what is near it. Windows only for now. The result is nil.
---| "MARK_RIVER"                 # (x1, y1, x2, y2, mark, mode): World generation marks a river's path.
---| "GEN_DIAGONAL_FIX"           # (plane): World generation fixes a plane's diagonal cave gaps. Windows only for now.
---| "GEN_UPDATE_MOIST_WALLS"     # (plane): World generation updates a plane's moist cave walls. The result is nil.
---| "GEN_TERRAIN_BLOB"           # (terrain, x, y, n, onto, maxDistance): A blob of terrain is laid.
---| "CREATE_CELESTIAL_PLANE"     # (): World generation builds the Celestial Plane. The result is nil.
---| "CREATE_SKY_PLANE"           # (): World generation builds the Sky. The result is nil.
---| "CREATE_AGARTHA_PLANE"       # (): World generation builds Agartha. The result is nil.
---| "CREATE_INFERNO_PLANE"       # (): World generation builds the Inferno. The result is nil.
---| "CREATE_HADES_PLANE"         # (): World generation builds Hades. Windows only for now. The result is nil.
---| "CREATE_ELEMENTAL_PLANE"     # (): World generation builds the Elemental Planes. The result is nil.
---| "LOAD_AZTLAN_PLANE"          # (): World generation loads Aztlan. Windows only for now. The result is nil.
---| "LOAD_PRIMAL_PLANE"          # (): World generation loads the Primal Plane. The result is nil.
---| "AUTO_RECRUIT"               # (player): A player's auto recruit runs. Windows only for now. The result is nil.
---| "USE_LIBRARY"                # (commander): A commander uses a library. The result is nil.
---| "ADD_GROUND_ITEMS"           # (x, y): Items are added to a square's ground. Windows only for now. The result is nil.
---| "VOID_INSANITY"              # (unit, x, y): The Crystal Globe's insanity on the Void. The result is nil.
---| "WANDERER_PICK_TARGET"       # (commander, objective): A wandering commander picks a target by its objective. The result is nil.
---| "WANDERER_PICK_UP"           # (commander): Free units on a wanderer's square join it. Windows only for now.
---| "SET_HOLDIT"                 # (player, x, y): Units on a square are told to hold. Windows only for now.
---| "LIBRARY_SCREEN"             # (commander): The library screen opens. Windows only for now. The result is nil.
---| "CHOP_BURN_SCREEN"           # (): The Chop & Burn window opens. The result is nil.
---| "SLAVE_HUNT_SCREEN"          # (): The Hunt for Slaves screen opens. The result is nil.
---| "UNIT_STATS_SCREEN"          # (unit): The unit stats screen opens. The result is nil.
---| "ITEM_INFO_SCREEN"           # (item): An item's info screen opens. Windows only for now. The result is nil.
---| "WEAPON_INFO_WINDOW"         # (weapon): A weapon's info window opens. Windows only for now. The result is nil.
---| "PLANE_INFO_SCREEN"          # (plane): A plane's info screen opens. The result is nil.
---| "BATTLE_INTRO_SCREEN"        # (attacker, defender, x, y): The Fight box before a shown battle. Windows only for now. The result is nil.
---| "COMMANDER_SET_DONE"         # (): The selected commander is marked done for this turn. Windows only for now. The result is nil.
---| "SELECT_NEXT_COMMANDER"      # (): The map selects the next commander. The result is nil.
---| "SELECT_PREV_COMMANDER"      # (): The map selects the previous commander. Windows only for now. The result is nil.
---| "RUN_HATREDS"                # (): The month's hatreds run. Windows only for now. The result is nil.
---| "SANDSTORM_CLOUD_UPDATE"     # (): The month's desert clouds. Windows only for now. The result is nil.
---| "TAINTED_MAGIC_PASS"         # (a, b): The month's pass while magic is tainted. Windows only for now. The result is nil.
---| "HERO_ARRIVAL"               # (player): A class's hero may arrive for a player. Windows only for now. The result is nil.
---| "CLEAN_SQUARE"               # (x, y): A square is cleared. The result is nil.
---| "ADJUST_FOR_SNOW"            # (x, y, w, h): Ground turns to snow by the temperature. The result is nil.
---| "START_TUNE"                 # (tune, force): The game starts a music tune. Windows only for now. The result is nil.
---| "SQUARE_SET_OWNER"           # (x, y, player): A square's owner is written. Windows only for now. The result is nil.
---| "TEAMLESS_OWN_TEAM"          # (): Each slot with no team gets a team of its own. The result is nil.
---| "ADD_BENEFIT"                # (unit): A unit gets a benefit. Windows only for now. The result is nil.
---| "SOCIETY_PRESETS"            # (society): The society's world constants are set. Windows only for now. The result is nil.
---| "MOVE_STEP_AFTER"            # (): A move step ends. The result is nil.
---| "NUMBER_BOX"                 # (title, prompt): The game's number entry box opens.
---| "ENCHANTS_DESTROYED_AT"      # (player, x, y): Active effects on a square end as a player takes it. The result is nil.
---| "STORM_SPAWN"                # (x, y): A square may spawn Storm Elementals. Windows only for now. The result is nil.
---| "WILD_SPAWN"                 # (x, y): A square may spawn its wild animals. Windows only for now. The result is nil.
---| "REVELS_SPAWN"               # (x, y): A square may spawn its revelers. Windows only for now. The result is nil.
---| "HORROR_LORD_PICK"           # (x, y): A greater Horror is drawn. Windows only for now. The result is nil.

---The game values coe5.get and coe5.set reach.
---@alias coe5.Variable
---| "turn"                  # integer: 0 on the first turn, -1 during setup. Read-only.
---| "networkGame"           # boolean. Read-only.
---| "loadedGame"            # integer: nonzero for a game loaded from a save. Setting it nonzero logs lua_loaded_game_set with your mod.
---| "loadedMapName"         # string: the map file the game started on, empty for a random map. Read-only.
---| "society"               # integer: 0 Random, 1 Dark Ages, 2 Agricultural, 3 Empire, 4 Fallen Empire, 5 Monarchy, 6 Dawn of a New Empire, 7-11 sub-societies; nothing else (the game's society table). Changes the moment the player clicks it.
---| "genAncientRuinChance"  # integer: the society's ancient ruin chance per mille (Elysium's ruins; the Sky's Ancient Temples and Magic Libraries), set when the world is built. Read-only.
---| "genLibraryChance"      # integer: the society's Magic Library chance per mille, set when the world is built. Read-only.
---| "genBattlefieldChance"  # integer: the society's battlefield chance per mille (the Sky's Gallows and Cloud Graveyards), set when the world is built. Read-only.
---| "genTowerChance"        # integer: the society's tower chance per mille (Elysium's towers, the Sky's Cloud Towers), set when the world is built. Read-only.
---| "genVillageChance"      # integer: the society's settlement chance per mille (the Sky's Cloud Cities, Villages and Hamlets), set when the world is built. Read-only.
---| "genTownShare"          # integer: the society's town share per mille, the settlements that are towns (the Sky's Cloud Cities), set when the world is built. Read-only.
---| "commonCause"           # integer: the Advanced Options checkbox. Changes the moment the player clicks it. Read during play and saved, so a change in a game counts.
---| "scoreGraphs"           # integer: the Advanced Options checkbox. Changes the moment the player clicks it. Read during play and saved, so a change in a game counts.
---| "battleReports"         # integer: the Advanced Options checkbox. Changes the moment the player clicks it. Read during play and saved, so a change in a game counts.
---| "mapW"                  # integer: the map's width in squares, written at Ok (with a random ±1). At most 1000, the game's square arrays.
---| "mapH"                  # integer: the map's height in squares, written at Ok (with a random ±1). At most 160, the game's square arrays.
---| "startFlags"            # integer: start positions, written at Ok: 8 clustered, 16 west/east, 0 random.
---| "wilder"                # integer: Wilder, written at Ok: 2 on, 1 off, 0 before the first game. 0 to 2, what the game stores.
---| "mapDepth"              # integer: despite the name, the width of the whole world: the map's width, raised as planes are added beside the map. At most 1000.
---| "northPart"             # integer: Random Map Settings' North/South box, the north part in percent; -1 is the default, 25. -1 to 100, what the game stores. Not saved.
---| "southPart"             # integer: the south part in percent; -1 is the default, 35. -1 to 100. Not saved.
---| "slotController"        # per slot, integer: coe5.controller.human, .ai, or .removed; .independent once the world is built.
---| "slotClass"             # per slot, integer: the class's `newclass` number (1 Baron, 2 Necromancer, ...). Below 1 is Random until the classes are rolled. At most the class table's last record: 249 on Windows, 199 on Mac.
---| "slotAi"                # per slot, integer: AI difficulty, 2 (Jester) by default. Read at each turn start and saved, so a change in a game counts. At most 10: the game stops on a level outside 1 to 10 at an AI's turn.
---| "slotTeam"              # per slot, integer: below 0 is no team.
---| "slotName"              # per slot, string, up to 31 characters. Humans' names are filled in after PLAY_GAME_NEW_WORLD's next.
---| "slotColorRed"          # per slot, number 0 to 1: the red of the player's colour, the square on Setup Participants. Each new game's setup starts every slot on the palette's colour (playercolor). Takes what the game stores: a finite number from -8421505 to 8421504 (playercolor's whole numbers over 255); NaN or an infinity is refused (reason=not_finite).
---| "slotColorGreen"        # per slot, number 0 to 1: the colour's green.
---| "slotColorBlue"         # per slot, number 0 to 1: the colour's blue.
---| "slotResource"          # per slot and resource, integer: how much of resource 0-13 (resourceName's order, 0 Gold) the player has. coe5.get("slotResource", slot, resource), coe5.set("slotResource", slot, resource, value).
---| "slotAiIncome"          # per slot and resource 0-13, integer: slotIncome as the game pays an AI of the player's AI level, for a human too: a human a script plays as an AI (coe5.runAiTurn) gets a human's 100% meanwhile, and this reads what its AI would get. 0 for a slot that is neither a human nor an AI. Read-only.
---| "slotAiTrade"           # per slot and resource 0-13, integer: what the trade step of the player's next turn start changes of the resource, as the game runs it after paying the incomes (slotAiIncome): the player's trade type (slotTradeType) buys a resource with gold or sells one for gold, with that turn start's trade points. Negative for what it pays (the gold of a buy, the resource of a sell, the trade points), positive for what it gets. Read with the stocks of now plus those incomes; the step's own random draw can give one unit more or less, and the more likely is taken; type 23's gem is that draw's, so no gem is listed. 0 with no trade type. Gold's gain at that turn start is slotAiIncome plus this, if nothing changes first. The trade points are that turn start's trade income alone: a Trade stock left over is overwritten, not added. The game fights a player's battles after its turn, before the next player's turn start, so a read in an AI_TURN `after` observer comes before them; the next player's PLAYER_TURN_START `before` observer comes after. Read-only.
---| "slotIncome"            # per slot and resource 0-13, integer: what the player gets of the resource at its next turn start, as the game works it out then: each special resource (all but Gold, Iron and Trade) only while the player has a commander who gathers it, then an AI's difficulty percent (a human's is 100; an AI's 100 plus its level's bonus, a quarter of a positive bonus more for the special resources, and that quarter less for Gold, Iron and Trade while the AI has any special income). Trade is the trade points. 0 for a slot that is neither a human nor an AI. Read-only.
---| "slotTradeType"         # per slot, integer 0-24: what the player's trade points turn into at its next turn start: 0 nothing, 1-22 a resource bought (odd) or sold (even: 1-2 iron, 3-4 sacrifices, 5-6 hands, 7-8 herbs, 9-10 fungus, 11-12 weed, 13-20 the four gems, 21-22 relics), 23 a random gem bought, 24 gems sold in turn. The game's AI picks it at the end of its move. Writable with 0 to 24; any other value is refused (out_of_range).
---| "slotTradeMode"         # per slot, integer: 1, or 2 when the AI's pick doubles its trade (a buy with gold above 500). Read-only.
---| "newWorldUniqueRandom"  # integer: Setup Participants' "unique random players" box, a local of the new game's setup call. Nil, and not writable, outside a wrap of a NEW_WORLD_* or PARTICIPANTS_* site, and inside PARTICIPANTS_DIFFICULTY_WAIT.
---| "cliMapW"               # integer: the command line's --mapw, below 1 when not given. Random Map Settings opens on exactly cliMapW x cliMapH when either is set. At most 1000.
---| "cliMapH"               # integer: the command line's --maph, below 1 when not given. At most 160.
---| "cliSociety"            # integer: the command line's --society, below 0 when not given. Read-only.
---| "subSociety"            # boolean: the command line's --subsoc, which lets the player pick a sub-society. Read-only.
---| "startDefault"          # integer: the start positions Random Map Settings opens with: 0 normal, 1 clustered, 2 west/east; nothing else.
---| "wilderDefault"         # boolean: whether Random Map Settings opens with Wilder on.
---| "setupNetwork"          # boolean: true while a network game is set up. Read-only.
---| "aiLevelName"           # index 0-10, string: the AI difficulty names that slotAi indexes. Read-only.
---| "screenW"               # integer: the game's UI width. Read-only.
---| "screenH"               # integer: the game's UI height. UI y counts up from the bottom. Read-only.
---| "hoveredId"             # integer: the id of the line under the pointer, as the file browser reads it right after its wait. A row id past the browser's last listed file doesn't reach its Delete (see FILE_BROWSER_WAIT).
---| "bannerFade"            # number: the mod banner column's opacity, 0 to 1. The game lowers it every frame and draws the banners only while it is above 0.
---| "activePlayer"          # integer: the slot whose turn it is, -1 outside a game. Read-only.
---| "season"                # integer: 0 Summer, 1 Autumn, 2 Winter, 3 Spring. Read-only.
---| "seasonName"            # index 0-3, string: the game's name for a season ("Winter"). Read-only.
---| "date"                  # string: the date as the map shows it, "Mid Summer of year 0". Read-only.
---| "worldW"                # integer: the world's width in squares. Read-only.
---| "worldH"                # integer: the world's height in squares. Read-only.
---| "uniqueRandomDefault"   # boolean: whether Setup Participants' "unique random players" box starts ticked.
---| "slotLockClass"         # per slot, integer: the class a loaded map locks the slot to. Read-only.
---| "slotLockController"    # per slot, integer: the controller a loaded map locks. Read-only.
---| "slotLockAi"            # per slot, integer: the AI difficulty a loaded map locks. Read-only.
---| "slotLockTeam"          # per slot, integer: the team a loaded map locks. Read-only.
---| "eventVar"              # index 0-9999, integer: the .c5m event variables that setvar and +varequal use.
---| "modLastIndex"          # integer: the last index in the game's mod list. Read-only.
---| "modName"               # index 0 to modLastIndex, string: the mod's .c5m name. Read-only.
---| "modFolder"             # index 0 to modLastIndex, string: the mod's folder. Read-only.
---| "modEnabled"            # index 0 to modLastIndex, integer: nonzero when enabled on the Mods screen. Read-only.
---| "monsterName"           # index 0 to monsterLastIndex, string (empty for an unused index). Read-only.
---| "monsterLastIndex"      # integer. Read-only.
---| "monsterSprite"         # index 0 to monsterLastIndex, integer: the monster's sprite, for coe5.ui.nameBox. Read-only.
---| "itemName"              # index 0 to itemLastIndex, string. Read-only.
---| "itemLastIndex"         # integer. Read-only.
---| "className"             # index 0 to classLastIndex, string; 0 is "no class". Read-only.
---| "classLeader"           # index 0 to classLastIndex, string: the class leader's monster name. Read-only.
---| "classLastIndex"        # integer. Read-only.
---| "terrainName"           # index 0 to terrainLastIndex, string. Read-only.
---| "fortPartWord"          # index 0 to 31, a fort part bit, string: the terrain word that sets it ("irongate" for 11, the bit of 2048), from the game's terrain reader; nil for a bit none sets. Read-only.
---| "monster"               # index: a monster 0-5999, then a word of a .c5m monster block, the parser's spelling: a field (hp, str, armor, mor, rank, mr) is an integer, a flag or move word (undead, flying, wateronly) a boolean, an ability word ("aihold", "mastery") its value, 0 for none. An unknown word is an error. Read-only.
---| "weapon"                # index: a weapon (weaponDmgType's), then a word of a .c5m weapon block: a field (dmg, dmgtype, aoe, range, init, trgrank, nextdmg, nextalwayswep, look, sound, flymode, flylook, flysound, sndvol, reloadsnd) an integer, a special word (drain, sweep, ...) a boolean. nextalwayswep holds the chained weapon as the game keeps it: the weapon's number for nextwep, its negative for nextalwayswep, -1 for none. coe5.set writes a weapon your own mod made with newweapon (any other is refused, reason=not_own_weapon), until the game next reads the mods.
---| "item"                  # index: an item 0-999, then a word of a .c5m magic item block: a field (type, rarity, itemwep) an integer, a flag or move word a boolean, an ability word (armor, protection, hp, and the monster ability words) its value. Read-only.
---| "class"                 # index: a class 0-199, then a word of a .c5m class block: hometerr an integer, an ability word (startplane, aiclass, stdtroops, goldbonus, likesnorth, ...) its value, 0 for none. Read-only.
---| "terrain"               # index: a terrain 0-999, then a word of a .c5m terrain block: a field (gold, iron, trade, herbs, fungus, weed, sacr, dead, hands, pop, lifeforce, gems, apcost, batmap) an integer, a flag or fort word (citadel, cave, walls, irongate) a boolean, ritrebate, bonusrelics, destroyto, mistrare their values. Read-only.
---| "ritual"                # index: a ritual (as ritualName), then a word of a .c5m ritual block: level and terr integers, a flag word (free, nostart, rarestart, ...) a boolean, an effect word ("maxcast", "nomonreq") its value, 0 for none. Read-only.
---| "terrainLastIndex"      # integer. Read-only.
---| "terrainIncome"         # index: a terrain (as terrainName), then a resource 0-13, integer: the terrain's own income of the resource per month, as its .c5m gives it (gold, iron, trade, herbs, fungus, weed, sacr, hands), before seasons, class bonuses and enchantments; nil for Gems (7-10, a square's squareGems), Relics and Lifeforce, which no terrain field gives. Read-only.
---| "ritualName"            # index 0 to ritualLastIndex, string. Read-only.
---| "ritualLastIndex"       # integer. Read-only.
---| "resourceName"          # index 0-13, string: Gold, Iron, ... Lifeforce. Read-only.
---| "incomeName"            # index 0-18, string: the game's income names, "Gold Income" ... "Lifeforce Income", then the cost types past 13: "Humanoid Corpses", "Gem Income", "Expendable Units Income", "Corpses", "Goblin Income". Read-only.
---| "classRecruitName"      # index: a class (0 to classLastIndex), then an offer 0-159, string: the monster name of the class's recruitment offer (addunitrec, addmercrec, addcomrec); nil at and past the list's end. Read-only.
---| "classRecruitCount"     # index: a class, then an offer 0-159, integer: units per offer, -1 for a commander (addcomrec); nil at and past the list's end. Read-only.
---| "classStartArmy"        # index: a class, then a line 0-15, string: a start army line, "6 Spearman" (addstartunits) or "High Lord" for a commander (addstartcom); nil at and past the last line. Read-only.
---| "classStandardTroops"   # index: a class, boolean: the class has stdtroops, so it also recruits Spearman, Swordsman, Archer, Crossbowman, Heavy Infantry, and Catapult. The game holds those six names in its code, not in a table, so a script lists them itself. Read-only.
---| "classAbility"          # index: a class, then an ability id (coe5.classAbility), integer: the class's value of it, through the game's own lookup (the first match), 0 when the class has none. The game's random class roll reads aiClass, randomSkip, aiLevelMin, and aiLevelMax. Read-only.
---| "messageBoxText"        # string: the game's message box's text, as it shows it. Nil outside a wrap of MESSAGE_BOX_WAIT. Read-only. Windows only for now.
---| "mapPointerX"          # integer: the map square under the pointer in this map frame, -1 when none (it is a square of the world, planes side by side). Nil outside a wrap of MAP_SCREEN_WAIT; read it after next. Read-only.
---| "mapPointerY"           # integer: that square's y, -1 when none. Nil outside a wrap of MAP_SCREEN_WAIT. Read-only.
---| "uiPointerX"            # number: the pointer's x in UI units, from the left, the space coe5.ui.drawText and drawRect take (the UI is 914 wide at 1152x756 and wider in a wider window: coe5.mapView's screenW). Updated each frame of any screen. Read-only.
---| "uiPointerY"            # number: the pointer's y in UI units, up from the bottom, as uiPointerX. With mouseButtons in a MAP_SCREEN_WAIT wrap, after next, a script takes a click on what it drew: test the pointer against its text's rectangle, set mouseButtons to 0 while the button is down, and return 0 for next's id (a click over the game's own buttons gives their id). Read-only.
---| "uiWheel"               # integer: the mouse wheel's running count while a preview world runs, nil outside one. A panel over the map scrolls by how far it moved since the last frame; the wheel over a coe5.setPreviewDocks rectangle doesn't zoom the map. Read-only.
---| "mouseButtons"          # integer: this frame's mouse buttons, 1 left, 2 right, 4 middle, added; the first frame of a press, then each frame it is held. In a wrap of MAP_SCREEN_WAIT, after next, set it to 0 to keep the map from acting on the click (a left click moves the selected commander), and keep setting it to 0 while the button stays down. 0 to 7.
---| "keyHeld"               # index: a key, as coe5.key names it or a character's byte (1-399, no ctrl or alt), boolean: the key is held down now. The key itself, so a key rebindKey moved reads as the key the player presses. Every key reads false again when the game window gains or loses focus, as the game clears its own key state then. Read-only.
---| "windowMode"            # integer: the window's mode now: 0 a window, 1 fullscreen, 2 a borderless fullscreen window. Read-only.
---| "fullscreen"            # integer: the Preferences' Fullscreen, 0 a window, 1 fullscreen, 2 borderless; the game saves it and uses it at the next start (windowMode is the window now). Writable, 0 to 2.
---| "battleGrid"            # integer: the battle's grid, 1 shown, as its Hide/show grid (key g) sets it. Writable, 0 or 1.
---| "frameMs"               # integer: the milliseconds the last frame took, what the map's sway and zoom glide advance by. Read-only.
---| "frameCount"            # integer: the frames the game's screens have drawn since it started. Read-only.
---| "mapEditorOpen"         # boolean: the map editor is open. Read-only.
---| "squareSeen"            # index: a slot, then x 0-999, then y 0-159, integer: what that player sees of the square: 1 darkened, 2 seen, 3 scried, 0 none of these. The map acts on a click only on a square above 1. Read-only.
---| "squareArmy"            # index: x 0-999, then y 0-159, integer: the square's army size, 0 for none, -1 while two players at war share it. Read-only.
---| "squareArmyOwner"       # index: x 0-999, then y 0-159, integer: the player whose army stands on the square (the one with the most units), -1 for none or a square outside the world. Read-only.
---| "squareOwner"           # index: x 0-999, then y 0-159, integer: the player who owns the square, -1 for none. Read-only.
---| "squareTerrain"         # index: x 0-999, then y 0-159, integer: the square's terrain type (terrainName takes it). Writable only while the world is built (see squareName): 0 to the last terrain, through the game's own terrain change, which also sets the square's gems, its owner's bookkeeping and its ground (set squareGround after it).
---| "squarePortal"          # index: x 0-999, then y 0-159, integer: the square's portal number, 0 for none. Above 0 it is an entrance, and coe5.portalExits lists where it leads; below 0 only an exit. Squares with the same number (either sign) link, on any plane, as a map file's portal x y n makes them. Writable only while the world is built (see squareName): -32768 to 32767.
---| "squareGround"          # index: x 0-999, then y 0-159, integer: the square's ground, what a map file's groundrow writes; the game sets it from the terrain and the plane. Writable only while the world is built (see squareName): 0 to 28.
---| "squareTemperature"     # index: x 0-999, then y 0-159, integer: the square's temperature, what a map file's temprow writes; below 1 is cold (snow ground and the game's cold rules), above 49 hot; at 25 or less, the game's temperature step and a plane's map file also give the square the cold flag, which draws conifers in its forest. Writable only while the world is built (see squareName): -32768 to 32767.
---| "squareName"            # index: x 0-999, then y 0-159, string: the square's name, what a map file's squarename writes, up to 31 characters (a longer one is cut, as squarename cuts it). The square values are writable only while the world is built: inside a wrap of PLAY_GAME_NEW_WORLD (the whole world), or a plane's generator (only that plane's squares). Elsewhere coe5.set is refused (not_world_building, or outside_plane for a square outside the area being built), as is a value the game never stores there (out_of_range).
---| "squarePlane"           # index: x 0-999, then y 0-159, integer: the plane the square lies on (planeName takes it), -1 for none, as the game finds it. Read-only.
---| "commanderLastIndex"    # integer: the last commander record in use or once used; a loop runs 0 to it. Read-only.
---| "selectedCommander"     # integer: the commander selected on the map, whose orders the commander panel shows; -1 for none. Read-only.
---| "commanderUnit"         # index 0-9989, integer: the commander's unit (unitOwner, unitHp, unitMonster take it), below 0 for an empty record. Read-only.
---| "commanderRitual"       # index: a commander 0-9989, then 0-63, integer: a ritual the commander knows (ritualName, ritualSummons take it); nil at and past the first slot below 1, the list's end. Read-only.
---| "commanderCostAvailable" # index: a commander 0-9989, then a cost type 0-18 (as ritualCostType), integer: what the owner of the commander's unit has of it where the unit stands, as the game counts it when a ritual is cast: 0-13 the stock, 14 humanoid corpses on the square, 15 gems, 16 the player's expendable units there, 17 all corpses there, 18 the player's goblins there; a cost is paid when this is at least its amount. Nil for a commander with no unit or a unit no player owns. Read-only.
---| "commanderAiTargetX"    # index 0-9989, integer: x of the square an AI commander's move heads for, coe5.aiMove.noSquare for none. Read-only.
---| "commanderAiTargetY"    # index 0-9989, integer: that square's y. Read-only.
---| "commanderAiMoveKind"   # index 0-9989, integer: what the move is: coe5.aiMove.none, .target (from the rally, friendly move, or conquest step), or .explore. Read-only.
---| "commanderAiMoves"      # index 0-9989, integer: the moves an AI commander made this turn; its move loop stops at 4 and clears them for the next turn. Read-only.
---| "commanderAiFromX"      # index 0-9989, integer: x of the square an AI commander stood on before its last step. Read-only.
---| "commanderAiFromY"      # index 0-9989, integer: that square's y. Read-only.
---| "commanderAiLastTargetX" # index 0-9989, integer: x of the target an AI commander's move loop ended on this turn. Read-only.
---| "commanderAiLastTargetY" # index 0-9989, integer: that target's y. Read-only.
---| "commanderRitualPlan"   # index 0-9989, integer: the ritual an AI commander plans to cast (ritualName takes it), as the game's AI keeps it from turn to turn until it casts or scraps it; 0 for none, -1 a spawnbooster (a commander the AI keeps on its square to raise a spawn, which never moves). coe5.scrapRitualPlan drops a plan. Read-only.
---| "commanderDone"         # index 0-9989, boolean: true once the player gave the commander the map's "Commander is done for this turn" (Space; the game also marks the other commanders of its group), which the map's next-commander key then skips and End Turn's "commanders have AP left" warning leaves out; no action points spent. The game clears it at its player's next turn start, for a new commander, and on load (saves don't hold it). Read-only.
---| "slotLeader"            # per slot, integer: the player's leader, a commander (commanderUnit takes it), -1 once the leader is dead. Read-only.
---| "unitLastIndex"         # integer: the last unit record; a loop runs 0 to it. Read-only.
---| "unitOwner"             # index 0-98999, integer: the unit's player (slot). Read-only.
---| "unitHp"                # index 0-98999, integer: the unit's hit points; 0 or below is a dead unit or an empty record. Read-only.
---| "unitMonster"           # index 0-98999, integer: the unit's monster (monsterName takes it), below 0 for an empty record. Read-only.
---| "unitCommander"         # index 0-98999, integer: the commander the unit follows (commanderUnit takes it), -1 for none: a commander's own unit too, unless it follows another. commanderStrength adds the commander's unit and every unit following it. The AI's troop pick-up and its pick-up of lone commanders set it. Read-only.
---| "unitStationary"        # index 0-98999, boolean: true when the unit never moves on the world map ("Cannot move on world map", the monster word stationary or an item's or ability's), as the game's AI checks it before it plans a commander's move; false for an empty record. Read-only.
---| "ritualSummons"         # index 0 to ritualLastIndex, boolean: the ritual has the summoning effect: it makes one of its ritualString results, at random. Read-only.
---| "ritualString"          # index: a ritual, then 0-47, string: one of its addstring results, as the .c5m wrote it ("c*Mound King", "1d3*Shade", "2*Cannon Car & 1*Mortar Car", "(167)Anakite"); nil at and past the first empty one. Read-only.
---| "ritualRitpow"          # index 0 to ritualLastIndex, integer: the ritual's school (ritpow). Read-only.
---| "ritualLevel"           # index 0 to ritualLastIndex, integer: its level, the ritual screen's "Level" group. Read-only.
---| "ritualCostType"        # index: a ritual, then 0-3, integer: a cost's resource (resourceName takes 0-13); nil at and past the first type below 0, the list's end. Read-only.
---| "ritualCost"            # index: a ritual, then 0-3, integer: that cost's base amount, before the caster's discounts (RITUAL_SCREEN_ROW's costs are the charged ones); nil where ritualCostType is. Read-only.
---| "ritualTerrain"         # index 0 to ritualLastIndex, integer: the terrain it needs: a terrain type (terrainName takes it), a group below -1, or -1 for none. Read-only.
---| "ritualEffect"          # index: a ritual, then 0-13, integer: an effect id (effectName gives its mod command); nil at and past the first id below 1, the list's end. Read-only.
---| "ritualEffectValue"     # index: a ritual, then 0-13, integer: that effect's value; nil where ritualEffect is. Read-only.
---| "ritualFlags"           # index 0 to ritualLastIndex, integer: the ritual's 64 flag bits. Read-only.
---| "ritualTerrainName"     # index 0 to ritualLastIndex, string: the game's own name for ritualTerrain, a group's included ("forest or jungle"). Read-only.
---| "ritualTerrainAt"       # index: a ritual, then x 0-999, then y 0-159, boolean: the square counts for the ritual's terrain, as the ritual screen checks it (true when it needs none); nil for a terrain code the game can't check. Read-only.
---| "ritualEffectMet"       # index: a ritual, then an effect 0-13 (as ritualEffect), then a commander 0-9989, boolean: the effect's rule holds for the commander where it stands, as the game's own checks decide it: nearby1req to nearby99req and nonearby1req to nonearby99req (the terrain within 1, 3, 5, 7 squares or on the plane, or none there); the unit rules minmonreq, monworldreq, monplayerreq, nomonreq, fewmonreq, nomonhomereq, nomonworldreq, nomonplayerreq, nomonelysiumreq, nomonmasteryreq (the ritual's "(&)", "(-)", "(+)", "(|)" strings, counted in order); promotion (an owned unit here, the caster too, named by the first line of one of the ritual's string pairs; a lone "(-)" line is skipped); levelreq, resrestrict, resreq, benrestrict, fxreq, nofxherereq, enchherereq, fortreq, nofortreq, hasportalreq, noportalreq, noplanereq, seasonreq, eventvarreq, noeventvarreq, homereq, homecitreq. An effect repeated after the first of its word is true (the game reads only the first). Nil for any other effect, or a commander with no unit, no owner, or (for the terrain rules) no square on the map. Read-only.
---| "ritualEffectTerrainName" # index: a ritual, then an effect 0-13, string: the game's own name for a nearby or nonearby effect's terrain, a group's included ("forest or jungle"); nil for any other effect. Read-only.
---| "effectName"            # index 0-1501, string: the mod command of a ritual effect id (ritualEffect), "maxcast" for 1057; nil for an id no mod command sets. Read-only.
---| "slotRitualCasts"       # per slot, then a ritual, integer: how many times that player has cast the ritual (what maxcast counts). Read-only.
---| "unitX"                 # index 0-98999, integer: the unit's square's x (ritualTerrainAt takes it). Read-only.
---| "unitY"                 # index 0-98999, integer: the unit's square's y. Read-only.
---| "unitBattleX"           # index 0-98999, integer: the battle cell of the unit's top-left corner, during a battle or on an open test field; nil off the field. Read-only.
---| "unitBattleY"           # index 0-98999, integer: its y, 0 to 19. Read-only.
---| "unitApLeft"            # index 0-98999, integer: the unit's action points left this turn, as the game reckons them (3, 4 for fast, 2 for slow, less those used; 0 for an empty record). The ritual screen casts while it is above 0. Read-only.
---| "unitApUsed"            # index 0-98999, integer: the unit's action points used this turn, the number unitApLeft subtracts; moving and the game's other actions add to it. Set it to 0 to give the unit every action point back.
---| "unitAbility"           # index: a unit, then an ability 0-1501 or its mod command ("aihold"; an unknown mod command is an error), integer: the unit's total of that ability, its monster's, items', and temporary ones, as the game adds them. A ritual's ritualRitpow is the ability that gives the caster's mastery level in its school. Read-only.
---| "tooltipLineHeight"     # number: the hover box's line height in UI units (14). The box's bottom sits at 16 less 2 a line, so a text past about 8 lines starts below the screen. Read-only.
---| "planeName"             # index 0-18, string: a plane of this world ("Elysium"), by the number a ritual's planereq names (squarePlane gives it); mods' planes from 11 (coe5.addPlane); nil past planeLastIndex. Read-only.
---| "planeX"                  # index 0-18, integer: the plane's first x on the world map, so a square's x on the plane is its world x less this; nil past planeLastIndex. Read-only.
---| "planeY"                  # index 0-18, integer: the plane's first y, 0; nil past planeLastIndex. Read-only.
---| "planeWidth"              # index 0-18, integer: the plane's width in squares; nil past planeLastIndex. Read-only.
---| "planeHeight"             # index 0-18, integer: the plane's height in squares; nil past planeLastIndex. Read-only.
---| "planeFlags"              # index 0-18, integer: the plane's flags; bit 0 means a human player has seen enough of it, which Reveal Map at Game Over goes by; nil past planeLastIndex. Read-only.
---| "infernoActive"           # boolean: the Inferno's wanderers on its plane may move; they wait while it is false Read-only.
---| "celestialActive"         # boolean: the Celestial Plane's wanderers on its plane may move; they wait while it is false Read-only.
---| "primalPortalsOpen"       # boolean: a portal to the Primal Plane stands open, so its gods may move Read-only.
---| "infernoCentreX"          # integer: the x of the Inferno's centre, where the Citadel of Abaddon stands Read-only.
---| "infernoCentreY"          # integer: its y Read-only.
---| "mapFilePlane"            # integer: the number of the map file's last plane line read Read-only.
---| "battleUnderground"       # boolean: the battle is on Agartha or the Inferno, where some spells are refused Read-only.
---| "battleSpoilsGold"        # integer: the gold the battle's dead have dropped so far, by their goldcarrier Read-only.
---| "battleSpoilsIron"        # integer: the iron the battle's dead have dropped so far, by their ironcarrier Read-only.
---| "worldNorthPart"          # integer: the north part world generation goes by: northPart, or 25 for the default Read-only.
---| "worldSouthPart"          # integer: the south part world generation goes by: southPart, or 35 for the default Read-only.
---| "genRuinTempleShare"      # integer: the society's share of ruins that are temples, per mille Read-only.
---| "genDeadForestChance"     # integer: the society's chance of a dead forest, per mille Read-only.
---| "genAncientForestRate"    # integer: the society's rate of ancient forests Read-only.
---| "genSwampChance"          # integer: the society's chance of a swamp, per mille Read-only.
---| "genBattlefieldShare"     # integer: the society's share of battlefields, per mille Read-only.
---| "genCastleChance"         # integer: the society's chance of a castle, per mille Read-only.
---| "genFordChance"           # integer: the society's chance that a river crossing is a ford, in percent Read-only.
---| "genSkyFeatureChance"     # integer: the society's chance of the Sky's rainbows and beanstalks, per mille Read-only.
---| "genRareChance"           # integer: the society's chance of rare sites, portals, Coral Towers and the Maelstrom, per mille Read-only.
---| "genMineChance"           # integer: the society's chance of a mine, per mille Read-only.
---| "droughtMonths"           # integer: the months of drought left on Elysium, 0 for none. Read-only.
---| "eventState"              # integer: where the mod pass stands in an event: 1 inside a playerevent, 2 inside a squareevent, -2 after endevent, -1 after a failed line, 0 outside any Read-only.
---| "eventFile"               # string: the file whose events the game is running, a map's event file or <folder>/<name>.c5m Read-only.
---| "loneHuman"               # integer: the game's one human slot, -1 for a game set up with none or several Read-only.
---| "routeMaxSkies"           # integer: the longest run of Sky or Dispersing Clouds squares a route may hold before the game routes it another way Read-only.
---| "pathLength"              # integer: how many squares the game's last route filled, at most 256 Read-only.
---| "battleSpeedSetting"      # integer: the Preferences' battle speed Read-only.
---| "pathX"                   # index 0-255, integer: the x of a square of the game's last route, from the far end back; pathLength says how many hold one Read-only.
---| "pathY"                   # index 0-255, integer: its y Read-only.
---| "commanderRepeatPercent"  # index 0-9989, integer: the power percent a commander's repeat ritual is paid and cast at Read-only.
---| "gameWinner"              # integer: the winner the game has settled on, -1 while the game goes on. Read-only.
---| "evilTimesMonths"         # integer: the months of Evil Times left. Read-only.
---| "tithePool"               # integer: the gold in the Tithe of El's pool. Read-only.
---| "spawnCarry"              # integer: a percent the game adds to the size of its next site or mover spawn, then clears. Read-only.
---| "rulershipSlot"           # integer: the slot the rulership check found, -1 for none. Read-only.
---| "messageLastIndex"        # integer: the last message in the game's list, -1 when empty. Read-only.
---| "autosaveOn"              # integer: nonzero while autosave is on. Read-only.
---| "endTurnWarning"          # integer: the Preferences' End turn warning checkbox. Read-only.
---| "nextCommanderMode"       # integer: the Preferences' Next Commander Selection. Read-only.
---| "graphicsDetail"          # integer: the Preferences' decoration detail: below 10 a quarter of the pieces, 10 half. Read-only.
---| "selectedCommanderGroup"  # integer: the order group of the selected commander when the map took it. Read-only.
---| "routeBoundOther"         # integer: the strength the current route may not run into, for an independent army. Read-only.
---| "routeBoundAi"            # integer: the same for an AI player's army. Read-only.
---| "routeBoundHuman"         # integer: the same for a human player's army. Read-only.
---| "routeExemptX"            # integer: the x of the square a route is never refused for strength: a player's move order's target. Read-only.
---| "routeExemptY"            # integer: its y. Read-only.
---| "routeMaxSeas"            # integer: the longest run of sea squares a route may hold before the game routes it another way. Read-only.
---| "pathNoSea"               # integer: 1 while the current route refuses sea. Read-only.
---| "pathNoSky"               # integer: 1 while the current route refuses Sky. Read-only.
---| "stepTargetX"             # integer: the x of the square the current move step is going to. Read-only.
---| "stepTargetY"             # integer: its y. Read-only.
---| "freeTroopsCount"         # integer: the free troops on the square the AI is gathering from. Read-only.
---| "battleDelay"             # integer: the battle's current per-step delay; the speed keys change it. Read-only.
---| "battleOver"              # integer: nonzero when a battle is told to stop early. Read-only.
---| "battleNotASiege"         # integer: nonzero for a battle with no siege setup, where assassin weapons do not fire. Read-only.
---| "mapVersion"              # integer: the version of the map file last read. Read-only.
---| "mapPlaneIndex"           # integer: the plane a map file's lines write into, -1 before its mapsize. Read-only.
---| "sampleHighest"           # integer: the highest sound coe5.playSample can play, -1 at reset. Read-only.
---| "recruitLocationLast"     # integer: the Recruit screen's last location, -1 for none. Read-only.
---| "genRealmWord"            # string: "Kingdom" or "Empire", the word the society gives the realm. Read-only.
---| "battleAttackerRankCount" # index 0-2, integer: the attacker's mobile units this round: of rank -1, of ranks -1 and 0, and all. Windows only for now. Read-only.
---| "battleDefenderRankCount" # index 0-2, integer: the same for the defender. Windows only for now. Read-only.
---| "battleSummons"           # index a monster 0-5999, integer: the summons of the monster in this battle so far, -1 after a failed one. Read-only.
---| "autoNameLast"            # index a name list 0-61, integer: the list's last name, -1 when empty or before the game first draws a name. Read-only.
---| "mapFileHasPlane"         # index a plane 0-10, integer: 1 for each plane line the map file holds. Read-only.
---| "aiLevelBonus"            # index an AI level 0-10, integer: the percent that AI level adds to its income. Read-only.
---| "recruitLocationX"        # index 0-255, integer: the x of a Recruit screen location. Read-only.
---| "recruitLocationY"        # index 0-255, integer: its y. Read-only.
---| "unitHomeX"               # index a unit, integer: the x of the unit's home square, as homereq compares it. Read-only.
---| "unitHomeY"               # index a unit, integer: its y. Read-only.
---| "itemSlotType"            # index an item, integer: the kind of equipment position the item fits. Read-only.
---| "planeLastIndex"        # integer: the world's last plane, as the game counts them, 10 without a mod's plane and up to 18 with them; planeName runs 0 to it. Read-only.
---| "planeTint"             # index 0-18, four numbers r, g, b, strength, each 0 to 1: the colour the map lays over the screen while the plane is in view, as the plane's record holds it (red at 0.15 on Elysium and the Sky while a Blood Sun is up); strength 0 while none is laid; nil past planeLastIndex. Read-only: coe5.setPlaneTint writes it.
---| "ticksMs"               # integer: the game's millisecond clock. Only the difference between two reads means anything, and it can jump back when it wraps. Read-only.
---| "traceLevel"            # integer: the game's debug trace level, 0 in a player's game (each -d on the command line adds 1). At 1 the game writes its trace (log.txt in the game folder on Windows, standard output on Mac), at 2 and 3 more of it; above 0 the game also shows a few debug details (a unit of no one named with its number). coe5.keepTraceLines picks the lines. 0 to 3.
---| "bugChecksOff"          # boolean: true skips the game's bug checks, as its --nochecks switch does. At trace level 2 and 3 the game runs them at many places a turn (each over every unit), and they also fix some of what they find.
---| "commanderName"         # index 0-9989, string: the commander's name ("Agrippa"); the trace names it "<name> the <monster>". Read-only.
---| "commanderStrength"     # index 0-9989, integer: the strength of the commander's army as the game counts it for the AI's moves (the comstr its AI sites get): the commander's unit and every unit following it. Nil for a commander with no living unit. Read-only.
---| "commanderGarrisonLeft" # index 0-9989, integer: the strength that stays on the commander's square when the commander leaves as the AI moves it: its army, the free troops the AI's gather adds to it there, less the units the gather leaves behind, by the game's own rule, measured as squareStrength measures the owner's units (the fort and ranged raise worked out on the staying units). The gather's one random drop (a follower with unit flag 8 that is a tenth to a quarter of the army, 50%) counts as leaving. Nil for a commander with no living unit on a square. Read-only.
---| "slotHomeX"             # per slot, integer: the player's home citadel's x, -1 for none. Read-only.
---| "slotHomeY"             # per slot, integer: the player's home citadel's y. Read-only.
---| "squareGems"            # index: x 0-999, then y 0-159, then a gem type 0-3, integer: the square's gem income of that type; the game lets a player with a gem gatherer own a square with any above 0. Read-only.
---| "squareCorpses"         # index: x 0-999, then y 0-159, then a corpse kind 0-7 (coe5.corpseKind), integer: the square's corpses of that kind, what Raise Dead, Raise Animals, reanimate, the ritual corpse costs and the corpse event triggers use. Set 0 to 32767.
---| "squareStrength"        # index: a slot (or -1 for every player), then x 0-999, then y 0-159, integer: the strength of the units on the square of that player and its teammates, now, as the game measures it for the AI: each living unit's strength as that player sees it, the sum raised by (100 + 60 x the square's fort level)% when over a tenth of the units are ranged. Sees through fog. Read-only.
---| "squareArmyStationary"  # index: x 0-999, then y 0-159, boolean: true when the game never moves the army on the square on the world map: every living unit there is stationary ("Cannot move on world map"), as the independents' moves check it. Also true for a square with no living unit. Sees through fog. Read-only.
---| "squareArmyHolds"       # index: x 0-999, then y 0-159, boolean: true when the independents' moves hold the army on the square, by the game's two hold tests before its stationary one: the square's hold flag (the event command holdit 1, a gold mine's attackers) or the hold flag on its top unit, the unit the move finds for the army's owner. False for a square with no army. Sees through fog. Read-only.
---| "squareCitadel"         # index: a slot (or -1 for any class), then x 0-999, then y 0-159, boolean: true when the square is a citadel for that player, as the game counts a player's citadels and its AI's conquest scores them: the terrain's citadel flag, or a citadel only the player's class has (its forest, wooden or mine citadels, a class's citadel terrain or enchantment there). Whoever owns it. Sees through fog. Read-only.
---| "squareRememberedStrength" # index: a slot, then x 0-999, then y 0-159, integer: the strength that player's AI remembers on the square, what the game's conquest and threat checks read: every unit's strength there (as squareStrength with -1) when the player's AI turn last saw it, or, once forgotten (5% a turn on a square out of view), the strength there now, which the read doesn't store; 0 off the world. Read-only.
---| "unitStrength"          # index: a slot (or -1 for no player), then a unit 0-98999, integer: the unit's strength as that player counts it, the per-unit strength squareStrength and commanderStrength add up: the player's own unit with its armor, items and afflictions (pass unitOwner for the strength its owner sees), any other unit, allies' too, as the bare monster it shows, without armor or items. 0 for a unit that isn't alive. Read-only.
---| "unitArmor"             # index 0-98999, integer: the unit's armor as its unit screen shows it: its monster's or items' armor, protection, and the states that add to it. Nil for a unit that isn't alive. Read-only.
---| "unitVisibleTo"         # index: a unit 0-98999, then a slot, boolean: whether that player sees the unit, as the game decides what the map shows: its own unit and any unit without stealth on any square (squareSeen tells the fog); a stealthy one only scried, or in view with a unit of that player next to it with spiritsight (acutesenses against plain stealth). False for a unit that isn't alive. Read-only.
---| "monsterAttackType"     # index: a monster 0-5999, then a row 0-15, integer: the kind of the monster's attack in that row, by its mod command: 1 meleeweapon, 2 rangedweapon, 3 spellweapon, 4 meleeweaponbonus, 5 rangedweaponbonus, 6 spellweaponbonus, 7 rangedweapon25, 8 meleeweaponspec, 10 prebatweapon, 11 rangedweapon50, 12 assassinweapon, 13 siegeweapon, 14 spellweapon50, 15 spellweaponsingle, 17 meleeweapon50s, 18 rangedweapon50s, 19 spellweapon50s, 21 rangedweapon50x; nil at and past the list's end (the first row whose type is below 1 or weapon below 0, as the game ends it). Read-only.
---| "monsterAttackWeapon"   # index: a monster, then a row 0-15, integer: the row's weapon (weaponDmgType takes it), or for a spell attack (types 3, 6, 14, 15, 19) its spell path; nil where monsterAttackType is. Read-only.
---| "monsterAttackValue"    # index: a monster, then a row 0-15, integer: the row's number as the .c5m wrote it: the damage value, a special damage weapon's kind (a bitmask: 262144 Confusion), or a spell attack's level; nil where monsterAttackType is. Read-only.
---| "weaponDmgType"         # index 0-4999, integer: the weapon's damage type (dmgtype): 1 slash, 2 blunt, 3 pierce, 4 fire, 5 cold, 6 shock, 7 magic, 8 fear, 9 poison, 10 acid, 11 phantasm, 12 special damage (its kind in the attack row's value), 13 special benefit, 14 charm, 15 polymorph, 16 sleep, 17 paralyze, 18 summon, 19 enslave, 20 swallow, 21 mirror image, 22 corrupt, 23 banish, 24 petrify, 25 animate, 26 heal. Read-only.
---| "ceremonyName"          # index 0 to ceremonyLastIndex, string: the game's name for a Voice of El ceremony number, its label on the Ceremonies screen: "Convert Settlement", "Break Seal", "Bolster Devotion", "Start Crusade", "Call the Inquisition", "Ceremony of Mastery" (three numbers, one per rank it raises a priest to), "Increase Divine Favor"; nil for a number with none. Match ceremonies by it, never by their numbers. Read-only.
---| "ceremonyLastIndex"     # integer: the last ceremony number the game takes; ceremonyName runs 0 to it. Read-only.
---| "unitXp"                # index 0-98999, integer: the unit's experience (xp). Read-only.
---| "unitKills"             # index 0-98999, integer: the unit's kills. Read-only.
---| "unitItem"              # index: a unit, then an equipment place 0-7: integer, the item there (itemName takes it), -1 for an empty place. A place keeps its number when an earlier one is empty. Read-only.
---| "unitKnownSpell"        # index: a unit, then 0-59: integer, a spell the unit knows. Nil at and past the first value below 0, the list's end. Read-only.
---| "unitMemorizedSpell"    # index: a unit, then 0-29: integer, a spell the unit memorized for battle. Nil at and past the list's end. Read-only.
---| "squareItem"            # index: x, y, then a place 0-5: integer, an item lying on the square (itemName takes it), -1 for an empty place. Read-only.
---| "commanderRepeatRitual" # index 0-9989, integer: the ritual the commander repeats each turn (ritualName takes it), -1 a slave hunt. Read-only.
---| "slotBolsterDevotions"  # index 0-31, integer: the Bolster Devotion ceremonies the player performed. Read-only.
---| "slotDivineFavor"       # index 0-31, integer: the player's Divine Favor, 0 to 4. Read-only.
---| "sealsBroken"           # integer: the seven seals broken, 0 to 7, one count for every player. Read-only.
---| "sealEventsDone"        # integer: the seals whose event has happened; the game catches up one seal a turn. Read-only.
---| "starsReliability"      # integer: the stars' reading for summoning control, -2 to 2 (2: "The stars are perfect for reliable summonings."). Read-only.
---| "starsPower"            # integer: the stars' reading for summoning size, -2 to 2 (2: "Summonings would be very powerful now."). Read-only.
---| "starsMonths"           # integer: the months the stars' readings still hold. Read-only.
---| "starsFlags"            # integer: 1 when the stars can't be read, 2 the water star. Read-only.
---| "apocalypseMonths"      # integer: 0 until the seventh seal's event, then the months of the apocalypse so far. Read-only.
---| "magicTaintedMonths"    # integer: the months of tainted magic left ("Horrors are drawing closer to our world and magic has become tainted"), 0 for none; the game's AI casts no ritual meanwhile. Read-only.
---| "mapZoom"               # number: the map view's UI units per square: 48 at the start, Page Up and Down glide it between 8 (12 outside the map editor) and 100. Read-only.
---| "mapCameraX"            # number: the world x, in squares, at the map view's centre column. Read-only: coe5.centerMap moves it.
---| "mapCameraY"            # number: the y, in squares; the square at the centre row is mapCameraY + 1. Read-only.
---| "mapViewPlane"          # integer: the plane the map view shows (planeName takes it). Read-only.
---| "enchantLastIndex"      # integer: the last record of the active ritual effects in use, at most 999. Read-only.
---| "enchantType"           # index 0-999, integer: the effect's type, 0 for a free record. Read-only.
---| "enchantKind"           # index 0-999, integer: the first name of enchantType, which still works. Read-only.
---| "enchantCaster"         # index 0-999, integer: the unit that cast the effect. Read-only.
---| "enchantOwner"          # index 0-999, integer: the caster's player. Read-only.
---| "enchantRitual"         # index 0-999, integer: the ritual that made the effect (ritualName takes it), 0 or below for none. Read-only.
---| "enchantX"              # index 0-999, integer: the effect's square. Read-only.
---| "enchantY"              # index 0-999, integer: its y. Read-only.
---| "battleSeed"            # integer: the random seed of the battle being fought, or of the last one. Read-only.
---| "battleX"               # integer: that battle's square. Read-only.
---| "battleY"               # integer: its y. Read-only.
---| "battleAttacker"        # integer: that battle's attacking player. Read-only.
---| "battleDefender"        # integer: that battle's defending player. Read-only.
---| "battleWidth"           # integer: that battle's battlefield columns. Read-only.
---| "battleRound"           # integer: that battle's round: -15 first, movement from 0. Read-only.
---| "battleWinner"          # integer: the winning side's player once the fight ends, -1 for none. Read-only.
---| "battleStarveRound"     # integer: the round past which every unit takes (round - it) damage each round. Read-only.
---| "battleChargeRound"     # integer: the round from which ranks 0 and -1 stop holding. Read-only.
---| "battleAmbush"          # boolean: that battle is an ambush. Read-only.
---| "battleAtSea"           # boolean: that battle is at sea. Read-only.
---| "battleSiege"           # boolean: that battle is a siege. Read-only.
---| "battleReplay"          # boolean: a battle replay is refighting a battle (its rounds and damage reach BATTLE_ROUND and DAMAGE_UNIT). Read-only.
---| "eventPlayer"           # integer: the player of the event being run. Read-only.
---| "eventTargetX"          # integer: that event's target square, -1 for none. Read-only.
---| "eventTargetY"          # integer: its y. Read-only.
---| "eventSquareX"          # integer: the square an event's trigger is being checked on. Read-only.
---| "eventSquareY"          # integer: its y. Read-only.
---| "unitLastSpawned"       # integer: the last unit a populate or a commander spawn made. Read-only.
---| "commanderLastSpawned"  # integer: the last commander a populate or a commander spawn made. Read-only.
---| "unitTempAbilityId"     # index: a unit, then 0-3: integer, a temporary ability's id (effectName and unitAbility take it): 0 at the list's end, -1 for a free place. Read-only.
---| "unitTempAbilityValue"  # index: a unit, then 0-3: integer, that temporary ability's value. Read-only.
---| "slotBlessingId"        # index: a slot 0-31, then 0-23: integer, a blessing the player has. Nil at and past the first id 0, the list's end. Read-only.
---| "slotBlessingValue"     # index: a slot, then 0-23: integer, that blessing's value. Nil at and past the list's end. Read-only.
---| "slotMostCitadels"      # index 0-31, integer: the most citadels the player has held; the game's AI plays desperate above 2 with under 2 now. Read-only.
---| "slotCitadels"          # index 0-31, integer: the citadels the player holds now, as the AI's turn counts them for its desperate play and its conquest. Read-only.
---| "aiMoveDelay"           # integer: the Preferences' Delay for AI moves, in milliseconds; 0 shows no AI move. Writable, 0 to 10000.
---| "aiGathersHands"        # integer: what the moving AI player gathers of hands, as its commanders' routine sets it before they move and its conquest step scores squares by it; the last AI's until the next AI moves. Read-only.
---| "aiGathersHerbs"        # integer: as aiGathersHands, for herbs. Read-only.
---| "aiGathersFungus"       # integer: as aiGathersHands, for fungus, the player's fungus ability added. Read-only.
---| "aiGathersSacrifices"   # integer: as aiGathersHands, for sacrifices. Read-only.
---| "aiGathersWeed"         # integer: as aiGathersHands, for weed. Read-only.
---| "aiGathersGems"         # integer: as aiGathersHands, for gems. Read-only.
---| "aiGathersLifeforce"    # integer: as aiGathersHands, for lifeforce. Read-only.
---| "aiWeighsIron"          # boolean: the AI's conquest weighs iron, set once any AI player's iron ability passed 40 and never cleared. Read-only.
---| "aiSaveResource"        # index: a cost type 0-18 (ritualCostType's), integer: nonzero when a failed ritual payment flagged it, so the AI's plan choice spares it; its commanders' routine clears them each turn. Read-only.
---| "slotAbility"           # index: a slot 0-31, then an ability 0-1501 or its mod command: integer, the player's value of it as the game reckons it: its class's value, then the best among its living units. Read-only.
---| "slotAbilitySum"        # index: as slotAbility: integer, the class's value, then the sum over its living units. Read-only.
---| "unitStateFlags"        # index 0-98999, integer: the unit's state bits with its items' (afflictions, insane 2^13, Fatigued 2^37). Nil for a unit that isn't alive. Read-only.
---| "unitReturns"           # index 0-98999, boolean: the unit comes back from death (immortal, reforming), as the game decides it. Nil for an empty record. Read-only.
---| "ceremonyCost"          # index 0 to ceremonyLastIndex, integer: a ceremony's cost as the game worked it out for the commander of the running CEREMONY_COSTS wrap (see there for the ceremonies the game multiplies it for), after that wrap's next (before it, the last costs the game worked out); nil outside the wrap, and for a number with no cost. Writable in it, 0 to 100000: the screen shows, the ceremony charges, and the AI weighs the new one (lua_refused reason=no_cost for a number with none, out_of_range past the bounds).
---| "weaponAoe"             # index 0-4999, integer: the weapon's area (aoe): 0 a single target, n a burst of n, -n n strikes, 1000s beams and cones. Read-only.
---| "weaponName"            # index 0-6998, string: the weapon's name, "" for an empty slot. Read-only.
---| "unitMoveFlags"         # index 0-98999, integer: the unit's move flags, the game's sum of its monster's move words and its items' (boots of flying): flying 1, immobile 4, slow 8, fast 16, swamp 32, water 64 (swims or sails), float 128, mountain 256, noland 512, passwall 2^32, stationary 2^35 (also for a unit the game holds in place), teleport 2^39, shipmove 2^40, tunnel 2^42, and the terrain words (forest1 2^23, snow 2^37 and the rest). Test a bit with &: coe5.get("unitMoveFlags", u) & 1 ~= 0 flies. It shows another player's unit with its items, which the game's AI never weighs: for what the AI may know, read monsterFlags or monsterMoveFlags of its unitMonster. Nil for an empty record. Read-only.
---| "unitFlags"             # index 0-98999, integer: the unit's flag words, its monster's and items' added as the game adds them (resrestrict's test): human 2^3, undead 2^4, spiritsight 2^5, stealth 2^17, invisible 2^22, ethereal 2^26, demonic 2^33, foreststealth 2^44, desertstealth 2^41, snowstealth 2^56, and the rest of the monster flag words. Shows another player's items, as unitMoveFlags does. Nil for an empty record. Read-only.
---| "monsterFlags"          # index 0-5999, integer: the monster's own flag words, as unitFlags' bits, as the .c5m set them: what anyone who knows the monster knows. Read-only.
---| "monsterMoveFlags"      # index 0-5999, integer: the monster's own move words, as unitMoveFlags' bits. Read-only.
---| "monsterHp"             # index 0-5999, integer: the monster's own hit points, a new unit's most before its items. Read-only.
---| "monsterAbility"        # index: a monster 0-5999, then an ability 0-1501 or its mod command ("mastery"), integer: the monster's own value of that ability as the game's lookup reads it (its first match), 0 for none; no items or temporary ones (unitAbility adds those for a living unit). Reads a monster no unit is yet: what a Ritual of Mastery makes (mastery 0x32b, an offset to the next form; montag 0x320), a school's level (ritualRitpow's ability), libmastery 0x32d. Read-only.
---| "commanderMoveFlags"    # index 0-9989, integer: the move flags of the commander's army as the AI's routes test them: its unit's move flags, widened (flying or float adds water 64), with each living unit following it (a bit stays only when every unit has it, noland and slow when any has). Either bit of 192 (water 64, float 128) lets the AI's conquest take a sea square; either bit of 129 (flying, float) lets an army into the Sky. Shows the army's items. Nil for a commander with no living unit. Read-only.
---| "commanderRitpow"       # index 0-9989, integer: the commander's ritual school, the first ritpow ability (501 up, as ritualRitpow names a school) its unit has; 0 for none. The AI ranks casters by its level (commanderRitualLevel): the pick-up of lone commanders, a split's new leader. Nil for a commander with no living unit. Read-only.
---| "commanderRitualLevel"  # index 0-9989, integer: the commander's level in that school (unitAbility of it), 0 for none; the AI's apprentice spam runs for level 1. Nil for a commander with no living unit. Read-only.
---| "commanderSpellLevel"   # index 0-9989, integer: the best level of the commander's spell attacks, with its magic and path boosts, any path, as the game's library and memorized spells reckon it; 0 for none. Shows the boosts of another player's items. Nil for a commander with no living unit. Read-only.
---| "terrainFlags"          # index: a terrain (as terrainName), integer: its 64 flag bits, the ones the AI reads: 1 a citadel, 2 burnable (the AI won't step a burnforest army onto it), 4 water (sea), 2^17 frozen water that thaws (the AI won't step onto it from other terrain in Winter or Spring with seasonTurnsLeft 2 or less), 2^21, 2^23 and 2^26 weigh in conquest's score (2^23 with a mine citadel class), 2^34 and 2^35 a library of level 1 and 2. Read-only.
---| "squareHasFort"         # index: x 0-999, then y 0-159, boolean: the square has a fort, as the game's AI buy asks it for an aihold monster. False off the world. Read-only.
---| "squareWaterlike"       # index: x 0-999, then y 0-159, boolean: the square counts as water where world generation and battles ask: sea terrain and the crossings (bridges, ports, fords, the Ferry, Frozen Sea, Coral Towers and the like), not lakes or rivers. True off the map. Read-only.
---| "squareOpenLand"        # index: x 0-999, then y 0-159, boolean: the square is open land, as world generation picks squares for its sites: not waterlike, not sea or flooded, and its terrain has no nostart. False off the map. Read-only.
---| "squareLibraryLevel"    # index: x 0-999, then y 0-159, integer: the square's library level as the AI's conquest scores it (100 a level): its terrain's library bits and the square owner's library enchantments there. 0 off the world. Read-only.
---| "seasonTurnsLeft"       # integer: the months left in this season, this one included (+seasonleft): 3 in its Early month, 2 in Mid, 1 in Late; in Summer, Autumn and Winter the weather may add one more month. Read-only.
---| "aiAttackDivOther"      # integer: the moving AI's attack divisor for an army of an independent or no one, which its turn sets before it recruits: 200 (175 while its team is over 4 times its enemies' strength), 50 less when desperate (fewer than 2 citadels after more than 2), 25 less on its attack turns (10%). Its conquest attacks a square whose remembered strength is at most comstr * 100 / divisor - 15 (aiAttackMargin), and its routes won't end on a stronger army (AI_MOVE_COMSTR). Read-only.
---| "aiAttackDivAi"         # integer: the same for an AI's army: 200, 125 while over 4 times, else 150 when 2 or more enemies and the team at most 2 times them. Read-only.
---| "aiAttackDivHuman"      # integer: the same for a human's army: 175, 125, or 150 as aiAttackDivAi. Read-only.
---| "aiAttackMargin"        # integer: 15, taken off after the division. The game's conquest and routes hold this 15 in their code, so it shows the margin and can't change it. Read-only.

---@class coe5.GameMenuLine
---@field label string cut to 63 characters (logged as api_cut)
---@field tooltip? string cut to 255 characters
---@field visible? fun(): boolean Shows the line while it returns true. In Lua, 0 counts as true: return a boolean.
---@field onClick fun(): integer? coe5.gameMenu.stay (or nothing, or false) keeps the menu open, coe5.gameMenu.close (or true) closes it, coe5.gameMenu.resign resigns. To end the game and start a new one, call coe5.restartGame() here instead of resigning.
---@field section? integer|string coe5.gameMenuSection.saving (the default), coe5.gameMenuSection.settings, or a section name, cut to 63 characters

---A line of the map editor's toolbox, for coe5.addMapEditorLine.
---@class coe5.MapEditorLine
---@field label string cut to 63 characters (logged as api_cut)
---@field tooltip? string cut to 255 characters
---@field visible? fun(): boolean Shows the line while it returns true, checked every frame. In Lua, 0 counts as true: return a boolean.
---@field onClick fun() runs when the line, or a key bound to its id, is clicked

---A line of the commander panel, for coe5.addCommanderLine.
---@class coe5.CommanderLine
---@field label string cut to 63 characters (logged as api_cut)
---@field tooltip? string the hover text, cut to 255 characters
---@field icon? integer a small icon (the panel's own: 0x75 Sentry to 0x85 Continue Moving), drawn left of the label as the game's lines have theirs; left out, none
---@field color? number[] the label's colour, {r, g, b} or {r, g, b, alpha}, each 0 to 1 (alpha 1 when left out); left out, the game's text colour (white). Any other count of parts, or a part outside 0 to 1, refuses the line (bad_color)
---@field visible? fun(): boolean Shows the line while it returns true, checked every frame with the commander selected (coe5.get("selectedCommander")). In Lua, 0 counts as true: return a boolean.
---@field onClick fun() runs when the line is clicked, with that commander still selected

---A mod's plane, for coe5.addPlane.
---@class coe5.Plane
---@field name string the plane's name, 1 to 31 characters, unlike every other plane's (a game plane's full or short name included)
---@field w? integer its width in squares, 1 to 999; leave w and h out with like "Sky", "Agartha", "Hades" or "Inferno" to take that plane's size in each world (Elysium's, the Inferno 4 more each way)
---@field h? integer its height in squares, 1 to 159
---@field icon? string its plane button's picture, a file in your mod's folder: "./plane.tga" or "./art/plane.png" (64 x 64 like the game's plane icons); without it, the icons of the plane it is `like`, else a coloured square
---@field fill? integer the terrain on every square before anything else (terrainName takes it); one past terrainLastIndex is laid as 0, logged. Needed without like and generate; 0 (Plain) when left out with them
---@field like? string build it with that game plane's recipe, CoE5SE's copy of the game's random map generator for it (planes/plane_recipes.lua): "Celestial", "Sky", "Agartha", "Inferno", "Hades", "Elemental" or "Void". A step the plane is too small for is skipped and logged (lua_refused reason=plane_too_small with its minimum); nothing is scaled down
---@field populate? string|false the game plane whose independents the game places on it ("Elysium" to "Void"), or false for none; like's plane by default, none without like. The game places independents by terrain number, so another plane's independents appear only on terrains both planes have
---@field swap? table<integer, integer> terrain = the terrain that replaces it, after the independents are placed (so they are placed by the first terrain); up to 64 pairs
---@field generate? fun(plane: coe5.PlaneBuild) runs at each world build after the recipe, with coe5.gen's tools and square writes open on the plane
---@field pieces? coe5.PlanePiece[] set pieces stamped after the recipe and generate: fixed squares first (center, x and y), then nearportal, then random, the largest first; never on another piece (plane_piece_skipped)
---@field zones? coe5.PlaneZone[] areas around the pieces with their own swap and monsters
---@field units? coe5.PlaneUnits[] the plane's own list: only these monsters are placed on it (outside the zones with a list or populate = false), never populate's

---A set piece of coe5.addPlane: a single-plane map file in your mod's folder, in the game's format.
---@class coe5.PlanePiece
---@field file string "./citadel.coem"; its mapsize is the piece's size, at most the plane's
---@field place? "center"|"centre"|"random"|"nearportal" the middle; count random spots on no portal, other piece or square kept for the gateway (100 draws each); or beside the plane's entrance
---@field count? integer copies for "random", 1 or more (1)
---@field x? integer with y and no place: its top left square, from the plane's top left (0, 0)
---@field y? integer

---A zone of coe5.addPlane: every square within radius of a copy of the piece `around`, by the game's square distance (the larger of the x and y distances). A square in two zones is the first one's.
---@class coe5.PlaneZone
---@field name string 1 to 31 bytes, unlike the plane's other zones
---@field around string a pieces entry's file
---@field radius integer 0 to 999 (0: the piece alone)
---@field swap? table<integer, integer> terrain = the terrain it becomes, in the zone, after the rare special terrain and before the independents; up to 32 pairs
---@field units? coe5.PlaneUnits[] only these monsters are placed in the zone
---@field populate? false no monsters in the zone; without it or units, the plane's own

---One monster of a unit list: {name, count, chance}. On each open square of its area (no army, no player's, no apcost 99 wall; water only for a monster with the water movement command, land only without noland) it rolls chance percent for count independents, doubled with Wilder on as the game's own dice are.
---@class coe5.PlaneUnits
---@field [1] string the monster's name, as the game names its own independents (a name several monsters share places one of them at random)
---@field [2] integer how many with Wilder off, 1 to 1000
---@field [3] integer the chance per square, 1 to 100

---A mod's independent faction, for coe5.addFaction. Each field is the newfaction block word of its name (factionlike for like, factionmonumentevent for monumentEvent), takes what the word takes, and is the preset's when left out.
---@class coe5.Faction
---@field name string 1 to 31 characters, unlike every other faction's and the vanilla factions' names
---@field like? "Independents"|"Special Monsters"|"Horrors"|"Hades"|"Inferno"|"Celestial"|"Empire"|"Kingdom" the vanilla faction whose rules it takes; Independents when left out
---@field color? number[] {r, g, b}, each 0 to 1; the preset's colour when left out
---@field home? string its home plane's name, a game plane's or a mod's
---@field active? "always"|"never"|integer when its turn spawns: always, never, or once that many portals are open (1 to 1000)
---@field populate? boolean whether its home's population is its own
---@field wander? "wander"|"indwander"|"infwander"|"hadeswander"|"celwander" the wander word its commanders take
---@field wanderGate? boolean
---@field tithe? boolean
---@field worldEvents? boolean
---@field conquest? "independent"|"normal" whether its squares are conquered as the Independents' are
---@field statue? boolean
---@field spawnSite? "owncitadel"|"monument"|"none" where its turn spawns
---@field spawnPlane? string with spawnSite "monument": the plane its monument stands on; Elysium when left out
---@field fuel? string the monster its spawns use up
---@field spawns? coe5.FactionSpawn[] its spawns, in order, replacing the preset's; {} or false for none
---@field monument? string|false its monument monster, false for none
---@field monumentRule? boolean whether its monument counts toward the game's rulership check, as the Monument of Hades does
---@field monumentEvent? coe5.FactionMonumentEvent|false its monument's founding event, false for none
---@field planeShift? coe5.FactionPlaneShift|false the units its monument shifts off its home, false for none
---@field loyalty? string|false the monster whose units on its monument's plane become its own, false for none
---@field gather? integer|false the ability number that makes its units carriers, 1 to 65535, false for none
---@field gateway? coe5.FactionGateway|false the gateway its carriers restore, false for none
---@field caravan? coe5.FactionCaravan|false its own caravan event, false for none
---@field horrors? boolean whether it takes a share of the Horrors' spawns
---@field avoid? coe5.FactionAvoid[]|false factions whose armies its own step onto only at a chance, replacing the preset's; false or {} for none
---@field units? coe5.FactionUnits[] its own units, placed at each world build after the game's population on free squares (nobody's or the Independents', no army, no wall); the squares become its own
---@field populateTerrain? coe5.FactionTerrain[] terrains whose Independents' squares and units become its own at each world build, before its units are placed

---One spawn of coe5.Faction: with at least min fuel units (and the terrain), a draw below chance makes group, using use of them.
---@class coe5.FactionSpawn
---@field min integer 0 to 10000
---@field use integer 0 to min
---@field chance integer 1 to 100
---@field group string the group, as sitepop names one
---@field terrain? "magma"|"ice"|"waste"

---@class coe5.FactionMonumentEvent
---@field turn integer the first month it may run, 0 to 100000
---@field chance integer in 1000 each month, 1 to 1000
---@field plane string the plane the monument is founded on
---@field terrain integer the terrain it is founded on, a number or a terrain group
---@field unfinished string the monster that grows into the monument
---@field news? string the news, with %s for the commander's name; none when left out

---@class coe5.FactionPlaneShift
---@field monster string
---@field percent integer 1 to 100
---@field each integer 1 to 100

---@class coe5.FactionGateway
---@field fuel integer the carriers' units it takes, 1 to 10000
---@field ruin integer the ruined gateway's terrain
---@field gate integer the restored gateway's terrain
---@field exit integer the terrain at the far end
---@field news? string the news, naming no one; none when left out

---@class coe5.FactionCaravan
---@field turn integer the first month it may run, 0 to 100000
---@field chance integer in 350 each month, 1 to 350
---@field plane string
---@field terrain integer a number or a terrain group
---@field group? string the caravan's group; the Merchants' crew when left out
---@field news? string the news, with %s for the commander's name; none when left out

---One monster of coe5.Faction's units: each free square of the terrain on the plane rolls chance percent for count of it, doubled with Wilder on as planeunits' are.
---@class coe5.FactionUnits
---@field monster string
---@field count integer 1 to 1000
---@field chance integer the chance per square, 1 to 100
---@field terrain? integer a terrain number or a terrain group; any terrain when left out
---@field plane? string a game plane's or a mod's plane's name; every plane when left out

---@class coe5.FactionTerrain
---@field terrain integer a terrain number or a terrain group
---@field plane? string every plane when left out

---@class coe5.FactionAvoid
---@field name string a mod's faction or a vanilla faction
---@field percent integer the chance to step onto its armies, 0 to 100

---The plane being built, as a recipe's steps and a generate function get it.
---@class coe5.PlaneBuild
---@field name string
---@field index integer its plane number in this world
---@field x integer its first column in world squares
---@field y integer its first row
---@field w integer
---@field h integer
---@field like? string the recipe's plane
---@field mod string the declaring mod
---@field elysiumW integer Elysium's width, for steps that read Elysium at the same offset
---@field elysiumH integer Elysium's height
---@field platform "windows"|"mac" the build, for a step whose random draws the two builds make in another order

---The map view, from coe5.mapView. UI units: x from the left, y up from the bottom.
---@class coe5.MapView
---@field zoom number UI units per square (8 to 100 in the map editor, 12 to 100 on the map)
---@field cameraX number the world x at the view's centre column, in squares
---@field cameraY number the y at the view's centre row, in squares (the centre shows row cameraY + 1)
---@field originX number the UI x of world square x = 0's left edge; square x's is originX + x * zoom
---@field originY number the UI y of row y = 0's top edge; row y's is originY - y * zoom
---@field screenW integer the UI's width
---@field screenH integer the UI's height
---@field plane integer the shown plane (planeName takes it)
---@field planeX integer the plane's first world x
---@field planeY integer the plane's first y
---@field planeW integer the plane's width in squares
---@field planeH integer the plane's height in squares
---@field firstX integer the first world x of the plane on screen
---@field lastX integer the last
---@field firstY integer the first y of the plane on screen
---@field lastY integer the last

---A recruitment offer, from coe5.shopOffers (a player's offers this turn) or coe5.classRecruits (a
---class's list). The fields marked shop or class come only from that one.
---@class coe5.Offer
---@field offer integer shop: the offer's number, for coe5.recruitSquares and coe5.aiBuyOffer
---@field monster? integer the monster (monsterName takes it); class: nil for a name no monster has
---@field name string the monster's name
---@field count integer units it buys; class: -1 for a commander (addcomrec)
---@field commander boolean the unit comes as a commander
---@field costs table<integer, integer> cost by type 0-18 (incomeName's order: 0 gold, 1 iron, ...)
---@field gold integer costs[0]; shop: rolled with the die when the offer came
---@field iron integer costs[1]
---@field goldDie? integer class: the open-ended die added to gold each time the offer comes
---@field chance? integer class: percent a month (before temples, libraries, and the AI level)
---@field flags integer the offer's flag bits
---@field oncePerOffer? boolean shop: bought once, then gone
---@field extra? boolean shop: a buy leaves the square's recruitment for the turn unused (mercenaries, commanders)
---@field aiNever? boolean shop: the game's AI never buys it, and coe5.aiBuyOffer refuses it
---@field terrain integer the terrain it is recruited on (recterr), -1 or 0 for any
---@field limiter? string shop: its "=" limiter ("=Soulless": it upgrades those units)
---@field limiters? string[] class: its reclimiter lines ("-Baron", "+Goblin Chieftain", "=Soulless")
---@field moves integer the monster's action points a turn: 4 fast, 2 slow, else 3
---@field spellLevel integer the highest level of the monster's spell attacks, 0 for none (a caster's measure)

---@class coe5
coe5 = {}

---Writes a lua_log line to coe5mod.log, while the player has CoE5SE's "Enable copious logs" on
---(Mod Settings, CoE5SE's page; off by default, so ask players to turn it on when you need
---their log). print works the same way. CoE5SE's own lines about your script (lua_error,
---lua_refused, lua_script_loaded) are always written. The text stays on that one line: a line break
---or other control character in it is written escaped (\n, \r, \t, \xNN).
---@param text string|number
function coe5.log(text) end

---Adds a main menu button above Quit, or after the game's button `afterId`: coe5.mainMenu.startNewGame,
---.loadGame, .network, .preferences, .mods, or .credits. Buttons several mods put after the same
---game button follow it in the order the mods added them. When the column has no room for every
---mod's button, its last row is More, which lists the rest.
---@param label string cut to 47 characters (logged as api_cut)
---@param onClick fun()
---@param afterId? integer
---@return boolean added false when `label` is empty (logged as api_refused) or memory ran out (lua_refused)
function coe5.addMainMenuButton(label, onClick, afterId) end

---Adds a line to the in-game Game Menu: at the end of its Saving & Exiting section, or with
---`section = coe5.gameMenuSection.settings`, at the end of its Settings section. A name, such as
---`section = "Cheats"`, puts the line in a section of that name after Saving & Exiting. Every mod
---that gives the same name (in any case) shares that section, headed with the first line's
---spelling. Sections come in the order their names were first added, and a section shows only
---while one of its lines does.
---@param line coe5.GameMenuLine
---@return boolean added false when `label` is empty, `section` is an empty name, or `section` is a number that is neither (logged as api_refused)
function coe5.addGameMenuLine(line) end

---Reads a game value. A per-slot value needs its slot (0-31, a mod's faction 32-47) or a faction's name, and a table value its index;
---slotResource needs the slot, then the resource (0-13), and a value with more indexes takes each
---in the order its note gives (squareSeen: the slot, x, then y).
---@param name coe5.Variable
---@param slot? integer|string slot or index; a faction's name for a slot (coe5.factionSlot's names)
---@param index? integer|string slotResource's resource, or a second index (unitVisibleTo's player, or a faction's name); for unitAbility and monsterAbility an ability's mod command, for monster, weapon, item, class, terrain and ritual a word of that .c5m block
---@param index3? integer a third index (squareSeen's y)
---@return any value an integer, boolean, or string, as the name's note says; nil for a frame value outside its site's wrap, an index past a list's end, or a name the game's table leaves empty
function coe5.get(name, slot, index, index3) end

---Changes a game value. A per-slot value needs its slot (0-31, a mod's faction 32-47) or a faction's name, and a table value its index;
---slotResource needs the slot, then the resource (0-13). A
---read-only value is an error. A boolean value takes only true or false: anything else, a number or
---nil, changes nothing and is refused (lua_refused reason=needs_boolean), because in Lua 0 is true.
---An integer the game itself never holds there (past a table the game indexes by it, or outside what
---the game stores, as the name's note says, or too big for the game's field) changes nothing and is
---refused (reason=out_of_range). A slot's colour takes what the name's note says; bannerFade takes any
---number, and one too big for a float is stored as an infinity. A square's terrain, portal, ground,
---temperature, and name are written only while the world is built (squareName's note says when);
---elsewhere the write is refused (reason=not_world_building, or outside_plane for a square outside
---the area being built).
---In a LAN game a content script on a client writes only what reaches the host: its player's resources,
---AI level and trade type, and its own units' action points, in its own turn. Another value (an event
---variable, a game setting, a square's corpses) the host never gets, and it is refused
---(reason=client_write_lost); another player's slot or unit is refused (not_own_player), and so is a
---write while the client waits for its turn (client_not_acting). Send the host what it should do
---through coe5.playerData instead. How the machine draws (hoveredId, bannerFade, fullscreen, ...)
---stays free.
---@param name coe5.Variable
---@param value integer|string|boolean
---@overload fun(name: coe5.Variable, slot: integer|string, value: integer|string|boolean)
---@overload fun(name: coe5.Variable, slot: integer|string, index: integer, value: integer|string)
function coe5.set(name, value) end

---Runs `fn` around a game call. `fn` gets `next` first, then the call's arguments; call `next` with
---them to go on to the game, and return what the call should return. Among the scripts' wraps, a
---higher `priority` runs first, outside the others, whatever order the mods load in; without one it
---is 0. Among the same priority, the wrap added last runs first. Every DLL's wrap runs outside every
---script's, whatever either's priority: a DLL sees each call before any script and gets back what
---the scripts returned. The log's wrap_order lines list each site's wrappers in the order they run, with
---"plugins:" before the DLLs' and "inner:" before the scripts'. Call `next` only while
---`fn` runs, and not from a coroutine; either is an error. A wrap that returns without calling
---`next` keeps the game's own call from running; the first time it does, the log names it
---(lua_wrap_no_next). An observe-only site (coe5.ObservedSite) is an error here: use coe5.observe.
---@param site coe5.Site
---@param fn fun(next: function, ...): ... the site's result (the AI sites: score, x, y)
---@param priority? integer higher runs first among the scripts' wraps; 0 when left out
---@return boolean wrapped false when the site isn't on this platform yet (logged as api_refused or lua_refused)
function coe5.wrap(site, fn, priority) end

---Watches a game call a script may not change. `before` gets a copy of the call's arguments right
---before it, and `after` the call's result, then the arguments, right after it; either may be nil,
---not both. What they return doesn't reach the game. Every DLL's observers of the site run before
---any script's, in both `before` and `after`. A site that isn't observe-only is an error.
---@param site coe5.ObservedSite
---@param before? fun(...)
---@param after? fun(result: any, ...)
---@return boolean added false when the site isn't on this platform yet (logged as api_refused)
function coe5.observe(site, before, after) end

---The turn: 0 on the first turn, -1 while a game is set up. The same as coe5.get("turn").
---@return integer
function coe5.turn() end

---This process's part in the game: coe5.net.single, coe5.net.host, or coe5.net.client.
---@return integer
function coe5.netRole() end

---The player this machine is playing now: on a LAN client the player whose turn is running, nil while it
---waits for a turn; in single player and hotseat the seat playing now; nil on a host.
---@return integer?
function coe5.localPlayer() end

---Your mod's table that lasts: kept in every save, and in a LAN game sent from the host to every client
---with each turn. Your Lua globals last only until the next load or LAN turn; put what must last here.
---Plain values only: booleans, numbers, strings, and tables of them with string, number or boolean
---keys. A function, a userdata, a table with a metatable, or a table that holds itself is left out when
---the game is saved (lua_refused, call=coe5.gameData, with at= its place). A load empties this table and
---fills it again in place, so keep coe5.gameData itself, never a table inside it, across a load. It is
---empty in a new game and in a save made before it existed: check for nil. A write while your files
---load (their top level) is undone and refused (lua_refused, reason=during_load): they run again at every
---load and LAN turn. Set first values in GAME_START or a later site. In a LAN game only the
---host's changes last; a client's are gone at its next turn (logged once a turn as game_data_not_sent):
---a client writes coe5.playerData instead.
---@type table
coe5.gameData = {}

---A table per player slot (0-47) that lasts as coe5.gameData does, empty until written. In a LAN game a
---client's turn file carries its playing player's table to the host, which takes it for that slot alone.
---That is how a client asks for what the game doesn't send (a square, an event variable, a message):
---write the request here in the client's turn, and act on it in a site the host runs, such as
---PLAYER_TURN_START or MONTH_START. Another slot's table written on a client stays on that client.
---@type table<integer, table>
coe5.playerData = {}

---Adds a message to a player's Messages window. Only human players get messages.
---@param player integer|string a player number (0-31, a mod's faction 32-47) or a faction's name (coe5.factionSlot's names), or coe5.message.everyHuman
---@param title string cut to 99 characters
---@param text string cut to 2047 characters, the game's buffer for it
---@return integer index the message's index, or coe5.message.notAdded when no message was added. With coe5.message.everyHuman it is always coe5.message.notAdded, even when the message was sent. A bad slot or a LAN client is logged (api_refused or lua_refused).
function coe5.sendMessage(player, title, text) end

---Shows the game's message box at once, with your title and text and one Ok button (Enter clicks
---it), and returns when the player closes it. Nothing is added to what you give. Use it when a
---human is at the screen, such as in an onPortalCrossing handler for a human's Enter Gateway or
---step. The box doesn't wrap: end each line with \n. The game fixes up the text's English, as on
---every box it shows: the word after a 1 and a space loses a last s that follows a
---small letter other than s or u ("1 gems" is drawn "1 gem", "21 gems" "21 gem", "option1 is"
---"option1 i"; "1 glass" stays).
---@param title string cut to 99 characters
---@param text string cut to 4999 characters
---@return boolean shown true once the player closed it; false when refused, with nothing shown: in a LAN game where no one can answer (lan_host, content_on_client, wait_screen), during the AI's or the independents' turn or inside an AI site (ai_turn), and in an onPortalCrossing handler for a route being planned (planning_route), each logged as lua_refused
function coe5.showMessageBox(title, text) end

---Reads one of your mod's settings. Every mod declares its settings with newsetting blocks in
---<ModName>.c5se next to its .c5m, and nowhere else (the format is in src/jazzy_api.h, "Mod
---Settings starter"); a script only reads them. Names match ignoring case. The value is the
---player's current pick in Mod Settings, read each time you call it; a declared setting that breaks
---a rule gives its default. In a LAN game a content mod reads the host's pick, a personal mod its player's. A setting you read only once, when the script
---loads, takes effect after a restart: give it a restart line, and changing it offers one.
---@param name string the setting's name, as its newsetting line gives it
---@return boolean|integer|nil value on/off: true or false; a choice: the picked option's number from 0; a key: its key value (coe5.key.none for none); nil when your mod declares no such setting (logged as lua_refused)
function coe5.setting(name) end

---Tells other mods that your script is filling in a new game's setup itself: while it is on, mods
---that change the setup screens leave them alone. Turn it on right
---before your code starts a new game's setup, and off once that setup returns. Every mod shares
---the one flag, so always turn it off again. A call that changes it logs setup_automated with your
---mod's name (by=<ModName>.lua).
---@param on boolean
function coe5.setSetupAutomated(on) end

---Whether a mod has said it is filling in a new game's setup itself (coe5.setSetupAutomated). A mod
---that changes the setup screens leaves them alone while it is true.
---@return boolean
function coe5.setupAutomated() end

---Hides the game's main menu button `id` on every frame where `hidden()` returns true. Only Read Tip,
---coe5.mainMenu.readTip, can be hidden; the buttons below it move up. Any number of mods may ask: the
---button is hidden while any of them says so.
---@param id integer coe5.mainMenu.readTip
---@param hidden fun(): boolean runs every main menu frame; keep it short
---@return boolean added false for any other button (logged as lua_refused)
function coe5.hideMainMenuButton(id, hidden) end

---Adds a line to your mod's page in Mod Settings, in a box titled "Screens" after your mod's
---settings. A click runs `open`, your own screen (a coe5.ui loop), then shows the page again. The
---tooltip comes last because it is optional; the C API's addScreen takes it second.
---@param label string 1 to 56 characters
---@param open fun()
---@param tooltip? string up to 255 characters
---@return boolean added false when the label or tooltip breaks a rule (the page lists it; logged as api_refused)
function coe5.addScreen(label, open, tooltip) end

---Sets key action `action`'s key (coe5.keyAction) to `key` (coe5.key, or a character's byte) on its
---screen, in the game's texts, and in the Keyboard Shortcuts list, in place of the game's key.
---coe5.key.none leaves the action with no key. The key lasts until the game closes or the next call;
---nothing is saved, so keep players' picks in [key] settings. Every mod shares one key per action and
---the last call wins: each change logs key_rebound with mod=, and key_rebound_over naming both mods
---when another mod set that key last.
---@param action integer
---@param key integer
---@return boolean changed false for an unknown action, a value that isn't a key, or on a LAN client a mod that isn't personal (not_personal_on_client; logged as api_refused)
function coe5.rebindKey(action, key) end

---Gives key action `action` a second key beside its own, which runs it too; the texts name both,
---"Esc/F10". coe5.key.none removes it.
---@param action integer
---@param key integer
---@return boolean changed false for an unknown action, a value that isn't a key, or on a LAN client a mod that isn't personal (not_personal_on_client; logged as api_refused)
function coe5.addActionKey(action, key) end

---Key action `action`'s key now.
---@param action integer
---@return integer|nil key nil for an unknown action (logged as api_refused), where the C API returns -1
function coe5.actionKey(action) end

---The index of the first monster in the game's table with this name, ignoring case: a class
---leader's with coe5.get("classLeader", class). Its sprite is coe5.get("monsterSprite", index).
---@param name string
---@return integer|nil index nil when no monster has this name
function coe5.findMonster(name) end

---Makes one unit of `monster` for `player` on square (x, y), following no commander, the way the
---game's recruit purchase makes a troop: the game's newunit, then what the game does after it (the
---unit's land or water shape for the square, the square's army recount and conquest check, the
---player's sight around it). The game checks no terrain, owner, or sight here, and neither does
---CoE5SE: your script picks the square. A unique monster that already has a living unit moves that
---unit here, as the game's own spawns do. For the variant the recruit screen would make, pass
---coe5.monsterVariant(monster).
---@param player integer|string a player number (0-31, a mod's faction 32-47) or a faction's name (coe5.factionSlot's names)
---@param monster integer a monster's index (coe5.findMonster)
---@param x integer below mapDepth
---@param y integer below mapH
---@return integer|nil unit the unit's index, or nil: outside a game (reason=no_game), a player past the slots (bad_player), a monster index no monster uses (bad_monster), a square outside the world (off_map), or no free unit record (no_free_slot), each logged as lua_refused
function coe5.spawnUnit(player, monster, x, y) end

---Makes one commander of `monster` for `player` on square (x, y), with its unit and a name the game
---gives it, the way the recruit purchase makes a commander, then does what coe5.spawnUnit does after.
---@param player integer|string a player number (0-31, a mod's faction 32-47) or a faction's name (coe5.factionSlot's names)
---@param monster integer a monster's index
---@param x integer below mapDepth
---@param y integer below mapH
---@return integer|nil commander the commander's index, or nil with the reasons coe5.spawnUnit gives (no_free_slot also when no commander record is free)
function coe5.spawnCommander(player, monster, x, y) end

---What the event word newunits does: the units of a summon string on square (x, y), as rituals and events
---write it ("c*Captain & 2d6*Spearman": a commander, then 2d6 units following it), made for `player`
---by the game's own reader. A monster name the game doesn't know is skipped, as in a ritual. As
---newunits, the player's sight isn't updated.
---@param player integer|string a player number (0-31, a mod's faction 32-47) or a faction's name (coe5.factionSlot's names)
---@param x integer below mapDepth
---@param y integer below mapH
---@param monsters string a summon string, up to 1999 characters
---@return integer|nil made the units made, or nil: outside a game (reason=no_game), a player past the slots (bad_player), a square outside the world (off_map), or a string too long or holding a 0 byte (bad_string), each logged as lua_refused
function coe5.newUnits(player, x, y, monsters) end

---Moves a commander and every living unit following him on his square to (x, y), any plane, with
---the teleport ritual's own call. It runs a walked step's checks: onto another player's army or
---independents at war it overruns weak defenders or sets up the battle, fought after the move with
---the others, and a hidden army there ambushes; a free square is conquered and its surroundings seen.
---A free square costs no action points; a square with an army at war spends them all, as walking in
---does. The commander's stored move order is cleared. Like the game's teleport, it checks nothing of
---the terrain: sea, a cave wall, or the Void take the army as any square does. coe5.mayEnterSquare
---says whether the army could walk there.
---@param commander integer 0-9989
---@param x integer
---@param y integer
---@return boolean teleported false when refused (lua_refused): outside a game (reason=no_game), a commander past the table (bad_commander), one with no living unit (dead_commander), a square outside the world (off_map) or on no plane (no_plane)
function coe5.teleportCommander(commander, x, y) end

---Whether the commander's army may walk onto (x, y), by the game's own test for a move: the army's
---move flags against the square's terrain (sea, walls, the Void, mountains for a mountain-only army),
---whatever stands there now.
---@param commander integer 0-9989
---@param x integer
---@param y integer
---@return boolean may false also when refused, as coe5.teleportCommander refuses (lua_refused)
function coe5.mayEnterSquare(commander, x, y) end

---The monster the recruit purchase makes for an offer of `monster`: the game's own pick, `monster` or
---one of the up to 9 records right after it with the same name, at random (a class's "Swordsman"
---with three looks).
---@param monster integer a monster's index
---@return integer|nil variant nil for a monster index no monster uses (logged as lua_refused, reason=bad_monster)
function coe5.monsterVariant(monster) end

---Where the portal on square (x, y) leads: the squares the game sends an army through it to. An
---entrance is a square whose squarePortal is above 0, and its exits are the squares with that
---portal number or its negative, the entrance left out, as the game finds them: the first 100 by
---x, then y. The game picks one of them at random each time, so the list is where it can lead.
---@param x integer
---@param y integer
---@return {x: integer, y: integer}[] exits empty for a square with no entrance or outside the world
function coe5.portalExits(x, y) end

---Calls `handler` before every crossing through a portal, so a mod can refuse it: a human's Enter
---Gateway (asked before the exit is drawn or seen), each step a human's route, a standing order, an
---AI commander or a wandering monster takes through a portal, an independent army's random exit,
---and every route the game plans, the AI's, a human's shown route and coe5.route's (with
---`planning` true). A handler that returns false refuses that one crossing, and the game goes on as
---for a portal it won't take itself: Enter Gateway does nothing (no exit is revealed, no action
---points are spent), a step is refused (the AI drops that move), a route goes another way or none,
---an independent army stays. Anything else, nil included, leaves the crossing to the game. Your mod
---decides everything else: which portals, one way or both, who may pass, and any message
---(coe5.showMessageBox, coe5.sendMessage, coe5.ui). Any number of mods may add handlers; each is asked in the order they
---were added, until one refuses. No handler means no call at all. On a LAN client none is asked, and on the host only a content mod's.
---A handler runs inside the game's own move or route, so keep it quick: route planning asks many
---times a turn (a commander's every route that reaches the portal). coe5.route inside a handler
---returns nil (lua_refused reason=inside_portal_handler), and crossings a handler causes aren't asked.
---A handler that errors stops your script, and its crossings go through.
---@param handler fun(crossing: {x: integer, y: integer, toX: integer?, toY: integer?, portal: integer, player: integer?, commander: integer?, planning: boolean}): boolean?
---  x, y: the square the army leaves. toX, toY: where it arrives; nil for Enter Gateway, asked before
---  the game draws one of the portal's exits. portal: the leaving square's squarePortal, above 0.
---  player: the army's player (nil for none). commander: the moving commander (nil for an
---  independent army without one). planning: true for a route being planned, false for a move.
---@return boolean added false when refused: a game build CoE5SE wasn't made for (site_unavailable), each logged as lua_refused
function coe5.onPortalCrossing(handler) end

---The living units on square (x, y), the units the game counts there (squareStrength, its square
---checks): hp above 0 and standing on the square, in unit order. A square's army is what stands on
---it; unitCommander tells which follow which commander. Sees through fog, as squareStrength does.
---@param x integer
---@param y integer
---@return integer[] units unit indexes (unitHp, unitMonster, unitStrength take them); empty for no living unit there or a square outside the arrays
function coe5.squareUnits(x, y) end

---The game's Reveal Map, the Game Over screen's button: every square of every plane in the world,
---the mods' planes included (Elysium2's copy aside), or of one plane, in view for `player`
---(squareSeen 2, a scried 3 included), then
---the game's sight update, which redraws the map for a human. Sight only rises within a turn, and
---the game lowers squares in view to seen before (1) at the player's next turn start, so they stay
---in view until then: terrain stays on the map after, armies show only where the player sees them.
---@param player integer|string a player number (0-31, a mod's faction 32-47) or a faction's name (coe5.factionSlot's names)
---@param plane? integer a plane's index (coe5.planeIndex), 0 to the last plane; left out, every plane
---@return boolean revealed false when refused: outside a game (reason=no_game), a player past the slots (bad_player), a plane past the last (bad_plane), each logged as lua_refused
function coe5.revealMap(player, plane) end

---Every army on the world map: each square of the world (x below mapDepth, y below mapH) with an
---army (squareArmy not 0), by x, then y. With viewer, only the squares that player sees now
---(squareSeen 2 or 3), which the game's AI refreshes its strength memory from; without it, every
---army, through fog. A viewer outside 0-31 is refused (lua_refused reason=bad_player) and the list
---is empty.
---@param viewer? integer a player slot 0-31
---@return {x: integer, y: integer, owner: integer, units: integer, commanders: integer[]}[] armies owner as squareArmyOwner, units as squareArmy (-1 while two players at war share the square), commanders the commanders whose unit stands there alive, in commander order, followers of another commander included (unitCommander tells)
function coe5.armies(viewer) end

---How far a commander's army has to go to reach square (x, y), by the game's own pathfinder, the
---route the AI's moves take: from the commander's square, through portals too, with the whole map
---known (like the AI's routes, it ignores what the player has seen). comstr is the strength the
---route's end may face: a square held by an army stronger than it allows is closed. Left out, it is
---the commander's own, the comstr the AI sites get. Only inside a wrap of an AI move site
---(AI_EXPLORE); anywhere else the call is refused (lua_refused reason=outside_ai_site) and returns
---nil. A route draws nothing from the game's random sequence that the game's later draws see.
---Each call, refused or not, counts 10,000 steps of the script's 20,000,000-step budget before the
---game routes, so about 2,000 routes fit in one call into the script (reason=step_limit past it).
---@param commander integer 0-9989
---@param x integer below mapDepth
---@param y integer below mapH
---@param comstr? integer -1 to 21474836; -1 routes as a player's own move order does
---@return integer|nil length the route's cost in moves, about 1 a square on open ground, 0 on the commander's square; nil for no route, a commander with no living unit on a square, or a refused call (reason=outside_ai_site, bad_commander, off_map, out_of_range)
function coe5.route(commander, x, y, comstr) end

---Drops an AI commander's planned ritual (commanderRitualPlan) the way the game's own "scrapping
---plan" does: the plan becomes 0, and the game's ritual step plans again when it next runs for the
---commander (in 80% of its runs), with each candidate passing AI_RITUAL_CANDIDATE. Only 0 is
---written: a script can't pick the plan. Only inside an AI site: a wrap of an AI move site, of
---AI_RITUAL_CANDIDATE, or of an army site (AI_GATHER_TROOPS, AI_PICK_UP_LONER, AI_SPLIT_ARMY), or
---an observer of AI_TURN (before the player's moves, so its commanders plan again that turn). Refused (lua_refused) and false: outside them (reason=outside_ai_site), a
---commander past the table (bad_commander), one with no living unit (no_unit), one whose player
---isn't an AI (not_ai), and a spawnbooster's plan, -1 (not_ritual). Each scrap logs
---ai_plan_scrapped with the commander, its player, and the ritual. Works on both builds where an AI
---site runs (Mac: AI_TURN).
---@param commander integer 0-9989
---@return boolean ok true when the commander has no plan now (scrapped, or it had none)
function coe5.scrapRitualPlan(commander) end

---Frees one unit of an AI player from the commander it follows (unitCommander becomes -1), as the
---game's own unfollowtarg ritual effect does, and resets that commander's cached action points, since
---its army changed. The unit stays on its square, a free unit of its player, which the AI's troop
---pick-up can take again. Only inside an AI site, as coe5.scrapRitualPlan, and only for a unit of the
---player whose AI turn runs (coe5.get("activePlayer")). Refused (lua_refused) and false: outside an AI
---site (reason=outside_ai_site), on a LAN client (network_game), a unit past the table (bad_unit),
---one dead or of no player (no_unit), one whose player isn't an AI (not_ai), one of another player
---(not_moving_player). Each release logs ai_follower_released with the
---unit, its old commander, and its player.
---@param unit integer 0-98999
---@return boolean ok true when the unit follows no one now (released, or it followed no one)
function coe5.releaseFollower(unit) end

---Moves one unit of an AI player from the commander it follows to another commander of the same
---player on the same square, the game's way: the army screen's own transfer, with its rules (no
---stationary unit, none that moves on its own, none led by a wandering commander, no insane
---commander's own unit) and both
---commanders' cached action points reset. Only inside an AI site, for a unit of the player whose AI
---turn runs, as coe5.releaseFollower. Refused (lua_refused) and false: the reasons of
---coe5.releaseFollower, then a commander past the table (bad_commander), one with no living unit
---(no_commander), one of another player (other_player), the commander's own unit (same_unit), a unit
---that follows no one (not_following), one that follows the commander already (already_following),
---the two on different squares (not_same_square), and one the game's transfer refuses
---(game_refused).
---Each transfer logs ai_follower_transferred.
---@param unit integer 0-98999
---@param commander integer 0-9989
---@return boolean ok true when the unit follows the commander now
function coe5.transferFollower(unit, commander) end

---A player's recruitment offers this turn, those with units to buy, in the shop's order: what
---Recruit lists, the other citadels' terrain-bound offers included. A magic item for sale and an
---offer already bought (once per offer) are left out. Refused (lua_refused reason=bad_player, an
---empty list) for a player outside 0-31.
---@param player integer|string a player number (0-31, a mod's faction 32-47) or a faction's name (coe5.factionSlot's names)
---@return coe5.Offer[] offers
function coe5.shopOffers(player) end

---A class's recruitment list (addunitrec, addmercrec, addcomrec, in its order): every offer the
---class can get, before the month's chance rolls. The standard troops of stdtroops aren't in it
---(classStandardTroops). Refused (lua_refused reason=bad_class, an empty list) for a class outside
---0-249 (0-199 on Mac).
---@param class integer slotClass's number
---@return coe5.Offer[] offers
function coe5.classRecruits(class) end

---One of the 61 offers the game's monthly shop build adds from its own code (coe5.builtinOffers):
---the standard troops, the mercenary bands, the Hero, the hired wizards, the Assassin, the guild
---offers, the ship guilds' offers and a magic item for sale. Its chance comes from
---coe5.builtinOfferChance. The fields a row takes (coe5.addShopOffer) have the same names.
---@class coe5.BuiltinOffer
---@field id integer its number from 1, in the build's order, for builtinOfferChance and replaceBuiltinOffer
---@field name string the monster ("Magic item" for the item for sale)
---@field count integer units it offers; -1 the magic item
---@field item boolean the magic item for sale: a random one each time
---@field commander boolean the unit comes as a commander, with a name
---@field gold integer base gold
---@field goldDie integer a die added to gold each time the offer comes
---@field goldDieExplodes boolean the die rolls again on its top face and adds the new roll less 1 (false only for the Captain)
---@field iron integer
---@field costs table<integer, integer> extra costs by cost type (the Void Ship: [15] = 100 gems)
---@field terrain integer where it is bought: 0 anywhere, a terrain group below -1 (-93 ports, -106 the sky plane)
---@field oncePerOffer boolean bought once, then gone
---@field extra boolean a buy leaves the square's recruitment for the turn unused
---@field aiImportant boolean the game's AI tries it first
---@field aiNever boolean the game's AI never buys it
---@field message boolean the month's Recruitment Offer message comes with it
---@field outOf integer what the chance is out of: 100, 150 (the Hero), 500 or 1000
---@field price string its price rule: "plain" (gold + die), "mercenary" (times mercpricemult), "hero" (5 less per Adventurers or Beastmasters guild, at most 5), "assassin", "shadow_stalker", or "item" (the item's extracost + 20 + two dice)

---The 61 offers the shop build adds from its own code, in its order (coe5.BuiltinOffer). The class
---lists and the special locations' offers come from records instead (coe5.classRecruits).
---@return coe5.BuiltinOffer[] offers
function coe5.builtinOffers() end

---Built-in offer `id`'s chance this month for `player`, worked out as the game does (guilds, library
---levels, town points, the class's recwizchance, recherochance, recasschance and mercboost), and
---what it is out of. Offers the game adds every month once their need is met (the standard troops,
---the Ship, the Galleon) give outOf when it is met and -1 when it isn't. The AI difficulty adds
---nothing to these. Refused (lua_refused reason=bad_id or bad_player, nil).
---@param id integer 1-61
---@param player integer|string a player number (0-31, a mod's faction 32-47) or a faction's name (coe5.factionSlot's names)
---@return integer? chance
---@return integer? outOf
function coe5.builtinOfferChance(id, player) end

---The player's guilds made by the guild ritual `guild` ("Apothecaries Guild", or a mod's guild
---ritual), counted as the shop counts them: on squares the player or a teammate holds, or with
---`ownSquares` only the player's own (the mercenary bands' and the Hero's count). A guild in a town an
---enemy took doesn't count, and an upgraded guild counts under its upgrade's name. Refused
---(lua_refused reason=bad_player or not_a_guild, nil).
---@param player integer|string a player number (0-31, a mod's faction 32-47) or a faction's name (coe5.factionSlot's names)
---@param guild string
---@param ownSquares? boolean
---@return integer? count
function coe5.guildCount(player, guild, ownSquares) end

---A monthly offer's fields, for coe5.addShopOffer and coe5.replaceBuiltinOffer. Every field is
---optional for replaceBuiltinOffer, which starts from the built-in offer's own.
---@class coe5.ShopRow
---@field name? string the monster (addShopOffer needs it)
---@field count? integer units, 1-1000 (1)
---@field commander? boolean the unit comes as a commander, with a name
---@field gold? integer base gold (0); setting gold or goldDie on a replaced offer drops its price rule
---@field goldDie? integer a die added to gold (0)
---@field goldDieExplodes? boolean (true)
---@field iron? integer
---@field costs? table<integer, integer> one extra cost: {[type] = amount}, type 2-18
---@field terrain? integer where it is bought: 0 anywhere (0), a terrain number, or a group below -1
---@field oncePerOffer? boolean
---@field extra? boolean
---@field aiImportant? boolean
---@field aiNever? boolean
---@field limiter? string a "=" line: the offer upgrades those units ("=Militia")
---@field chance? integer|fun(player: integer): integer each month's chance (0); a replaced offer keeps its own rule without one
---@field outOf? integer what chance is out of (100)
---@field message? boolean the Recruitment Offer message when it comes (true)

---Adds an offer every player's shop rolls at each turn start, after the game's own: with `chance` in
---`outOf`, its gold die rolled, through the game's own add and message calls. Any number of rows, from
---any number of mods; a row of a script that stopped no longer rolls. On a LAN client no row rolls, and on the host a content mod's do. Refused
---(lua_refused, false): reason=no_name, no_monster, bad_field, bad_costs, out_of_memory,
---site_unavailable.
---@param row coe5.ShopRow
---@return integer|false row its number
function coe5.addShopOffer(row) end

---Turns built-in offer `id` off and adds a row in its place, built from the offer's own fields, chance
---rule and price rule, with each field of `row` replacing that part. The game still rolls the offer,
---so no other offer's dice change; the offer it made is emptied and its line taken out of the month's Recruitment Offer message, and the row
---rolls after the game's offers. It counts toward the 50 rows. Two scripts replacing one offer get a
---row each. Refused (lua_refused, false): reason=bad_id and those of addShopOffer.
---@param id integer 1-61
---@param row? coe5.ShopRow
---@return integer|false row its number
function coe5.replaceBuiltinOffer(id, row) end

---The squares where the game's AI recruiting can buy for the player: its own citadels (squares
---whose terrain is a citadel for its class), by y then x, at most 100, each {x, y, used}, used true
---when the square's recruitment is used this turn. With an offer (coe5.Offer.offer), only the
---squares where coe5.aiBuyOffer would buy it now. Refused (lua_refused, an empty list):
---reason=bad_player, bad_offer (past the player's last offer), windows_only on Mac.
---@param player integer|string a player number (0-31, a mod's faction 32-47) or a faction's name (coe5.factionSlot's names)
---@param offer? integer
---@return {x: integer, y: integer, used: boolean}[] squares
function coe5.recruitSquares(player, offer) end

---Buys one of an AI player's offers on square (x, y) the way the game's own recruiting step does:
---the same purchase (the units, or a commander, made there and every cost paid), then an offer that
---isn't extra uses the square's recruitment for the turn and a once-per-offer offer is gone. Only
---inside a wrap of AI_RECRUIT, for the player recruiting. The game's own square rules hold: the
---offer must be buyable there (its terrain, costs, limiter), the square one of the player's
---recruit squares, unused this turn, and the monster's .c5m AI words met (aipowcom1-3, aimaxshop,
---aihold, a ship by the sea). Refused (lua_refused) and false: reason=outside_ai_recruit,
---bad_player, other_player, not_ai, bad_offer, off_map, not_recruit_square, item, sold_out,
---not_for_ai, cannot_pay, too_few_to_upgrade, not_here, square_used, aipowcom, aimaxshop, no_sea,
---aihold; windows_only on Mac; the second result is that reason word. False alone: a commander
---offer the game's buy couldn't make (no free commander, or no unit to upgrade), with the square
---and the offer used all the same, as the game's own step leaves them. Each buy logs
---ai_offer_bought with the gold before and after.
---@param player integer|string a player number (0-31, a mod's faction 32-47) or a faction's name (coe5.factionSlot's names), the player of the AI_RECRUIT call
---@param offer integer coe5.Offer.offer
---@param x integer
---@param y integer
---@return boolean bought
---@return string? reason the lua_refused reason when refused
function coe5.aiBuyOffer(player, offer, x, y) end

---The buys and ritual plans scripts made for `player` this turn, in order, each with the mod that
---made it: a call "aiBuyOffer" (each buy) with offer, monster, count, x, y and commander (the new
---commander; nil for troops), and "planRitual" or "scrapRitualPlan" with commander and ritual. The
---list starts again with the first call of a new turn; it holds 64 a turn, and the second result
---counts the ones past that. It only reads: no call changes. Empty for a player with no call this
---turn.
---@param player integer|string
---@return {mod: string, call: string, offer?: integer, monster?: integer, count?: integer, x?: integer, y?: integer, commander?: integer, ritual?: integer}[] actions
---@return integer cut
function coe5.aiActions(player) end
-- On a played AI's Recruit screen: inside a RECRUIT_WAIT wrap, when the screen is the active
-- player's and that player is human, coe5.aiBuyOffer also buys for that player, at the screen's
-- selected square only (else not_screen_square), through the game's AI buy, with the Recruit
-- screen's own checks in place of the AI's rules (square_used, not_enough_gold, not_enough_iron,
-- not_enough_units, cannot_pay, wrong_plane, no_upgrade_units, sold_out, not_here, not_possible,
-- item, too_few_to_upgrade). Windows only.

---The game's AI moves for `player` in its own turn, from where the player left its commanders: the
---way Play as AI hands a played AI back to the game. Only in a HUMAN_TURN `after` observer for that
---player's turn. The player is an AI while the game's ai_move runs, then gets back its controller.
---Nothing of a turn's start runs again: no income, no action points back, no upkeep.
---Refused (lua_refused) and false: reason=network_game, outside_human_turn_end, not_this_turn,
---resigning.
---@param player integer the player whose human turn just ended
---@return boolean ran
function coe5.runAiTurn(player) end

---The Recruit screen, inside a RECRUIT_WAIT wrap: its player, its selected square (x and y -1 with
---none), whether a ctrl key is down (a ctrl click toggles an offer's auto-recruit), and its click
---latch (1 once a frame had no mouse button; the screen acts on a click only then). nil, refused
---(lua_refused), outside a RECRUIT_WAIT wrap (outside_recruit_wait), on Mac (windows_only), or
---when the screen can't be read (no_frame).
---@return {player: integer, x: integer, y: integer, ctrl: boolean, latch: integer}|nil screen
function coe5.recruitScreen() end

---Inside a wrap, after its next(): the other scripts' wraps of the same site, inside that next(),
---that changed what their own next() gave them, innermost first. `answer` is true when a wrap
---returned something else than its next() returned (or skipped next()); `arguments` when it passed
---other arguments to its next(). A wrap that passed everything on unchanged isn't listed. It only
---reads: no wrap's arguments or answer change. Wrap at 2000000000 to see every script's change;
---a plugin DLL's wrap isn't listed. An empty list (lua_refused) outside a wrap (outside_wrap) or
---before next() (next_not_called). On Mac the AI steps Mac's game copied into their callers list no
---changes, for now.
---@return {mod: string, priority: integer, answer: boolean, arguments: boolean}[] changes
function coe5.changedBy() end

---Centres the map view on world square (x, y), on the plane that holds it, as the game centres the
---map on a commander or a message's square: the view switches plane as a plane button would, then
---the camera moves (a glide on the same plane). Only the view changes: nothing a game turn decides
---reads the camera or the view plane. Refused (lua_refused) and false: reason=no_game, off_map (x
---or y outside the world: mapW and mapH), no_plane (a square on no plane), windows_only (Mac,
---for now).
---@param x integer a world square's x (planes side by side, as mapPointerX)
---@param y integer
---@return boolean centred
function coe5.centerMap(x, y) end

---Builds a preview world: every plane from a seed, the whole world in view for a viewing seat, and
---the map on `plane` at the first turn, before anyone moves. The same seed and the same mods give
---the same squares and units, and a preview world can't be played, saved or continued, so this is
---for a tool that shows a modder a plane as a player gets it. Only from the main menu: a
---main menu button's function, or a MAIN_MENU_WAIT wrap below priority 0. The world starts when that
---script code is done. Windows only for now: on Mac the function doesn't exist, so test for it with
---rawget(coe5, "startPreviewWorld"). Nothing it does reaches a real game; other mods' Game Menu lines stay
---hidden in the world, and only this script's show.
---@param world {plane: string, seed: integer, mapSize: integer, society: integer, wilder?: boolean, startPositions?: integer, uniqueRandom?: boolean, seats: integer[]} `plane`: a plane's name, a game plane or an enabled mod's. `mapSize`: coe5.mapSize. `society`: as the game value `society`, 0 Random; the seed rolls Random. `startPositions`: coe5.startPositions, clustered when left out. `uniqueRandom`: Setup Participants' "unique random players". `seats`: the players' class numbers, 1 to 24 of them, 0 for a Random class the seed rolls. The first is a human in slot 0, with a class, home and units as in a real game; the rest are AIs
---@return boolean started false when refused (lua_refused): not_in_main_menu, network_game, already_active, bad_plane, bad_seed, bad_map_size, bad_society, bad_start_positions, bad_seats, bad_seat_class, hooks_not_installed
function coe5.startPreviewWorld(world) end

---Ends the preview world at once, with no AI turn, and goes back to the main menu. The game re-reads
---every mod's files as a world ends, so a file saved before this call is what the next world builds.
---From a Game Menu line's onClick or a map frame.
---@return boolean ended false when refused (lua_refused): no_preview_world, already_ending, not_at_first_turn
function coe5.endPreviewWorld() end

---Whether a preview world is running, and how long the last one took to build.
---@return boolean active
---@return number? seconds from coe5.startPreviewWorld to the map's first frame, nil before the first world of this launch
function coe5.previewWorldActive() end

---Tells CoE5SE where a tool's panels sit on a preview world's map. While the pointer is over one, the
---wheel doesn't zoom the map, a click doesn't reach the map (read it with mouseButtons after next in a
---MAP_SCREEN_WAIT wrap), and the game's hover box shows coe5.setPreviewHoverText's text or nothing. Keys
---still work. A left press on the map keeps MAP_DRAW running, so panels stay on screen while the button is
---down; mouseButtons after next reads the press, and the game's left clicks act as in the vanilla game. An
---empty list clears them, and so does the world's end. Nothing here reaches a real game.
---@param docks {x: number, y: number, w: number, h: number}[] rectangles in UI units, y up from the bottom, as coe5.ui.drawRect takes
---@return boolean set false when refused (lua_refused): no_preview_world, too_many_docks, bad_dock
function coe5.setPreviewDocks(docks) end

---What the game's hover box says while the pointer is over a coe5.setPreviewDocks rectangle.
---@param text? string nil or left out for no box
---@return boolean set false when refused (lua_refused): no_preview_world
function coe5.setPreviewHoverText(text) end

---Whether a tool's own text field takes the keyboard on a preview world's map. While on, no key press
---reaches the game: no shortcut, no key action, no held key moving the view. coe5.takePreviewFieldKeys
---reads them. alt+Tab, alt+Enter and alt+F4 still reach the window, and a screen opened over the map gets
---its own keys. The world's end turns it off. Nothing here reaches a real game.
---@param on boolean
---@return boolean set false when refused (lua_refused): no_preview_world
function coe5.setPreviewKeyField(on) end

---The keys a coe5.setPreviewKeyField field took since the last call, oldest first: a string for typed text
---(ASCII and the game's 16 accented letters, as the game's own text boxes take), or coe5.key.backspace,
---coe5.key.enter (keypad Enter too) or coe5.key.esc. Backspace repeats while held; Enter and Esc come once.
---@return (string|integer)[]? keys nil when refused (lua_refused): no_preview_world
function coe5.takePreviewFieldKeys() end

---What each line of a plane's newplane block did in this preview world, in file order. A game plane has no block, so an empty list.
---state is off (an edit switched it off), refused (the parser), failed (the build couldn't), partly, overridden, nothing, rolledNo, worked, or notMeasured (a word with no counter yet, never worked). edited: an edit ran in its place or added it; fileText: the file's line then.
---did and of are copies placed of copies asked, rolls won of rolls, or squares changed. reason is the refusal's or the build's word, by the earlier or later line it is refused or overridden because of, blames the lines placed before a piece that found no room.
---armies, squares and rects are what it holds now, in world squares: from the unit table, the terrain record, a piece's rectangles.
---@param plane string the plane's name
---@return {line: integer, file: string, mod: string, word: string, text: string, state: string, plane?: string, off?: boolean, edited?: boolean, fileText?: string, added?: boolean, row?: any, zone?: string, reason?: string, by?: integer, from?: integer, to?: integer, did: integer, of: integer, placed: integer, units: integer, w?: integer, h?: integer, blames?: integer[], order?: integer, orderOf?: integer, copies?: table[], close?: string, replaces?: string, sameNameTerrain?: integer, sameNameLeft?: integer, armies: table[], squares: table[], rects?: table[]}[]? rows nil when refused (lua_refused): no_preview_world
function coe5.previewLineResults(plane) end

---Each mod event of this preview world's passes and what it did: `passes` it was reached in, `ran` passes an action line ran in, `stopped` passes a trigger stopped it in with the last such line's `stopLine` and `stopText`, and `planes`, the indexes of the planes its targets stood on. Refused outside a preview world (lua_refused reason=no_preview_world). Windows only for now.
---actions and turns are its action lines and turn triggers, one per line; named is the plane indexes its +plane and randloc lines name; held is the triggers that held in the pass that ran or got furthest; changed is the plane indexes whose units or terrain it changed. stopLine is the furthest line any pass stopped at.
---With a plane index, each row also holds what it did there: did, one row per action line that changed something (units made or changed, left of those still standing as it left them, later of those a later line changed or that moved off the plane, killed, squares changed, squaresLeft still holding its terrain), and armies and squares, the world squares it changed there. Compared with the unit table and every plane's terrain after each action line, so it holds whatever the line's word.
---@param plane? integer a plane's index
---@return {mod: string, file: string, line: integer, text: string, actions: string, turns: string, held: string, named: integer[], passes: integer, ran: integer, stopped: integer, stopLine?: integer, stopText?: string, planes: integer[], changed: integer[], did?: {line: integer, text: string, units: integer, left: integer, later: integer, killed: integer, squares: integer, squaresLeft: integer}[], armies?: {x: integer, y: integer, units: integer}[], squares?: {x: integer, y: integer}[]}[]|nil
function coe5.previewEventResults(plane) end

---What scripts did in this preview world, one row per place, so a loop's writes are one row with their `count`. `kind` is call (a coe5 call or written value that changes the world), refused (`reason` says why), error (`text` is the message), or wrap (the wrap of site `call`, at `line`, changed an answer). Refused outside a preview world (lua_refused reason=no_preview_world). Windows only for now.
---@return {mod: string, file: string, line: integer, call: string, count: integer, kind: string, reason?: string, text?: string}[]|nil
function coe5.previewScriptResults() end

---What the pass at this preview world's end and the next world's lines take in place of a file's lines; nothing is written to a file. Replaces the list; an empty table empties it. A world that doesn't start at the menu's next frame empties it, and a real game's pass never reads one.
---An edit names its line by mod, block and text, and which of the equal lines of the block (occurrence, from 1), never by number. A line changed in the file finds no edit, so the file wins.
---off drops the line, changed runs in its place, added with changed runs after it. Results then say off and edited, with fileText.
---@param edits {mod: string, plane: string, text: string, occurrence?: integer, off?: boolean, changed?: string, added?: boolean}[]
---@return boolean set false when refused (lua_refused): no_preview_world, bad_edit, out_of_memory
function coe5.setPreviewLineEdits(edits) end

---Every line of every plane's block in this preview world, of any mod, which mod each new monster came from, by name, and which each new terrain came from, by its selectterr number.
---skipped is the words the last pass didn't know, scripts the mods with a script, changedScripts each script file changed since it loaded, which runs only after a relaunch.
---@return {lines: {plane: string, mod: string, file: string, line: integer, word: string, text: string, state: string, from?: integer, to?: integer}[], monsters: table<string, string>, terrains: table<integer, string>, skipped: {mod: string, file: string, line: integer, word: string, close?: string}[], scripts: string[], changedScripts: {mod: string, file: string}[]}|nil index nil when refused (lua_refused): no_preview_world
function coe5.previewLineIndex() end

---What placed the units on world square (x, y) in this preview world: one row per kind of unit and source. A row names its line, or the game step that made them (step: world, population with planeIndex, month, event, script, gateway). An event's action line names itself: eventLine, its event's first line, and actionLine, with text, file and mod; step event with no eventLine is what no line was caught doing.
---placedAs and changes say what event lines did to them since: the monster they were placed as, and each change first to last, with from, to and the line's fields.
---@param x integer
---@param y integer
---@return {monster: integer, units: integer, line?: integer, file?: string, mod?: string, word?: string, text?: string, plane?: string, step?: string, planeIndex?: integer, eventLine?: integer, actionLine?: integer, scriptLine?: integer, placedAs?: integer, changes?: {from: integer, to: integer, eventLine: integer, actionLine: integer, file: string, mod: string, text: string}[]}[]? rows nil when refused (lua_refused): no_preview_world, bad_square
function coe5.previewArmySource(x, y) end

---Every write of world square (x, y)'s terrain in this preview world, first to last, each with the terrain it left and its line or game step (as coe5.previewArmySource's rows).
---@param x integer
---@param y integer
---@return {terrain: integer, line?: integer, file?: string, mod?: string, word?: string, text?: string, plane?: string, step?: string, planeIndex?: integer, eventLine?: integer, actionLine?: integer}[]? rows nil when refused (lua_refused): no_preview_world, bad_square
function coe5.previewSquareSource(x, y) end

---Every plane the enabled mods declare, in load order. A plane that failed stays in the list with its
---reason: the word its refusal logged, a newplane block's or a coe5.addPlane call's, or no_room when the last world had no room for it.
---@return {name: string, mod: string, file: string, line: integer, lines: integer, w: integer, h: integer, failed?: string}[] planes `file` and `line`: where the plane's declaration starts. `lines`: how many plane lines it took. `w` and `h`: its size as declared, 0 for a plane refused
function coe5.declaredPlanes() end

---Shows the army on world square (x, y) in the game's own army view, the screen a right click on an
---army opens: its battlefield, both sides deployed. It waits there until the player leaves it, then
---returns. The game's random sequence is put back after, so a view changes nothing a later turn
---draws. Refused (lua_refused) and false: reason=no_game, off_map (outside the world: mapW and
---mapH), no_army (no army on the square), windows_only (Mac, for now). Logs lua_army_viewed.
---@param x integer a world square's x (planes side by side, as mapPointerX)
---@param y integer
---@return boolean shown
function coe5.viewArmy(x, y) end

---Shows `player`'s sight on the human `seat`: the map then shows the seat what that player sees,
---fog and all, and the game shows the seat that player's moves. The seat's own sight is kept the
---first time, and coe5.restoreSight puts it back. Nothing else changes: the player's own sight and
---every other player's stay as they are. At the seat's next turn start the game ages the seat's
---sight as always, so the player's "in view now" squares become "seen before" ones. Refused
---(lua_refused) and false: reason=network_game, no_game, bad_player (outside 0-31), same_player,
---seat_not_human, out_of_memory; windows_only on Mac, for now.
---@param seat integer a human player
---@param player integer the player whose sight to show
---@return boolean shown
function coe5.showSight(seat, player) end

---Puts back the sight coe5.showSight kept for the seat, as it was at that first call. Refused
---(lua_refused) and false: reason=bad_player, no_game, not_shown (nothing kept in this game; a new
---or loaded game drops it), windows_only on Mac, for now.
---@param seat integer
---@return boolean restored
function coe5.restoreSight(seat) end

---Gives `seat` the game's own Battle Report message for every battle an AI faction fights that the
---seat isn't in (a battle of independents or Horrors alone gives none), as if it saw the square: "You have observed a battle in ...", with the jump to the square, the replay and the
---stats, on its Messages screen at its next turn start. The seat gets no sight: what it and every
---other player see stays as it is. The game sends the reports only while its Battle reports option
---is on (coe5.get("battleReports")), and only while the seat is human. nil or -1 turns it off; a
---new or loaded game starts with none. Refused (lua_refused) and false: reason=network_game,
---bad_player (outside 0-31), not_installed; windows_only on Mac, for now.
---@param seat integer? a player 0-31, or nil or -1 for none
---@return boolean set
function coe5.reportBattlesTo(seat) end

---Adds a line to the map editor's toolbox, right under "Create Fancy Terrain". The editor's Tab
---(Hide Toolbox) hides it with the rest. CoE5SE gives the line an id the editor itself never uses
---(44 to 999, in the order lines are added); a click on the line runs onClick, and so does a key
---bound to the id with coe5.ui.bindKey in a MAP_EDITOR_WAIT wrap, toolbox hidden or not.
---@param line coe5.MapEditorLine
---@return integer|false id the line's id, or false when `label` is empty, all 956 ids are taken, or the site isn't on this build (logged as api_refused)
function coe5.addMapEditorLine(line) end

---Adds a line at the end of the selected commander's orders on the map (the column with Sentry,
---Transfer Units and Use Special Power), drawn as the game draws its own: the icon, the label beside
---it, the hover text. The column's frame grows to hold it. The map draws the column only while a
---commander is selected. Past 16 shown lines of all mods (fewer when the screen's bottom comes first),
---the mods' lines go on pages: a small box under the column says "Page 1 of 2" and has Previous and
---Next; the game's own lines show on every page, and selecting another commander goes back to the
---first. A click on the line runs onClick and nothing else: the map doesn't take the click.
---@param line coe5.CommanderLine
---@return integer|false id the line's id (2000 to 9997, in the order lines are added), or false when `label` is empty, `icon` is below 0 or past the sprite numbers (bad_icon), `color` isn't 3 or 4 parts from 0 to 1 (bad_color), all 7998 ids are taken, or the site isn't on this build (logged as api_refused or lua_refused)
function coe5.addCommanderLine(line) end

---The map view now (the map screen's or the map editor's), in UI units: x from the left, y up from
---the bottom, screenW x screenH. World square (x, y) is `zoom` units wide, its top left corner at
---(originX + x * zoom, originY - y * zoom). firstX to lastX and firstY to lastY are the squares of
---the shown plane at least partly on the screen (first is above last when none is). A map file
---numbers a square x - planeX, y - planeY.
---@return coe5.MapView|nil view nil when the shown plane isn't one of the game's
function coe5.mapView() end

---Gives an AI commander with no plan (commanderRitualPlan 0) the plan of casting one of the rituals
---it knows (commanderRitual), as the game's own planner writes a candidate it keeps. The game's
---ritual step then treats it as any plan it holds: casts it when the caster can, keeps it while it
---can't cast it right now or can't pay, and scraps it as the game scraps a plan ("cannot cast",
---"pointless", "aibad"). The game's own reasons for drawing a candidate (airare, aialways, the
---income checks) don't apply, as they don't to a plan held from an earlier turn. Set from an
---AI_TURN observer, the commander's ritual step sees it that same turn. Only inside an AI site, as
---coe5.scrapRitualPlan. Refused (lua_refused) and false: outside them (reason=outside_ai_site), a
---commander past the table (bad_commander), one with no living unit (no_unit), one whose player isn't
---an AI (not_ai), a commander with a plan already, a spawnbooster's -1 included (has_plan: scrap it
---first), and a ritual the commander doesn't know (not_known). Each plan logs ai_plan_set with the
---commander, its player, and the ritual.
---@param commander integer 0-9989
---@param ritual integer a ritual the commander knows
---@return boolean ok true when the plan is set
function coe5.planRitual(commander, ritual) end

---The AI's "aibad" test of a ritual for a commander, as its ritual plan choice drops a candidate: the
---commander too strong for an aiweakonly ritual, the player's casts at aimaxcast, or an immobile caster off
---the ritual's terrain.
---@param commander integer 0-9989, with a living unit
---@param ritual integer a ritual (1 up)
---@return boolean|nil bad true when the AI drops the ritual, nil when refused (lua_refused: no_game, dead_commander, bad_ritual)
function coe5.aiRitualBad(commander, ritual) end

---The AI's "bad time" test of a ritual for a commander at the caster's square or a target square: water or
---the Sky for a caster that can't stand there, the ritual's aiwhere, ainothere, ainotclose1-3 and
---ainotnearhome, and another team's enchantment there.
---@param commander integer 0-9989, with a living unit
---@param ritual integer a ritual (1 up)
---@param x? integer a target square's x, or -1 (the default) for the caster's square
---@param y? integer its y, or -1
---@return boolean|nil bad true when the AI waits, nil when refused (lua_refused: no_game, dead_commander, bad_ritual, off_map)
function coe5.aiRitualBadTime(commander, ritual, x, y) end

---How much the AI values an item for a unit: its item redistribution hands each item on a square to the
---unit there that values it most.
---@param unit integer a living unit
---@param item integer an item (itemName's index)
---@return integer|nil value nil when refused (lua_refused: no_game, dead_unit, bad_item)
function coe5.aiItemValue(unit, item) end

---The AI's rating of an item for use by a unit: 0 not usable, 1 kept, above 1 used when its commanders
---use items.
---@param unit integer a living unit
---@param item integer an item (itemName's index)
---@return integer|nil rating nil when refused (lua_refused: no_game, dead_unit, bad_item)
function coe5.aiItemUseRating(unit, item) end

---The square at the same spot on another plane, as a planeshift finds it: the same offset, wrapped to that plane's size and kept off its edges when that plane is smaller. Nil for a square outside every plane.
---@param plane integer a plane 0-10
---@param x integer integer
---@param y integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.planeSameSpot(plane, x, y) end

---Whether two planes count as one place for the game's wanderers and for reforming immortals: the same plane, or both among Elysium, the Sky and Agartha.
---@param planeA integer a plane 0-10
---@param planeB integer a plane 0-10
---@return boolean|nil result nil when refused (lua_refused)
function coe5.planesAlike(planeA, planeB) end

---An ability's total over every living unit on a plane, their items, blessings and temporary abilities counted. Hades counts its monuments this way.
---@param plane integer a plane 0-10, -1 for any, -2 for Elysium, the Sky and Agartha
---@param ability integer|string an ability's word or id
---@return integer|nil result nil when refused (lua_refused)
function coe5.planeAbilitySum(plane, ability) end

---The portal entrances on a plane whose portal number a square of otherPlane holds too. The Inferno counts its ways out this way.
---@param plane integer a plane 0-10, -1 for any, -2 for Elysium, the Sky and Agartha
---@param otherPlane integer a plane 0-10, -1 for any
---@return integer|nil result nil when refused (lua_refused)
function coe5.portalsLinkedTo(plane, otherPlane) end

---The portal entrances a player owns on a plane. portal 0 counts every portal number, another counts only that one.
---@param player integer a player slot, -1 for any
---@param plane integer a plane 0-10, -1 for any
---@param portal integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.countPlayerPortals(player, plane, portal) end

---The squares of a terrain on a plane, as +minterrains counts them.
---@param terrain integer a terrain
---@param plane integer a plane 0-10, -1 for any, -2 for Elysium, the Sky and Agartha
---@return integer|nil result nil when refused (lua_refused)
function coe5.countTerrainOnPlane(terrain, plane) end

---The next plane in the order the top bar shows them, as the < and > keys step: direction 1 forward, -1 back.
---@param plane integer a plane 0-10
---@param direction integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.planeOrderStep(plane, direction) end

---Moves a unit to the same spot on another plane, as the game's planeshift does. A noSight other than 0 leaves sight as it is.
---Windows only for now.
---@param unit integer a living unit
---@param plane integer a plane 0-10
---@param noSight integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.planeShiftUnit(unit, plane, noSight) end

---Links two squares with a portal number no other portal uses, as startplane's home portal and the Strange Portals do. twoWay 0 makes the second square an exit only.
---@param x1 integer a square's x
---@param y1 integer a square's y
---@param x2 integer a square's x
---@param y2 integer a square's y
---@param twoWay integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.newPortalPair(x1, y1, x2, y2, twoWay) end

---Shows a player every square of a portal number, and returns how many.
---@param player integer a player slot
---@param portal integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.revealPortalExits(player, portal) end

---Sends a monster up from a plane, as Hades sends its Dispossessed Spirits: on each square that rolls under percent, each of the monster's units, by string, has an n percent chance to planeshift to Elysium, and the game places as many again where they land.
---Windows only for now.
---@param plane integer a plane 0-10
---@param percent integer integer
---@param n integer integer
---@param monster string a monster by string
---@return boolean|nil result nil when refused (lua_refused)
function coe5.planeRiseToElysium(plane, percent, n, monster) end

---Moves the map to a plane, as its top bar button does.
---@param plane integer a plane 0-10
---@return boolean|nil result nil when refused (lua_refused)
function coe5.viewPlane(plane) end

---The plane a unit stands on.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitPlane(unit) end

---A unit's fire resistance as a hit reads it: fire immunity counts 100, its fireres is added, and state bit 12 of unitStateFlags takes 50 off.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitFireResistance(unit) end

---A unit's cold resistance as a hit reads it: cold immunity counts 100, and its coldres is added.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitColdResistance(unit) end

---A unit's shock resistance as a hit reads it: shock immunity counts 100, and its shockres is added.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitShockResistance(unit) end

---A unit's poison resistance as a hit reads it: poison immunity counts 100, and its poisonres is added.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitPoisonResistance(unit) end

---A unit's acidres total with its items, as a hit reads it.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitAcidResistance(unit) end

---A unit's morale as a battle reads it, with experience and items; a mindless unit reads 99.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitMorale(unit) end

---Where a unit deploys in battle: 1 the front, 0 the middle, -1 the back, its items and afflictions counted.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitBattleRank(unit) end

---2 for battleslow2, which moves every fourth round, 1 for battleslow, which moves on odd rounds, else 0.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitBattleSlowness(unit) end

---Whether a unit is fast in battle: battlefast, fast, or Quickness.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitBattleFast(unit) end

---Whether a unit can't act: asleep, paralysed, petrified, webbed and the like.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitIncapacitated(unit) end

---A unit's move speed by its move flags: 4 fast, 3 normal, 2 slow.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitSpeedClass(unit) end

---2 for a unit with flying and float, 1 for flying, 0 for neither.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitFlightType(unit) end

---The first name of coe5.unitFlightType, which still works.
---@deprecated
function coe5.unitFlightKind(unit) end

---Whether a unit's move flags, its items' counted, let it move on water.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitHasWaterMove(unit) end

---Whether a unit is noland, its items counted.
---Windows only for now.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitNoLand(unit) end

---Whether a unit's monster has morale 99, the game's mark of a mindless one.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitMindless(unit) end

---The percent of its Hit Points a unit regenerates each turn.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitRegenPercent(unit) end

---How many afflictions a unit has.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitAfflictionCount(unit) end

---Whether a unit's attacks hold a spell.
---Windows only for now.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitHasSpellAttack(unit) end

---Whether a unit counts as undead, as noundead and the game's hits test it.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitIsUndead(unit) end

---Whether a unit counts as a demon, as nodemon and the game's hits test it.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitIsDemon(unit) end

---An ability's sum over a unit, its 8 items, blessings and temporary abilities. add is the game's own switch; it passes 0 or 1.
---Windows only for now.
---@param unit integer a living unit
---@param ability integer|string an ability's word or id
---@param add integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitAbilitySum(unit, ability, add) end

---Whether a monster has a ranged weapon or a spell attack.
---Windows only for now.
---@param monster integer a monster
---@return boolean|nil result nil when refused (lua_refused)
function coe5.monsterHasRanged(monster) end

---A monster's nametype, the list of names its commanders draw from.
---@param monster integer a monster
---@return integer|nil result nil when refused (lua_refused)
function coe5.monsterNameType(monster) end

---The living units following a commander.
---@param commander integer a commander with a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.commanderLedCount(commander) end

---The action points a commander has left this turn.
---@param commander integer a commander with a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.commanderApLeft(commander) end

---Whether a commander may take an order now, as the AI's turn tests it.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.commanderMayAct(commander) end

---The largest burnforest in a commander's army.
---Windows only for now.
---@param commander integer a commander with a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.armyBurnValue(commander) end

---An ability's total over a commander and the units following it. across and sources are the game's own switches for what is counted.
---@param commander integer a commander with a living unit
---@param ability integer|string an ability's word or id
---@param across integer integer
---@param sources integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.armyAbilityTotal(commander, ability, across, sources) end

---An item's value of an ability, 0 for none.
---Windows only for now.
---@param item integer an item
---@param ability integer|string an ability's word or id
---@return integer|nil result nil when refused (lua_refused)
function coe5.itemAbility(item, ability) end

---The value of an item's nth copy of an ability, from 0.
---@param item integer an item
---@param ability integer|string an ability's word or id
---@param n integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.itemAbilityNth(item, ability, n) end

---A player's commanders; a freeOnly other than 0 counts only those following no one.
---Windows only for now.
---@param player integer a player slot
---@param freeOnly integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.countFreeCommanders(player, freeOnly) end

---Whether a player sees a living unit on a square.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.squareHasSeenUnit(player, x, y) end

---The strength of a team's living units on a square, or everyone's with -1. A fort other than 0 adds 10 per ranged unit and the square's fort bonuses.
---@param player integer a player slot, -1 for any
---@param x integer a square's x
---@param y integer a square's y
---@param fort integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.squareSideStrength(player, x, y, fort) end

---What a square costs a commander to enter, 1000 to an action point and 99999 for refused. flag is the game's own switch; it passes 0 for a plain route.
---@param commander integer a commander with a living unit
---@param x integer a square's x
---@param y integer a square's y
---@param flag integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.squareMoveCost(commander, x, y, flag) end

---A weapon's range for a unit, its spellrange and rangebonus counted.
---@param unit integer a living unit
---@param attackType integer integer
---@param weapon integer a weapon
---@return integer|nil result nil when refused (lua_refused)
function coe5.weaponRangeFor(unit, attackType, weapon) end

---Whether a square is a loner's first preference, as its wandering steps read it.
---@param unit integer a living unit
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitLovesSquare(unit, x, y) end

---Whether a square is a loner's second preference.
---@param unit integer a living unit
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitLikesSquare(unit, x, y) end

---Whether a loner refuses a square; a square off the map counts.
---@param unit integer a living unit
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitHatesSquare(unit, x, y) end

---Whether a wandering commander stands on a square.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.wandererAt(x, y) end

---Moves one unit alone to a square, as the game's teleportloc does.
---Windows only for now.
---@param unit integer a living unit
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.teleportUnit(unit, x, y) end

---Gives a unit experience; a mindless unit gets none.
---Windows only for now.
---@param unit integer a living unit
---@param xp integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitAddXp(unit, xp) end

---Empties the slot holding an item, as the AI hands items on. -1 when the unit doesn't hold it.
---Windows only for now.
---@param unit integer a living unit
---@param item integer an item
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitRemoveItem(unit, item) end

---Takes Hit Points off a unit with none of the checks and death effects of a hit, as a meteor's aftermath does.
---Windows only for now.
---@param unit integer a living unit
---@param damage integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.damageUnitPlain(unit, damage) end

---Makes each living unit a player has on a square hold, and returns how many.
---Windows only for now.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.squareHold(player, x, y) end

---Spends the action points of a player's units on a square, or everyone's with -1.
---@param player integer a player slot, -1 for any
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.spendAllApAt(player, x, y) end

---Uses each item of a commander's unit the AI rates worth using.
---Windows only for now.
---@param commander integer a commander with a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.commanderUseItems(commander) end

---Kills a unit with 999 damage, twice, as an overrun does.
---Windows only for now.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.killUnit(unit) end

---Gives a unit an item by string, as the event word newitem does.
---@param unit integer a living unit
---@param item string a string
---@return boolean|nil result nil when refused (lua_refused)
function coe5.giveItem(unit, item) end

---Takes an item from a unit by string, as the event word removeitem does.
---@param unit integer a living unit
---@param item string a string
---@return boolean|nil result nil when refused (lua_refused)
function coe5.takeItem(unit, item) end

---At a percent chance gives a unit a random item, as the event words randitem (type -1) and randmisc (type 7) do.
---@param unit integer a living unit
---@param type integer the item type, as the manual's Magic Item Types
---@param percent integer integer
---@param level integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.giveRandomItem(unit, type, percent, level) end

---A ritual's first value of an effect, 0 for none, as the AI reads its ai words.
---Windows only for now.
---@param ritual integer a ritual
---@param ability integer|string an ability's word or id
---@return integer|nil result nil when refused (lua_refused)
function coe5.ritualEffectFirst(ritual, ability) end

---A ritual's first value of an effect, or your default without one.
---Windows only for now.
---@param ritual integer a ritual
---@param ability integer|string an ability's word or id
---@param default integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.ritualEffectOr(ritual, ability, default) end

---Whether the AI calls a ritual pointless for a commander: the corpses and resources it needs aren't on the caster's square.
---@param commander integer a commander with a living unit
---@param ritual integer a ritual
---@return boolean|nil result nil when refused (lua_refused)
function coe5.ritualPointless(commander, ritual) end

---Whether a square the commander's player has seen would take the ritual's terrain.
---@param commander integer a commander with a living unit
---@param ritual integer a ritual
---@return boolean|nil result nil when refused (lua_refused)
function coe5.ritualCastableAnywhere(commander, ritual) end

---A player's blessing value of an ability, 0 for none.
---@param player integer a player slot
---@param ability integer|string an ability's word or id
---@return integer|nil result nil when refused (lua_refused)
function coe5.slotBlessingOf(player, ability) end

---What a spell attack of a magic path weighs in a unit's strength.
---@param path integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.magicPathWeight(path) end

---The first name of coe5.magicPathWeight, which still works.
---@deprecated
function coe5.spellPathWeight(path) end

---A unit's boost to a magic path: its magic boost and its path boosts.
---@param unit integer a living unit
---@param path integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitPathBonus(unit, path) end

---The highest level of a unit's spell attacks of a magic path, or of any with -1.
---@param unit integer a living unit
---@param path integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitSpellWeaponLevel(unit, path) end

---How many spells of a magic path a unit may memorize.
---@param unit integer a living unit
---@param path integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.memorizeAllowance(unit, path) end

---The spell if the unit has it memorized, else -1.
---@param unit integer a living unit
---@param spell integer a spell
---@return integer|nil result nil when refused (lua_refused)
function coe5.memorizedSlot(unit, spell) end

---A random spell of a magic path and level, -1 for none. path and level -1 for any; flags are the game's own.
---@param path integer integer
---@param level integer integer
---@param flags integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.randomSpell(path, level, flags) end

---A random spell a unit doesn't know yet, from up to 1000 draws.
---@param unit integer a living unit
---@param path integer integer
---@param level integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.randomSpellNotKnown(unit, path, level) end

---The spells a unit knows of a magic path and level, -1 for any.
---@param unit integer a living unit
---@param path integer integer
---@param level integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.countKnownSpells(unit, path, level) end

---The spells a unit has memorized of a magic path, -1 for any.
---@param unit integer a living unit
---@param path integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.countMemorizedSpells(unit, path) end

---The memorized spells of a magic path whose weapon has a type, -1 for any.
---@param unit integer a living unit
---@param path integer integer
---@param weaponType integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.countMemorizedOfType(unit, path, weaponType) end

---The spells the game holds of a magic path and level.
---@param path integer integer
---@param level integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.countSpells(path, level) end

---The magic path of a monster's first spell attack, -1 for none; items and boosts don't count.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitFirstMagicPath(unit) end

---The first name of coe5.unitFirstMagicPath, which still works.
---@deprecated
function coe5.unitFirstSpellPath(unit) end

---A random memorized spell of a magic path, -1 for none.
---@param unit integer a living unit
---@param path integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.randomMemorizedSpell(unit, path) end

---The spell of a unit's first item with combatspell 1, -1 for none.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.itemCastableSpell(unit) end

---Whether the game would call a spell worth casting for a unit now.
---@param unit integer a living unit
---@param spell integer a spell
---@return boolean|nil result nil when refused (lua_refused)
function coe5.spellUseful(unit, spell) end

---Whether a monster attack row's type is one of the five spell weapon types.
---@param attackType integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.isSpellAttack(attackType) end

---A spell by string, -1 for none.
---@param name string a string
---@return integer|nil result nil when refused (lua_refused)
function coe5.findSpell(name) end

---The nth ritual of a name, from 0, -1 for none.
---@param name string a string
---@param n integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.findRitual(name, n) end

---A random ritual of a school and level.
---@param ritpow integer integer
---@param level integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.randomRitual(ritpow, level) end

---Whether a commander may learn a ritual by the ritual's requirements.
---@param commander integer a commander with a living unit
---@param ritual integer a ritual
---@return boolean|nil result nil when refused (lua_refused)
function coe5.ritualMayLearn(commander, ritual) end

---Whether a ritual is in a commander's known list.
---@param commander integer a commander with a living unit
---@param ritual integer a ritual
---@return boolean|nil result nil when refused (lua_refused)
function coe5.commanderKnowsRitual(commander, ritual) end

---Memorizes a spell for a unit, as the game's stats screen does.
---@param unit integer a living unit
---@param spell integer a spell
---@return integer|nil result nil when refused (lua_refused)
function coe5.memorizeSpell(unit, spell) end

---Takes a spell out of a unit's memorized list.
---Windows only for now.
---@param unit integer a living unit
---@param spell integer a spell
---@return integer|nil result nil when refused (lua_refused)
function coe5.unmemorizeSpell(unit, spell) end

---Fills a unit's battle spells the way the game picks them.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.chooseMemorized(unit) end

---Clears a unit's memorized spells, then picks them again.
---Windows only for now.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.repickMemorized(unit) end

---Teaches a unit a random spell it doesn't know of a magic path and level, then picks its battle spells.
---@param unit integer a living unit
---@param path integer integer
---@param level integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.learnRandomSpell(unit, path, level) end

---Teaches a unit the spells its magic paths start it with.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.learnStartSpells(unit) end

---Puts a ritual in a commander's known list at the first free place.
---Windows only for now.
---@param commander integer a commander with a living unit
---@param ritual integer a ritual
---@return integer|nil result nil when refused (lua_refused)
function coe5.commanderLearnRitual(commander, ritual) end

---Teaches a commander every free or starting ritual of a school at exactly that level.
---@param commander integer a commander with a living unit
---@param ritpow integer integer
---@param level integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.learnLevelRituals(commander, ritpow, level) end

---Teaches a commander the rituals its schools start it with, and returns how many.
---On Mac true, since its function there counts nothing.
---@param commander integer a commander with a living unit
---@return integer|boolean|nil result nil when refused (lua_refused)
function coe5.getStartRituals(commander) end

---Ends an active ritual effect by caster, type and square, the game's only way to free one.
---@param caster integer integer
---@param type integer the effect's type
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.removeEnchant(caster, type, x, y) end

---Adds a random blessing effect to a player, as Bolster Devotion does.
---@param player integer a player slot
---@param kind integer integer
---@param show integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.bolsterBlessing(player, kind, show) end

---Adds to a player's blessing of an ability, up to cap.
---@param player integer a player slot
---@param ability integer|string an ability's word or id
---@param add integer integer
---@param cap integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.addPlayerBlessing(player, ability, add, cap) end

---Removes every unit on a square, as the event word killsquare does.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.killSquare(x, y) end

---A square's name as messages give it to a player.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return string|nil result nil when refused (lua_refused)
function coe5.squareShownName(player, x, y) end

---A square's income of a resource for a player, in hundredths.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@param resource integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.squareIncome(player, x, y, resource) end

---A square's gold for a player, in hundredths.
---Windows only for now.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.squareGoldIncome(player, x, y) end

---The lifeforce a draining square gives.
---Windows only for now.
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.squareLifeforce(x, y) end

---A player's trade bonus in percent, their team's trade enchantments counted.
---Windows only for now.
---@param player integer a player slot
---@return integer|nil result nil when refused (lua_refused)
function coe5.tradeBonusPercent(player) end

---1 and the terrain shown when another player's illusion covers a square for this player, else 0.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.illusionTerrain(player, x, y) end

---Turns a square's terrain into its ruined form.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.ruinSquare(player, x, y) end

---Digs out a Cave Wall or Moist Cave Wall, and returns 1 when it dug.
---On Mac true, since its function there returns nothing.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return integer|boolean|nil result nil when refused (lua_refused)
function coe5.digTunnelSquare(player, x, y) end

---Changes a square to terrain to when it is from or from2, -1 for any, as the event word alterterrain does.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@param from integer integer
---@param from2 integer integer
---@param to integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.alterTerrain(player, x, y, from, from2, to) end

---Turns a Gateway into a Destroyed Gateway.
---Windows only for now.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@param flag integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.destroyGateway(player, x, y, flag) end

---The Baron's conscription for a player, as the first month of Summer runs it.
---Windows only for now.
---@param player integer a player slot
---@return boolean|nil result nil when refused (lua_refused)
function coe5.conscription(player) end

---A Voice of El player's chance of free units by the seals broken.
---Windows only for now.
---@param player integer a player slot
---@return boolean|nil result nil when refused (lua_refused)
function coe5.priestFreespawn(player) end

---Calls the Inquisition's army for a player.
---@param player integer a player slot
---@return boolean|nil result nil when refused (lua_refused)
function coe5.inquisitionSpawn(player) end

---Makes the game count every player's income again the next time it is read.
---@return boolean|nil result nil when refused (lua_refused)
function coe5.resetIncomeCaches() end

---Marks each portal active or not, as the game does when a portal opens or closes.
---Windows only for now.
---@return integer|nil result nil when refused (lua_refused)
function coe5.fixPortals() end

---Teleports a unit to a random square of a terrain code, as the game's random teleports do; followers other than 0 brings them along. False when 500,000 draws found none.
---On Mac true, since its function there returns nothing.
---@param unit integer a living unit
---@param terrain integer integer
---@param followers integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.randomTeleport(unit, terrain, followers) end

---Horrors come for the units marked by them, at a chance the game multiplies by ten for the most marked.
---@param chance integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.popHorrors(chance) end

---Frees every follower whose commander can't lead it.
---Windows only for now.
---@return boolean|nil result nil when refused (lua_refused)
function coe5.removeBadFollowers() end

---Sends every human player a World News line, as the game's own events do.
---@param text string a string
---@return boolean|nil result nil when refused (lua_refused)
function coe5.worldNews(text) end

---Switches a player's auto recruit of a monster on a square on or off.
---Windows only for now.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@param monster integer a monster
---@return integer|nil result nil when refused (lua_refused)
function coe5.toggleAutoRecruit(player, x, y, monster) end

---The living units of a monster on a square, by string ("" for any), owned by fromOwner (-1 for any) become slot 27's, Hades', and the call returns how many.
---@param x integer a square's x
---@param y integer a square's y
---@param monster string a string
---@param fromOwner integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.changeLoyalty(x, y, monster, fromOwner) end

---Moves every active ritual effect on a square to another, as the game does when squares swap, and returns how many.
---Windows only for now.
---@param x1 integer a square's x
---@param y1 integer a square's y
---@param x2 integer a square's x
---@param y2 integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.moveEnchants(x1, y1, x2, y2) end

---Swaps the units on two squares; the squares stay as they are.
---On Mac true, since its function there returns nothing.
---@param x1 integer a square's x
---@param y1 integer a square's y
---@param x2 integer a square's x
---@param y2 integer a square's y
---@return integer|boolean|nil result nil when refused (lua_refused)
function coe5.swapLocations(x1, y1, x2, y2) end

---Moves a square's gems to another square, which leaves the first with none.
---Windows only for now.
---@param x1 integer a square's x
---@param y1 integer a square's y
---@param x2 integer a square's x
---@param y2 integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.moveGems(x1, y1, x2, y2) end

---Whether a player has a living commander; the game removes a player without one.
---@param player integer a player slot
---@return boolean|nil result nil when refused (lua_refused)
function coe5.playerHasCommander(player) end

---Whether a player still owns their home citadel.
---@param player integer a player slot
---@return boolean|nil result nil when refused (lua_refused)
function coe5.playerHasCitadel(player) end

---The first value of an ability in a player's class, 0 for none.
---@param player integer a player slot
---@param ability integer|string an ability's word or id
---@return integer|nil result nil when refused (lua_refused)
function coe5.playerClassAbility(player, ability) end

---Whether a player, or anyone with -1, may own a square.
---@param player integer a player slot, -1 for any
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.canOwnSquare(player, x, y) end

---What world generation scores a square as a player's start, by their class's likesnorth, likessouth and likescoast, then a second number the score's sum keeps.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.startPlacementScore(player, x, y) end

---Whether a square passes a player's likesnorth, likessouth and likescoast when world generation picks their start.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.startLikesFilter(player, x, y) end

---A player's income of a resource: gold, iron, or the rest summed by square.
---@param player integer a player slot
---@param resource integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.playerIncomeByType(player, resource) end

---A player's ironbonus in percent.
---@param player integer a player slot
---@return integer|nil result nil when refused (lua_refused)
function coe5.ironBonusPercent(player) end

---A player's goldbonus in percent.
---Windows only for now.
---@param player integer a player slot
---@return integer|nil result nil when refused (lua_refused)
function coe5.goldBonusPercent(player) end

---An ability's sum over a player's team's living units on a square.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@param ability integer|string an ability's word or id
---@return integer|nil result nil when refused (lua_refused)
function coe5.squareTeamAbilitySum(player, x, y, ability) end

---An ability over the units with Hit Points on a square, a player's or, with -1, everyone's.
---@param player integer a player slot, -1 for any
---@param x integer a square's x
---@param y integer a square's y
---@param ability integer|string an ability's word or id
---@return integer|nil result nil when refused (lua_refused)
function coe5.squareUnitsAbility(player, x, y, ability) end

---The troll forest's spawns on a square, as the game's Troll Forest does.
---Windows only for now.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.trollForestSpawn(x, y) end

---Whether a square is Sky, as the game's routes count Sky squares.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.squareIsSky(x, y) end

---Whether a square's terrain can burn.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.terrainBurnableAt(x, y) end

---The terrain a terrain ruins into, -1 for none.
---@param terrain integer a terrain
---@return integer|nil result nil when refused (lua_refused)
function coe5.terrainRuinForm(terrain) end

---Whether a square counts as south, as south1 and south2 test it.
---Windows only for now.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.squareIsSouth(x, y) end

---Whether a square counts as north, as north1 tests it.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.squareIsNorth(x, y) end

---The squares within 1 of a square whose terrain is water.
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.countWaterNear(x, y) end

---The squares within 1 of a square whose terrain isn't water.
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.countLandNear(x, y) end

---The ground a battle on a square uses where no battle map cell sets one.
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.squareDefaultGround(x, y) end

---Whether a square is coast, as likescoast tests it.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.squareIsCoast(x, y) end

---The temperature the game uses for a square, the heat sources near it counted.
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.squareEffectiveTemp(x, y) end

---Two squares trade their whole records and the units on them, as the Void's shuffle does. 1 when they swapped.
---On Mac true, since its function there returns nothing.
---@param x1 integer a square's x
---@param y1 integer a square's y
---@param x2 integer a square's x
---@param y2 integer a square's y
---@return integer|boolean|nil result nil when refused (lua_refused)
function coe5.swapSquares(x1, y1, x2, y2) end

---Moves every living unit on a square to another.
---Windows only for now.
---@param x1 integer a square's x
---@param y1 integer a square's y
---@param x2 integer a square's x
---@param y2 integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.teleportSquare(x1, y1, x2, y2) end

---Whether a unit may stand on land, as the drown check tests it.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitMayBeOnLand(unit) end

---A unit's magic resistance total.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.unitMagicResistance(unit) end

---Whether a unit counts as huge for a weapon's huge.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitIsHuge(unit) end

---Whether a unit is Lifeless on its unit stats screen.
---Windows only for now.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitLifeless(unit) end

---Whether a unit can be primed: an animal with magic resistance below 6.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitPrimable(unit) end

---Takes an ability off a unit's first matching temporary slot, true when it did.
---Windows only for now.
---@param unit integer a living unit
---@param ability integer|string an ability's word or id
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitRemoveTempAbility(unit, ability) end

---Frees the followers of a commander whose unit is dead.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.checkFollowerLinks(commander) end

---A commander burns the forest on its square, as the map's order does.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.orderBurnForest(commander) end

---A commander chops the forest on its square, as the map's order does.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.orderChopForest(commander) end

---A commander hunts slaves on its square, as the map's order does.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.orderSlaveHunt(commander) end

---A commander eats the village on its square, as the map's order does.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.orderEatVillage(commander) end

---A commander uses the special site on a square, as a human's commander does.
---@param x integer a square's x
---@param y integer a square's y
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.useSpecialSite(x, y, commander) end

---Whether an army the AI fears stands within 3 squares of a square.
---Windows only for now.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@param strength integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.aiThreatNear(player, x, y, strength) end

---How many other commanders of a team head for a square's surroundings.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@param commander integer a commander with a living unit
---@param range integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiCountAllyTargetsNear(player, x, y, commander, range) end

---Refreshes what an AI player remembers of each seen square's strength.
---@param player integer a player slot
---@return boolean|nil result nil when refused (lua_refused)
function coe5.aiUpdateStrengthMemory(player) end

---Makes n of a player's units on a square guards, which never leave it.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@param n integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.aiMakeGuardsAt(player, x, y, n) end

---Leaves one guard on each seen square the AI player holds with an army, as its turn does after its moves.
---Windows only for now.
---@param player integer a player slot
---@return boolean|nil result nil when refused (lua_refused)
function coe5.aiMakeDefenders(player) end

---The commanders, melee units and ranged units of a player on a square, as the AI's recruiting counts them.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiCountTroops(player, x, y) end

---Whether a unit stays put for the AI: a guard, or an aihold unit on a fort.
---Windows only for now.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.aiUnitStays(unit) end

---The nearest square matching a terrain code over the whole map, as the AI looks for a ritual's terrain: 1 and its x and y.
---@param terrain integer integer
---@param x integer a square's x
---@param y integer a square's y
---@param player integer a player slot, -1 for any
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiNearestTerrain(terrain, x, y, player) end

---A player's commander with the most of an ability.
---Windows only for now.
---@param player integer a player slot
---@param ability integer|string an ability's word or id
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiRitualMaster(player, ability) end

---The ship capacity a commander's army has left.
---Windows only for now.
---@param commander integer a commander with a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiCommanderBoatPlaces(commander) end

---Drops one follower from a commander's ship crew, true when it did.
---Windows only for now.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.aiShipDropCrew(commander) end

---Links one free ship on a commander's square, true when it did.
---Windows only for now.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.aiShipTakeBoat(commander) end

---Links one free sailor on a commander's square, true when it did.
---Windows only for now.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.aiShipTakeCrew(commander) end

---The ship capacity of a commander and its followers.
---Windows only for now.
---@param commander integer a commander with a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiGroupBoatTotal(commander) end

---The ship capacity on a player's ports, then their ships.
---Windows only for now.
---@param player integer a player slot
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiPortsBoatTotals(player) end

---A player's ships on a square.
---Windows only for now.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiShipsAt(player, x, y) end

---A player's commanders planning a ritual, by the ritual's name.
---Windows only for now.
---@param player integer a player slot
---@param ritual integer a ritual
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiCountPlanners(player, ritual) end

---What a square scores for a commander's planned ritual.
---Windows only for now.
---@param commander integer a commander with a living unit
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiPlanSquareBonus(commander, x, y) end

---Hands each item on a square to the player's unit there that values it most.
---Windows only for now.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.aiRedistributeItems(player, x, y) end

---The AI's ship handling for one commander.
---Windows only for now.
---@param commander integer a commander with a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiShips(commander) end

---The AI's conquest step for a commander: the best score, -999 for none, then its square.
---Windows only for now.
---@param commander integer a commander with a living unit
---@param strength integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiConquerStep(commander, strength) end

---The AI's exploration step for a commander: the best score, then its square.
---Windows only for now.
---@param commander integer a commander with a living unit
---@param strength integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiExploreStep(commander, strength) end

---The AI's far exploration step for a commander: the best score, then its square.
---@param commander integer a commander with a living unit
---@param strength integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiFarExploreStep(commander, strength) end

---The AI's rally step for a commander: a score, then its square.
---Windows only for now.
---@param commander integer a commander with a living unit
---@param strength integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiRallyStep(commander, strength) end

---The AI's move to a friendly square for a commander: a score, then its square.
---Windows only for now.
---@param commander integer a commander with a living unit
---@param strength integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.aiFriendlyMoveStep(commander, strength) end

---The unit covering a battle cell, -1 for none.
---Only during a battle or on an open test field.
---@param x integer a battle cell's x
---@param y integer a battle cell's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleUnitAt(x, y) end

---The summed fear a player's unit feels on a battle cell.
---Windows only for now. Only during a battle or on an open test field.
---@param x integer a battle cell's x
---@param y integer a battle cell's y
---@param player integer a player slot
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleFearAt(x, y, player) end

---The best localleadership of a player's unit near a battle cell.
---Windows only for now. Only during a battle or on an open test field.
---@param x integer a battle cell's x
---@param y integer a battle cell's y
---@param player integer a player slot
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleLocalLeadership(x, y, player) end

---A team's living mobile units on the field of a rank range.
---Only during a battle or on an open test field.
---@param player integer a player slot
---@param lowRank integer integer
---@param highRank integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleCountRank(player, lowRank, highRank) end

---The one team left on the field, as that team's first player.
---Only during a battle or on an open test field.
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleWinner() end

---Whether two units on the field can reach each other past a siege wall.
---Windows only for now. Only during a battle or on an open test field.
---@param unit integer a living unit
---@param other integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.battleLineOfSight(unit, other) end

---Whether a unit is in range of another, the line test counted.
---Windows only for now. Only during a battle or on an open test field.
---@param unit integer a living unit
---@param other integer a living unit
---@param range integer integer
---@param flags integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.battleInRange(unit, other, range, flags) end

---Whether an enemy touches a unit on the field.
---Windows only for now. Only during a battle or on an open test field.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitInContact(unit) end

---Whether a unit fits on a battle cell.
---Only during a battle or on an open test field.
---@param unit integer a living unit
---@param x integer a battle cell's x
---@param y integer a battle cell's y
---@param mode integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.battleCanStand(unit, x, y, mode) end

---The squared distance between two units' footprints, then the distance in cells.
---Windows only for now. Only during a battle or on an open test field.
---@param unit integer a living unit
---@param other integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleUnitDistance(unit, other) end

---A unit's nearest enemy, then its squared distance.
---Windows only for now. Only during a battle or on an open test field.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleNearestEnemy(unit) end

---The lowest and highest column holding a player's enemies.
---Only during a battle or on an open test field.
---@param player integer a player slot
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleEnemyExtent(player) end

---An ability of the battle cell under a unit: a wall or a parapet.
---Windows only for now. Only during a battle or on an open test field.
---@param unit integer a living unit
---@param ability integer|string an ability's word or id
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleSquareAbility(unit, ability) end

---The Wall Cover of the cell under a unit.
---Only during a battle or on an open test field.
---@param unit integer a living unit
---@param kind integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleWallCover(unit, kind) end

---The rank a unit advances by this round.
---Only during a battle or on an open test field.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleAdvanceRank(unit) end

---A random living unit of a monster on a square, by string, a player's or, with -1, anyone's. -1 for none.
---@param player integer a player slot, -1 for any
---@param x integer a square's x
---@param y integer a square's y
---@param monster string a string
---@return integer|nil result nil when refused (lua_refused)
function coe5.findUnitAt(player, x, y, monster) end

---A random living unit on a square holding an item, by string. -1 for none.
---@param player integer a player slot, -1 for any
---@param x integer a square's x
---@param y integer a square's y
---@param item string a string
---@return integer|nil result nil when refused (lua_refused)
function coe5.findItemHolderAt(player, x, y, item) end

---A random square of a plane and terrain code, as the game's rituals and world generation pick one: 1, then its x and y.
---@param plane integer a plane 0-10, -1 for any, -2 for Elysium, the Sky and Agartha
---@param terrain integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.randomSquareOfTerrain(plane, terrain) end

---The ground the game falls back on for a square, by its 3x3.
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.groundFromNeighbours(x, y) end

---Gives a unit one of the game's random afflictions.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.unitRandomAffliction(unit) end

---Gives a unit an affliction by its bit in unitStateFlags.
---@param unit integer a living unit
---@param bit integer a mask of bits
---@return boolean|nil result nil when refused (lua_refused)
function coe5.addAffliction(unit, bit) end

---Shows a player every square of a terrain code on a plane, and returns how many.
---Windows only for now.
---@param player integer a player slot
---@param plane integer a plane 0-10, -1 for any, -2 for Elysium, the Sky and Agartha
---@param terrain integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.revealTerrainOnPlane(player, plane, terrain) end

---A square of a terrain code near a square, the nearest more often than not: 1, then its x and y; 0 for none.
---On Mac the plane is 0-10, since its function there has no -1 or -2.
---@param plane integer a plane 0-10, -1 for any, -2 for Elysium, the Sky and Agartha
---@param terrain integer integer
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.nearTerrainSquare(plane, terrain, x, y) end

---Puts a random rare terrain on a square, as world generation does.
---Windows only for now.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.putRareSquare(x, y) end

---A grove's monthly step on a square: its spawns and its spread.
---Windows only for now.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.groveReversion(x, y) end

---Shows a player's team every square within a radius, on that square's plane.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@param radius integer a number
---@param value integer integer
---@param flag integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.revealDisc(player, x, y, radius, value, flag) end

---Sets what a player, or every player with -1, has seen within a radius.
---@param player integer a player slot, -1 for any
---@param x integer a square's x
---@param y integer a square's y
---@param radius integer a number
---@param level integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.setSeenDisc(player, x, y, radius, level) end

---Plays one of the game's sounds, change raising or lowering it from its volume.
---@param sample integer integer
---@param change integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.playSample(sample, change) end

---A random name from one of the game's name lists, as nametype draws one.
---@param nameType integer a name list 0-61
---@return string|nil result nil when refused (lua_refused)
function coe5.autoName(nameType) end

---Shows the game's Yes/Cancel box, and returns whether the player picked Yes.
---@param text string a string
---@return boolean|nil result nil when refused (lua_refused)
function coe5.confirm(text) end

---Whether any of a square's four sides is water, as world generation places river sources.
---Windows only for now.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.squareSideWater(x, y) end

---Whether a square is flooded or its terrain is water, as a plane's flood reads it.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.squareIsWet(x, y) end

---The wet squares in the 3x3 around and on a square.
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.wetNeighbours(x, y) end

---The Manhattan distance to the nearest other square of any of four terrains.
---@param x integer a square's x
---@param y integer a square's y
---@param terrain1 integer integer
---@param terrain2 integer integer
---@param terrain3 integer integer
---@param terrain4 integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.nearestTerrainDistance(x, y, terrain1, terrain2, terrain3, terrain4) end

---The squares of a plane matching a terrain code.
---@param plane integer a plane 0-10
---@param terrain integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.countTerrainCode(plane, terrain) end

---Opens the game's location picker, as a ritual that asks where, and returns the square the player picked.
---Windows only for now.
---@param title string the picker's title
---@param player integer a player slot
---@param x integer the square's x it starts on
---@param y integer the square's y it starts on
---@param multiplane integer 1 keeps the player on the plane in view, 0 lets them pick any plane they have seen
---@param range? integer how far from the first square they may pick; leave it out for any distance
---@return integer|nil x nil when the player cancelled or the call was refused (lua_refused)
---@return integer|nil y
function coe5.chooseLocation(title, player, x, y, multiplane, range) end

---Opens the map's income popup for a player and a resource, as a top bar button does.
---Windows only for now.
---@param player integer a player slot
---@param resource integer a resource 0-18
---@return boolean|nil result nil when refused (lua_refused)
function coe5.openIncomeDetail(player, resource) end

---Opens a plane's info screen, as a right click on its top bar button does.
---@param plane integer a plane 0-10
---@return boolean|nil result nil when refused (lua_refused)
function coe5.openPlaneInfo(plane) end

---Opens the Square terrain info window for a player and a square. mode is the game's own switch.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@param mode integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.openSquareInfo(player, x, y, mode) end

---Opens the Ceremonies screen for a commander.
---Windows only for now.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.openCeremonies(commander) end

---Opens the Rituals screen for a unit.
---@param unit integer a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.openRitualScreen(unit) end

---Opens the library screen for a commander.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.openLibrary(commander) end

---Opens the Chop & Burn window for the selected commander.
---@return boolean|nil result nil when refused (lua_refused)
function coe5.openChopBurn() end

---Opens the Hunt for Slaves screen for the selected commander.
---@return boolean|nil result nil when refused (lua_refused)
function coe5.openSlaveHunt() end

---Opens the army screen's menu for a commander.
---@param commander integer a commander with a living unit
---@return boolean|nil result nil when refused (lua_refused)
function coe5.openArmyMenu(commander) end

---Opens a weapon's info window.
---Windows only for now.
---@param weapon integer a weapon
---@return boolean|nil result nil when refused (lua_refused)
function coe5.openWeaponInfo(weapon) end

---Opens an item's info screen.
---Windows only for now.
---@param item integer an item
---@return boolean|nil result nil when refused (lua_refused)
function coe5.openItemInfo(item) end

---Opens the Select Monster screen on the monsters whose value of an ability lies in a range, and returns the monster picked.
---Windows only for now.
---@param low integer integer
---@param high integer integer
---@param ability integer|string an ability's word or id
---@return integer|nil result nil when refused (lua_refused)
function coe5.monsterPicker(low, high, ability) end

---Opens the game's class picker, and returns the class picked: -1 for its Random line. It needs no
---game running, so it works from the main menu.
---@return integer|nil result nil when refused (lua_refused)
function coe5.classPicker() end

---How many of a player's commanders have action points left, as End Turn's warning counts them.
---@param player integer a player slot
---@return integer|nil result nil when refused (lua_refused)
function coe5.countCommandersWithAp(player) end

---Marks the selected commander done for this turn, as Space does.
---Windows only for now.
---@return boolean|nil result nil when refused (lua_refused)
function coe5.selectedCommanderDone() end

---Selects the next commander and centres the map on it.
---@return boolean|nil result nil when refused (lua_refused)
function coe5.selectNextCommander() end

---Selects the previous commander and centres the map on it.
---Windows only for now.
---@return boolean|nil result nil when refused (lua_refused)
function coe5.selectPrevCommander() end

---The living units of a monster on a square, by string, a player's or, with -1, anyone's.
---@param player integer a player slot, -1 for any
---@param x integer a square's x
---@param y integer a square's y
---@param monster string a string
---@return integer|nil result nil when refused (lua_refused)
function coe5.countUnitsAt(player, x, y, monster) end

---The living units of a monster on a plane, by string.
---@param player integer a player slot, -1 for any
---@param plane integer a plane 0-10, -1 for any, -2 for Elysium, the Sky and Agartha
---@param monster string a string
---@return integer|nil result nil when refused (lua_refused)
function coe5.countUnits(player, plane, monster) end

---The nearest living unit on a plane whose value of an ability lies in a range.
---Windows only for now.
---@param plane integer a plane 0-10, -1 for any, -2 for Elysium, the Sky and Agartha
---@param x integer a square's x
---@param y integer a square's y
---@param ability integer|string an ability's word or id
---@param low integer integer
---@param high integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.nearestUnitAbilityIs(plane, x, y, ability, low, high) end

---Whether a square holds an army, as a player's checks read it.
---@param player integer a player slot, -1 for any
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.armyAtForPlayer(player, x, y) end

---A commander's full name as messages give it: "Jazz the Dwarf Queen".
---@param commander integer a commander with a living unit
---@return string|nil result nil when refused (lua_refused)
function coe5.commanderTheName(commander) end

---The flags of every ritual a player knows, OR'd together.
---Windows only for now.
---@param player integer a player slot
---@return integer|nil result nil when refused (lua_refused)
function coe5.playerRitualFlags(player) end

---How many player slots hold a controller.
---@param controller integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.countSlotsWithController(controller) end

---A class by string, -1 for none.
---@param name string a string
---@return integer|nil result nil when refused (lua_refused)
function coe5.findClass(name) end

---A weapon's value of an extra trait, 0 for none.
---@param weapon integer a weapon
---@param id integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.weaponExtra(weapon, id) end

---Whether a unit's immunities stop an affliction, by its bits in unitStateFlags.
---@param unit integer a living unit
---@param bits integer a mask of bits
---@return boolean|nil result nil when refused (lua_refused)
function coe5.afflictionImmune(unit, bits) end

---The monster a Soulless made from a unit becomes.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.soullessForm(unit) end

---Gives a unit a benefit, by its bits, as a weapon's special benefit does.
---Windows only for now.
---@param unit integer a living unit
---@param bits integer a mask of bits
---@return boolean|nil result nil when refused (lua_refused)
function coe5.addBenefit(unit, bits) end

---A player's class may send one of its heroes, as the game's turn does.
---Windows only for now.
---@param player integer a player slot
---@return boolean|nil result nil when refused (lua_refused)
function coe5.heroArrival(player) end

---Clears what lies on a square, its hold included. flag is the game's own switch; world generation passes 0.
---@param x integer a square's x
---@param y integer a square's y
---@param flag integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.cleanSquare(x, y, flag) end

---Gives a square its starting gems by its terrain.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.initSquareGems(x, y) end

---Gives a square its starting corpses by its terrain.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.initSquareCorpses(x, y) end

---Runs the month's hatreds: each hates unit may kill a hated unit of its team.
---Windows only for now.
---@return boolean|nil result nil when refused (lua_refused)
function coe5.runHatreds() end

---Runs the month's desert clouds.
---Windows only for now.
---@return boolean|nil result nil when refused (lua_refused)
function coe5.sandstormCloudUpdate() end

---Starts one of the game's music tunes.
---@param tune integer integer
---@param force integer integer
---@return boolean|nil result nil when refused (lua_refused)
function coe5.startTune(tune, force) end

---Shows the game's number entry box, and returns the number entered.
---@param title string a string
---@param prompt string a string
---@return integer|nil result nil when refused (lua_refused)
function coe5.numberBox(title, prompt) end

---The unit or terrain object on a battle cell's second layer: a tree, a boulder, or a second unit.
---Only during a battle or on an open test field.
---@param x integer a battle cell's x
---@param y integer a battle cell's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleObjectAt(x, y) end

---The cheapest step for a unit toward a battle cell: a result, then the step.
---Only during a battle or on an open test field.
---@param unit integer a living unit
---@param x integer a battle cell's x
---@param y integer a battle cell's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.battlePathfind(unit, x, y) end

---A free neighbouring cell touching an enemy: a result, then the step.
---Windows only for now. Only during a battle or on an open test field.
---@param unit integer a living unit
---@return integer|nil result nil when refused (lua_refused)
function coe5.battleKillerMove(unit) end

---Whether a plane can be reached from a square, as the game's routes ask.
---@param plane integer a plane 0-10
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.planeIsReachable(plane, x, y) end

---Ends the other players' active effects on a square that can't stand against a player taking it.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.destroyEnchantsAt(player, x, y) end

---A square's owner, if their class allows it, gets Storm Elementals and Storm Clouds there.
---Windows only for now.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.stormSpawn(x, y) end

---A square's owner, if their class allows it, has a chance of Snakes, Moose and the like there.
---Windows only for now.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.wildSpawn(x, y) end

---Maenads and Satyr Revelers come to a square.
---Windows only for now.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.revelsSpawn(x, y) end

---A square's owner, if their class allows it, has a chance of a Forest Shepherd there.
---Windows only for now.
---@param x integer a square's x
---@param y integer a square's y
---@return boolean|nil result nil when refused (lua_refused)
function coe5.forestShepherdSpawn(x, y) end

---Writes a square's owner, as the game's conquest does; a mod that wants the rest of a conquest also calls what it needs.
---Windows only for now.
---@param x integer a square's x
---@param y integer a square's y
---@param player integer a player slot
---@return boolean|nil result nil when refused (lua_refused)
function coe5.setSquareOwner(x, y, player) end

---A random living unit on a square with every bit of a state mask, the game's search for an afflicted unit. monster -1 for any. -1 for none.
---@param player integer a player slot, -1 for any
---@param x integer a square's x
---@param y integer a square's y
---@param monster integer integer
---@param stateMask integer a mask of bits
---@param resistanceMask integer a mask of bits
---@return integer|nil result nil when refused (lua_refused)
function coe5.findAfflictedUnitAt(player, x, y, monster, stateMask, resistanceMask) end

---The nth value of an ability in a class, from 0; 0 for none.
---@param class integer a class
---@param ability integer|string an ability's word or id
---@param n integer integer
---@return integer|nil result nil when refused (lua_refused)
function coe5.classAbilityNth(class, ability, n) end

---A side's units on a square, as the game sizes a battlefield.
---Windows only for now.
---@param player integer a player slot
---@param x integer a square's x
---@param y integer a square's y
---@return integer|nil result nil when refused (lua_refused)
function coe5.countSideUnits(player, x, y) end

---The one slot with a human controller, -1 for none or several, worked out now.
---@return integer|nil result nil when refused (lua_refused)
function coe5.loneHumanNow() end

---The game's pick of a random enemy of a player, as the AI and a ritual cast draw one: a human or AI slot
---not on the player's team, from the game's own random numbers.
---@param player integer a player slot
---@return integer|nil enemy a player slot, -1 for none found in 1000 tries; nil when refused (lua_refused: no_game, bad_player)
function coe5.aiRandomEnemy(player) end

---Whether square (x, y) counts for a terrain code, as the game checks a ritual's terrain
---(ritualTerrainAt): -1 any terrain, a terrain type (terrainName takes it) by its name, so another
---type of the same name counts, and a group (-2 to -1500, as ritualTerrain and the newlocplanes,
---randnewloc and nearby words give them) by the game's own test.
---@param code integer
---@param x integer
---@param y integer
---@return boolean|nil counts false for a square off the world; nil for a code the game can't check (below -1500, or 2000 and up)
function coe5.terrainMatch(code, x, y) end

---Keeps only some of the game's debug trace lines (log.txt in the game folder on Windows, standard
---output on Mac, at "traceLevel" 1 to 3): from now on the game writes a line only when its format text, after its leading spaces,
---starts with one of the words, and drops the rest before it formats them. The words match the
---game's format text, not the finished line: "bt%d:" keeps the battle lines that read "bt3: ...".
---nil or an empty list keeps every line again. CoE5SE keeps one list for every mod: the last call
---wins. The list does nothing in a network game.
---@param words? string[] each a word a kept line starts with ("ailog", "removing player")
---@return boolean set false when refused (lua_refused reason=not_a_list, not_a_string, empty_word, leading_space, bad_character, out_of_memory; site_unavailable when the game's trace functions aren't the code CoE5SE knows, logged as trace_filter_installed)
function coe5.keepTraceLines(words) end

---Declares a plane of the mod's, at script load only. At each new world CoE5SE adds it beside the
---game planes (between the Void and Elysium2, plane 10), or, when the world's width (mapDepth, at
---most 1000 squares) is used up, under a smaller game plane (Celestial to the Void); a plane with no
---room is left out (plane_skipped, and a line on the CoE5SE page). It is filled with one terrain, then
---built by like's recipe (CoE5SE's copy of the game's random map generator for that plane) and
---generate, then its pieces are stamped. After the game places its rare special terrain, the recipe
---places its own, the zones' swaps run, and a plane with no portal to another plane gets an entrance:
---a gateway on a flat square of Elysium and one on a land square of the plane, joined both ways,
---never on a wall, a portal or a piece. With no such land square after 100 draws, the plane's gateway
---goes on any square of it an army can enter, water included; with none, it gets no gateway
---(plane_entrance_missing). When the game places independents, populate's plane places them (or only
---its units list), each zone with a units list only from it and each populate = false zone not at
---all, then swap applies. Its plane number is handed out at each new world and load, from 11, in the
---order the planes were declared: every mod's newplane blocks first, then the scripts' planes, in the
---order the scripts run; coe5.planeIndex reads it. Up to 8 planes, all mods together. The plane gets a
---button after the game's own on the map's top bar, the map editor's plane bar and a chooseloc
---ritual's target location screen (its icon, greyed until the player has seen a square of it), and on
---the Game Over map view, never greyed. A save holding one loads only where every plane it holds is
---declared again; a game without CoE5SE refuses it. A .c5se's newplane block declares the same plane
---without a script (the reference guide's "New Planes").
---@param plane coe5.Plane
---@return boolean declared false when refused (lua_refused reason=not_at_load, bad_name, name_taken, bad_size, bad_terrain, unknown_plane, no_recipe, bad_swap, too_many_swaps, recipes_not_loaded, bad_icon, icon_not_found, icon_wrong_case, icon_link, path_too_long, too_many_planes, plane_table_not_moved, hooks_not_installed)
function coe5.addPlane(plane) end

---Gives a monster a shape on a mod's plane, at script load only: a unit of that monster turns into
---shapeMonster there, as elysiumshape..voidshape do on the game planes, and it turns back by
---shapeMonster's own shapechanging commands. A monster with no shape on a mod's plane keeps its form
---there, as a monster with no agarthashape keeps its form on Agartha. A name finds a monster as the
---game's commands do, with an optional offset ("2:Hound" for the third); the names resolve at each new
---world and load, and one that doesn't is refused then (lua_refused reason=no_such_plane,
---no_such_monster, no_such_shape). A later call for the same monster and plane wins. A monster
---block's planeshape line in a .c5se declares the same.
---@param monster string
---@param planeName string a mod's plane (coe5.addPlane), from any mod
---@param shapeMonster? string nil for none
---@return boolean declared false when refused (lua_refused reason=not_at_load, bad_name, bad_plane, game_plane for a game plane by its full or short name ("Celestial Plane", "Celestial"), out_of_memory)
function coe5.setPlaneShape(monster, planeName, shapeMonster) end

---The plane number of the plane with that name in this world (a mod's plane or a game plane), or
---nil when the world has none; planeName and squarePlane use the same numbers.
---@param name string
---@return integer|nil index
function coe5.planeIndex(name) end

---The player number of the faction with that name in this game: a mod's faction (a .c5se newfaction block),
---or a vanilla independent faction by its name ("Independents", "Special Monsters", "Horrors", "Hades",
---"Inferno", "Celestial", "Empire"); nil when there is none. Every call that takes a player takes it, and
---takes the name itself too (coe5.get("slotName", "Ash Court"), coe5.spawnUnit("Ash Court", m, x, y)), as .c5m
---lines take the quoted name; a name no faction has is an error there. A mod's faction gets its number at each game's start, so ask in the game.
---@param name string
---@return integer|nil player
function coe5.factionSlot(name) end

---Declares an independent faction of the mod's, at script load only, as a .c5se newfaction block does:
---the same player number, rules, save rules and checks. Each field goes through its block word's check,
---and a field refused refuses the whole call (lua_refused field= the field), so the faction is declared
---as asked or not at all. It stays for the session; up to 16 factions, all mods together.
---@param faction coe5.Faction
---@return boolean declared false when refused (lua_refused reason=not_at_load, no_name, name_too_long, unknown_field, bad_field, bad_number, bad_text, missing_value, too_long, bad_color, a block word's reason, name_taken, too_many_factions, faction_slots_not_ready, hooks_not_installed)
function coe5.addFaction(faction) end

---Lays a colour over the map while a plane is in view, as the game lays red at 0.15 over Elysium and
---the Sky while a Blood Sun is up: one rectangle over the whole screen, over the ground and under the
---units. Any plane, the game's included, by its number or name (a game plane's full or short name,
---"Elysium2", or a mod's plane in this world). It wins over the game's own tint, and over the Blood
---Sun copy a plane like "Sky" gets, until coe5.clearPlaneTint, a load or the game's end; it isn't
---saved, so set it again from your own events. coe5.get("planeTint", plane) reads it back. Not in
---network games.
---@param plane integer|string
---@param r number 0 to 1
---@param g number 0 to 1
---@param b number 0 to 1
---@param strength number 0 to 1: how much of the colour covers the map (the Blood Sun's 0.15)
---@return boolean set false when refused (lua_refused reason=no_such_plane, out_of_range, hooks_not_installed)
function coe5.setPlaneTint(plane, r, g, b, strength) end

---Drops your tint on a plane: from the game's next tint update (each map screen, after each ritual)
---the plane has the game's own tint again.
---@param plane integer|string
---@return boolean cleared false when refused (lua_refused reason=no_such_plane)
function coe5.clearPlaneTint(plane) end

---Draws a mod's terrain (a number past 355) on the world map with a game terrain's looks, at script
---load only, as a terrainlooks block does. `ground`: the game terrain whose ground it takes (its
---place in the blend order and its edges); `picture` ("./file.tga", sides powers of two) is drawn
---instead of that ground's picture on this terrain's squares only. `decor`: the game terrain whose
---decorations it takes, exactly; `sprites` = {[40] = "./pool.tga"} draws tree.trs sprite 40 of them
---as the picture (a .tga or .png), on this terrain only. Terrains by number or name.
---@param looks {terrain: integer|string, ground: integer|string|nil, picture: string|nil, decor: integer|string|nil, sprites: table<integer, string>|nil}
---The ground picture also shows in battle, on the cells of the battle square's own ground.
---@return boolean set false when refused (lua_refused reason=not_at_load, no_terrain, unknown_terrain, not_a_mod_terrain, not_a_game_terrain, no_looks, no_decor, no_ground, looks_taken, bad_sprite, too_many_sprites, not_a_tga, size_not_power_of_two, icon_not_found, hook_not_installed)
function coe5.setTerrainLooks(looks) end

---Adds a battle map file of the game's format (made in the game's battle map editor, --batmapeditor),
---at script load only, as a battlemap block does. Battles on a square of one of `terrains` pick from
---the maps naming that terrain; with none, battles that would use one of the game's maps in `forts`
---("citadel", "village", "villageupgr": its file name in the game's data/batmaps) pick from the maps
---naming it and the game's own, which `replace` = true leaves out. `weight` (1 to 1000, default 10,
---the game's own map's) is the map's share of its pool; every word of `when` must hold: "siege",
---"open", "ambush", "sea", "land", "cold", "warm", "summer", "autumn", "winter", "spring"; `planes`
---names the planes it serves (any); `noscatter` = true keeps the map free of random trees and rocks;
---`grounds` = {[8] = "./ash.tga"} draws the map's cells of that ground (0 to 28) with the picture (a .tga,
---sides powers of two).
---The same square always gets the same map. Terrains by number or name.
---@param map {file: string, terrains: (integer|string)[]|nil, forts: string[]|nil, replace: boolean|nil, weight: integer|nil, when: string[]|nil, planes: string[]|nil, noscatter: boolean|nil, grounds: table<integer, string>|nil}
---@return boolean added false when refused (lua_refused reason=not_at_load, no_file, bad_file, file_not_found, file_wrong_case, file_link, path_too_long, not_a_battle_map, bad_weight, bad_list, bad_terrain, unknown_terrain, too_many_terrains, unknown_battle_map, too_many_forts, unknown_condition, no_name, too_many_planes, bad_ground, not_a_tga, size_not_power_of_two, icon_not_found, icon_wrong_case, no_place, replace_without_fort, conditions_never_met, hook_not_installed, out_of_memory)
function coe5.addBattleMap(map) end

---Gives a terrain its own random plants and rocks in battle, in place of the game's trees, bushes,
---rocks and rock pillars, at script load only, as a battlescatter block does: `objects` lists
---{monster name, chance in 1000 cells} pairs, tried in order, one object a cell at most; none, or no
---`objects`, for no scatter. The same square always gets the same scatter. One per terrain across
---all mods. Terrain by number or name.
---@param scatter {terrain: integer|string, objects: {[1]: string, [2]: integer}[]|nil}
---@return boolean set false when refused (lua_refused reason=not_at_load, no_terrain, unknown_terrain, bad_terrain, bad_list, no_name, bad_chance, too_many_objects, scatter_taken, hook_not_installed, out_of_memory)
function coe5.setBattleScatter(scatter) end

---Adds a mod command, as a DLL's addModCommand does, and only from your <ModName>_commands.lua while
---it loads. A line whose first word is `name`, exactly, capitals included, goes to `handler` instead
---of to the game, which would stop with "mod command '<name>' did nothing": `args` is the rest of the
---line with its leading spaces and tabs trimmed, `mod` the name of the mod whose line it is (nil if
---CoE5SE can't name it), and `level` the mod pass's level: 1 mod info (every pass reads each mod at
---level 1 first), 2 content (startup, leaving the Mods screen, loading a game, a game's end; where
---most commands act), 4 art files (spr, sample; read for a mod's checksum when a game is saved or
---loaded). Level 3, the event passes, never reaches a mod command, and neither does a line inside an
---event. The handler normally runs from the startup pass on, before <ModName>.lua has loaded, so it
---uses only what the commands file defines until then. The command works in a mod's <ModName>.c5se
---too. A prefix such as "mymod_" keeps the name from clashing with another mod's (duplicate). In a
---LAN game the handler runs on the host for a content mod, and at level 2 on a client for a content or personal mod.
---@param name string under 48 characters, no space or tab, not one of the game's commands, ignoring case
---@param handler fun(args: string, mod: string|nil, level: integer)
---@return boolean added false when refused (lua_refused reason=not_in_commands_file, no_name, bad_name, duplicate, game_command, site_unavailable, out_of_memory, script_stopped)
function coe5.addModCommand(name, handler) end

---Names a subfolder of your mod's area for your data files. The area is coe5se_data\<your mod's
---name> in the game's user folder (%APPDATA%\coe5 on Windows, ~/.coe5 on Mac); without this call
---your files go in the area itself. The subfolder is always inside the area: no mod reaches
---another's. The area holds 10 MB in all, its subfolders included, or up to 100 MB when your
---mod's .c5se has a maxdata line (maxdata 50).
---@param name string up to 64 characters, no / \ : * ? " < > |, not starting or ending with a space or a dot
---@return boolean named false for a name that isn't one plain folder name (logged as lua_refused)
function coe5.dataFolder(name) end

---Reads data file `name`.ini from your data folder, as t[section][key] = value. Values are strings:
---use tonumber for numbers. Keys before the first [section] line are in t[""]. A line it can't read
---(a [ with no ], a line with no =) is skipped, and the rest still reads (logged as bad_lines).
---@param name string the file's name without .ini: the same rules as coe5.dataFolder's
---@return table<string, table<string, string>>|nil t nil when there is no such file, the file is over 1 MB, or the name breaks a rule (logged as lua_refused)
function coe5.readIni(name) end

---Writes t[section][key] = value as data file `name`.ini in your data folder, in place of the old
---file, sections and keys in sorted order. Values are strings or numbers. A section name, key or
---value over 1,024 bytes is refused (problem=too_long), and so is one that wouldn't read back the
---same: a line break, ] in a section name, = in a key, a key starting with [ ; or #, or a space at
---either end. One refused section name, key or value fails the whole write: nothing is written and
---the old file stays as it was. Split a long list over several keys.
---@param name string the file's name without .ini
---@param t table<string, table<string, string|number>>
---@return boolean written false when something is refused, the file would pass 1 MB, your mod's area would pass 10 MB or its .c5se's maxdata (data_quota), or it can't be written (logged as lua_refused)
function coe5.writeIni(name, t) end

---The names of the data files in your data folder, without .ini, sorted.
---@return string[]
function coe5.listIni() end

---Deletes data file `name`.ini from your data folder.
---@param name string
---@return boolean deleted false when there was no such file, or it can't be deleted (logged as lua_refused)
function coe5.deleteIni(name) end

---Shows one battle with no game, from the main menu: a commander of `monster` against `count` units of
---`enemy` (the independents), on a grass field with no fort, at the battle screen's default speed.
---With `spell` (a spell's number, -1 for none) the commander knows and has memorized only that spell.
---With `standIn`, a weapon your own mod made, the spell casts with that weapon's values in place of its
---own weapon's, for this battle only. Returns when the battle ends; the units are gone afterwards.
---It also takes coe5.openTestField's table in place of its arguments, for mixed groups.
---@param monster integer a monster's index (coe5.findMonster)
---@param spell integer|nil -1 or nil for none
---@param enemy integer a monster's index
---@param count? integer 1 or more; 40 when left out
---@param standIn? integer a weapon your mod made
---@overload fun(field: {commander: integer, spell?: integer, standIn?: integer, friends?: {[1]: integer, [2]?: integer}[], enemies: {[1]: integer, [2]?: integer}[]}): boolean
---@return boolean shown false when refused (lua_refused): in_game, bad_monster, bad_enemy, bad_count, bad_spell, no_spell_weapon, not_own_weapon, bad_weapon, no_free_slot; with a table, also bad_friend, bad_list, no_enemies
function coe5.testBattle(monster, spell, enemy, count, standIn) end

---Adds a line to the unit card's traits, after the game's own and CoE5SE's (Polymorph Immunity and the rest), on
---each unit `visible` says true for. Call it while your script loads.
---@param line {label: string, text?: string, icon?: integer|string, visible: fun(unit: integer, monster: integer): boolean}
---label up to 63 characters; icon a game icon number (0 to 667) or a picture in your mod's folder ("./icon.tga",
---32 x 32), 211 (Magic Shield) when left out; visible runs each time the card draws (in a LAN game, only a personal mod's on a client).
---@return integer|false id false when refused (lua_refused): no_label, label_too_long, no_visible, bad_icon (plus findModImage's), too_many_pictures, out_of_memory
function coe5.addTraitLine(line) end

---One record of the game's spell table, mods' spells included: its name, path, level, the weapon that is
---its effect, and the damage value the weapon is used with.
---@param spell integer
---@return {name: string, path: integer, level: integer, weapon: integer, dmg: integer}|nil info nil past the table or for an empty slot
function coe5.spellInfo(spell) end

---Opens a test field from the main menu, with no game: a commander of `monster` and `count` units of `enemy`
---(the independents) deployed on a grass field, as coe5.testBattle has them, and left standing. Draw it a
---frame at a time with coe5.drawTestField, attack with coe5.fireTestField, fight with coe5.fightTestField,
---and coe5.closeTestField when done. For mixed groups on both sides, pass one table instead:
---coe5.openTestField{commander = wizard, friends = {{archer, 3}}, enemies = {{spearman, 5}, {troll, 1}}}, each
---group a monster's index and a count (1 when left out). Friends follow the commander. No count is capped, and the
---enemies need at least one unit; spell and standIn are optional fields. The units come back as the second value,
---and coe5.closeTestField takes every one of them off.
---@overload fun(field: {commander: integer, spell?: integer, standIn?: integer, friends?: {[1]: integer, [2]?: integer}[], enemies: {[1]: integer, [2]?: integer}[]}): boolean, {commander: integer, friends: integer[], enemies: integer[]}|nil
---@param monster integer
---@param spell integer|nil
---@param enemy integer
---@param count? integer
---@param standIn? integer a weapon your mod made
---@return boolean opened false when refused (lua_refused): coe5.testBattle's reasons, or already_open; with a table, also bad_friend, bad_list (a side or a group that isn't a table), no_enemies
---@return {commander: integer, friends: integer[], enemies: integer[]}|nil units the units made, in group order
function coe5.openTestField(monster, spell, enemy, count, standIn) end

---Adds units to one side of the open test field, each group a monster's index and a count, as
---coe5.openTestField's table takes them. The game's own deploy places them, so the units already on the field
---keep their cells. "friends" follow the commander.
---@param side "friends"|"enemies"
---@param groups {[1]: integer, [2]?: integer}[]
---@return integer[]|nil units the units made, nil when refused (lua_refused): not_open, bad_side, bad_friend, bad_enemy, bad_list, bad_count, no_free_slot (the game's unit table is full)
function coe5.addTestFieldUnits(side, groups) end

---Moves a unit on the open test field to battle cell (x, y), its top-left corner there, to place a group exactly
---where an area attack lands. coe5.get("unitBattleX", unit) and "unitBattleY" read where a unit stands.
---@param unit integer
---@param x integer below battleWidth
---@param y integer 0 to 19
---@return boolean moved false when refused (lua_refused): not_open, not_on_field, off_field (the unit doesn't fit there), blocked (another unit, a tree or a boulder covers a cell of it)
function coe5.moveTestFieldUnit(unit, x, y) end

---One frame of the battle screen on the open test field, its speed arrows working. BATTLE_DRAW and
---BATTLE_WAIT run in it.
function coe5.drawTestField() end

---The test field's commander makes one attack, as in a battle, then every projectile flies to its end: its
---first weapon, or its spell cast as an item casts it. The weapon reaches across the field for that attack.
---Units it hits take the damage.
function coe5.fireTestField() end

---The battle on the test field, as the game fights one; returns when it ends.
function coe5.fightTestField() end

---Ends the test field's running battle (coe5.fightTestField) as the battle screen's skip to the end does:
---nothing more drawn, no delay, the battle over. Call it from a BATTLE_WAIT wrap during the battle.
function coe5.stopTestFight() end

---Closes the test field: its units gone, and everything it borrowed put back.
function coe5.closeTestField() end

---A weapon look's name in the game's visual effect table, the numbers look and flylook take ("Fireball").
---@param look integer
---@return string|nil name nil past the table or for an unnamed row
function coe5.lookName(look) end

---Puts `text` on the system clipboard as plain text, for the player to paste.
---@param text string
---@return boolean copied false when the text holds a 0 byte (bad_string) or the system refused (clipboard_refused), logged as lua_refused
function coe5.copyText(text) end

---Makes a new mod in the player's mods folder: <name>/<name>.c5m, holding the icon and description
---lines and then `text`, beside a plain banner. It starts disabled, as every mod the game finds.
---It never writes into an existing mod: a name whose folder exists is refused.
---@param name string 1 to 40 letters, digits, spaces and _, no space first or last
---@param text string the .c5m lines after the icon and description
---@return boolean saved false when refused (lua_refused): no_name, name_too_long, bad_name, bad_string, path_too_long, exists, folder_failed, write_failed
function coe5.saveNewMod(name, text) end

---Call it in a Game Menu line's onClick, in a single player game. When onClick returns, CoE5SE
---ends the game the way Resign does, and after the Game Over screen it starts a new game's setup
---instead of going back to the main menu. Your wraps can fill that setup in while
---coe5.restartPhase() is "starting".
---@return boolean asked false outside a Game Menu line's onClick, in a network game, or unless exactly one human player is in the game (logged as api_refused)
function coe5.restartGame() end

---Call it in a Game Menu line's onClick, in a single player game. It shows the game's Load Game
---list, with every mod's wraps of LOAD_GAME_BROWSER, while the game goes on underneath. Cancel
---returns false and the game goes on. When a save is picked, it returns true, and when onClick
---returns, CoE5SE ends the game the way Resign does and then loads the save, as the main menu's
---Load Game does. coe5.restartPhase() is "ending" until then. CoE5SE clicks Ok and Exit on the
---screens the end shows, and Ok on the loaded turn's first Messages screen (View messages, key m,
---still shows them).
---@return boolean picked false on Cancel (logged as load_game phase=cancelled), outside a Game Menu line's onClick, in a network game, unless exactly one human player is in the game, or while a Load Game list from coe5.loadGame is already up, as from a wrap of its sites (logged as api_refused, reason=already_picking)
function coe5.loadGame() end

---Where a restart is: "ending" from the onClick that called coe5.restartGame or picked a save with
---coe5.loadGame until the old game's MAIN_MENU_PLAY_GAME returns, "starting" during the new game
---CoE5SE started (its setup and play), and nil otherwise. A game started from the main menu, or
---loaded with coe5.loadGame, is always nil. A MAIN_MENU_PLAY_GAME wrap that calls next() again
---cancels the restart or load: that second game is nil (logged as restart_game phase=cancelled
---reason=play_game_reentered).
---@return "ending"|"starting"|nil phase
function coe5.restartPhase() end

---The game's own screen widgets. A screen is a loop. Each pass calls screenBegin, draws sections,
---then buttonRow and its lines, and calls wait, which returns the clicked id.
---
---    while true do
---        coe5.ui.screenBegin("My Mod")
---        coe5.ui.sectionBegin("Options")
---        coe5.ui.text(10, "Say hello")
---        coe5.ui.sectionEnd()
---        coe5.ui.buttonRow()
---        coe5.ui.text(9, "Back")
---        coe5.ui.bindEscape(9)
---        local id = coe5.ui.wait()
---        if id == 9 then return end
---        if id == 10 then coe5.log("hello") end
---    end
---@class coe5.ui
coe5.ui = {}

---The game's own random map generator steps, one per function, for a plane's recipe or generate function
---(coe5.addPlane). They work only while a mod's plane is being built, and only on it: a square off
---the plane is refused (lua_refused reason=outside_plane), a rectangle is cut to the plane, and
---outside a build every call is refused (not_world_building). A refused call returns nil.
---The map generator draws with marks: a walk, blob, rectangle or neighbour pass writes a mark number
---(1 to 32767) on squares, and a marks-to-terrain pass turns marked squares into a terrain. The marks
---are the plane's own for the build: empty at its start, gone after it.
---@class coe5.gen
coe5.gen = {}

---The game's own random draw, 0 to n - 1, as its map generator draws (0 for n below 2, with no draw).
---@param n integer
---@return integer
function coe5.gen.random(n) end

---A square's mark, 0 for none.
---@param x integer
---@param y integer
---@return integer
function coe5.gen.markAt(x, y) end

---Sets a square's mark (0 clears it).
---@param x integer
---@param y integer
---@param mark integer
function coe5.gen.setMark(x, y, mark) end

---The map generator's own terrain write: the terrain only; the square's gems, its owner's citadel and
---income, and its ground stay as they were (setGround sets the ground after, as the generator does;
---coe5.set("squareTerrain", ...) is the in-game change).
---@param x integer
---@param y integer
---@param terrain integer 0 to terrainLastIndex
function coe5.gen.setTerrain(x, y, terrain) end

---A terrain's flag bits: 4 water, 0x2000 forest, 0x10000 mountain, among others. Works outside a build.
---@param terrain integer
---@return integer
function coe5.gen.terrainFlags(terrain) end

---The squares within radius of (x, y), on its plane, holding the terrain or in the terrain group.
---@param x integer
---@param y integer
---@param code integer a terrain number, or a terrain group (a negative terrain number)
---@param radius integer
---@return integer
function coe5.gen.terrainNear(x, y, code, radius) end

---Marks a rectangle, cut to the plane.
---@param x integer
---@param y integer
---@param w integer
---@param h integer
---@param mark integer
function coe5.gen.markRect(x, y, w, h, mark) end

---The map generator's random walk ("markmapcave") from a square of the plane, marking count new squares;
---with a terrain each step keeps to squares whose whole 3 x 3 is that terrain, with -1 it goes anywhere.
---Gives up after 1,000,000 steps.
---@param x integer
---@param y integer
---@param terrain integer a terrain, or -1
---@param count integer 1 to the plane's squares
---@param mark integer
function coe5.gen.markWalk(x, y, terrain, count, mark) end

---The map generator's round blob from a square of the plane, size squares.
---@param x integer
---@param y integer
---@param size integer 1 to the plane's squares
---@param mark integer
function coe5.gen.markBlob(x, y, size, mark) end

---Marks the cheapest path between two squares of the plane (the game's river path): mode 0 any
---ground, 1 through Cave Wall and Magma River, 2 around Cave Wall; it stops at water.
---@param x1 integer
---@param y1 integer
---@param x2 integer
---@param y2 integer
---@param mark integer
---@param mode integer 0 to 2
---@return integer
function coe5.gen.markRiver(x1, y1, x2, y2, mark, mode) end

---Every square of the plane with the mark and terrain from becomes to ("mmct").
---@param mark integer
---@param from integer
---@param to integer
---@return integer changed
function coe5.gen.marksToTerrain(mark, from, to) end

---Every square of the plane with the mark becomes terrain at percent, whatever its terrain ("mm2t";
---one draw per marked square).
---@param mark integer
---@param terrain integer
---@param percent integer 0 to 100
---@return integer changed
function coe5.gen.marksToTerrainChance(mark, terrain, percent) end

---Marks every square of terrain with code next to it ("mmnbors").
---@param terrain integer
---@param code integer a terrain number or terrain group
---@param mark integer
---@return integer marked the plane's squares with that mark
function coe5.gen.markNearTerrain(terrain, code, mark) end

---Marks every Void square with code within radius (Celestial's clouds and flames).
---@param code integer
---@param mark integer
---@param radius integer
---@return integer marked the plane's squares with that mark
function coe5.gen.markVoidNear(code, mark, radius) end

---A random Cave Wall square 1 in from the plane's edge with at least need Cave Walls in its 3 x 3
---(its 5 x 5 above 9), 100,000 tries.
---@param need integer 0 to 25
---@return integer|nil x nil when none
---@return integer|nil y
function coe5.gen.findCaveSpot(need) end

---Temperature 25 on the rectangle ("setfixtemp").
---@param x integer
---@param y integer
---@param w integer
---@param h integer
function coe5.gen.setFixedTemperature(x, y, w, h) end

---The game's own temperature step: 25, -10 on Frozen Waste, the heat of mountains, deserts and jungles, the cold flags.
---@param x integer
---@param y integer
---@param w integer
---@param h integer
function coe5.gen.setTemperatures(x, y, w, h) end

---The game's heat blob, a step of setTemperatures: a random walk from (x, y) adds amount to the temperature of each
---new square it reaches, going back to (x, y) after each, until squares are counted. Squares the walk reaches off
---the plane count but keep their temperature.
---@param x integer
---@param y integer
---@param squares integer 1 to the plane's square count
---@param amount integer -32768 to 32767
function coe5.gen.heatBlob(x, y, squares, amount) end

---The ground kinds of the rectangle from each square's terrain, only on squares of terrain unless -1.
---@param x integer
---@param y integer
---@param w integer
---@param h integer
---@param terrain integer a terrain, or -1
function coe5.gen.setGround(x, y, w, h, terrain) end

---One flooding pass: the plane's open squares joined to water flood (Agartha's sea).
---@return integer flooded
function coe5.gen.floodPlane() end

---Cave Wall next to water becomes Moist Cave Wall.
function coe5.gen.updateMoistWalls() end

---Rock Ground touching open ground only at a corner becomes Cave Wall, until none is left.
---@return integer
function coe5.gen.fixDiagonals() end

---tries times, a random neighbour holding terrain onto takes the square's terrain (the Sky's cloud forests).
---@param x integer
---@param y integer
---@param tries integer
---@param onto integer
---@return integer changed
function coe5.gen.spreadTerrain(x, y, tries, onto) end

---Elysium's terrain blob: n squares of t1 or t2 grown from (x, y), wx and wy weighting east-west and
---north-south growth (forests 40 to 99 each way, deserts 999 and 999). a, b, c are the game's last
---three numbers (forests 0, 0, 0; deserts 0, 0, 1). Squares grown off the plane are put back.
---@param x integer
---@param y integer
---@param wx integer
---@param wy integer
---@param n integer
---@param t1 integer
---@param t2 integer
---@param a? integer
---@param b? integer
---@param c? integer
function coe5.gen.specBlob(x, y, wx, wy, n, t1, t2, a, b, c) end

---Elysium's river from one square of the plane to another: River on the cheapest path.
---@param x integer
---@param y integer
---@param tx integer
---@param ty integer
function coe5.gen.drawRiver(x, y, tx, ty) end

---Elysium's random special terrain for one square by its terrain (villages, farms, mines, ports; towers, castles and
---ruins only on Elysium).
---@param x integer
---@param y integer
function coe5.gen.decorateSquare(x, y) end

---count random items lying on the square, at level (Agartha's statue: 2 to 6 at 25).
---@param x integer
---@param y integer
---@param count integer 1 to 50
---@param level integer
function coe5.gen.addGroundItems(x, y, count, level) end

---A random square of the plane holding the terrain or in the terrain group (randloc's search,
---500,000 tries).
---@param code integer
---@return integer|nil x nil when none
---@return integer|nil y
function coe5.gen.randomSquareOfTerrain(code) end

---A random square of the plane with the mark that holds the terrain or is in the terrain group, the search the
---Inferno's builder uses ("randommarkloc", 10,000,000 tries); with openLand, only open land
---(squareOpenLand). Draws with the C runtime's rand, as the game does, not coe5.gen.random.
---@param mark integer 1 to 32767
---@param code integer a terrain number or terrain group
---@param openLand? boolean
---@return integer|nil x nil when none
---@return integer|nil y
function coe5.gen.randomMarkedSquare(mark, code, openLand) end

---The map generator's terrain blob ("terrblob", the Sky's cloud banks): n times a random walk of up to 25 steps
---from (x, y), held within maxDist of it and on the plane, lays terrain on the first square holding onto (-1: any
---square not already that terrain). Squares reached off the plane are put back.
---@param terrain integer
---@param x integer
---@param y integer
---@param n integer 1 to the plane's squares
---@param onto integer a terrain, or -1
---@param maxDist integer
---@return integer laid
function coe5.gen.terrainBlob(terrain, x, y, n, onto, maxDist) end

---A random square of the plane matching code whose 3 x 3, itself included, holds min to max squares matching
---nearCode (the Sky's Castle Arcanum: Sky, Sky, 9, 100), 500,000 tries.
---@param code integer a terrain number or terrain group
---@param nearCode integer a terrain number or terrain group
---@param min integer 0 to 9
---@param max integer
---@return integer|nil x nil when none
---@return integer|nil y
function coe5.gen.findSpotByNeighbours(code, nearCode, min, max) end

---Whether a monster's path leads from the square to Elysium's centre, or 10 squares west or east of it, through
---the portals the world has so far: the Sky's test for a Cloud City.
---@param x integer
---@param y integer
---@return boolean
function coe5.gen.reachesElysium(x, y) end

---A portal from a square of the plane to a square of any plane: n on the first, n (two ways) or -n
---(one way) on the second. A portal pairs with every square holding its number.
---@param x1 integer
---@param y1 integer
---@param x2 integer
---@param y2 integer
---@param twoway boolean
---@param n integer 1 to 32767
---@return integer n
function coe5.gen.setPortalPair(x1, y1, x2, y2, twoway, n) end

---setPortalPair with a number no square holds yet, picked as the game picks one (1000 to 10999).
---@param x1 integer
---@param y1 integer
---@param x2 integer
---@param y2 integer
---@param twoway boolean
---@return integer n
function coe5.gen.linkNewPortal(x1, y1, x2, y2, twoway) end

---For a step with a minimum plane size: true when the plane being built is at least minW x minH; else false, and
---the log names the step and its minimum (lua_refused reason=plane_too_small).
---@param step string
---@param minW integer
---@param minH integer
---@return boolean
function coe5.gen.refuseSmallPlane(step, minW, minH) end

---Stamps a map file from your mod's folder ("./citadel.coem", the game's single-plane format) with
---its top left square at (x, y), through the game's map reader, as a newplane block's planepiece does:
---the plane's square names, portals and special square flags (squarespec) under it are cleared first. Its whole mapsize
---must be on the plane (outside_plane). The pieces coe5.addPlane declares, stamped after generate,
---never go on it.
---@param file string
---@param x integer
---@param y integer
---@return boolean|nil stamped nil when refused (lua_refused reason=outside_plane, not_world_building, no_plane, no_mod_folder, bad_file, file_not_found, file_wrong_case, file_link, path_too_long, no_mapsize, out_of_memory, hook_not_installed)
function coe5.gen.stampPiece(file, x, y) end

---Clears the screen and draws the title. Each text the coe5.ui calls draw is cut to the game's
---buffer for it, the most the game draws (logged as api_cut): 99 characters for a title, 4999 for
---a line, list row, or tooltip, 999 for a checkbox, 998 for a choice.
---@param title string
function coe5.ui.screenBegin(title) end

---Starts a framed section.
---@param header? string
function coe5.ui.sectionBegin(header) end

---Ends the section.
function coe5.ui.sectionEnd() end

---Draws one line. Id 0 makes it plain text; any other id makes it clickable.
---@param id integer
---@param text string
function coe5.ui.text(id, text) end

---Draws one checkbox line. A click on it makes wait return `id`; your script then flips its own
---value, as with choice.
---@param id integer
---@param checked boolean
---@param label string
function coe5.ui.checkbox(id, checked, label) end

---Draws one radio line. Your script tracks which one is selected.
---@param id integer
---@param selected boolean pass a boolean such as `id == picked`: in Lua, 0 counts as true
---@param label string
function coe5.ui.choice(id, selected, label) end

---Hover text for the line drawn just before.
---@param text string
function coe5.ui.tooltip(text) end

---Starts the bottom row of buttons (Ok, Back, ...). Always last.
function coe5.ui.buttonRow() end

---Starts a list, Load Game's layout: call it after screenBegin, then listRow for each item, then
---listEnd. Use it to pick one of several things, each opening its own screen.
function coe5.ui.listBegin() end

---Draws one striped row of the list, 450 wide. A click on it makes wait return `id`.
---@param id integer
---@param text string
function coe5.ui.listRow(id, text) end

---Ends the list, with the gap before the button row.
function coe5.ui.listEnd() end

---Makes the Esc key click line `id`.
---@param id integer
function coe5.ui.bindEscape(id) end

---Makes a key click line `id`: coe5.key.enter, coe5.key.esc (as bindEscape), or a lowercase letter's
---byte, string.byte("s"). Key 0 binds nothing. An id or key that doesn't fit the game's 32-bit int
---binds nothing either (logged as lua_refused, reason=out_of_range), as for bindEscape's id.
---@param id integer
---@param key integer
function coe5.ui.bindKey(id, key) end

---Ends the frame: shows the screen and returns the clicked line's id.
---@return integer id the clicked line's id, 0 when nothing was clicked this frame
function coe5.ui.wait() end

---Draws a section's header: sectionBegin(header) is sectionBegin() then this.
---@param text string
function coe5.ui.sectionHeader(text) end

---Sets the screen's width. screenBegin uses Random Map Settings' width, coe5.ui.setupScreenWidth().
---NaN, an infinity, or a negative width changes nothing (lua_refused reason=not_finite or negative).
---@param width number
function coe5.ui.screenWidth(width) end

---The width Random Map Settings uses.
---@return number
function coe5.ui.setupScreenWidth() end

---Starts the menu screens' music, as the game does once before a screen's loop. Does nothing on Mac.
function coe5.ui.screenMusic() end

---Moves where the next line of the row starts, `x` from the row's left edge: a column of your own
---in a row the game draws, such as the file browser's (FILE_BROWSER_ROW_DATE). NaN or an infinity
---changes nothing (lua_refused reason=not_finite).
---@param x number
function coe5.ui.columnX(x) end

---Opens a row `width` UI units wide, all of it clicked as `id` (0 for none), with a background in
---r, g, b (each 0 to 1) at `alpha` (none when left out or 0): a table's row, or its highlighted
---one. Draw the cells with coe5.ui.text at coe5.ui.columnX, then close it with coe5.ui.rowEnd.
---Refused (lua_refused), drawing nothing: a row already open (row_open), a width of 0 or less, an
---id past the game's 32-bit int, or a colour outside 0 to 1 (out_of_range), NaN or an infinity
---(not_finite). A row left open is closed at coe5.ui.wait or
---screenBegin (row_open).
---@param id integer
---@param width number
---@param r? number
---@param g? number
---@param b? number
---@param alpha? number
function coe5.ui.rowBegin(id, width, r, g, b, alpha) end

---Closes the row coe5.ui.rowBegin opened. With none open it does nothing (lua_refused reason=no_row).
function coe5.ui.rowEnd() end

---Draws a button with a face of its own, `width` by `height` UI units, in r, g, b (each 0 to 1; a slate
---grey when left out): an edge, a lit top and a shaded bottom, lighter under the pointer, its label
---centered and `line2` in smaller text under it. A click on it makes wait return `id`. The screen lays
---it out as a row, with a small gap under it. Refused (lua_refused), drawing nothing: a row open
---(row_open), more than 64 on a screen (too_many), an id of 0 or past the game's 32-bit int, a width or
---height of 0 or less, or a colour outside 0 to 1 (out_of_range), NaN or an infinity (not_finite).
---@param id integer
---@param label string cut to 99 characters
---@param width number
---@param height number 28 fits one line, 42 fits line2 too
---@param r? number
---@param g? number
---@param b? number
---@param line2? string cut to 99 characters
function coe5.ui.button(id, label, width, height, r, g, b, line2) end

---The game's "Enter name" box, with a monster's picture. It runs its own loop until Ok or Cancel.
---Enter clicks Ok, and Esc clicks Cancel. CoE5SE draws the box itself, as the game draws it, because
---the game's own box binds no key to Esc and, on Mac, always says "Enter name", takes 25 characters,
---and has no Cancel.
---@param title string cut to 99 characters
---@param prompt string cut to 499 characters
---@param sprite integer a monster's picture: coe5.get("monsterSprite", monster)
---@param maxLength? integer 1 to 42; 32 when left out. The game's field takes two characters fewer (32 takes 30)
---@return string|nil text the typed text, or nil on Cancel, or at once for a sprite no monster has (lua_refused reason=bad_sprite)
function coe5.ui.nameBox(title, prompt, sprite, maxLength) end

---Draws one striped row: the monster's picture, then `text`, all clicked as `id`. With `text` only it
---is the row of the game's own monster list ("Select Monster"), the picture as high as the line.
---With `line2` it is two lines under one stripe as wide as the screen's sections, the second one
---subtext (the recruit screen's smaller text, and fainter), and the picture is as high as both
---lines, beside them; a click anywhere on the row is `id`. A monster index no monster uses draws nothing (lua_refused
---reason=bad_monster), and so does an id past the game's 32-bit int (out_of_range).
---@param id integer
---@param monster integer a monster's index (coe5.findMonster)
---@param text string
---@param line2? string a second line under it
function coe5.ui.monsterRow(id, monster, text, line2) end

---Draws one line from (x1, y1) to (x2, y2) at UI coordinates, in r, g, b (each 0 to 1) at `alpha`
---(0 clear, 1 solid), `width` screen pixels wide. A line straight across or straight up the screen
---is drawn on whole pixels, a pixel it covers in part at that part of `alpha`, so it moves by
---fractions of a pixel with the map while the zoom glides. Only inside a MAP_EDITOR_DRAW or MAP_DRAW
---wrap; anywhere else it draws nothing (lua_refused reason=not_drawing). NaN or an infinity draws nothing
---(not_finite), and so does a width of 0 or less (out_of_range).
---@param x1 number
---@param y1 number
---@param x2 number
---@param y2 number
---@param r number
---@param g number
---@param b number
---@param alpha number
---@param width? number 1 when left out
function coe5.ui.drawLine(x1, y1, x2, y2, r, g, b, alpha, width) end

---Draws one line of text in the game's GUI font with its bottom left at (x, y), UI coordinates,
---`size` UI units high, in r, g, b at `alpha`. Only inside a MAP_EDITOR_DRAW or MAP_DRAW wrap, as drawLine.
---The text is cut to 499 bytes (logged as api_cut).
---@param x number
---@param y number
---@param size number
---@param text string
---@param r number
---@param g number
---@param b number
---@param alpha number
function coe5.ui.drawText(x, y, size, text, r, g, b, alpha) end

---Draws a filled rectangle with its bottom left corner at (x, y), UI coordinates, `width` across
---and `height` up, in r, g, b at `alpha`: a panel behind drawn text, or a square of fog over the
---map. Only inside a MAP_EDITOR_DRAW or MAP_DRAW wrap, as drawLine. A width or height of 0 or
---less draws nothing (out_of_range), and so does NaN or an infinity (not_finite).
---@param x number
---@param y number
---@param width number
---@param height number
---@param r number
---@param g number
---@param b number
---@param alpha number
function coe5.ui.drawRect(x, y, width, height, r, g, b, alpha) end

---Draws the game's icon for a resource as the next element of a screen's line: the icon the
---recruit screen shows beside a cost, as high as the text. In a row (coe5.ui.rowBegin) a
---coe5.ui.text after it sits beside it, and coe5.ui.tooltip after it is its hover text. `resource`
---is 0 Gold to 13 Lifeforce, the numbers slotResource takes. A resource outside 0 to 13 draws
---nothing (out_of_range), and so does a value that isn't a whole number (bad_type).
---@param id integer the click id, as coe5.ui.text's
---@param resource integer 0 Gold, 1 Iron, 2 Herbs, 3 Fungus, 4 Sacrifices, 5 Hand of Glory, 6 Enchanted Weed, 7 Rubies, 8 Sapphires, 9 Diamonds, 10 Emeralds, 11 Trade, 12 Relics, 13 Lifeforce
function coe5.ui.resourceIcon(id, resource) end

---Draws the game's icon for a resource with its bottom left corner at (x, y), UI coordinates,
---`size` UI units square. Only inside a MAP_EDITOR_DRAW, MAP_DRAW or BATTLE_DRAW wrap, as
---drawText. `resource` as coe5.ui.resourceIcon's. A size of 0 or less, or a resource outside 0
---to 13, draws nothing (out_of_range); so does NaN or an infinity (not_finite) and a resource
---that isn't a whole number (bad_type).
---@param x number
---@param y number
---@param size number
---@param resource integer 0 Gold to 13 Lifeforce
function coe5.ui.drawResourceIcon(x, y, size, resource) end

---How wide coe5.ui.drawText draws `text` at `size`, in UI units, by the game's own text measure, so
---a panel right aligns a number or cuts a long name exactly. Only in a preview world, inside a
---MAP_DRAW wrap. Windows only for now: on Mac the function doesn't exist.
---@param text string
---@param size number as coe5.ui.drawText's
---@return number|nil width nil when refused (lua_refused): no_preview_world, out_of_range, not_drawing, no_font
function coe5.ui.textWidth(text, size) end

-- ---- Game numbers ------------------------------------------------------------------------------
-- The game's own numbers that a script passes to CoE5SE or gets back, by name. The values are the
-- game's, and the same as the C names in jazzy_api.h (given with each table).

---The game's main menu buttons, for coe5.addMainMenuButton's afterId (C: MAIN_MENU_*). The ids
---the table skips (5, 7, 8) have no button.
---@class coe5.mainMenu
coe5.mainMenu = {
  startNewGame = 1,
  loadGame = 2,
  preferences = 3,
  network = 4,
  credits = 6,
  quit = 9,
  mods = 10,
  readTip = 11,
}

---What a Game Menu line's onClick returns (C: GAME_MENU_*).
---@class coe5.gameMenu
coe5.gameMenu = {
  stay = 0,    -- the menu stays open
  close = 1,   -- the menu closes, as Ok does
  resign = 6,  -- the player resigns, with no question asked
  save = 7,         -- the save dialog; the menu stays open
  saveAndQuit = 2,  -- the save dialog, then the game exits
}

---Random Map Settings' start positions for allied players, for coe5.startPreviewWorld's startPositions (C: START_POSITIONS_*).
---@class coe5.startPositions
coe5.startPositions = {
  random = 0,
  clustered = 1,
  westEast = 2,
}

---Random Map Settings' five map sizes, for coe5.startPreviewWorld's mapSize (C: MAP_SIZE_*).
---@class coe5.mapSize
coe5.mapSize = {
  small = 0,
  medium = 1,
  large = 2,
  huge = 3,
  enormous = 4,
}

---Which section of the Game Menu a line goes in, addGameMenuLine's `section` (C: GAME_MENU_SECTION_*).
---A name instead makes a section of the mod's own.
---@class coe5.gameMenuSection
coe5.gameMenuSection = {
  saving = 0,    -- the end of Saving & Exiting (the default)
  settings = 1,  -- the end of Settings, after the game's own lines
}

---The map editor's ids, as MAP_EDITOR_WAIT's next returns them (C: MAP_EDITOR_*).
---@class coe5.mapEditor
coe5.mapEditor = {
  hideToolbox = 2,  -- "Hide Toolbox" and Tab: the editor shows or hides its toolbox
}

---Map screen actions (C: MAP_ACTION_*).
---@class coe5.mapAction
coe5.mapAction = {
  specialPower = 5,  -- Use Special Power: a caster's ritual screen
  gameMenu = 6,      -- the Game Menu (F10)
  squareInfo = 96,   -- the square info window for the square under the pointer
  endTurn = 7,       -- End Turn (y): MAP_SCREEN_WAIT returning it ends the human turn
}

---The answers to the boxes that stop a turn, as their waits return them (C: BOX_*).
---@class coe5.box
coe5.box = {
  messagesOk = 1,     -- the Messages screen's Ok (MESSAGES_WAIT)
  warnOk = 1,         -- the warning box's Ok (WARN_WAIT): an elimination's box and the like
  firstButton = 11,   -- the message box's first button (MESSAGE_BOX_WAIT): the End Turn Warning's Yes
  secondButton = 12,  -- its second button
  cancel = 2,         -- its Cancel, or Esc
}

---Key values: the game's key codes, the same on Windows and Mac (C: KEY_*). A letter, digit, or
---punctuation key is its character's byte, string.byte("y"); these are the other keys. Add ctrl,
---alt, or ctrlShift (with a lowercase letter) for a combination: coe5.key.ctrl + string.byte("i")
---is ctrl+i. The map screen already binds F1 to F6 and F9 to F12; F7 and F8 are free.
---@class coe5.key
coe5.key = {
  none = 0,         -- no key
  backspace = 8,
  tab = 9,
  enter = 13,
  esc = 27,
  space = 32,
  delete = 127,
  f1 = 328,
  f2 = 329,
  f3 = 330,
  f4 = 331,
  f5 = 332,
  f6 = 333,
  f7 = 334,
  f8 = 335,
  f9 = 336,
  f10 = 337,
  f11 = 338,
  f12 = 339,
  pageDown = 340,
  pageUp = 341,
  endKey = 342,     -- End
  home = 343,
  up = 344,
  down = 345,
  left = 346,
  right = 347,
  ctrl = 100000,    -- added to a key: ctrl+key
  alt = 200000,     -- added to a key: alt+key
  ctrlShift = 300000,  -- added to a lowercase letter: ctrl+shift+letter
}

---coe5.netRole's answer (C: NET_ROLE_*).
---@class coe5.net
coe5.net = {
  single = 0,  -- single player and hotseat, and any menu before a game
  host = 1,    -- runs the game server: events, AI, battles, months
  client = 2,  -- connected to a game server: plays one player's turn
}

---coe5.sendMessage's player and result (C: MESSAGE_*).
---@class coe5.message
coe5.message = {
  everyHuman = -2,  -- every human player, as a .c5m event's `message -2`
  notAdded = -1,    -- the result when no message was added
}

---A player slot's controller, the slotController value (C: CONTROLLER_*).
---@class coe5.controller
coe5.controller = {
  removed = -1,     -- an empty slot
  human = 0,
  ai = 1,
  independent = 2,  -- the independents, slots 24-30, once the world is built
}

---A class ability id, classAbility's second index (C: CLASS_ABILITY_ID_*). The game's random class
---roll reads the first four.
---@class coe5.classAbility
coe5.classAbility = {
  aiClass = 203,          -- aiclass: who may play the class (coe5.aiClassValue)
  randomSkip = 204,       -- a percent: the random roll skips the class that often
  aiLevelMin = 210,       -- an AI below this difficulty level never rolls the class; 0 no bound
  aiLevelMax = 211,       -- an AI above this difficulty level never rolls the class; 0 no bound
  standardTroops = 222,   -- stdtroops: 1 when the class also recruits the standard troops
}

---An aiclass value, what classAbility gives for coe5.classAbility.aiClass (C: AI_CLASS_*): who may play
---the class.
---@class coe5.aiClassValue
coe5.aiClassValue = {
  humanOnly = -1,   -- an AI's random roll never gets the class
  any = 0,
  aiOnly = 1,       -- only an AI's random roll gets it, and Choose Class doesn't list it
  neither = 2,      -- no player gets it, by roll or by Choose Class
}

---A corpse kind, squareCorpses' third index (C: CORPSE_KIND_*), in the game's order. The humanoid kinds
---(Raise Dead's) are small, human, giant, vanara and bandar; the animal kinds (Raise Animals') smallAnimal,
---largeAnimal and markata.
---@class coe5.corpseKind
coe5.corpseKind = {
  small = 0,        -- a humanoid with small (Goblins, Hoburgs, Bakemono)
  human = 1,        -- any other humanoid of battle size 1
  giant = 2,        -- a humanoid of battle size 2 or 3
  smallAnimal = 3,  -- an animal with under 20 hp
  largeAnimal = 4,  -- an animal with 20 hp or more
  markata = 5,      -- ape 1
  vanara = 6,       -- ape 2
  bandar = 7,       -- ape 3
}

---GAME_OVER_WAIT's mode (C: GAME_OVER_MODE_*).
---@class coe5.gameOverMode
coe5.gameOverMode = {
  plain = 4,  -- nothing drawn behind the screen: plain black
  fog = 9,    -- what the game passes: fog and falling particles
}

---A new game's setup: PLAY_GAME_NEW_WORLD's mode, and the setup screens' results and ids (C:
---NEW_WORLD_*, MAP_SCREEN_OK, MAP_SETTINGS_*, PARTICIPANTS_OK).
---@class coe5.newGame
coe5.newGame = {
  normal = 0,
  network = 1,
  skipParticipants = 2,  -- any mode but 0 and 1 skips Setup Participants
  mapScreenOk = 1,       -- what NEW_WORLD_MAP_SCREEN returns on Ok (0 is Cancel)
  mapSettingsOk = 9,     -- Random Map Settings' Ok, as MAP_SETTINGS_WAIT returns it
  mapSizeFirst = 100,    -- its first map size button (Small); size n is 100 + n, to 104
  societyFirst = 200,    -- its first society button (Random); society n is 200 + n
  participantsOk = 1,    -- Setup Participants' Ok
}

---The ritual screen: why a ritual can't be cast, and its rows' ids (C: RITUAL_REASON_*, RITUAL_ROW_*),
---for RITUAL_SCREEN_ROW, RITUAL_SCREEN_CAN_CAST, and RITUAL_SCREEN_LOCATION_CHECK.
---@class coe5.ritual
coe5.ritual = {
  reasonMirror = -2,    -- needs a mirror monster
  reasonMaxcast = -3,   -- cast as often as its maxcast allows
  reasonImmortal = -4,
  reasonMastery = -6,
  reasonPlane = -7,
  rowCastable = 1000,   -- a castable ritual's row id is its slot + 1000
  rowGreyed = 12288,    -- a greyed-out one's is its slot + 12288 (0x3000)
}

---A battle target check's answers (C: TARGET_USEFUL, TARGET_NOT_AFFECTED), for BATTLE_TARGET_CHECK.
---@class coe5.target
coe5.target = {
  useful = 1,         -- the target is worth the spell or mind weapon
  notAffected = -3,   -- the weapon wouldn't affect it: casters pass it over
}

---The game's file browser (Load Game's list) and the square info window (C: FILE_BROWSER_*,
---SQUARE_INFO_OK), for FILE_BROWSER_ROW_DATE, FILE_BROWSER_WAIT, and SQUARE_INFO_WAIT.
---@class coe5.screen
coe5.screen = {
  fileBrowserRowFirst = 10000,  -- row n's id is 10000 + n
  fileBrowserDelete = 7,        -- what the browser's Delete key returns
  squareInfoOk = 1,             -- the square info window's Ok
}

---An AI commander's move (C: AI_MOVE_*, AI_NO_SQUARE, AI_EXPLORE_BELOW, AI_FAR_EXPLORE_BELOW), for
---the AI sites (AI_RALLY, AI_FRIENDLY_MOVE, AI_CONQUER, AI_EXPLORE, AI_FAR_EXPLORE) and
---commanderAiMoveKind.
---@class coe5.aiMove
coe5.aiMove = {
  none = 0,              -- no move planned: the commander rests
  target = 1,            -- a square from the rally, friendly move, or conquest step
  explore = 2,           -- an exploration, which the game goes on with next turn while the square stays unseen
  noSquare = -1,         -- x and y of no square
  exploreBelow = 50,     -- AI_EXPLORE runs only while the best score so far is below this
  farExploreBelow = 10,  -- the game's far exploration runs only while the best score is below this
}

-- BEGIN key actions (generated from key_actions.h: change that list, not this block)
---The key actions (C: KEY_ACTION_* in key_actions.h): one for each key of each line of the game's
---Keyboard Shortcuts list, for coe5.rebindKey, coe5.addActionKey, and coe5.actionKey. The
---comment gives the screen, the game's key, and the line.
---@class coe5.keyAction
coe5.keyAction = {
  mapShortcutsList = 1,          -- Map, ?: List keyboard shortcuts
  mapRecruit = 2,                -- Map, r: Recruit
  mapEndTurn = 3,                -- Map, y: End turn
  mapMessages = 4,               -- Map, m: View messages
  mapNextCommander = 5,          -- Map, n: Next commander
  mapPreviousCommander = 6,      -- Map, j: Previous commander
  mapCommanderDone = 7,          -- Map, Space: Commander is done for this turn
  mapTransfer = 8,               -- Map, t: Transfer troops
  mapSpecialPower = 9,           -- Map, p: Use commander's special power
  mapUnitOverview = 10,          -- Map, F1: Unit overview
  mapPlayerOverview = 11,        -- Map, F2: Player overview
  mapClassDescription = 12,      -- Map, F3: Class description
  mapScoreGraphs = 13,           -- Map, F5: View score graphs
  mapActiveMods = 14,            -- Map, F6: View active mods
  mapGameMenu = 15,              -- Map, F10: Settings, save, quit (the Game Menu)
  mapSquareInfo = 16,            -- Map, i: Square terrain info
  mapCommanderStats = 17,        -- Map, v: View commander stats
  mapTrade = 18,                 -- Map, l: Trade administration
  mapSentry = 19,                -- Map, s: Sentry commander
  mapLocationPower = 20,         -- Map, o: Use location's special power
  mapBurnForestScreen = 21,      -- Map, b: Burn forest screen
  mapBurnForestNow = 22,         -- Map, B: Burn forest immediately
  mapHuntSlaves = 23,            -- Map, h: Hunt for slaves
  mapPlaneShift = 24,            -- Map, u: Plane shift
  mapContinue = 25,              -- Map, ,: Continue moving
  mapContinueAll = 26,           -- Map, .: Continue moving for all commanders
  mapSquareDetails = 27,         -- Map, ctrl+i: Detailed square info
  mapAstrology = 28,             -- Map, ctrl+a: Astrology reading
  mapRepeatOrders = 29,          -- Map, ctrl+r: Execute all repeat orders now
  mapScry = 30,                  -- Map, alt+s: Scry
  mapCenter = 31,                -- Map, Home: Center map on starting citadel
  mapHideIcons = 32,             -- Map, Tab: Hide map icons (hold)
  mapZoomIn = 33,                -- Map, PgUp: Zoom in
  mapZoomOut = 34,               -- Map, PgDn: Zoom out
  mapScrollUp = 35,              -- Map, Up: Scroll map up
  mapScrollDown = 36,            -- Map, Down: Scroll map down
  mapScrollLeft = 37,            -- Map, Left: Scroll map left
  mapScrollRight = 38,           -- Map, Right: Scroll map right
  mapMoveUpLeft = 39,            -- Map, q: Move commander up-left
  mapMoveUp = 40,                -- Map, w: Move commander up
  mapMoveUpRight = 41,           -- Map, e: Move commander up-right
  mapMoveLeft = 42,              -- Map, a: Move commander left
  mapMoveRight = 43,             -- Map, d: Move commander right
  mapMoveDownLeft = 44,          -- Map, z: Move commander down-left
  mapMoveDown = 45,              -- Map, x: Move commander down
  mapMoveDownRight = 46,         -- Map, c: Move commander down-right
  mapLookUpLeft = 47,            -- Map, Q: Info on the square up-left
  mapLookUp = 48,                -- Map, W: Info on the square up
  mapLookUpRight = 49,           -- Map, E: Info on the square up-right
  mapLookLeft = 50,              -- Map, A: Info on the square to the left
  mapLookRight = 51,             -- Map, D: Info on the square to the right
  mapLookDownLeft = 52,          -- Map, Z: Info on the square down-left
  mapLookDown = 53,              -- Map, X: Info on the square down
  mapLookDownRight = 54,         -- Map, C: Info on the square down-right
  mapLookHere = 55,              -- Map, S: Info on the commander's square
  armyShortcutsList = 56,        -- Transfer troops, ?: List keyboard shortcuts
  armySelectAll = 57,            -- Transfer troops, a: Select/deselect all units
  armySelectAfflicted = 58,      -- Transfer troops, b: Select units with battle afflictions
  armyDeselectSlowAp = 59,       -- Transfer troops, c: Deselect units with fewer AP left
  armySelectFast = 60,           -- Transfer troops, f: Select all fast units
  armySelectHorror = 61,         -- Transfer troops, h: Select horror marked units
  armyShowItems = 62,            -- Transfer troops, i: Highlight units with magic items
  armyDeselectLandlubbers = 63,  -- Transfer troops, l: Deselect all landlubbers
  armySelectType = 64,           -- Transfer troops, m: Select the type under the pointer
  armySelectSlow = 65,           -- Transfer troops, s: Select all slow units
  armySelectWounded = 66,        -- Transfer troops, w: Select all wounded units
  armySelectTen = 67,            -- Transfer troops, +: Select 10 of the type under the pointer
  armyDeselectTen = 68,          -- Transfer troops, -: Deselect 10 of the type under the pointer
  armyScrollUp = 69,             -- Transfer troops, Up: Scroll battlefield up
  armyScrollDown = 70,           -- Transfer troops, Down: Scroll battlefield down
  armyScrollLeft = 71,           -- Transfer troops, Left: Scroll battlefield left
  armyScrollRight = 72,          -- Transfer troops, Right: Scroll battlefield right
  armySelectMenu = 73,           -- Transfer troops, F2: Select units menu
  armyExit = 74,                 -- Transfer troops, Esc: Exit screen
  battleShortcutsList = 75,      -- Battle, ?: List keyboard shortcuts
  battleScrollLeft = 76,         -- Battle, Left: Scroll battlefield left
  battleScrollRight = 77,        -- Battle, Right: Scroll battlefield right
  battleMessagesUp = 78,         -- Battle, -: Scroll battle messages back
  battleMessagesDown = 79,       -- Battle, +: Scroll battle messages forward
  battleMessagesPageUp = 80,     -- Battle, PgUp: Scroll battle messages a page back
  battleMessagesPageDown = 81,   -- Battle, PgDn: Scroll battle messages a page forward
  battlePause = 82,              -- Battle, Space: Pause battle
  battleTerrainOnly = 83,        -- Battle, Tab: View terrain only (hold)
  battleSlowMotion = 84,         -- Battle, z: Slow motion
  battleNormalSpeed = 85,        -- Battle, n: Normal speed
  battleFastForward = 86,        -- Battle, f: Fast forward
  battleSpeedUp = 87,            -- Battle, s: Speed up one notch
  battleSkip = 88,               -- Battle, q: Skip entire battle
  battleLog = 89,                -- Battle, l: Hide/show battle log
  battleGrid = 90,               -- Battle, g: Hide/show grid
  battleTerrain = 91,            -- Battle, t: Hide/show terrain
  battleUnitList = 92,           -- Battle, F1: View unit list
  mapEditorShortcutsList = 93,   -- Map editor, ?: List keyboard shortcuts
  mapEditorMenu = 94,            -- Map editor, Tab: Hide/show menu
  mapEditorPencil1 = 95,         -- Map editor, 1: Pencil size 1
  mapEditorPencil2 = 96,         -- Map editor, 2: Pencil size 2
  mapEditorPencil3 = 97,         -- Map editor, 3: Pencil size 3
  mapEditorClearSquare = 98,     -- Map editor, c: Clear special attributes for square
  mapEditorNameSquare = 99,      -- Map editor, n: Name square
  mapEditorGateway = 100,        -- Map editor, g: Create gateway
  mapEditorOverlay = 101,        -- Map editor, o: Toggle overlay on/off
  mapEditorQuit = 102,           -- Map editor, q: Quit map editor
  mapEditorRandomWorld = 103,    -- Map editor, r: Recreate random world
  mapEditorTerrainList = 104,    -- Map editor, t: Select terrain from a list
  mapEditorUnflood = 105,        -- Map editor, u: Unflood all squares
  mapEditorFixedTemp = 106,      -- Map editor, E: Set fixed temp on entire map
  mapEditorRemoveFancy = 107,    -- Map editor, F: Remove all fancy terrains
  mapEditorLoadOverlay = 108,    -- Map editor, O: Load overlay image
  mapEditorFixTemps = 109,       -- Map editor, T: Fix temperatures
  mapEditorCircularTemp = 110,   -- Map editor, ctrl+u: Circular temperature on map
  mapEditorLoad = 111,           -- Map editor, ctrl+l: Load map
  mapEditorDrawTemp = 112,       -- Map editor, ctrl+e: Enter temperature to draw with
  mapEditorResize = 113,         -- Map editor, ctrl+r: Resize map (scale)
  mapEditorSave = 114,           -- Map editor, ctrl+s: Save map
  mapEditorRemoveCold = 115,     -- Map editor, ctrl+t: Remove all cold temperatures
  mapEditorReduceSquare = 116,   -- Map editor, alt+r: Reduce square
  mapEditorDestroySquare = 117,  -- Map editor, alt+d: Destroy square
  mapEditorMoveUp = 118,         -- Map editor, U: Move entire map up
  mapEditorMoveLeft = 119,       -- Map editor, H: Move entire map left
  mapEditorMoveDown = 120,       -- Map editor, J: Move entire map down
  mapEditorMoveRight = 121,      -- Map editor, K: Move entire map right
  mapEditorZoomIn = 122,         -- Map editor, PgUp: Zoom in
  mapEditorZoomOut = 123,        -- Map editor, PgDn: Zoom out
  mapEditorScrollUp = 124,       -- Map editor, Up: Scroll map up
  mapEditorScrollDown = 125,     -- Map editor, Down: Scroll map down
  mapEditorScrollLeft = 126,     -- Map editor, Left: Scroll map left
  mapEditorScrollRight = 127,    -- Map editor, Right: Scroll map right
  mapEditorPlaneAbove = 128,     -- Map editor, <: Plane above
  mapEditorPlaneBelow = 129,     -- Map editor, >: Plane below
  mapEditorRefresh = 130,        -- Map editor, F5: Refresh map
  battleEditorShortcutsList = 131, -- Battle map editor, ?: List keyboard shortcuts
  battleEditorGroundMode = 132,  -- Battle map editor, Tab: Switch to/from ground edit mode
  battleEditorCenterLine = 133,  -- Battle map editor, ctrl+w: Show center line
  battleEditorReplacer = 134,    -- Battle map editor, r: Replacer
  battleEditorDraw = 135,        -- Battle map editor, q: Draw normally
  battleEditorLoad = 136,        -- Battle map editor, ctrl+l: Load
  battleEditorSave = 137,        -- Battle map editor, ctrl+s: Save
  battleEditorMonster = 138,     -- Battle map editor, m: Select monster
  battleEditorClear = 139,       -- Battle map editor, C: Clear all
  battleEditorScrollUp = 140,    -- Battle map editor, u: Scroll terrain up
  battleEditorScrollLeft = 141,  -- Battle map editor, h: Scroll terrain left
  battleEditorScrollDown = 142,  -- Battle map editor, j: Scroll terrain down
  battleEditorScrollRight = 143, -- Battle map editor, k: Scroll terrain right
  battleEditorNoDeploy = 144,    -- Battle map editor, alt+b: No deploy mark
  battleEditorCatapult = 145,    -- Battle map editor, alt+c: Catapult mark
  battleEditorEffigy = 146,      -- Battle map editor, alt+e: Effigy mark
  battleEditorMage = 147,        -- Battle map editor, alt+m: Mage mark
  battleEditorMelee = 148,       -- Battle map editor, alt+s: Melee soldier mark
  battleEditorDemonHand = 149,   -- Battle map editor, alt+d: Demon hand mark
  battleEditorDemonHead = 150,   -- Battle map editor, alt+h: Demon head mark
  battleEditorFirewall = 151,    -- Battle map editor, alt+v: Firewall mark
  battleEditorNoTrees = 152,     -- Battle map editor, alt+t: No random trees/bushes here
  battleEditorClearNoTrees = 153, -- Battle map editor, alt+y: Clear no random trees/bushes mark
  recruitClearHere = 154,        -- Recruitment, c: Clear auto-recruitment at this location
  recruitClearAll = 155,         -- Recruitment, v: Clear auto-recruitment everywhere
  recruitLocation1 = 156,        -- Recruitment, 1: Recruitment location 1
  recruitLocation2 = 157,        -- Recruitment, 2: Recruitment location 2
  recruitLocation3 = 158,        -- Recruitment, 3: Recruitment location 3
  recruitLocation4 = 159,        -- Recruitment, 4: Recruitment location 4
  recruitLocation5 = 160,        -- Recruitment, 5: Recruitment location 5
  recruitLocation6 = 161,        -- Recruitment, 6: Recruitment location 6
  recruitLocation7 = 162,        -- Recruitment, 7: Recruitment location 7
  recruitLocation8 = 163,        -- Recruitment, 8: Recruitment location 8
  recruitLocation9 = 164,        -- Recruitment, 9: Recruitment location 9
  recruitPrevious = 165,         -- Recruitment, Left: Previous recruitment location
  recruitNext = 166,             -- Recruitment, Right: Next recruitment location
  recruitShortcutsList = 167,    -- Recruitment, ?: List keyboard shortcuts
  recruitExit = 168,             -- Recruitment, Esc: Exit recruitment
  recruitExitEnter = 169,        -- Recruitment, Enter: Exit recruitment, second key
  participantsAdd = 170,         -- Setup participants, n: Add new player
  participantsColors = 171,      -- Setup participants, r: Randomize player colors by team
  participantsAllHuman = 172,    -- Setup participants, H: Set all players to human controlled
  participantsAllAi = 173,       -- Setup participants, A: Set all players to AI controlled
  participantsAiUp = 174,        -- Setup participants, +: Increase all AI levels by 1
  participantsAiDown = 175,      -- Setup participants, -: Decrease all AI levels by 1
  participantsRemove = 176,      -- Setup participants, Backspace: Remove last player
  participantsOk = 177,          -- Setup participants, Enter: Ok
  participantsCancel = 178,      -- Setup participants, Esc: Cancel
  viewArmyShowItems = 179,       -- Army on the battlefield, i: Highlight units with magic items
  viewArmyShortcutsList = 180,   -- Army on the battlefield, ?: List keyboard shortcuts
  viewArmyScrollLeft = 181,      -- Army on the battlefield, Left: Scroll battlefield left
  viewArmyScrollRight = 182,     -- Army on the battlefield, Right: Scroll battlefield right
  viewArmyTerrainOnly = 183,     -- Army on the battlefield, Tab: View terrain only (hold)
  viewArmyExit = 184,            -- Army on the battlefield, Enter: Exit screen
  viewArmyExitEsc = 185,         -- Army on the battlefield, Esc: Exit screen, second key
  unitListCommanders = 186,      -- Battle unit list, c: Only show commanders
  unitListShowAll = 187,         -- Battle unit list, A: Show all (terrain units too)
  unitListShortcutsList = 188,   -- Battle unit list, ?: List keyboard shortcuts
  unitListExit = 189,            -- Battle unit list, Enter: Exit screen
  unitListExitEsc = 190,         -- Battle unit list, Esc: Exit screen, second key
  fileBrowserDelete = 191,       -- Load game, Delete: Delete save file under pointer
  fileBrowserDeleteCtrl = 192,   -- Load game, ctrl+d: Delete save file, second key
  fileBrowserSortTime = 193,     -- Load game, ctrl+t: Sort by time
  fileBrowserSortName = 194,     -- Load game, ctrl+n: Sort by name
  fileBrowserShortcutsList = 195, -- Load game, ?: List keyboard shortcuts
  fileBrowserCancel = 196,       -- Load game, Esc: Cancel
}
-- END key actions
