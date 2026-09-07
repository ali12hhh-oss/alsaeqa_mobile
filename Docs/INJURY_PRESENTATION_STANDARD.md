# Universal Injury / Death Presentation Standard

This contract applies to the hero, permanent companions, temporary companions, workers, and enemies.

## Runtime states

- Healthy: normal locomotion and combat presentation.
- Injured: contextual hit reaction and reduced effectiveness when applicable.
- Critical: visibly impaired movement/combat and stronger reactions.
- KnockedOut: movement disabled until recovery/rescue logic restores the actor.
- Dead: movement disabled, death presentation, and ragdoll when enabled.

## Body regions

Head, torso, left arm, right arm, left leg, and right leg are first-class hit regions. Damage can provide an explicit region; otherwise the health system derives a region from the world hit location.

## Organ layer

Heart, lungs, and liver are tracked independently. A fatal heart/lung injury can immediately enter the Dead state. Organ events expose Blueprint presentation hooks so the final animation, camera, audio, Niagara, and material work can be authored with the real project assets.

## Dismemberment

Arm and leg severing is a runtime state, not a cosmetic-only event. The component tracks the severed limb, exposes it to Blueprint, can hide the corresponding skeletal bone, and fires a dedicated dismemberment presentation event. Final gore, detached meshes, blood simulation, decals, and Niagara effects must use the project's licensed real assets rather than placeholders.

## Knockout

Knockout stops movement and fires a dedicated start/end presentation. Recovery is explicit so story sequences, companion rescue, and AI systems can decide when an actor becomes active again.

## Death

Death disables movement and collision on the capsule and can switch the skeletal mesh to ragdoll. Health and injury death events are synchronized so the same death cannot be reported twice.

## Blueprint visual contract

Implement per character family:

1. `PlayInjuryPresentation` for directional/body-part hit reactions.
2. `PlayDismembermentPresentation` for limb loss and detached-part presentation.
3. `PlayKnockoutPresentation` for collapse, unconsciousness, and recovery.
4. `PlayOrganInjuryPresentation` for severe internal-impact presentation.
5. `PlayDeathPresentation` for the authored death montage/ragdoll handoff.

Animation Blueprints should consume injury state and severity for locomotion blend, combat stance, hit reactions, limp/drag behavior, and recovery. IK should adapt feet/hands to the injured state. The final visual pass belongs to the real skeletal meshes, animation libraries, materials, Niagara systems, and audio supplied with the project.

## Combat contract

`FALSAEQADamageInfo` carries amount, damage type, instigator, hit location, explicit hit region, critical flag, knockout flag, and dismemberment flag. Melee and enemy damage paths must pass the complete structure into `ALSAEQAHealthComponent::ApplyDamageInfo` so no damage path bypasses the universal injury layer.

## Performance

Avoid per-frame expensive skeletal queries. Runtime injury state is stored in compact maps/sets; visual effects should be event-driven and pooled where practical for Android.
