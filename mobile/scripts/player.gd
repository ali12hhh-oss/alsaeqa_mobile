extends CharacterBody3D

## Runtime hero base for the real ALSAEQA character.
## The visual is supplied by real_asset_world.gd; this controller owns the
## gameplay body and discovers the imported Skeleton3D/AnimationPlayer so the
## next animation/combat stages can use the real rig without another hero node.

@export var walk_speed := 4.0
@export var sprint_speed := 7.0
@export var acceleration := 18.0
@export var gravity := 18.0
@export var roll_speed := 10.0

var sprinting := false
var rolling := false
var thunder_charging := false
var thunder_charge := 0.0

var hero_visual: Node3D
var hero_skeleton: Skeleton3D
var hero_animation_player: AnimationPlayer
var hero_visual_ready := false

func _ready() -> void:
    _bind_real_hero_visual()

func _bind_real_hero_visual() -> void:
    hero_visual = null
    hero_skeleton = null
    hero_animation_player = null
    hero_visual_ready = false

    for child in get_children():
        if child is Node3D:
            var candidate := child as Node3D
            if candidate.name.begins_with("hero_real_") or candidate.name.begins_with("Hero_Real_"):
                hero_visual = candidate
                break

    if hero_visual == null:
        push_warning("Hero gameplay body exists, but no real hero visual is attached yet")
        return

    hero_skeleton = _find_first_skeleton(hero_visual)
    hero_animation_player = _find_first_animation_player(hero_visual)
    hero_visual_ready = _has_real_visual_mesh(hero_visual)

    if not hero_visual_ready:
        push_error("Hero visual is attached but contains no renderable real mesh")

func _find_first_skeleton(node: Node) -> Skeleton3D:
    for child in node.get_children():
        if child is Skeleton3D:
            return child as Skeleton3D
        var nested := _find_first_skeleton(child)
        if nested != null:
            return nested
    return null

func _find_first_animation_player(node: Node) -> AnimationPlayer:
    for child in node.get_children():
        if child is AnimationPlayer:
            return child as AnimationPlayer
        var nested := _find_first_animation_player(child)
        if nested != null:
            return nested
    return null

func _has_real_visual_mesh(node: Node) -> bool:
    for child in node.get_children():
        if child is MeshInstance3D and (child as MeshInstance3D).mesh != null:
            return true
        if _has_real_visual_mesh(child):
            return true
    return false

func _physics_process(delta: float) -> void:
    if not is_on_floor():
        velocity.y -= gravity * delta

    var input_vector := Input.get_vector("move_left", "move_right", "move_forward", "move_back")
    var direction := Vector3(input_vector.x, 0.0, input_vector.y)
    if direction.length() > 1.0:
        direction = direction.normalized()

    if not rolling:
        var speed := sprint_speed if sprinting else walk_speed
        var target := direction * speed
        velocity.x = move_toward(velocity.x, target.x, acceleration * delta)
        velocity.z = move_toward(velocity.z, target.z, acceleration * delta)
        if direction.length_squared() > 0.01:
            look_at(global_position + Vector3(direction.x, 0.0, direction.z), Vector3.UP)

    move_and_slide()

func start_sprint() -> void:
    sprinting = true

func stop_sprint() -> void:
    sprinting = false

func light_attack() -> void:
    CinematicDirector.combat_impact(false)

func heavy_attack() -> void:
    CinematicDirector.combat_impact(true)

func start_thunder_charge() -> void:
    thunder_charging = true
    thunder_charge = 0.0

func release_thunder() -> void:
    if not thunder_charging:
        return
    thunder_charging = false
    var ratio: float = clampf(thunder_charge, 0.0, 1.0)
    GameState.thunder_charge = ratio
    CinematicDirector.thunder_impact(ratio)
    thunder_charge = 0.0

func cancel_thunder() -> void:
    thunder_charging = false
    thunder_charge = 0.0

func _process(delta: float) -> void:
    if thunder_charging:
        thunder_charge = min(thunder_charge + delta * 0.55, 1.0)
