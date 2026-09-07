# ALSAEQA — Real 3D Art Production Batch 02

This batch converts the visual pass from a procedural-only design into a real imported-asset production pipeline.

## Locked visual rule

Normal gameplay must use authored 3D meshes and skeletal animation. Primitive/procedural geometry is only an emergency fallback and must never be treated as finished art.

## Character lineup

| ID | Final role | Base source | Required customization |
|---|---|---|---|
| Hero | Main hero | Quaternius Universal Base Characters | unique silhouette, survivor/miner outfit, asymmetrical thunder armor, signature hair, weapon, materials |
| Companion | Permanent companion | Universal Base Characters + Fantasy Outfits | different body proportions, hairstyle, layered travel armor, family-symbol accessory, unique weapon |
| Mine_Overseer_A/B/C | Enemy faction | humanoid base + fantasy outfits / approved enemy pack | three silhouettes and equipment combinations |
| Tyrant_Soldier_A/B/C | Enemy faction | humanoid base + armor | shield/polearm variants and heavier silhouette |
| Storm_Hunter_A/B/C | Enemy faction | humanoid base | anti-thunder equipment, coils/insulators, ranged weapon |
| Fortress_Elite_A/B/C | Elite faction | humanoid base | ornate heavy armor and large silhouette |
| Desert_Raider_A/B/C | Desert faction | humanoid base | cloth/mask/curved weapon silhouette |
| Ice_Guard_A/B/C | Ice faction | humanoid base | winter armor/frozen-metal weapon silhouette |
| Boss_01..N | Bosses | approved rigged creature/humanoid sources | unique silhouette, weapon, attacks, phase presentation |
| GiantSnake | Creature | approved creature source to be selected/imported | continuous snake mesh, scales, authored head/jaw/fangs, full skeletal/creature animation |
| Horse | Mount | Ultimate Animated Animal Pack / approved animal source | saddle/tack, unique markings, riding animations |
| Beast | Mount | approved animal/creature source | unique anatomy and gait |
| ThunderBeast | Specialized mount | approved creature base | unique horns/crest, thunder-reactive materials, special abilities/animations |
| FlyingBeast | Mount | approved winged creature source | wings, takeoff, flight, aerial turns, landing |

## Animation contract

Every playable/AI character requires at minimum: idle, walk, run, directional locomotion where available, attack, hit reaction and death. Hero/companion additionally require traversal and ability montages. Mounts require locomotion plus mount/dismount and species-specific movement.

## Import order

1. Base skeletal meshes.
2. Modular outfit parts.
3. Textures/materials.
4. Animation library.
5. Retargeting/IK setup.
6. Character-specific animation blueprints/montages.
7. Weapons and accessories.
8. Art profiles.
9. Gameplay integration.
10. Android performance/LOD verification.

## Approved source notes

Quaternius Universal Base Characters is CC0, includes six game-ready male/female models, humanoid rigs, multiple hairstyles and Unreal-compatible FBX/glTF. Universal Animation Library is CC0 and provides 120+ retargetable humanoid animations. Universal Animation Library 2 is CC0 and adds melee combos and parkour. Fantasy Outfits is CC0 and provides modular rigged outfit parts. These sources permit commercial use and modification according to their published licensing pages.

## No false completion

The Git repository must not contain fake OBJ/GLB placeholders. Binary art is considered integrated only after the Unreal project has imported the actual mesh, material/texture and animation assets and the editor audit confirms them.

## Current verification state

**Pipeline:** ready for local binary asset import.

**Build:** 🔵 ● البناء: جارٍ — no success claim until the Unreal 5.8 build runner actually compiles the project.
