extends Node

## Stage 2 progression ("The Hidden Mark"): the hero searches the rocky
## terrain around the mine for a weapon cache, guarded by a small pair of
## sentries left behind by whoever placed it. Finding the chest AND
## defeating every guard set to protect it clears the stage — same
## all-guards-required pattern as Stage 1, just a much smaller guard count
## (2, not 7-9), since this is meant to read as a brief ambush, not another
## full mine-clearing fight.

const REQUIRED_GUARDS := 2

var weapon_found := false
var guards_defeated := 0
var _completed := false

signal stage_ready
signal objective_progress(weapon_found: bool, guards_current: int, guards_required: int)

func _ready() -> void:
    # "stage2_controller" lets WeaponChest find this specific controller;
    # "stage_controller" is the shared group GuardEnemy looks up regardless
    # of which stage is active.
    add_to_group("stage2_controller")
    add_to_group("stage_controller")

func notify_weapon_found() -> void:
    if weapon_found:
        return
    weapon_found = true
    GameState.add_clue("Stage2_HiddenMark")
    objective_progress.emit(weapon_found, guards_defeated, REQUIRED_GUARDS)
    _try_complete()

## Same registration pattern as Stage1Controller: guards register themselves
## in _ready() so this controller knows the real spawned count rather than
## assuming a fixed number.
func register_guard() -> void:
    GameState.total_guards_stage1 += 1
    objective_progress.emit(weapon_found, guards_defeated, GameState.total_guards_stage1)

func notify_guard_defeated() -> void:
    guards_defeated += 1
    GameState.defeated_slavers = guards_defeated
    GameState.save_game()
    objective_progress.emit(weapon_found, guards_defeated, GameState.total_guards_stage1)
    _try_complete()

func _try_complete() -> void:
    if _completed:
        return
    if weapon_found and GameState.total_guards_stage1 > 0 and guards_defeated >= GameState.total_guards_stage1:
        _completed = true
        GameState.advance_to_next_stage()
        stage_ready.emit()
