# ALSAEQA — Enemy, Boss, Traversal and Save Systems

## Current gameplay foundation

The project now has reusable gameplay contracts for four important pillars:

1. **Giant snake AI** — a dedicated controller tracks hidden, ambush, chase, attack, coil, retreat and defeated states. Blueprint animation, navigation and encounter scripting can drive the presentation without changing the core state model.
2. **Status effects** — a reusable component supports timed poison and status notifications. Poison refreshes duration and keeps the strongest damage-per-second value instead of stacking uncontrolled duplicate effects.
3. **Boss phases** — bosses can transition automatically from phase one to phase two at 60% health, phase three at 25%, and defeated at zero. The phase event is exposed for attacks, animation, music, VFX and cinematic responses.
4. **Fall/rescue** — traversal can enter a fall state and recover the actor at a controlled rescue location. This is the gameplay hook for slips, ledge saves, companion rescues and cinematic recovery moments.
5. **Save/checkpoint data** — a save container stores checkpoint identity, region, progression stage, player transform, completed discoveries, unlocked powers and acquired weapons.

## Giant snake encounter design

Snakes are not ordinary damage sponges. Encounters should use terrain:

- Hidden snake: no attack until the player enters the detection zone.
- Ambush: sudden bite or charge, followed by a short recovery window.
- Chase: snake pressures traversal and prevents a simple retreat.
- Coil: snake occupies bridges, pillars, cave routes or narrow ledges.
- Retreat: wounded snakes can disengage and reappear later rather than always dying immediately.
- Boss snake: use multiple phases, changing attack patterns and arena hazards.

## Poison rules

A venom hit applies the `Poison` status through the reusable status component. The status has a duration and damage-per-second value. Repeated venom hits refresh the timer and retain the stronger intensity rather than creating an uncontrolled list of identical effects.

The final UI/VFX/audio representation remains an Unreal Blueprint/content task.

## Boss rules

A boss phase is a gameplay state, not merely a health bar. Each phase can change:

- attack selection;
- movement and arena pressure;
- weak points;
- environmental hazards;
- defensive windows;
- cinematic opportunities;
- required player ability/weapon usage.

## Fall and rescue

The fall component deliberately separates the gameplay state from presentation. A level Blueprint can trigger a fall, a companion can provide a rescue location, or a traversal sequence can resolve to a checkpoint. Slow motion, camera work, animation and VFX should subscribe to the exposed events.

## Save/checkpoint policy

Checkpoints must be placed deliberately at safe traversal/combat boundaries. A death or failed rescue should return the player to the last valid checkpoint rather than silently moving progression backward.

Progression remains sequential: saving does not bypass required stages or award undiscovered powers/weapons. The authoritative progression component remains responsible for whether a reward is actually legal.

## Unreal asset boundary

These files provide production-ready C++ gameplay contracts. Actual `.uasset` behavior trees, animations, montages, Niagara effects, skeletal meshes, level maps and cinematic sequences must be authored/imported through Unreal Editor or an equivalent Unreal content pipeline; they are not represented by text-only GitHub commits.
