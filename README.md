# Conquest of Elysium 5 Script Extender (CoE5SE)

[![Latest release](https://img.shields.io/github/v/release/musical-pianist/coe5se?label=latest)](https://github.com/musical-pianist/coe5se/releases/latest)
[![Docs](https://img.shields.io/badge/docs-coe5se--docs-blue)](https://musical-pianist.github.io/coe5se-docs/)

CoE5SE is the script extender for Conquest of Elysium 5. It raises the game's limits, and lets modders call other aspects of the game never before possible. Custom Planes? Smarter AI? New and unique UI? All are possible with CoE5SE. Built for Windows and Mac.

This repository contains CoE5SE's full source, and until it's on other platforms (goal is nexus) you can verify its safety with the tools included. See [Verify This Build](#verify-this-build).

## Install

Run the command that matches your OS. The scripts are in the [CoE5SE Installer](https://github.com/musical-pianist/coe5se-installer) repo.

Windows:

```
irm https://raw.githubusercontent.com/musical-pianist/coe5se-installer/main/install.ps1 | iex
```

Mac:

```
curl -fsSL https://raw.githubusercontent.com/musical-pianist/coe5se-installer/main/install.sh | bash
```

Or install by hand: download a release zip from https://github.com/musical-pianist/coe5se/releases and read how-to-install.html inside it.

## CoE5SE Features

### For Players

CoE5SE makes all of this possible:

- You can have way more mods before running into limits.
- Set each AI anywhere from Piss Boy to Emperor in a game you're already playing, and turn battle reports, score graphs, and common cause on or off.
- Save your whole new-game setup (map, society, every player slot, every AI's difficulty) as a preset, and stop forgetting your settings between campaigns.
- Restart from the in-Game Menu. No more resign, wait 10 seconds, Ok, repick every campaign detail, repeat. Just true Civ goodness.
- Change any key of the game's Keyboard Shortcuts list: 196 actions on 10 screens.
- Various UI improvements. Delete saves from Load Game, and see ritual needs at a glance. Useful for when modders forget part of the description.
- Each mod can have its own settings to customize your experience.
- More and better error reports. Know when potentially save-breaking changes occur, like mods moving or replacing spells.

Presets, restarts, keys and UI changes come from the [Jazzy mods](https://github.com/musical-pianist/coe5se-mods-suite). I'm a pianist ok? Aren't we all weird?

### For Modders

Write Lua scripts that read units, battles, players, classes, the world, rituals, spells and planes, and spawn and move units. Add main menu buttons, Game Menu lines, screens and key bindings. Now's your chance to truly craft scripted campaigns with story, custom worlds, and weapons like anti-tank missiles. Every function is in the Lua reference, [`coe5.lua`](coe5.lua).

Go past the game's limits, in single player and network games alike:

<!-- limits start -->
| Mods can add | Without CoE5SE | With CoE5SE |
| --- | --- | --- |
| Rituals | 2,250 | 10,000 |
| Spells | none | 3,000 |
| Magic paths | none | 58 |
| Custom planes<br>(like the Chaos Wastes) | none | 8 |
| Factions | none | 16 |
| Weapons | 3,799 | 5,799 |
| Monsters | 3,999 | 4,999 |
| Magic items | 699 | 1,699 |
| Classes | 169 | 219 |
<!-- limits end -->

Every other cap CoE5SE sets, down to the length of a spell's name, is on the [Limits](https://musical-pianist.github.io/coe5se-docs/commands/limits/) page.

Put CoE5SE's commands in a .c5se next to your .c5m, or in the .c5m on lines starting with `#!@`. The vanilla game skips both. See [Files](https://musical-pianist.github.io/coe5se-docs/commands/#where).

Use new commands in your mod:

- Write your own [spells](https://musical-pianist.github.io/coe5se-docs/commands/spells/), with their own path, level and effect, and give your units new magic to cast.
- Add your own [magic paths](https://musical-pianist.github.io/coe5se-docs/commands/spell-paths/), with their own names (Chaos, Runecraft, Adeptus Mechanicus Battle Hymns), and share them with other mods by name.
- Give your spells the [limits and effects](https://musical-pianist.github.io/coe5se-docs/commands/spell-modifiers/) the game's own spells have: a slow cast, no casting at sea, a bonus for the army of whoever knows it.
- Give a monster [Small or Combat Caster](https://musical-pianist.github.io/coe5se-docs/commands/monster-traits/), and take off any one trait while keeping the rest.
- Write your own lists of [commander names](https://musical-pianist.github.io/coe5se-docs/commands/commander-names/), so your Dwarf Captains go by Gimli and Balin.
- Point at another monster or ritual [by its name](https://musical-pianist.github.io/coe5se-docs/commands/monster-names/), so that, as example, you can set up your mod to affect evolutions and units that don't have to worry about other mods.
- The game gives recruitment a fixed table of bonuses. With CoE5SE you [define the table](https://musical-pianist.github.io/coe5se-docs/commands/recruitment/). Make everything rare with talent hard to find, or drown your users in mages.
- Give your rituals [additional effects](https://musical-pianist.github.io/coe5se-docs/commands/ritual-effects/) not supported by the base game.
- Decide which [corpse](https://musical-pianist.github.io/coe5se-docs/commands/corpses/) each monster leaves, and what your rituals and events raise from the dead.
- Add your own [planes](https://musical-pianist.github.io/coe5se-docs/commands/planes/) to the world. Build one the way the game builds its planes, read it from your own map file, or fill it with your own terrain and monsters.
- Make your own independent [factions](https://musical-pianist.github.io/coe5se-docs/commands/factions/), like the game's Inferno or Hades. These are not playable factions but rather the ones that inhabit terrain and react on events.
- Give your [events](https://musical-pianist.github.io/coe5se-docs/commands/events/) if-then logic, so a single event can have multiple decision trees or checks within the game.
- [Arrange your mod](https://musical-pianist.github.io/coe5se-docs/commands/other-commands/#include) in as many .c5se files and folders as you wish, for your own organization: by faction, by region, or by units, spells, and events.
- Ensure another mod's Lua scripts load before yours with a [`requires`](https://musical-pianist.github.io/coe5se-docs/commands/mod-loading/#requires) line in your .c5m. This is useful when you are making a mod on top of another mod or as a "core" and various additions mod strategy.
- Put [`needs_script_extender`](https://musical-pianist.github.io/coe5se-docs/commands/mod-loading/#needs-script-extender) in your .c5m. This is ignored by the Script Extender yet would be seen by the vanilla game and fail to load the mod, so players can't dummy themselves into confusion.

Tie your mod's variables to [settings](https://musical-pianist.github.io/coe5se-docs/commands/mod-settings/), and you can craft a more dynamic experience around each player's preferences.

Every command, with examples, is in the [CoE5SE docs](https://musical-pianist.github.io/coe5se-docs/). [ToolsAndExamples](ToolsAndExamples/) has MachineCult, an example mod.

## Uninstall

Disable CoE5SE on the Mods screen. To remove it, delete glu32.dll or coe5se.dylib from the game folder and the CoE5SE folder from your mods folder.

## Safety

Let me start with a cop out. All other script extenders I can find didn't have good safety standards and trusted mods explicitly. SKSE, Cyberpunk, whole teams. When you download this base mod and then other mods on top, you DO carry some risk. For now, it's just me, but this might change. That said, I'm actively working on measures for those of us security minded.

As of right now, here are the measures applied:

- This script extender only supports Lua. I tried very hard to make it so exploits cannot happen by other mods, including reading Factorio white papers (my extender is inspired by their architecture). Each mod's script runs in its own sandbox, without Lua's io, os, package and debug libraries. A script reads Lua files only from its own mod folder and writes only to its own data folder. CoE5SE refuses every other mod's DLL, and no script runs in network games.
- CoE5SE has no network access and no registry access. The only program it ever starts is the game itself, when it restarts CoE5 for you.
- On a game build it doesn't know, CoE5SE asks before it runs, with No as the default. It checks the game's code before changing it and skips any spot that doesn't match.
- With CoE5SE disabled on the Mods screen, the game runs unmodified.

[SECURITY.md](SECURITY.md) lists everything CoE5SE loads, writes and patches, with where each one lives in [`src/`](src/).

### Verify This Build

GitHub builds every release from this repo's source and signs each file it builds. [The workflow](.github/workflows/release.yml) shows each step. To check a download, run the [GitHub CLI](https://cli.github.com/) on any DLL, dylib or release zip:

```
gh attestation verify glu32.dll -R musical-pianist/coe5se
```

You can also rebuild them yourself with windows-verify.cmd or mac-verify.command from the release zip. Each rebuilds every DLL and dylib from src and says OK only on a byte-for-byte match.

### Use at Your Own Risk

CoE5SE rewrites the game's code while it runs. That is how it does what a .c5m can't, and how it can crash, ruin saves, or just cause you a generally unpleasant time. A game update can break it, and every mod on it, until I update it. Also just... many games with mod support period are vulnerable. There is no warranty. I will not help recover save files.

## Reporting a Problem

Open an [issue](https://github.com/musical-pianist/coe5se/issues) with what happened and the enabled mods, and we might progress to looking at logs (`coe5mod.log` in the game folder) if the issue isn't obvious. For a problem in a mod's script, turn on "Enable copious logs" on CoE5SE's Mod Settings page first.

Please give your critique and offer your support! Most script extenders aren't built in a couple of days and take teams. It takes time to get this right.

## Not Affiliated with Illwinter

Illwinter makes Conquest of Elysium 5. CoE5SE is not their work; I'm one overly enthusiastic fan. A problem with CoE5SE goes in this repository's issues, not to Illwinter.

Script extenders have been around in PC modding for a long time (SKSE, F4SE; the name is a nod to them), and studios have mostly left them alone. That doesn't mean Illwinter has to. This project continues as long as Illwinter is fine with it. If they aren't, this be all but smoke.

## Why Did I Make This Mod

At a base level, I made this for myself. All the "Jazzy" mods above I've been wishing for since I first played this game! That said, I'm not a traditional modder. I hate creative writing and I hate sprites. So I decided my contribution to the community would be make other's stories a little bit easier to write and play. Hopefully, I'm not 5 years late to the table on this. If I am and there's no adoption, well, at least I made the mods I need to have a good time lol.

## License

Terms of use: [LICENSE.txt](LICENSE.txt). The example mods in `ToolsAndExamples/` and `coe5.lua` are free to copy and change for your own mods.
