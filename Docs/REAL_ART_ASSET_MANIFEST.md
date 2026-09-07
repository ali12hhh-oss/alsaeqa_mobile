# ALSAEQA — Real 3D Art Asset Manifest

This manifest is the source of truth for the final visual pass.

## Rule

Primitive/procedural geometry is **fallback only**. Final gameplay art must use authored/imported 3D meshes, skeletal rigs, real animation clips, textured weapons and authored environments.

## Approved base sources

| Area | Source | License | Unreal target | Status |
|---|---|---|---|---|
| Hero/Companion bases | Quaternius Universal Base Characters | CC0 | UE5.8 | Awaiting local import |
| Character outfits | Quaternius Modular Character Outfits - Fantasy | CC0 | UE5.8 | Awaiting local import |
| Humanoid locomotion/combat | Quaternius Universal Animation Library | CC0 | UE5.8 | Awaiting local import |
| Extended combat/parkour | Quaternius Universal Animation Library 2 | CC0 | UE5.8 | Awaiting local import |
| Enemy humanoids | Quaternius Bestiary - Dungeon Monsters Kit / other approved Quaternius animated packs | QAL/CC0 as applicable | UE5.8 | Selection/import pending |
| Mount animals | Quaternius Ultimate Animated Animal Pack | CC0 | UE5.8 | Awaiting local import |
| Environment | Quaternius modular environment/nature packs | CC0 where marked | UE5.8 | Awaiting local import |
| Weapons/props | Quaternius Fantasy Props / RPG / Modular Weapons packs | CC0 | UE5.8 | Awaiting local import |

## Required unique production variants

### Hero — `ALSAEQA_Hero`
- One-sided thunder shoulder/arm protection.
- Survivor/miner-origin clothing integrated with fantasy armor.
- Distinct hair silhouette and wrapped forearm.
- Thunder-forged signature blade.
- Four visual progression states: Survivor, Awakened Heir, Storm Warrior, Thunder Giant Descendant.
- No permanent emissive glow; thunder response is event-driven.

### Companion — `ALSAEQA_Companion`
- Separate female base silhouette from Hero.
- Layered travel clothing/light armor.
- Family-symbol accessory retained across progression.
- Distinct hair and weapon silhouette.
- Capture/damage and rescued/restored visual variants.
- Staged visual evolution during family-search arc (25–40).

### Enemy factions
1. Mine Overseer — industrial armor, lamp, mining tools.
2. Tyrant Soldier — disciplined armor, shield/polearm.
3. Storm Hunter — insulated anti-thunder equipment.
4. Fortress Elite — heavy ornate armor.
5. Desert Raider — cloth/mask/curved weapon language.
6. Ice Guard — winter armor/frozen-metal weapon language.

Each faction requires at least three silhouette/equipment variants. Recolor-only variants are rejected.

### Bosses
Every boss must have its own silhouette, weapon, attack language, armor/material identity and animation set. Scaling an ordinary enemy is not sufficient.

### Giant Snake
A continuous authored creature mesh with head, jaw, eyes, fangs, body scales/material and skeletal or equivalent creature animation. Required clips: idle, coil, crawl/slither, turn, ambush, bite, charge, retreat, hurt, death. Segmented ellipsoid construction is not final art.

### Mounts
- Horse: believable horse anatomy/tack and gallop/turn/stop animations.
- Beast: distinct fantasy quadruped silhouette and combat movement.
- ThunderBeast: unique horns/crest, thunder-reactive armor/material details and dedicated ability animations.
- FlyingBeast: authored wings, takeoff, flight, banking, landing and aerial turn animations.

## Animation acceptance

Each imported character must have:
- idle
- walk
- jog/run
- sprint
- directional movement where available
- jump/fall/land
- hit reactions
- death
- combat attacks
- ability-specific montage hooks
- traversal/context animations when required

## Environment acceptance

Each major region must replace greybox primitives with authored modular meshes while retaining collision and Android-friendly LODs:

`Mine`, `Forest`, `ThunderMountain`, `Caves`, `MonsterValley`, `AbandonedCity`, `TyrantFortress`, `BlackDesert`, `IceKingdom`, `LandOfGiants`.

## Import policy

Raw source packs are not redistributed by ALSAEQA. They are acquired from their official source and imported locally using `Tools/Unreal/ImportRealArt.py`. The repository stores integration scripts, profiles, documentation and configuration rather than third-party source archives.

## Verification states

`Planned` → `Awaiting local import` → `Imported` → `Customized` → `Integrated` → `Verified`.

`Verified` means the Unreal project opens the asset, the mesh renders, the intended animation class/retargeting works, collisions/LOD are acceptable, and the relevant gameplay actor uses the imported asset instead of procedural fallback.
