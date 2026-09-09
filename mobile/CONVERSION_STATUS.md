# ALSAEQA Mobile Conversion

The original Unreal project remains in this repository as the source/reference project. The mobile runtime is being rebuilt under `mobile/` as a real Android application.

## Current foundation

- Godot 4 Android-first runtime project.
- GitHub Actions Android APK pipeline; no local development environment is required on the user's device.
- Persistent progression state with local save data.
- Canonical 70-stage progression ported to mobile data.
- Mobile cinematic director for story beats, combat impacts, heavy finishers, thunder impacts, and automatic stage transitions.
- Mobile hero controller with walking, sprinting, rolling, light/heavy attacks, interaction input, and thunder charging.
- Stage 1 progression preserved exactly: 5 counted workers must reach safety and the designated slaver must be defeated. The additional mine guards create pressure but are not an extra progression gate.

## Asset policy

No placeholder art is being promoted as final game art. The real ALSAEQA asset packs remain the source of visual content and will be imported/converted into mobile-ready formats as the conversion advances.

## Quality target

The mobile build keeps the original ALSAEQA identity: third-person adventure, cinematic storytelling, dramatic combat, thunder powers, rescues, companions, mounts, and connected stages. Rendering and asset budgets are optimized for Android rather than replacing the game's identity with a browser-style experience.
