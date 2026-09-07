# ALSAEQA — Master Completion Specification

This document is the single implementation contract for the canonical ALSAEQA project. It consolidates the gameplay, cinematic, rescue, companion, traversal, combat, mount, storm, persistence and real-art requirements that must be completed in the same project. No primitive or placeholder art is permitted as final presentation.

## 1. Core experience

ALSAEQA is a cinematic third-person thunder adventure. The hero is the last heir of the ancient Thunder Giants. The permanent female companion is a full story character, not a decorative follower. Temporary companions, rescued prisoners, enemies, bosses, mounts and creatures are all part of the same persistent adventure.

The introduction, memories and major story turns are cinematic. Gameplay must continuously mix exploration, traversal, combat, rescue, discovery and scripted action moments so the experience does not become repetitive.

## 2. Permanent companion

The companion must support:
- follow / assist / separate / captured / rescued states;
- contextual combat and support behavior;
- family-search story progression;
- capture and rescue cinematics;
- emotional reactions, facial/body expressions and contextual dialogue;
- mount/dismount participation where the story permits;
- persistent rescue/story state after saving and loading;
- environmental and combat reactions rather than standing idle.

The existing disabled companion tick must not be treated as a finished behavior system. Final behavior may be Blueprint/Behavior Tree driven, but it must be real and testable.

## 3. Temporary companions

The four documented temporary companions remain canonical:
- Nadir — scout/tracking
- Mira — healer/support
- Khaled — shield/close combat
- Sana — ranged/crowd control

Each temporary companion needs a real recruit/release lifecycle, AI, role behavior, combat/support actions, separation/rejoin handling, rescue/capture outcome, cinematic hooks and save/load persistence. A temporary companion can be lost, released or successfully escorted according to the stage design.

## 4. Prisoners and rescues

Prison rescue is a real gameplay system, not only dialogue. It must support:
- prison locations and guarded routes;
- discovery/infiltration/combat or alternate rescue approaches;
- captive states;
- escort/escape behavior;
- scripted escape sequences;
- companion/temporary-companion recruitment from rescues;
- persistent world flags for rescued people;
- later visits, dialogue, rewards and world reactions based on rescue outcomes.

Rescue counts and outcomes must survive Save/Load.

## 5. Cinematic action system

Major action moments must support:
- heavy/critical hit slow motion;
- perfect dodge and parry moments;
- finishers;
- ledge catches;
- sliding saves;
- climbing failure and rescue;
- falling while climbing and partner rescue;
- companion rescue moments;
- enemy/boss impact reactions;
- facial and body expressions;
- camera composition and short controlled time dilation;
- safe interruption/recovery so cinematics never soft-lock gameplay.

The final implementation must use animation/Sequencer/Blueprint hooks where appropriate, not teleport-only approximations.

## 6. Traversal and rescue

Traversal must include real ledge detection, grab/catch, climbing, sliding, falling, recovery and rescue animations. Hand/foot IK and camera response should be used where the imported animation/rig supports them. Fall rescue must not remain a simple teleport to a rescue location.

## 7. Combat

Combat must become animation-driven:
- attack montages and hit windows;
- animation notifies for damage timing;
- combo/light/heavy attack flow;
- hit reactions and impact feedback;
- critical-hit classification and multiplier;
- damage metadata suitable for cinematic reactions;
- target filtering and friendly-fire rules;
- boss/enemy resistances and stagger;
- thunder weapon integration;
- camera and time-dilation hooks for hero moments.

Immediate damage on button press must not be the final melee implementation.

## 8. Thunder weapon and VFX

The thunder weapon is a high-priority visual gap. It must have real authored weapon geometry/materials plus Niagara-based effects where the imported art requires them: charge, energized blade, trail, swing arc, impact, hit sparks, lightning arcs and storm interaction. Do not create primitive replacement art.

## 9. Storm and wind

Create a unified wind/storm environment driver affecting, when supported by the real assets:
- trees and foliage;
- grass;
- clothing/capes;
- hair/groom;
- dust/sand;
- particles;
- thunder/lightning intensity;
- ambient audio and camera atmosphere.

Wind strength must be data-driven by the storm state and level conditions rather than independent hard-coded effects.

## 10. Giant Snake

The Giant Snake must have one authoritative AI/attack implementation, not duplicated attack logic. Its ambush, pursuit, bite/charge, retreat, resistance/status effects, obstacle coiling and cinematic events must be functional. Obstacle coiling must be a real movement/animation behavior or a safe authored cinematic sequence, not an empty function.

## 11. Mounts and ThunderBeast

Mount progression remains locked behind the story. Types are Horse, Beast, ThunderBeast and FlyingBeast. Taming requires discovery, behavior learning, approach/trust and route progression.

ThunderBeast is a specialized storm creature, not simply a faster horse. Its abilities must produce real gameplay/VFX/camera/audio results. No-op abilities such as LightningCrossing and ThunderSense must be implemented or explicitly gated until their assets/behavior exist. Storm Mode's stage unlock must be consistent everywhere.

## 12. Progression and stages

The 1–70 roadmap is the canonical story sequence. Stage data must eventually drive real events rather than only increment a stage number. Weapons and power lists must match the roadmap exactly. Interludes and persistent world flags must execute through a common stage/event system.

Known roadmap consistency checks:
- Storm Mode stage must use one canonical unlock stage everywhere.
- Roadmap weapons must match the progression enum/data.
- Companion family and hero family search events must have gameplay state, not only documentation.

## 13. Save/load

Save data must cover all persistent gameplay state required by the story:
- checkpoint;
- progression/powers/weapons/discoveries;
- memories;
- persistent world flags;
- permanent companion story/rescue state;
- temporary companion roster/status/outcomes;
- rescued prisoner outcomes;
- tamed mount profiles and progression;
- relevant quest/event completion state.

Any state that changes later dialogue, encounters, access, rewards or revisits must be serializable.

## 14. Real art pipeline

The canonical release asset package is `ALSAEQA_REAL_ASSETS.zip`, containing the user's 115 real assets. CI must download the single archive, extract it into `Content/Art`, preserve internal folders and refuse to build if no `.uasset`/`.umap` content is present.

The final game must use the real authored assets. Procedural primitives/placeholders are not an acceptable final visual fallback. Asset mapping should be performed after the archive is available so every mesh, skeleton, animation, material, VFX and map can be matched accurately.

## 15. Acceptance rule

A feature is not considered complete because a class, enum, document or stub exists. It is complete only when its gameplay state, visual/audio feedback, animation/cinematic hooks, persistence and real-asset integration work together in an actual playable flow.

All changes belong to this one canonical ALSAEQA project. Do not create parallel editions or replacement projects.
