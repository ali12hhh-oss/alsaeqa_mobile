extends Control

## Settings overlay: master volume control and save-reset, opened from the
## home screen "الضبط" button. Volume actually drives the engine's Master
## audio bus (see GameState.set_master_volume), it is not a cosmetic slider.

@onready var volume_slider: HSlider = $Panel/VolumeSlider
@onready var volume_value_label: Label = $Panel/VolumeValueLabel
@onready var close_button: Button = $Panel/CloseButton
@onready var reset_button: Button = $Panel/ResetButton
@onready var reset_confirm_label: Label = $Panel/ResetConfirmLabel

func _ready() -> void:
    visible = false
    close_button.pressed.connect(hide_screen)
    reset_button.pressed.connect(_on_reset_pressed)
    volume_slider.min_value = 0.0
    volume_slider.max_value = 1.0
    volume_slider.step = 0.05
    volume_slider.value_changed.connect(_on_volume_changed)
    reset_confirm_label.visible = false

func show_screen() -> void:
    visible = true
    volume_slider.value = GameState.master_volume
    _update_volume_label()

func hide_screen() -> void:
    visible = false

func _on_volume_changed(value: float) -> void:
    GameState.set_master_volume(value)
    _update_volume_label()

func _update_volume_label() -> void:
    volume_value_label.text = "%d%%" % int(round(GameState.master_volume * 100.0))

## Clearing saved progress is destructive, so it needs a second confirming
## tap rather than firing on the first press.
var _reset_armed := false

func _on_reset_pressed() -> void:
    if not _reset_armed:
        _reset_armed = true
        reset_confirm_label.visible = true
        reset_button.text = "اضغط مرة أخرى للتأكيد"
        return
    _reset_armed = false
    reset_confirm_label.visible = false
    reset_button.text = "إعادة تعيين التقدم"
    if FileAccess.file_exists(GameState.SAVE_PATH):
        DirAccess.remove_absolute(GameState.SAVE_PATH)
    GameState.current_stage = 1
    GameState.rescued_workers = 0
    GameState.defeated_slavers = 0
    GameState.total_guards_stage1 = 0
    GameState.discovered_clues.clear()
    GameState.currency = 50
    GameState.inventory.clear()
    GameState.inventory_changed.emit()
    GameState.currency_changed.emit(GameState.currency)
    GameState.save_game()
