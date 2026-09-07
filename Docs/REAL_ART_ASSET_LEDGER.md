# ALSAEQA — Real Art Asset Ledger

This ledger is the source of truth for visual integration. A row is **not** considered complete merely because code supports it. `Imported` means the actual binary source asset has been imported into Unreal and verified in the Editor.

## Licensing policy

- Prefer CC0 assets with explicit commercial-use permission.
- Keep the original source URL, license, retrieval date and modification notes.
- Do not copy assets from another game/project without permission.
- Do not claim third-party authorship.
- Do not use primitive meshes as final visual assets.
- Runtime/procedural primitives are emergency fallback only.

## Primary licensed sources

| Source | Intended use | License | Status |
|---|---|---|---|
| Quaternius Universal Base Characters | Hero, companion and humanoid enemy bases | CC0; commercial use and modification permitted | Source selected; Unreal import pending |
| Quaternius Modular Character Outfits - Fantasy | Hero/companion/enemy clothing and faction differentiation | CC0; commercial use and modification permitted | Source selected; Unreal import pending |
| Quaternius Universal Animation Library | Humanoid locomotion/combat/reactions/death | CC0; commercial use and modification permitted | Source selected; Unreal import pending |
| Quaternius Universal Animation Library 2 | Parkour, melee combos and extra combat movement | CC0; commercial use and modification permitted | Source selected; Unreal import pending |
| Quaternius Fantasy Props MegaKit | Weapons, tools, chests, props and world dressing | CC0; commercial use and modification permitted | Source selected; Unreal import pending |
| Quaternius Ultimate Modular Ruins / Nature packs | Ruins, caves, terrain dressing and vegetation | CC0; commercial use and modification permitted | Candidate source; import pending |
| Poly Haven | PBR materials/HDRI/world surface detail | CC0 | Candidate source; import pending |

## Required final assets

| Role | Required identity | Binary imported | Modified for ALSAEQA | Gameplay linked | Editor verified |
|---|---|---:|---:|---:|---:|
| Hero | Last Thunder Giant heir | No | No | Bridge ready | No |
| Companion | Permanent female companion | No | No | Bridge added | No |
| Mine Overseer A/B/C | Three distinct faction looks | No | No | Enemy class ready | No |
| Tyrant Soldier A/B/C | Three distinct looks | No | No | Enemy class ready | No |
| Storm Hunter A/B/C | Three distinct looks | No | No | Enemy class ready | No |
| Fortress Elite A/B/C | Three distinct looks | No | No | Enemy class ready | No |
| Desert Raider A/B/C | Three distinct looks | No | No | Enemy class ready | No |
| Ice Guard A/B/C | Three distinct looks | No | No | Enemy class ready | No |
| Bosses | Unique silhouette per boss | No | No | Boss phases ready | No |
| Giant Snake | Rigged creature with attack/death animations | No | No | Gameplay class ready | No |
| Horse | Rideable animated mount | No | No | Mount system ready | No |
| Beast | Rideable animated mount | No | No | Mount system ready | No |
| ThunderBeast | Unique thunder-capable mount | No | No | Abilities ready | No |
| FlyingBeast | Flying traversal mount | No | No | Mount system ready | No |
| Weapons | Hero/enemy melee and special weapons | No | No | Combat hooks ready | No |

## World asset targets

Mine, Deep Cave, Forest, Thunder Mountain, Monster Valley, Abandoned City, Tyrant Fortress, Black Desert, Ice Kingdom and Land of Giants each require authored environment assets. The existing procedural/runtime world is fallback/greybox and must not be treated as the final art pass.

## Modification standard

Each final character must be assembled from licensed parts and then made project-specific through combinations of body proportions, hair, outfit parts, materials, accessories, weapons, decals/details and faction-specific presentation. Enemy variants must not be a single mesh with only a color swap.

## Verification gate

The visual phase is complete only when:

1. Source files are present and license records are committed.
2. Unreal imports the real meshes/materials/animations without errors.
3. Skeletal meshes, animation blueprints/montages and materials are visible in the Editor.
4. Character, companion, enemies, bosses, snake and mounts spawn with their real visuals.
5. Maps contain authored environment assets rather than primitive placeholders.
6. Android LOD/material/texture budgets are checked.
7. A real Unreal 5.8 build succeeds.

Until all gates pass, project status must not claim final visual completion.
