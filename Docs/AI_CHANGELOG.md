# ALSAEQA Engineering Change Log

## 2026-09-11 — Hero animation pipeline repair

### Scope
Repair only the real canonical-hero animation attachment step. No gameplay/UI/world changes were made in this change.

### Previous failure
- Workflow run: `34626291983`
- Job: `android`
- Failed step: `Attach real Universal Animation Library to canonical hero`
- Earlier observed failure: the pipeline selected `Universal Animation Library 2/Unity/UAL2_Standard.glb` and found only one compatible authored clip.
- Result: Godot validation/runtime/APK steps were skipped.

### Root causes identified
1. Candidate selection preferred the wrong `UAL2_Standard.glb` merely because it contained `standard` in its path.
2. The importer can keep authored clips as global Blender Action datablocks instead of assigning every clip to the donor armature's active action/NLA list.
3. Probe runs were not removing unused Action datablocks, allowing stale animation data to survive between candidate probes.

### Change made
Commit: `ad613176153cd0299fa3f67b0fdb64a52c6ec8e9`

- Prefer the Godot-targeted `AnimationLibrary_Godot_Standard` asset when present.
- Rank animation-library candidates explicitly instead of selecting the first lexical `standard` path.
- Remove unused Blender Action datablocks during scene reset.
- Track actions introduced by the current donor import.
- Collect assigned, NLA, and newly imported authored Action datablocks.
- Print candidate armature/action counts and every candidate clip for diagnosis.
- Keep the strict requirement of at least 3 real authored compatible clips.
- Keep real pose-change verification before export.
- Keep the fixed canonical male hero; no protagonist replacement.

### Required verification
The next CI run must prove, in order:
1. At least 3 real authored animation clips are discovered.
2. At least 3 clips pass bone compatibility with the canonical hero.
3. Pose verification reports changed bones.
4. Animated canonical hero GLB is exported.
5. Godot validation succeeds.
6. Runtime visual inspection artifact is produced.
7. Android APK export succeeds.

If any item fails, stop and record the exact failure before making another unrelated change.
