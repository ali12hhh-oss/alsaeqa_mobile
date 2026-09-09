extends Node

## Persistent mobile game state. Keeps the original ALSAEQA progression model
## while removing Unreal-specific dependencies.

var current_stage: int = 1
var rescued_workers: int = 0
var defeated_slavers: int = 0
var discovered_clues: Array[String] = []
var thunder_charge: float = 0.0
var hero_name: String = "الصاعقة"

const SAVE_PATH := "user://alsaeqa_save.json"

func _ready() -> void:
    load_game()

func complete_stage_if_ready() -> bool:
    if current_stage == 1 and rescued_workers >= 5 and defeated_slavers >= 1:
        current_stage = 2
        add_clue("Stage1_MineNetwork")
        save_game()
        return true
    return false

func add_clue(clue_id: String) -> void:
    if clue_id not in discovered_clues:
        discovered_clues.append(clue_id)

func save_game() -> void:
    var data := {
        "current_stage": current_stage,
        "rescued_workers": rescued_workers,
        "defeated_slavers": defeated_slavers,
        "discovered_clues": discovered_clues,
        "thunder_charge": thunder_charge,
        "hero_name": hero_name
    }
    var file := FileAccess.open(SAVE_PATH, FileAccess.WRITE)
    if file:
        file.store_string(JSON.stringify(data))

func load_game() -> void:
    if not FileAccess.file_exists(SAVE_PATH):
        return
    var file := FileAccess.open(SAVE_PATH, FileAccess.READ)
    if not file:
        return
    var parsed = JSON.parse_string(file.get_as_text())
    if parsed is Dictionary:
        current_stage = int(parsed.get("current_stage", 1))
        rescued_workers = int(parsed.get("rescued_workers", 0))
        defeated_slavers = int(parsed.get("defeated_slavers", 0))
        discovered_clues = Array(parsed.get("discovered_clues", []))
        thunder_charge = float(parsed.get("thunder_charge", 0.0))
        hero_name = str(parsed.get("hero_name", "الصاعقة"))
