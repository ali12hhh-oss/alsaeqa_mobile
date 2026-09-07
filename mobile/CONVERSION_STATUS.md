# ALSAEQA Mobile Conversion

The original Unreal project remains the source/reference project. The mobile runtime is rebuilt under `mobile/` as a real Android application.

## Foundation

- Godot 4 Android-first runtime.
- GitHub Actions builds the Android application without requiring Unreal/Unity/Godot/Android Studio on the user's device.
- Persistent progression state and canonical 70-stage progression data.
- Mobile cinematic director for story beats, combat impacts, heavy finishers, thunder impacts, and automatic stage transitions.
- Mobile hero controller with walking, sprinting, rolling, light/heavy attacks, interaction input, and thunder charging.
- Stage 1 preserved exactly: 5 workers must reach safety and the designated slaver must be defeated; additional guards create pressure and are not an extra gate.

## Real asset integration

The complete canonical source is pulled from the original ALSAEQA Releases at build time:

- `ALSAEQA_REAL_ASSETS.zip`
- `ALSAEQA_EXTRA_MONSTERS.zip.zip`
- `ALSAEQA_EXTRA_ULTIMATE_MODULAR_RUINS.zip.zip`

CI verifies every archive by SHA-256, expands nested archives, preserves the original source tree in `build/source_assets/`, inventories the source files, converts editable 3D source models to mobile-friendly GLB, and copies compatible textures/audio/data files into `mobile/assets/converted/` before Godot import.

The runtime bridge `scripts/real_asset_world.gd` binds the converted library to logical roles (hero, workers, guards, beasts, environment) without hard-coding source filenames. This keeps the art replaceable/editable without rewriting gameplay code.

**Important:** the original source assets remain the authoritative editable source. Generated GLB files are runtime derivatives, not replacements for the source files.

## Visual policy

Procedural primitive art is not treated as final ALSAEQA art. The mobile scene now references the real-asset bridge instead of the former procedural visual blockout.

## Quality target

The mobile build keeps ALSAEQA's identity: third-person adventure, cinematic storytelling, dramatic combat, thunder powers, rescues, companions, mounts, creatures, and connected stages. Rendering, texture sizes, meshes, animation budgets, and loading are optimized for Android without changing the game's core identity.
