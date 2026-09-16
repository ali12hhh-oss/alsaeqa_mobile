extends Node

## Persistent mobile game state. Keeps the original ALSAEQA progression model
## while removing Unreal-specific dependencies.

var current_stage: int = 1
var rescued_workers: int = 0
var defeated_slavers: int = 0
## Total number of guards actually spawned for the current stage, set once
## by Stage1Controller as guards register themselves at runtime (the count
## depends on how many guard-role assets the real asset library produced,
## not a fixed design-time number).
var total_guards_stage1: int = 0
var discovered_clues: Array[String] = []
var thunder_charge: float = 0.0
var hero_name: String = "الصاعقة"

## Store/inventory economy. inventory maps item_id -> owned count; items are
## defined in store_catalog.gd, not duplicated here, so this stays a plain
## data map regardless of how the catalog grows.
var currency: int = 50
var inventory: Dictionary = {}

signal currency_changed(new_amount: int)
signal inventory_changed

const SAVE_PATH := "user://alsaeqa_save.json"

func _ready() -> void:
    load_game()

func complete_stage_if_ready() -> bool:
    # All guards must be defeated to clear the stage — not just one
    # designated target. They do not need to be defeated in a single burst;
    # defeats accumulate one at a time as the hero fights through the mine,
    # and the stage completes once the running total reaches every guard
    # that was actually spawned.
    if current_stage == 1 and rescued_workers >= 5 and total_guards_stage1 > 0 and defeated_slavers >= total_guards_stage1:
        current_stage = 2
        add_clue("Stage1_MineNetwork")
        save_game()
        return true
    return false

func add_clue(clue_id: String) -> void:
    if clue_id not in discovered_clues:
        discovered_clues.append(clue_id)

## Adds an item to the inventory regardless of source (store purchase or a
## gameplay pickup/reward). Kept as one shared entry point so both cases
## always go through the same accounting and the same signal.
func add_item(item_id: String, count: int = 1) -> void:
    if count <= 0:
        return
    inventory[item_id] = int(inventory.get(item_id, 0)) + count
    inventory_changed.emit()
    save_game()

func item_count(item_id: String) -> int:
    return int(inventory.get(item_id, 0))

## Returns true and deducts currency/grants the item only if the purchase
## actually succeeds (enough currency); callers should check the return
## value rather than assuming success.
func purchase_item(item_id: String, price: int) -> bool:
    if price < 0 or currency < price:
        return false
    currency -= price
    add_item(item_id, 1)
    currency_changed.emit(currency)
    save_game()
    return true

func add_currency(amount: int) -> void:
    if amount == 0:
        return
    currency = max(currency + amount, 0)
    currency_changed.emit(currency)
    save_game()

func save_game() -> void:
    var data := {
        "current_stage": current_stage,
        "rescued_workers": rescued_workers,
        "defeated_slavers": defeated_slavers,
        "total_guards_stage1": total_guards_stage1,
        "discovered_clues": discovered_clues,
        "thunder_charge": thunder_charge,
        "hero_name": hero_name,
        "currency": currency,
        "inventory": inventory
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
        total_guards_stage1 = int(parsed.get("total_guards_stage1", 0))
        discovered_clues = Array(parsed.get("discovered_clues", []))
        thunder_charge = float(parsed.get("thunder_charge", 0.0))
        hero_name = str(parsed.get("hero_name", "الصاعقة"))
        currency = int(parsed.get("currency", 50))
        var raw_inventory = parsed.get("inventory", {})
        inventory = raw_inventory if raw_inventory is Dictionary else {}
