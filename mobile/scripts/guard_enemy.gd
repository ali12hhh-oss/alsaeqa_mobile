extends CharacterBody3D
class_name GuardEnemy

## Stage 1 mine guard enemy body.
## Wraps a real imported character visual with a CharacterBody3D so the
## hero's melee attack shape-query (which requires an actual PhysicsBody3D
## collider — see player.gd _resolve_attack_hits) can hit it and call
## receive_damage(). Every guard counts toward the stage's clear condition;
## there is no single "designated" target — defeating one guard does not by
## itself complete anything, all of them must fall (one at a time, not
## simultaneously) before Stage1Controller marks the stage clear. Guards do
## not move on their own in this first pass (they hold patrol/guard points);
## chase/patrol AI is a later addition, not invented here to avoid shipping
## unfinished behavior as if it were final.

@export var max_health: float = 60.0

var health: float
var _defeated := false
var _stage1: Node
var _hit_flash_time := 0.0

func _ready() -> void:
    add_to_group("guard_enemy")
    health = max_health
    _stage1 = get_tree().get_first_node_in_group("stage1_controller")
    collision_layer = 1
    collision_mask = 1
    if _stage1 != null and _stage1.has_method("register_guard"):
        _stage1.call("register_guard")
    else:
        push_warning("GuardEnemy could not find Stage1Controller in group 'stage1_controller' to register itself")

func _physics_process(delta: float) -> void:
    # Guards currently hold position; gravity keeps them grounded so they do
    # not float if placed slightly above the mesh surface.
    if not is_on_floor():
        velocity.y -= 18.0 * delta
    else:
        velocity.y = -0.5
    move_and_slide()

## Called by the hero's attack resolution (player.gd _resolve_attack_hits)
## on any collider that exposes this method.
func receive_damage(amount: float, _source_position: Vector3, _heavy: bool) -> void:
    if _defeated:
        return
    health = maxf(health - amount, 0.0)
    _hit_flash_time = 0.15
    if health <= 0.0:
        _defeat()

func _defeat() -> void:
    if _defeated:
        return
    _defeated = true
    set_physics_process(false)
    collision_layer = 0
    collision_mask = 0

    var player := _find_first_animation_player(self)
    var played_death_animation := false
    if player != null:
        for animation_name in player.get_animation_list():
            if String(animation_name).to_lower().contains("death") or String(animation_name).to_lower().contains("die"):
                player.play(animation_name)
                played_death_animation = true
                break
    if not played_death_animation:
        # No death animation available in the real asset library for this
        # rig; fall back to visibly removing the guard rather than leaving
        # a frozen T-posed corpse standing in the scene.
        visible = false

    if _stage1 != null and _stage1.has_method("notify_guard_defeated"):
        _stage1.call("notify_guard_defeated")

func _find_first_animation_player(node: Node) -> AnimationPlayer:
    for child in node.get_children():
        if child is AnimationPlayer:
            return child as AnimationPlayer
        var nested := _find_first_animation_player(child)
        if nested != null:
            return nested
    return null
