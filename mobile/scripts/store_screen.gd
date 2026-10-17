extends Control

## Store overlay: spend currency earned in-game to buy narrative/collectible
## items (see store_catalog.gd). Opened from the home screen "المتجر"
## button and closable at any time.

@onready var currency_label: Label = $Panel/CurrencyLabel
@onready var item_list: VBoxContainer = $Panel/Scroll/ItemList
@onready var close_button: Button = $Panel/CloseButton

func _ready() -> void:
    visible = false
    close_button.pressed.connect(hide_screen)
    GameState.currency_changed.connect(_on_currency_changed)

func show_screen() -> void:
    visible = true
    _refresh()

func hide_screen() -> void:
    visible = false

func _on_currency_changed(_new_amount: int) -> void:
    if visible:
        _refresh()

func _refresh() -> void:
    currency_label.text = "رصيدك: %d" % GameState.currency
    for child in item_list.get_children():
        child.queue_free()

    for item in StoreCatalog.all_items():
        var row := HBoxContainer.new()
        row.custom_minimum_size = Vector2(0, 64)

        var info := VBoxContainer.new()
        info.size_flags_horizontal = Control.SIZE_EXPAND_FILL
        var name_label := Label.new()
        name_label.text = "%s  —  %d" % [item.display_name, item.price]
        name_label.add_theme_font_size_override("font_size", 22)
        var desc_label := Label.new()
        desc_label.text = item.description
        desc_label.add_theme_font_size_override("font_size", 14)
        desc_label.modulate.a = 0.75
        info.add_child(name_label)
        info.add_child(desc_label)
        row.add_child(info)

        var owned_label := Label.new()
        owned_label.text = "لديك: %d" % GameState.item_count(item.id)
        owned_label.custom_minimum_size = Vector2(90, 0)
        owned_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
        row.add_child(owned_label)

        var buy_button := Button.new()
        buy_button.text = "شراء"
        buy_button.custom_minimum_size = Vector2(110, 0)
        buy_button.disabled = GameState.currency < item.price
        buy_button.pressed.connect(_on_buy_pressed.bind(item.id, item.price))
        row.add_child(buy_button)

        item_list.add_child(row)

func _on_buy_pressed(item_id: String, price: int) -> void:
    GameState.purchase_item(item_id, price)
    _refresh()
