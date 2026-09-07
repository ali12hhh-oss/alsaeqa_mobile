# Inter-Stage Events and Revisitability

## Design principle
A stage has a main objective, but the stage itself never becomes permanently closed. Completion means the current objective is resolved, not that the region is finished forever.

## Non-player-controlled separators
Short non-interactive sequences connect major playable sections. They can be triggered after an objective, on entering a safe route, after a rescue, or when a persistent world condition changes.

Examples:
- the hero and temporary companions discuss the next route;
- the permanent companion notices a family symbol;
- rescued prisoners describe an enemy convoy;
- distant thunder reveals a giant moving between mountains;
- a companion practices a newly awakened skill;
- an enemy commander receives news that the hero survived;
- the party reaches camp and discusses the previous battle.

These are transitions, not gameplay locks. They should be short enough to preserve momentum and should lead naturally into the next playable objective.

## Stage lifecycle
Each region supports:
- MainObjective
- OptionalObjectives
- ActiveEvents
- CompletedEvents
- PersistentWorldFlags
- AvailableRevisitObjectives
- RequiredPowerOrWeapon
- NextStoryStage

The main objective can be completed while the region remains available for future updates.

## Revisit examples
A forest visited early may later contain:
- a newly opened thunder path;
- a rescued settlement;
- a giant patrol;
- a family clue for the companion;
- a hidden chamber exposed by Thunder Eye;
- a new weapon trial.

A prison cleared during one stage may later become:
- a safe rescue location;
- a source of enemy intelligence;
- a new hostage location;
- an entrance to a deeper underground route.

## Capture continuity
When the permanent companion is captured, the previous region records the separation and the next stage records the rescue investigation. The player can return to earlier regions while the rescue arc is active, where new clues can appear.

After rescue, the original regions remain revisitable and can receive companion-specific dialogue and discoveries.

## No hard final stage
The current stage list is a story progression, not a declaration that the game ends there. New stages, sub-regions, family arcs, giant encounters, weapons, abilities, and revisit events can be appended while preserving earlier content.
