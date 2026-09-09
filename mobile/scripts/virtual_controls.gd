extends Control

## Touch-first controls. Desktop input remains available for GitHub CI smoke tests.

@onready var hero := get_parent().get_parent().get_node("Hero")

func _ready() -> void:
    $Sprint.button_down.connect(hero.start_sprint)
    $Sprint.button_up.connect(hero.stop_sprint)
    $Light.pressed.connect(hero.light_attack)
    $Heavy.pressed.connect(hero.heavy_attack)
    $Roll.pressed.connect(hero.start_roll)
    $Crouch.pressed.connect(hero.toggle_crouch)
    $Listen.pressed.connect(hero.toggle_listen)
    $Thunder.button_down.connect(hero.start_thunder_charge)
    $Thunder.button_up.connect(hero.release_thunder)
