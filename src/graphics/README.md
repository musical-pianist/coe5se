# Built-in map rendering and frame controls

This integrates the tested FpsOfElysium 0.1.10 renderer into CoE5SE's own native module. It does not add an external plugin, change DLL approvals or require an adapted loader. The separate local rendering profiler is not included.

## Controls

- CoE5SE Mod Settings → **Smoother map rendering**: on by default on supported Windows CoE5 5.43. Off uses the original decoration drawing path.
- Preferences → Graphics 1: adds **144, 165, 240 and 360 Frames per second** alongside the original bounded choices. A legacy unlimited choice is migrated to the bounded 360 target. Native whole-millisecond timing is approximate; the tooltip explains this.
- Preferences → Graphics 2 → **Precise frame rate limit**: off by default. Uses the selected target with QPC and high-resolution one-shot waits; no spin loop. While enabled, background rendering is limited to 30 FPS. The original game clock and unrelated sleeps are retained.
- Preferences → Graphics 2 → **Wait for GPU each frame**: exposes the game's existing glFinish setting. This is independent of VSync. The game's current default is preserved unless a saved preference or explicit launch argument applies.

FPS/GPU-wait/precise preferences live in `CoE5SE-Graphics.ini` beside CoE5SE's module and are replaced atomically. The rendering toggle uses CoE5SE's existing Mod Settings storage. Explicit launch arguments retain priority on restart. Existing standalone FpsOfElysium settings are not automatically imported.

The limit is a target, not a promise that a slow scene reaches it. Higher throughput can increase hardware load. No overclock, voltage, power-limit or graphics-quality changes are made.

## Rendering and refusal

Low/Medium/High and Highest/Ultra keep their original object generation, sorting and order. Compatible consecutive decoration quads are batched using up to eight texture-array pages. Original UVs, vertex colors, RGBA8 textures, complete existing mip chains and filtering are retained. Array cache storage is bounded to 128 MiB; updated/deleted textures invalidate their cached layers and released storage is reused. Unsupported textures, contexts and graphics states use original drawing. Reserved texture and sampler bindings are restored at native boundaries.

The complete Windows 5.43 executable must match SHA-256 `489d0b82fa790ec36e69a23e2335d0c2048fa57f4051fe8159c64f2d33508e55`. Graphics imports and eight code calls are checked before renderer installation. The optional timing path has a separate import/call check. Conflict diagnostics identify the affected function/module; this does not detect arbitrary inline hooks or all later third-party mutations.

The module uses the core API's existing UI/logging/wrapper functions. It exposes no new Lua API, raw addresses, file access or native-plugin loading. Its additional Windows addresses are named centrally in `game_addresses.h`; existing mappings are reused. Mac addresses that have not been investigated are explicitly marked `MAC_NOT_LOOKED_UP`. The generated address tables are left for the maintainer to regenerate, as described in CONTRIBUTING.md; the Windows module directly includes the central mapping.

## Validation

Windows x64, CoE5 5.43, CoE5SE dev at `b1150f2786e01ba4ebd484cd4633005e003a79cb`, AMD RX 7900 XTX:

- Normal `src/build.sh` builds the integrated core, loader and frozen stub.
- `src/graphics/tests/run_windows.sh` checks 20,000 cache updates/deletions, wrapped/full hash-table cases, all 48 import conflicts, all eight changed callsites, both quality branches, frame-choice persistence/migration/priority, precise-pacing guards/passthrough/live switching and null/API/foreign-executable refusal.
- Actual isolated core launches cover all five qualities with the precise option off/on. The original external FPS plugin is absent. The existing loader approval mechanism is retained.
- An optional `COE5SE_GRAPHICS_ORACLE` test build compares the original and candidate against the same restored framebuffer in the integrated core. Three images per quality at 1280 × 720 found 0–5 differing RGBA channel values out of 3,686,400, each at most 1/255. Background restoration and checked final state matched; both paths reported zero GL errors. This is the existing isolated rounding class, not bit-exact identity or proof for every scene/GPU.

`tests/image_oracle.h` is included only with that compile-time test flag. Default builds include no framebuffer readback, captures or profiling. The oracle is for correctness, not performance measurement.

No new foreground whole-game FPS benchmark is claimed by this integration. NVIDIA/Intel hardware execution, long sessions, every plane and the Mac optimization remain unverified. On Mac this module is a no-op, preserving the existing renderer and timing behavior; a Mac build/runtime check is requested from the maintainer. The contribution does not claim a completed Mac port.

## Credits

Renderer and frame-control work by Chazammm, adapted from [FpsOfElysium](https://github.com/Chazammm/FpsOfElysium). Thanks to musical_pianist for the texture-array-batching contribution that informed the multi-page and state-tracking work. Original FpsOfElysium code retains its MIT notice in `LICENSE.FpsOfElysium.txt`; the upstream CoE5SE license is unchanged.
