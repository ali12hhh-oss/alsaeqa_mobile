extends Control

## Inventory overlay: shows everything the player currently owns, whether
## bought from the Store or collected during gameplay (both paths go
## through GameState.add_item, so this always reflects the real total).

@onready var item_list: VBoxContainer = $Panel/Scroll/ItemList
@onready var close_button: Button = $Panel/CloseButton
@onready var empty_label: Label = $Panel/EmptyLabel

func _ready() -> void:
    visible = false
    close_button.pressed.connect(hide_screen)
    GameState.inventory_changed.connect(_on_inventory_changed)

func show_screen() -> void:
    visible = true
    _refresh()

func hide_screen() -> void:
    visible = false

func _on_inventory_changed() -> void:
    if visible:
        _refresh()

func _refresh() -> void:
    for child in item_list.get_children():
        child.queue_free()

    var owned_any := false
    for item in StoreCatalog.all_items():
        var count: int = GameState.item_count(item.id)
        if count <= 0:
            continue
        owned_any = true

        var row := HBoxContainer.new()
        row.custom_minimum_size = Vector2(0, 56)

        var info := VBoxContainer.new()
        info.size_flags_horizontal = Control.SIZE_EXPAND_FILL
        var name_label := Label.new()
        name_label.text = item.display_name
        name_label.add_theme_font_size_override("font_size", 22)
        var desc_label := Label.new()
        desc_label.text = item.description
        desc_label.add_theme_font_size_override("font_size", 14)
        desc_label.modulate.a = 0.75
        info.add_child(name_label)
        info.add_child(desc_label)
        row.add_child(info)

        var count_label := Label.new()
        count_label.text = "x%d" % count
        count_label.custom_minimum_size = Vector2(70, 0)
        count_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
        count_label.add_theme_font_size_override("font_size", 22)
        row.add_child(count_label)

        item_list.add_child(row)

    empty_label.visible = not owned_any
