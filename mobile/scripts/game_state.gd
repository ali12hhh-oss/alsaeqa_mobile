extends Node

## Persistent mobile game state. Keeps the original ALSAEQA progression model
## while removing Unreal-specific dependencies.

var current_stage: int = 1
var rescued_workers: int = 0
var defeated_slavers: int = 0
## Total number of guards actually spawned for the current stage, set once
## by the active stage controller as guards register themselves at runtime
## (the count depends on how many guard-role assets the real asset library
## produced, not a fixed design-time number). Reset per stage by the stage
## controller that owns it.
var total_guards_stage1: int = 0
var discovered_clues: Array[String] = []
var thunder_charge: float = 0.0
var hero_name: String = "الصاعقة"

## Store/inventory economy. inventory maps item_id -> owned count; items are
## defined in store_catalog.gd, not duplicated here, so this stays a plain
## data map regardless of how the catalog grows.
var currency: int = 50
var inventory: Dictionary = {}

## Combat gear. owned_gear is every EquipmentCatalog item id the hero has
## ever found/earned (permanent, unlike consumable store items); equipped_*
## holds the currently active id per slot, empty string = nothing equipped
## in that slot. player.gd reads get_damage_bonus()/get_defense_bonus()
## rather than any raw stat, so future stages can add stronger gear here
## without touching combat code.
var owned_gear: Array[String] = [EquipmentCatalog.STARTING_OUTFIT]
var equipped_weapon: String = ""
var equipped_armor: String = ""
var equipped_outfit: String = EquipmentCatalog.STARTING_OUTFIT

## Master volume, 0.0 (silent) to 1.0 (full). Applied to the engine's real
## Master audio bus in settings_screen.gd, not just stored as a number, so
## it actually controls the music/combat sound volume the user hears.
var master_volume: float = 0.8

signal currency_changed(new_amount: int)
signal inventory_changed
signal settings_changed
signal gear_changed
signal stage_advanced(new_stage: int)

const SAVE_PATH := "user://alsaeqa_save.json"

func _ready() -> void:
    load_game()
    _apply_master_volume()

## Called by the active stage's controller once its own win condition is
## met. GameState does not know what any individual stage requires — each
## StageNController owns and checks its own conditions and calls this
## exactly once when they are satisfied.
func advance_to_next_stage() -> void:
    current_stage += 1
    total_guards_stage1 = 0
    defeated_slavers = 0
    save_game()
    stage_advanced.emit(current_stage)

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

## Grants gear the hero found (a chest, a reward, etc.) and equips it
## immediately — stage rewards are meant to be felt right away, not left
## sitting unused in a menu.
func grant_and_equip_gear(gear_id: String) -> void:
    if gear_id not in owned_gear:
        owned_gear.append(gear_id)
    equip_gear(gear_id)

func equip_gear(gear_id: String) -> void:
    var gear: EquipmentCatalog.Gear = EquipmentCatalog.find_gear(gear_id)
    if gear == null or gear_id not in owned_gear:
        return
    match gear.slot:
        EquipmentCatalog.Slot.WEAPON:
            equipped_weapon = gear_id
        EquipmentCatalog.Slot.ARMOR:
            equipped_armor = gear_id
        EquipmentCatalog.Slot.OUTFIT:
            equipped_outfit = gear_id
    gear_changed.emit()
    save_game()

func get_damage_bonus() -> float:
    var total := 0.0
    for gear_id in [equipped_weapon, equipped_armor, equipped_outfit]:
        var gear: EquipmentCatalog.Gear = EquipmentCatalog.find_gear(gear_id)
        if gear != null:
            total += gear.damage_bonus
    return total

func get_defense_bonus() -> float:
    var total := 0.0
    for gear_id in [equipped_weapon, equipped_armor, equipped_outfit]:
        var gear: EquipmentCatalog.Gear = EquipmentCatalog.find_gear(gear_id)
        if gear != null:
            total += gear.defense_bonus
    return total

func set_master_volume(value: float) -> void:
    master_volume = clampf(value, 0.0, 1.0)
    _apply_master_volume()
    settings_changed.emit()
    save_game()

func _apply_master_volume() -> void:
    var bus_index := AudioServer.get_bus_index("Master")
    if bus_index < 0:
        return
    AudioServer.set_bus_mute(bus_index, master_volume <= 0.001)
    if master_volume > 0.001:
        AudioServer.set_bus_volume_db(bus_index, linear_to_db(master_volume))

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
        "inventory": inventory,
        "master_volume": master_volume,
        "owned_gear": owned_gear,
        "equipped_weapon": equipped_weapon,
        "equipped_armor": equipped_armor,
        "equipped_outfit": equipped_outfit
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
        master_volume = float(parsed.get("master_volume", 0.8))
        var raw_owned_gear = parsed.get("owned_gear", [EquipmentCatalog.STARTING_OUTFIT])
        owned_gear = Array(raw_owned_gear) if raw_owned_gear is Array else [EquipmentCatalog.STARTING_OUTFIT]
        equipped_weapon = str(parsed.get("equipped_weapon", ""))
        equipped_armor = str(parsed.get("equipped_armor", ""))
        equipped_outfit = str(parsed.get("equipped_outfit", EquipmentCatalog.STARTING_OUTFIT))
