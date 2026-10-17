extends RefCounted
class_name StoreCatalog

## Shared item catalog for the Store and Inventory screens.
## Items are narrative/collectible for now (no active gameplay effect is
## wired to them yet, e.g. no player health system exists to attach a
## "health potion" effect to) so nothing here overpromises a mechanic that
## does not actually work — they are real, purchasable, persistent
## inventory items, and can be wired to gameplay effects later without
## changing this catalog's shape.

class Item:
    var id: String
    var display_name: String
    var description: String
    var price: int

    func _init(p_id: String, p_name: String, p_description: String, p_price: int) -> void:
        id = p_id
        display_name = p_name
        description = p_description
        price = p_price

static func all_items() -> Array[Item]:
    return [
        Item.new("torch", "شعلة", "تضيء أعمق أركان المنجم.", 10),
        Item.new("old_map", "خريطة قديمة", "ترسم ممرات لم يسبق أن رآها أحد.", 15),
        Item.new("iron_ration", "زاد حديدي", "يكفي لأيام من الرحلة الشاقة.", 8),
        Item.new("lucky_charm", "تميمة الحظ", "قطعة معدن يتشبث بها العبيد الناجون.", 25),
        Item.new("healing_herb", "عشبة شافية", "تنمو قرب مصادر المياه الجوفية.", 12),
    ]

static func find_item(item_id: String) -> Item:
    for item in all_items():
        if item.id == item_id:
            return item
    return null
