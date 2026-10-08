extends Node

## Stage 3 ("Echo Under Stone"), per Docs/ALSAEQA_MASTER_PROJECT_BIBLE.md:
## the hero's first memory fragment of the Thunder lineage. Per explicit
## project direction, this stays in the mine's widened surroundings (rocky
## terrain, small caves, sparse vegetation) rather than jumping ahead to
## the Forest of Whispers, which stays Stage 6 as canon states.
##
## The stage's one mandatory gate is defeating the single DungeonMonster
## boss inside the main cave; CaveMonster threats along the way are real
## (they can hurt the hero) but optional, not a counted gate — the same
## register_guard()/notify_guard_defeated() contract every StageNController
## already uses, so with exactly one boss registering, the gate is
## effectively "defeat 1/1", consistent with how Stage 1/2 count multiple.

var _completed := false

signal stage_ready
signal boss_progress(defeated: bool)

func _ready() -> void:
    add_to_group("stage3_controller")
    add_to_group("stage_controller")

func register_guard() -> void:
    GameState.total_guards_stage1 += 1

func notify_guard_defeated() -> void:
    GameState.defeated_slavers += 1
    GameState.save_game()
    boss_progress.emit(true)
    _try_complete()

func _try_complete() -> void:
    if _completed:
        return
    if GameState.total_guards_stage1 > 0 and GameState.defeated_slavers >= GameState.total_guards_stage1:
        _completed = true
        GameState.advance_to_next_stage()
        stage_ready.emit()
