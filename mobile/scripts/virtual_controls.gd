extends Control

## Touch-first controls. Desktop input remains available for GitHub CI smoke tests.

@onready var hero := get_parent().get_parent().get_node("Hero")

func _ready() -> void:
    $Sprint.button_down.connect(hero.start_sprint)
    $Sprint.button_up.connect(hero.stop_sprint)
    $Light.pressed.connect(hero.light_attack)
    $Heavy.pressed.connect(hero.heavy_attack)
    $Roll.pressed.connect(_roll)
    $Thunder.button_down.connect(hero.start_thunder_charge)
    $Thunder.button_up.connect(hero.release_thunder)

func _roll() -> void:
    hero.rolling = true
    hero.velocity = -hero.global_transform.basis.z * hero.roll_speed
    await get_tree().create_timer(0.45).timeout
    hero.rolling = false
