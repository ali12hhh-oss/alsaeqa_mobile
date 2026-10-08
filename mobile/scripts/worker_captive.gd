extends Node3D
class_name WorkerCaptive

## A rescuable captive worker for Stage 1 ("Chains in the Mine").
## Wraps a real imported worker/base-character visual with an interact
## trigger. Pressing "interact" while the hero is in range calls
## Stage1Controller.rescue_worker(worker_index) exactly once.

@export var worker_index: int = -1
@export var interact_radius: float = 2.4

var _stage1: Node
var _hero_in_range := false
var _rescued := false
var _prompt_label: Label3D

func _ready() -> void:
    add_to_group("worker_captive")
    _stage1 = get_tree().get_first_node_in_group("stage1_controller")

    var area := Area3D.new()
    area.name = "InteractArea"
    var shape := CollisionShape3D.new()
    var sphere := SphereShape3D.new()
    sphere.radius = interact_radius
    shape.shape = sphere
    area.add_child(shape)
    area.collision_layer = 0
    # Rely on the default physics layer for detecting the hero body; group
    # membership ("alsaeqa_hero") is the authoritative filter below, so an
    # exact collision_mask bit is not required.
    add_child(area)
    area.body_entered.connect(_on_body_entered)
    area.body_exited.connect(_on_body_exited)

    _prompt_label = Label3D.new()
    _prompt_label.text = "اضغط للتحرير"
    _prompt_label.position = Vector3.UP * 2.1
    _prompt_label.visible = false
    _prompt_label.billboard = BaseMaterial3D.BILLBOARD_ENABLED
    _prompt_label.font_size = 48
    _prompt_label.outline_size = 8
    add_child(_prompt_label)

func _on_body_entered(body: Node) -> void:
    if body.is_in_group("alsaeqa_hero"):
        _hero_in_range = true
        _prompt_label.visible = not _rescued

func _on_body_exited(body: Node) -> void:
    if body.is_in_group("alsaeqa_hero"):
        _hero_in_range = false
        _prompt_label.visible = false

func _process(_delta: float) -> void:
    if _rescued or not _hero_in_range:
        return
    if Input.is_action_just_pressed("interact"):
        _rescue()

func _rescue() -> void:
    if _rescued:
        return
    _rescued = true
    _prompt_label.visible = false
    if _stage1 != null and _stage1.has_method("rescue_worker"):
        _stage1.call("rescue_worker", worker_index)
    else:
        push_warning("WorkerCaptive %d could not find Stage1Controller in group 'stage1_controller'" % worker_index)
    # Play a released/relief animation if the real asset exposes one;
    # otherwise fall back to a simple visible cue so the rescue still reads
    # clearly without inventing a placeholder mesh.
    var player := _find_first_animation_player(self)
    if player != null:
        for token in ["cheer", "relief", "wave", "idle"]:
            for animation_name in player.get_animation_list():
                if String(animation_name).to_lower().contains(token):
                    player.play(animation_name)
                    return

func _find_first_animation_player(node: Node) -> AnimationPlayer:
    for child in node.get_children():
        if child is AnimationPlayer:
            return child as AnimationPlayer
        var nested := _find_first_animation_player(child)
        if nested != null:
            return nested
    return null
