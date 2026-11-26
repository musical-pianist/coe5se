# Dev notes

What `dev` has gained since the last release, one line per change. CONTRIBUTING.md, "Dev notes", shows how to add yours.

---

## Features

- Windows players can enable batching for textured scrollbars and tooltip frames.
- Windows players can enable faster world map texture preparation and reuse prepared opaque terrain when zooming or panning.
- Content mods can benefit from an optional Windows cache for repeated numeric `+player` arguments during turn-start rule processing.
- Native graphics plugins can call `jazzyBeforeTextureUpdate` before modifying a cached texture on the game render thread.

## Bugs

- World-map texture preparation checks layer transfers, preserves other mods' GL errors, releases tile snapshots when the rendering context changes and handles external army sprite bounds with MSAA enabled. Army texture copies are spread across map draws.
- Texture batching preserves rectangle textures across texture units and uses original textures when a render context moves between threads. Deferred army uploads retain reusable layers, and rejected sprites can yield cache slots to new textures.
- With native plugins loaded, texture batching checks rectangle targets and sampling state between primitives. It respects legacy shaders, per-unit LOD bias and mixed mip formats. Exited render threads release their cache registrations for reuse.

## Lua

- Owned test fields support additional stock fortifications and sampled Elemental Plane locations on Windows. `coe5.testFieldTerrainInfo` reports the loaded fort parts and availability.
