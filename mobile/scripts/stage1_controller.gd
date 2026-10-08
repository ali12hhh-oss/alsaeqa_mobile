extends Node

## Stage 1 progression: exactly five counted workers must reach safety and
## every guard that was spawned for the mine must be defeated — not just one
## designated target. Defeats do not need to happen in a single sequence or
## burst; they accumulate one guard at a time as the hero fights through the
## mine, and the stage completes once the running total reaches every guard
## that actually exists in the scene.

const REQUIRED_WORKERS := 5
const MINIMUM_MINE_GUARDS := 7

var worker_safe: Array[bool] = [false, false, false, false, false]
var guards_defeated := 0
var _completed := false

signal stage_ready
signal rescue_progress(current: int, required: int)
signal guard_progress(current: int, required: int)

func _ready() -> void:
    # WorkerCaptive identifies this controller via the stage1_controller
    # group specifically (workers only exist in Stage 1). GuardEnemy looks
    # up the generic stage_controller group instead, since that script is
    # shared across every stage's guards.
    add_to_group("stage1_controller")
    add_to_group("stage_controller")

func rescue_worker(worker_index: int) -> void:
    if worker_index < 0 or worker_index >= REQUIRED_WORKERS:
        return
    if worker_safe[worker_index]:
        return
    worker_safe[worker_index] = true
    GameState.rescued_workers = _safe_worker_count()
    GameState.save_game()
    rescue_progress.emit(GameState.rescued_workers, REQUIRED_WORKERS)
    _try_complete()

## Called once by each GuardEnemy in its _ready(), since the real spawn
## count depends on the real asset library and is not a fixed design-time
## number — this is how Stage1Controller learns how many guards exist.
## NOTE: GameState.total_guards_stage1/defeated_slavers are shared,
## generically-named "current stage guard" counters (reset by
## GameState.advance_to_next_stage() on every transition), not literally
## Stage-1-only despite the field name — only one stage controller is ever
## active at a time, so this is safe without renaming the fields.
func register_guard() -> void:
    GameState.total_guards_stage1 += 1
    guard_progress.emit(guards_defeated, GameState.total_guards_stage1)

## Called once per guard death. Guards are defeated one at a time across the
## fight, not all at once — the stage only completes once every registered
## guard has been individually defeated.
func notify_guard_defeated() -> void:
    guards_defeated += 1
    GameState.defeated_slavers = guards_defeated
    GameState.save_game()
    guard_progress.emit(guards_defeated, GameState.total_guards_stage1)
    _try_complete()

func _safe_worker_count() -> int:
    var count := 0
    for safe in worker_safe:
        if safe:
            count += 1
    return count

func _try_complete() -> void:
    if _completed:
        return
    if _safe_worker_count() >= REQUIRED_WORKERS and GameState.total_guards_stage1 > 0 and guards_defeated >= GameState.total_guards_stage1:
        _completed = true
        GameState.advance_to_next_stage()
        stage_ready.emit()
