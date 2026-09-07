# ALSAEQA — Real 3D Art Pipeline

## Goal

The final game presentation must use authored 3D meshes, skeletal characters, real animation clips, textured weapons/props, and authored environment kits. Runtime primitive geometry is only a safety fallback and is **not** the final art direction.

## Commercial-safe sources selected

The following official Quaternius pages were checked on 2026-09-03. The listed packs are CC0/commercial-safe according to their pages; assets can be modified and used in commercial games. Do not redistribute the raw packs as standalone asset collections.

| Production role | Source | Use |
|---|---|---|
| Hero + companion humanoid base | Universal Base Characters | 6 rigged game-ready human bases, male/female, retargetable |
| Locomotion + combat | Universal Animation Library | 120+ humanoid animations, locomotion, combat, death and more |
| Advanced combat/parkour | Universal Animation Library 2 | 130+ combat, combo, parkour and movement animations |
| Animals / mounts | Ultimate Animated Animal Pack | 12 animated animals with attack, death, gallop, walk, jump and other clips |
| Monsters / enemy pool | Ultimate Monsters | 50 fully animated monsters |
| Dungeon / ruins | Ultimate Modular Ruins | Modular ruins, dungeon pieces and props |
| Fantasy weapons/props | Fantasy Props MegaKit | Weapons, tools, furniture, chests and world props |
| Town/city structures | Medieval Village MegaKit | 300+ modular walls, floors, roofs, stairs and buildings |
| Forest/rocks/vegetation | Stylized Nature MegaKit | Trees, plants, flowers, rocks, grass and bushes |
| General RPG dressing | Ultimate RPG Pack | 100+ RPG models and props |

Official source pages:
- https://quaternius.com/packs/universalbasecharacters.html
- https://quaternius.com/packs/universalanimationlibrary.html
- https://quaternius.com/packs/universalanimationlibrary2.html
- https://quaternius.com/packs/ultimateanimatedanimals.html
- https://quaternius.com/packs/ultimatemonsters.html
- https://quaternius.com/packs/ultimatemodularruins.html
- https://quaternius.com/packs/fantasypropsmegakit.html
- https://quaternius.com/packs/medievalvillagemegakit.html
- https://quaternius.com/packs/stylizednaturemegakit.html
- https://quaternius.com/packs/ultimaterpg.html
- License: https://quaternius.com/license.html

## Import layout

After downloading the official packages, import into Unreal 5.8 using this structure:

```text
Content/Art/
  Characters/
    Hero/
    Companion/
    Enemies/
    Bosses/
  Creatures/
    GiantSnake/
    Mounts/
  Weapons/
  Environment/
    Mine/
    Forest/
    ThunderMountain/
    Caves/
    MonsterValley/
    AbandonedCity/
    TyrantFortress/
    BlackDesert/
    IceKingdom/
    LandOfGiants/
  Animations/
    Locomotion/
    Combat/
    Parkour/
    Reactions/
    Death/
  Materials/
  VFX/
```

## Character production

### Hero
- Universal Base Character humanoid rig.
- Distinct hero outfit assembled from modular fantasy clothing.
- Thunder-themed material accents, not a primitive glow-only representation.
- Real sword/weapon mesh attached to the hand socket.
- Animation Blueprint: idle, walk, jog, sprint, jump, fall, land, light attack, heavy attack, hit reaction, dodge, thunder abilities and death.

### Companion
- Separate female base character and outfit.
- Different silhouette and weapon loadout from hero.
- Independent animation state machine.
- Combat and rescue montages remain separate from the hero.

### Enemies / bosses
- Use dedicated creature/humanoid meshes from the licensed source pool where available.
- Bosses receive unique silhouette, materials, weapons and attack montages rather than merely larger scale.
- Giant Snake uses a dedicated creature mesh and attack animation set; no segmented primitive fallback is considered final.

### Mounts
- Real skeletal animal/creature mesh.
- Idle, walk, run, sprint, turn, jump, land and injured animations.
- ThunderBeast receives its own emissive material/VFX layer and specialized ability montages.

## Animation retargeting

The Universal Animation Libraries are designed for retargeting and include Unreal-compatible exports. Use the matching humanoid skeleton for Universal Base Characters, then create an Unreal IK Rig / IK Retargeter pair.

Required animation groups:

1. Locomotion: idle, walk, jog, sprint, strafe, 8-direction movement.
2. Traversal: jump, fall, land, climb, crawl, swim, parkour.
3. Combat: light, heavy, combo 1–4, recovery, hit reactions, dodge, parry.
4. Weapon: sword/axe/staff-style attacks and equip/unequip where compatible.
5. State: injured, staggered, knockdown, death.
6. Companion: independent locomotion, combat, rescue/capture/rejoin states.
7. Creature: walk, run/gallop, turn, attack, hurt, death and special attacks.

Root motion must be selected intentionally per montage. Do not fake skeletal animation with transform oscillation once real assets are installed.

## Real environment production

The world remains one interconnected world. The following regions receive distinct authored kits/material treatment rather than cubes:

- Mine: modular timber supports, rails, rock walls, carts, lamps and debris.
- Forest: layered trees, undergrowth, rocks, paths, cliffs and points of interest.
- Thunder Mountains: exposed rock, ancient structures, storm VFX and high-altitude silhouettes.
- Caves: modular rock formations, crystals/ancient traces and authored collision.
- Monster Valley: large rocks, vegetation, creature nests and traversal routes.
- Abandoned City: modular buildings, streets, ruins, props and destructible-looking dressing.
- Tyrant Fortress: modular stone/wood structures, gates, towers, weapons and banners.
- Black Desert: authored rock formations, sparse vegetation, ruins and atmospheric dressing.
- Ice Kingdom: snow/ice materials, structures, frozen props and traversal hazards.
- Land of Giants: oversized architecture, rocks, ancient structures and scale landmarks.

The existing RuntimeWorldBuilder remains useful for development/testing, but its primitive/greybox output must be replaced progressively by imported authored assets before final release.

## Weapons and props

Weapons are real meshes with sockets and authored materials. The code should select a weapon asset through a configurable `TSoftObjectPtr<UStaticMesh>` or Blueprint data asset rather than hard-coding a primitive mesh.

Props use static meshes with simple collision; animated props use skeletal/static mesh animation as appropriate.

## Hazards and VFX

Hazards are visualized with authored meshes plus Niagara where appropriate:
- falling rocks
- collapsing mine supports
- spikes/traps
- fire/lava zones
- ice hazards
- electrical storm zones
- boss arena hazards
- breakable/blocked traversal objects

Gameplay collision remains independent from presentation so the art can be replaced without changing combat/progression logic.

## Repository rule

Do **not** commit an asset merely because a website says it is downloadable. Keep a manifest containing source URL, asset name, license, date checked, and intended use. Do not redistribute paid/proprietary source archives. CC0 assets may be used in the game, but the repository should avoid turning the repository itself into an asset-pack mirror.

## Release-connected source-art pipeline

The canonical source archive is now published in GitHub Release `assets-v1` as `ALSAEQA_REAL_ASSETS.zip`. Its SHA-256 is pinned in `Tools/Assets/assets-manifest.json`. The CI pipeline downloads the archive, verifies the digest, expands nested source-pack ZIPs, and exposes the expanded FBX/OBJ/GLTF source tree to the Unreal Editor import stage. Raw source binaries remain outside Git so the repository does not become an asset-pack mirror.

The Unreal import automation writes authored imported assets under `/Game/Art`. It does not create primitives, greyboxes, placeholders, or procedural final-art substitutes. Any additional asset packs uploaded to the same Release can be added to the manifest later without changing the game's C++ systems.

## Current limitation

The repository can automate acquisition and Unreal import of the released source art, but binary source art is not directly committed to Git. Final visual assignment of a particular mesh/material/animation to a specific Blueprint still requires the Unreal asset registry/import result; the codebase keeps gameplay logic independent from those visual assets so the real art can be wired without replacing it with placeholders.
