# ALSAEQA — Universal Damage, Injury, Dismemberment and Death Standard

## Scope

This is one universal system for every living character or creature using ALSAEQA health:

- Hero
- Permanent and temporary companions
- Friendly workers and NPCs
- Human enemies and slavers
- Creatures and monsters
- Elite enemies and bosses
- Future character classes

It is not a hero-only system and must not become a collection of incompatible per-character health/death systems.

## Core model

Health represents overall survivability. Injury represents the physical state produced by damage and its gameplay consequences.

Supported states:

- Healthy
- Injured
- Critical
- KnockedOut / unconscious
- Dead

Each supported body region has independent severity. Arms and legs can also have an explicit severed state.

## Body regions and organs

Humanoid profiles support:

- Head
- Torso
- Left arm
- Right arm
- Left leg
- Right leg

The common organ layer supports:

- Heart
- Lungs
- Liver

Creatures and bosses should use equivalent anatomy profiles appropriate to their actual body rather than being forced into a human skeleton model.

## Gameplay consequences

Injuries are gameplay state, not merely visual decoration.

Examples:

- Leg damage can reduce locomotion performance and change locomotion presentation.
- Arm damage can affect weapon handling, blocking and attack presentation.
- Severe head damage can produce unconsciousness or a critical state.
- Severe torso or vital-organ damage can become fatal.
- Limb loss remains an explicit body-state result until an intentional recovery/reset rule applies.
- Knockout is separate from death and can be recoverable.
- Death is terminal for ordinary enemies and can instead route into respawn, rescue, checkpoint or story rules for characters that require those outcomes.

## Universal health bridge

Every actor that owns `UALSAEQAHealthComponent` automatically receives a `UALSAEQAInjuryComponent` at runtime if it does not already have one.

This gives the same authoritative injury/death layer to the hero, companions, workers, NPCs, enemies, creatures and bosses without duplicating the system.

Health death and injury fatality are synchronized into the existing health death event, preventing separate death pipelines.

## Presentation contract

C++ owns authoritative state. Blueprint, animation assets and real project assets own final presentation:

- Hit reactions
- Stagger and balance loss
- Limping or altered locomotion
- Knockout/fall/recovery
- Limb-loss presentation
- Death presentation
- Creature-specific reactions
- Niagara, sound and camera treatment

No placeholder geometry or replacement art is introduced by the injury system.

## Production expansion path

The common system is designed to expand into:

- Bleeding/damage-over-time
- Fractures and temporary impairment
- Armor and protection zones
- Creature-specific weak points
- Hit-location-aware combat
- Animation Notify-driven hit windows
- AI reactions to injuries
- Carry/rescue/revive behavior
- Persistent story-critical injuries
- Save/load of permanent body-state outcomes
- Species/anatomy profiles

All future features must extend the common injury system rather than creating parallel health/death implementations.
