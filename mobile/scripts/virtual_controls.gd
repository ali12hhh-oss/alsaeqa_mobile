extends Control

## Touch-first controls. Desktop input remains available for GitHub CI smoke tests.

@onready var hero := get_parent().get_parent().get_node("Hero")

func _ready() -> void:
    $Sprint.button_down.connect(Callable(hero, "start_sprint"))
    $Sprint.button_up.connect(Callable(hero, "stop_sprint"))
    $Light.pressed.connect(Callable(hero, "light_attack"))
    $Heavy.pressed.connect(Callable(hero, "heavy_attack"))
    $Roll.pressed.connect(Callable(hero, "start_roll"))
    $Crouch.pressed.connect(Callable(hero, "toggle_crouch"))
    $Listen.pressed.connect(Callable(hero, "toggle_listen"))
    $Thunder.button_down.connect(Callable(hero, "start_thunder_charge"))
    $Thunder.button_up.connect(Callable(hero, "release_thunder"))
