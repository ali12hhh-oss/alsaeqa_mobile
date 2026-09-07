# ALSAEQA — Continuous Stage Difficulty 1–70

## Core rule
Every stage is harder than the previous stage. Difficulty must increase continuously from stage 1 through stage 70, while avoiding sudden unfair spikes.

## What increases
Difficulty is expressed through multiple systems rather than health alone:
- enemy movement and pursuit pressure
- detection range
- attack damage and attack frequency
- enemy combinations and encounter density
- environmental hazards
- traversal precision
- puzzle complexity
- resource pressure
- boss phase complexity
- storm and Thunder interactions
- mount traversal risk in later stages

## Runtime enemy scaling
The reusable enemy character reads the current saved stage and applies continuous scaling:
- movement pressure: +0.4% per completed stage after stage 1
- detection pressure: +0.25% per completed stage
- outgoing damage: +1.2% per completed stage
- attack frequency: +0.3% per completed stage
- effective attack reach: +0.15% per completed stage

These values are intentionally incremental so stage N remains harder than stage N-1 without requiring a difficulty jump between story arcs.

## Encounter design rule
A later stage should not rely on inflated statistics alone. At least one meaningful new challenge should accompany progression: a new enemy behavior, combination, hazard, traversal constraint, puzzle layer, or boss mechanic.

## Checkpoint fairness
Checkpoints remain available before major difficulty walls. Death returns the player to the last checkpoint and preserves permanent progression; difficulty itself is not reset.

## Milestones
- 1–10: foundational combat and traversal
- 11–20: mixed encounters and power mastery
- 21–24: preparation and higher pressure
- 25–40: companion family search under increasing danger
- 41–44: hero family search begins
- 45–55: mount traversal and protection challenges
- 56–59: ThunderBeast bonding under storm pressure
- 60–69: specialized ThunderBeast mastery and endgame traversal
- 70: highest combined challenge and narrative climax

## Non-negotiable
No later stage may intentionally be easier than the immediately preceding stage unless a temporary story beat explicitly lowers combat pressure; the overall challenge of that stage must still exceed the prior stage through exploration, traversal, puzzle, or decision complexity.
