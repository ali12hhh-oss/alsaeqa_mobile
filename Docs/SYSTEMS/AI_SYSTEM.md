# AI System

## Enemy state model
The foundation supports Idle, Patrol, Alert, Chase, Attack, Stunned and Dead states.

## Intended behavior
- Perception detects the player or other valid targets.
- Alert pauses normal patrol and evaluates distance.
- Chase uses navigation rather than direct movement when navigation data is available.
- Attack is selected by distance, weapon reach and enemy archetype.
- Stunned temporarily suppresses offensive behavior.
- Dead disables movement and collision after death resolution.

## Archetypes
- Mine overseer / human fighter.
- Fast cave predator.
- Forest hunter.
- Flying creature.
- Thunder-infused monster.
- Giant guardian.
- Boss with multi-phase mechanics.

## Boss rule
A boss must have readable mechanics, attack patterns, openings and phase changes; increased health alone is not considered a boss mechanic.
