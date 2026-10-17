extends Node3D
class_name WeaponChest

## An interactable reward chest for Stage 2: contains the hero's first real
## weapon/armor/outfit upgrade since escaping the mine. Same interact
## pattern as WorkerCaptive (Area3D + "interact" action), but grants gear
## instead of calling a rescue method.

@export var gear_ids: Array[String] = ["hidden_mark_blade", "wanderer_garb", "light_hide_armor"]
@export var interact_radius: float = 2.2

var _stage2: Node
var _hero_in_range := false
var _opened := false
var _prompt_label: Label3D

signal opened

func _ready() -> void:
    add_to_group("weapon_chest")
    _stage2 = get_tree().get_first_node_in_group("stage2_controller")

    var area := Area3D.new()
    area.name = "InteractArea"
    var shape := CollisionShape3D.new()
    var sphere := SphereShape3D.new()
    sphere.radius = interact_radius
    shape.shape = sphere
    area.add_child(shape)
    area.collision_layer = 0
    add_child(area)
    area.body_entered.connect(_on_body_entered)
    area.body_exited.connect(_on_body_exited)

    _prompt_label = Label3D.new()
    _prompt_label.text = "اضغط لفتح الصندوق"
    _prompt_label.position = Vector3.UP * 1.6
    _prompt_label.visible = false
    _prompt_label.billboard = BaseMaterial3D.BILLBOARD_ENABLED
    _prompt_label.font_size = 48
    _prompt_label.outline_size = 8
    add_child(_prompt_label)

func _on_body_entered(body: Node) -> void:
    if body.is_in_group("alsaeqa_hero"):
        _hero_in_range = true
        _prompt_label.visible = not _opened

func _on_body_exited(body: Node) -> void:
    if body.is_in_group("alsaeqa_hero"):
        _hero_in_range = false
        _prompt_label.visible = false

func _process(_delta: float) -> void:
    if _opened or not _hero_in_range:
        return
    if Input.is_action_just_pressed("interact"):
        _open()

func _open() -> void:
    if _opened:
        return
    _opened = true
    _prompt_label.visible = false
    for gear_id in gear_ids:
        GameState.grant_and_equip_gear(gear_id)
    if _stage2 != null and _stage2.has_method("notify_weapon_found"):
        _stage2.call("notify_weapon_found")
    else:
        push_warning("WeaponChest could not find Stage2Controller in group 'stage2_controller'")
    opened.emit()
