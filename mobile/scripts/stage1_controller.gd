extends Node

## Stage 1 progression: exactly five counted workers must reach safety and the
## designated slaver must be defeated. Additional mine guards create pressure
## but are not a separate progression gate.

const REQUIRED_WORKERS := 5
const MINIMUM_MINE_GUARDS := 7

var worker_safe: Array[bool] = [false, false, false, false, false]
var designated_slaver_defeated := false

signal stage_ready
signal rescue_progress(current: int, required: int)

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

func defeat_designated_slaver() -> void:
    if designated_slaver_defeated:
        return
    designated_slaver_defeated = true
    GameState.defeated_slavers = 1
    GameState.save_game()
    _try_complete()

func _safe_worker_count() -> int:
    var count := 0
    for safe in worker_safe:
        if safe:
            count += 1
    return count

func _try_complete() -> void:
    if _safe_worker_count() >= REQUIRED_WORKERS and designated_slaver_defeated:
        if GameState.complete_stage_if_ready():
            stage_ready.emit()
