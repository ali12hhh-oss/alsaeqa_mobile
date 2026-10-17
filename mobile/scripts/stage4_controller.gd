extends Node

## Stage 4, per explicit project direction (see Docs/PROJECT_CONTINUITY.md
## for the canon note this represents): a transition zone the hero builds
## between Stage 3's rocky mine surroundings and Stage 6's full forest
## entry — not literally "the forest" itself, which stays Stage 6's.
##
## Sequence: the hero observes the companion, captive in chains, held by a
## gang (leader + second-in-command + members). Staying unseen
## (crouch/listen — the existing stealth states) while they move toward a
## cave is the intended path; being spotted triggers an intentionally
## overwhelming ambush (gang_alarm_zone.gd / GangMember) that is not meant
## to be a viable alternate route to clearing the stage. The leader,
## second, and the rest of the gang follow the companion into the cave and
## are NOT fought here — per explicit direction, that fight (and freeing
## the companion) is Stage 5's. Stage 4's only mandatory gate is defeating
## the 3 guards left outside the cave entrance.

var _completed := false

signal stage_ready

func _ready() -> void:
    add_to_group("stage4_controller")
    add_to_group("stage_controller")

func register_guard() -> void:
    GameState.total_guards_stage1 += 1

func notify_guard_defeated() -> void:
    GameState.defeated_slavers += 1
    GameState.save_game()
    _try_complete()

func _try_complete() -> void:
    if _completed:
        return
    if GameState.total_guards_stage1 > 0 and GameState.defeated_slavers >= GameState.total_guards_stage1:
        _completed = true
        GameState.advance_to_next_stage()
        stage_ready.emit()
