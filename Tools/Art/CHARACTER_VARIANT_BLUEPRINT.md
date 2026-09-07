# ALSAEQA — Character Variant Production Blueprint

This document defines how imported real 3D meshes become unique ALSAEQA characters.

## Non-negotiable

No primitive body construction and no recolor-only variants. A variant must change at least two of: silhouette, hair, outfit modules, armor profile, weapon, accessories, proportions, or material treatment.

## Hero

Base: Universal Base Characters humanoid male.

Variant recipe:
1. Regular male base.
2. Distinct hairstyle chosen from the source pack.
3. Miner/survivor outfit modules for early stages.
4. One-sided shoulder/forearm armor added from modular fantasy outfit pieces.
5. Wrapped forearm and utility equipment retained as visual identity.
6. Signature thunder blade attached to the correct hand socket.
7. Four saved appearance states: Survivor, Awakened Heir, Storm Warrior, Thunder Giant Descendant.

## Companion

Use a separate female base and a different hairstyle/outfit silhouette from the Hero.

Variant recipe:
1. Female base.
2. Distinct hairstyle.
3. Layered travel/light armor outfit.
4. Family-symbol accessory.
5. Unique polearm/paired weapon silhouette.
6. Capture and restored appearance states.
7. Family-search visual evolution from stages 25–40.

## Enemy factions

Each faction receives three production variants by combining different source outfit modules, head/hair pieces, weapons, shields, and proportions:

- Mine Overseer: lamp + industrial armor + mining tool.
- Tyrant Soldier: shield + polearm + disciplined armor.
- Storm Hunter: insulated gear + anti-thunder equipment + ranged weapon.
- Fortress Elite: heavy armor + ornate equipment + large weapon.
- Desert Raider: cloth layers + face covering + curved weapon.
- Ice Guard: winter layers + frozen-metal weapon + heavy boots.

## Acceptance test

A character is not accepted until:
- the skeletal mesh renders in Unreal;
- the intended Anim Blueprint/retargeting works;
- locomotion, combat, hit and death animations play;
- weapon sockets are correct;
- LODs are suitable for Android scalability;
- the silhouette is distinguishable from other characters at thumbnail size;
- gameplay uses the imported mesh and does not rely on procedural fallback.

## Asset source policy

Approved packs are acquired from their official source and are not redistributed as raw third-party archives in this repository. Quaternius Universal Base Characters provides six rigged game-ready bases and 20 hairstyles, and is CC0; its animation libraries provide retargetable humanoid animations for Unreal. See the official sources recorded in `Docs/REAL_ART_ASSET_MANIFEST.md`.
