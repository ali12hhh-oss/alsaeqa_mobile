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
    # worker_captive.gd/weapon_chest.gd both gate on
    # Input.is_action_just_pressed("interact"), bound only to the physical
    # E key in project.godot — there was no touch equivalent at all, so a
    # touch-only player could never rescue a worker or open the weapon
    # chest. Pressing and releasing the action within one frame makes
    # is_action_just_pressed() see it exactly like a real key press.
    $Interact.pressed.connect(_on_interact_pressed)

func _on_interact_pressed() -> void:
    Input.action_press("interact")
    await get_tree().process_frame
    Input.action_release("interact")
