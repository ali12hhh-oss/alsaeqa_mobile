# ALSAEQA — Hero Real-Art Finalization

## Approved source
- Asset: Quaternius Universal Base Characters
- Source: https://quaternius.com/packs/universalbasecharacters.html
- License: CC0
- Intended use: commercial game, modification and combination permitted by the source license.

## Final hero specification
- Role: Hero
- Presentation: young male protagonist
- Story state: poor/exploited mine worker at the opening
- Clothing: worn worker outfit, dusty and visibly weathered
- Face/hair: selected from the Universal Base Characters options and customized for ALSAEQA
- Visual identity: ALSAEQA-specific materials/colors/accessories; not a stock character presentation
- Rig: humanoid
- Gameplay animation contract: idle, walk, run, directional locomotion, attacks, hit reaction, death, dodge/traversal and thunder abilities

## Unreal destination
`/Game/Art/Characters/Hero/`

The runtime visual component accepts a real imported `USkeletalMesh` and optional animation blueprint/material overrides through `UALSAEQAArtProfile`. It has no primitive/procedural/placeholder fallback.

## Important binary status
The repository tooling can commit UTF-8 source files, but it cannot upload binary FBX/GLB/`.uasset` payloads. Therefore this document does **not** falsely claim that the hero binary is already imported into Unreal.

The hero is approved and fully specified; final binary completion is counted only after the actual Quaternius files are imported into Unreal Editor and an editor audit confirms a real `USkeletalMesh` under `/Game/Art/Characters/Hero/`.

## Acceptance gate
1. Approved Quaternius source archive acquired.
2. Hero mesh imported to `/Game/Art/Characters/Hero/`.
3. Materials/textures assigned.
4. Humanoid animations retargeted.
5. `Hero` art profile points to the imported mesh and animation blueprint.
6. In-editor play test confirms the hero renders and animates.
7. No primitive/procedural/placeholder visual is present.
8. Android performance/LOD check passes.

Until all eight gates pass, the hero remains **approved / binary import pending**, not falsely marked complete.
