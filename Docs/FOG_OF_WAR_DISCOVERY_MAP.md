# Discovery Map / Fog of War

## Purpose
The world map supports exploration without revealing the intended route in advance.

## Map states
- Unknown: hidden by fog.
- Explored: terrain discovered by the player.
- Landmark: discovered cave, prison, camp, ruin, altar, settlement, or major enemy site.
- Objective clue: a soft clue toward the current story objective, never a permanent arrow covering the whole screen.
- Secret: revealed only by a power, companion skill, environmental clue, or later revisit.

## Discovery rules
The map is not a complete blueprint at the beginning of a region. Exploration, elevated viewpoints, rescued scouts, companion observations, and special abilities progressively reveal it.

The player should be able to navigate from memory, landmarks, signs, environmental silhouettes, NPC hints, and partial map information.

## Ability interaction
- Thunder Eye can reveal hidden paths and secret objects.
- Thunder Echo can reveal ancient routes and traces of past activity.
- Thunder Grip can make previously impossible traversal routes usable.
- Companion abilities can reveal family symbols or subtle tracks.

## Revisitability
Map data is persistent, but the world state can change. A previously explored location may gain a new marker, route, enemy patrol, prisoner camp, family clue, or ability-gated passage.

## Story continuity
Map discoveries should connect regions rather than create isolated levels. A clue in one region can point toward another region, while a later event can send the player back to the original location.

## UX rule
Never require the player to follow a single highlighted line unless a temporary story sequence specifically calls for it. Prefer readable clues and optional guidance so discovery remains part of the adventure.
