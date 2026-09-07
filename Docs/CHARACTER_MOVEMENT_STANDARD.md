# ALSAEQA — Universal Character Movement Standard

This standard applies to the hero, permanent companion, friendly NPCs, hostile NPCs and future recruitable companions. Movement is authored as a layered system: locomotion state + traversal state + combat state + contextual animation/presentation.

## Hero locomotion
- Idle, walk, jog, sprint.
- Directional acceleration/deceleration and turn-in-place.
- Crouch and crouch-walk.
- Listen/stealth stance: crouched, movement locked while listening, presentation hook for heightened hearing.
- Roll/dodge with direction from current movement input and safe cooldown.
- Airborne/fall/landing.
- Recovery after heavy landing and damage.
- Mount/dismount transitions.
- Contextual interaction poses.

## Combat movement
- Attack locomotion and attack recovery.
- Light/heavy attack presentation.
- Hit reactions by direction/impact class.
- Stagger, knockdown, get-up.
- Death and recovery/respawn.
- Future animation-notify hit windows must replace purely time-independent collision assumptions for final combat quality.

## Companion movement
The permanent companion must not simply mirror the hero:
- Independent navigation and obstacle avoidance.
- Follow distance bands and regroup behavior.
- Walk/jog/sprint matching the current context.
- Crouch and stealth follow.
- Listen/observe states.
- Contextual rescue/interactions.
- Combat locomotion, dodge/evade, hit reaction, downed/revive and regroup.
- Mount and dismount when mount support is active.
- Companion-specific animation presentation hooks.

## Friendly NPCs / rescued workers
- Injured, exhausted, frightened and normal locomotion variants.
- Limp/fall/recover states where the authored animation set supports them.
- Follow-escort behavior after rescue.
- Avoidance around the hero and other rescued workers.
- Group movement so five rescued workers do not overlap or move as clones.

## Enemies
Enemies need their own movement state machine rather than reusing hero movement:
- Patrol, investigate, search, alert, chase.
- Walk/jog/run/sprint according to alert level.
- Crouch/stealth where archetype supports it.
- Strafe, combat footwork, retreat and reposition.
- Dodge/evade for elite archetypes.
- Hit reaction, stagger, knockback, knockdown, get-up and death.
- Contextual ladder/door/cover interactions when the level supports them.
- Group spacing and role-aware pursuit.
- Mine-specific escalation: rescue events increase alert/search pressure without resetting completed rescues.

## Animation architecture
C++ exposes state/presentation contracts; final animation graphs, blend spaces, montages, IK and motion matching belong in Blueprint/Animation assets. No fake animation asset names are treated as existing until the real assets are present.

## Mobile quality
Movement logic must remain lightweight on Android:
- Avoid per-frame global actor scans.
- Prefer navigation/overlap/query systems with bounded scope.
- Keep cinematic and animation work event-driven.
- Do not allocate repeatedly in hot movement paths.
- Scale animation/VFX complexity by device profile.

## Current implementation
The hero now exposes Crouch, Listen and Roll states plus Blueprint presentation hooks, and core keyboard/mouse mappings have been added. These are gameplay contracts; final animation/IK/Sequencer content still requires the real Unreal assets.

## Future extension
The same state vocabulary is intended to be shared by companion/enemy animation blueprints so every character family can receive authored motion without rewriting gameplay progression.
