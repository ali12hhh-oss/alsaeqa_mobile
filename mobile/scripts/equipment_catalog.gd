extends RefCounted
class_name EquipmentCatalog

## Equippable gear catalog: weapons, armor, and outfits that actually change
## the hero's combat stats (unlike StoreCatalog's narrative-only items).
## Designed to grow stage by stage — each stage can add new tiers here
## without touching GameState or player.gd, which only ever read the
## currently-equipped item's bonus values.

enum Slot { WEAPON, ARMOR, OUTFIT }

class Gear:
    var id: String
    var display_name: String
    var description: String
    var slot: Slot
    var damage_bonus: float
    var defense_bonus: float

    func _init(p_id: String, p_name: String, p_description: String, p_slot: Slot, p_damage: float = 0.0, p_defense: float = 0.0) -> void:
        id = p_id
        display_name = p_name
        description = p_description
        slot = p_slot
        damage_bonus = p_damage
        defense_bonus = p_defense

## Default starting gear the hero already has from the mine (Stage 1) —
## the plain outfit attached by attach_hero_outfit.py, no bonuses.
const STARTING_OUTFIT := "mine_rags"

static func all_gear() -> Array[Gear]:
    return [
        Gear.new("mine_rags", "أسمال المنجم", "ما تبقّى من ثياب سنوات العبودية.", Slot.OUTFIT, 0.0, 0.0),
        # Stage 2 reward set — the hero's first real upgrade since escaping
        # the mine.
        Gear.new("hidden_mark_blade", "نصل العلامة الخفية", "سلاح حراس مجهولين، محفور عليه رمز لا يعرفه بعد.", Slot.WEAPON, 12.0, 0.0),
        Gear.new("wanderer_garb", "ثياب الرحّالة", "أخف وأمتن من أسمال المنجم بكثير.", Slot.OUTFIT, 0.0, 0.0),
        Gear.new("light_hide_armor", "درع جلدي خفيف", "حماية أساسية لا تُبطئ الحركة.", Slot.ARMOR, 0.0, 8.0),
    ]

static func find_gear(gear_id: String) -> Gear:
    for gear in all_gear():
        if gear.id == gear_id:
            return gear
    return null

static func gear_for_slot(slot: Slot) -> Array[Gear]:
    var result: Array[Gear] = []
    for gear in all_gear():
        if gear.slot == slot:
            result.append(gear)
    return result
