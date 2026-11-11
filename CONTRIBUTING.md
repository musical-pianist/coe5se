# Contributing to CoE5SE

Pull requests are welcome. For anything bigger than a fix, open an issue first, so we agree on it before you build it.

## Sending a pull request

1. Fork this repository and branch from `dev`.
2. Build with `src/build.sh` for Windows and `src/build_mac.sh` for Mac. Each names the compiler it needs.
3. Both write into the repository's root. Copy what they build into your installed CoE5SE folder, and test in the game on the CoE5 patch named in the latest release's title.
4. [Sign off](#signing-off) every commit, then open a pull request. GitHub sets its base branch to `main`, so change it to `dev`.

I build and test each pull request before I merge it into `dev`, and it ships in the next release. I squash your commits into one, with you as its author. `main` holds only releases.

## Guidelines

I look for these in every pull request. When one doesn't fit your change, say why in its description.

- **Vanilla stays vanilla.** With only CoE5SE installed, the game should play as vanilla. A balance change, an AI change or a fix to a game bug typically belongs in a "Community Patch" I am happy to endorse. Graphics and turn optimizations, multiplayer through Steam relay, and further game mappings or mod commands are great examples of what CoE5SE takes as contributions.
- **Everything dynamic.** CoE5SE tries to set a standard similar to vanilla's rituals, where the strings are the unique ids and all else is managed by `modprio` and CoE5SE. This is broken on occasion, but it is the end goal for all extended functionality.
- **Follow vanilla patterns.** When CoE5SE is implementing a new feature and its behaviour is in question, the answer is almost always to follow how vanilla patterns its tables, commands, savegames, mod loading, etc. For example, vanilla prioritizes a mod's local copy and ignores its duplicate on Steam. CoE5SE will not change this behaviour.
- **Extend before inventing.** Give an existing mod command a new argument, or raise a limit, before you add a new command. `spawnoffs`, for example, takes a monster's name as well as the game's offset. A new command should look and behave like what's expected in vanilla.
- **Windows and Mac.** A change works on both. If you could build or test only one, say so in the pull request, and I'll do the other.

## Patterns that may get your pull request rejected or delayed

If you're unsure about one, ask in an issue first.

These usually get turned down:

- Commits without a sign-off.
- A change that lets a script reach outside the game: files, processes, the network, memory addresses, or another mod's state.
- Anything that touches Illwinter's official servers.
- Executables or other compiled files.
- Anything that pulls from the internet while the game runs, like Google Fonts.
- Obfuscated code of any kind.

These take longer to merge:

- A change tested on only Windows or only Mac.
- Big blocks of comments in the code (see [Comments](#comments)).

## Comments

- One short line above each function, struct, enum or section, saying what it is or returns.
- A new section in a big file starts with a header line: `// ---- Name ----`.
- Where Windows and Mac take different routes, the comment's last line is `// @platform-split <name>: <what differs>`.
- Why you made the change, what you tried and how it works go in the pull request's description, not in the code.
- If you'd like to keep a longer note, start it with `// @internal`. It stays in the source, and the release leaves it out.

For example, from `src/frozen_stub/loader_start.h`:

```c
// Writes the game's Workshop content folder to `out`; 0 when it doesn't fit.
static __attribute__((unused)) int findWorkshopRoot(const char *gameDir, char *out, size_t size) {
```

Not:

```c
// Writes the Workshop folder of the game's Steam library to `out`: the nearest folder above
// `gameDir` named steamapps (in any case), plus workshop/content/<app id>, so a game moved deeper
// (a Mac .app bundle) still finds it.
```

## Adding a game address

An address goes in `src/game_addresses.h` as `ADDR(<Windows>, <Mac>)`. Write the Windows address as an offset from `CoE5.exe`'s image base, and the Mac address as its full address in `coe5_mac`. If you found it on one build only, say so, and I'll look up the other.

`address_table.h` and `game_address_ids.h` are made from `game_addresses.h`. Change them by hand if your build needs the new address; I regenerate both when I take your change.

These files are generated, and each release overwrites them. If one needs to change, say how in the pull request:

- every `*_sites.h` and `*_sites_mac.h` except `lua_sites.h`
- `c5m_words.h`, `record_words.h`, `ritual_effect_names.h` and `stub_hash.h`

## Signing off

Every commit needs a sign-off line, which `git commit -s` adds:

```
Signed-off-by: Your Name <you@example.com>
```

Your GitHub username and its no-reply address are fine. Signing off means you agree to the terms below. To sign off commits you already made, run `git rebase --signoff dev`, then force-push.

## Contribution terms

CoE5SE's license requires my permission before anyone changes its files. You have it for one purpose: you may change CoE5SE's source to build it, test it, and send the change to this repository.

When you sign off a commit, you confirm that:

1. You wrote the change, or you have the right to send it under these terms.
2. You give musical_pianist, and whoever owns CoE5SE after them, a perpetual, worldwide, non-exclusive, royalty-free, irrevocable license to use, copy, change, sublicense and distribute the change, under CoE5SE's license or any other terms.
3. You understand that the change and your sign-off are public, and stay in this repository's history.

You keep the copyright on your change.

## Reporting a problem

Report bugs in this repository's issues, and security problems through CoE5SE's Nexus Mods page, per `SECURITY.md`.
