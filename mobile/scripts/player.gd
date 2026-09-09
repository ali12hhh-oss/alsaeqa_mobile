extends CharacterBody3D

## Canonical runtime hero controller.
## The real imported character remains the visual child; this body owns
## movement state, physics and mobile/desktop gameplay input.

@export var walk_speed := 3.8
@export var jog_speed := 5.2
@export var sprint_speed := 7.0
@export var acceleration := 18.0
@export var braking := 22.0
@export var air_acceleration := 7.0
@export var gravity := 18.0
@export var max_fall_speed := 28.0
@export var turn_speed := 11.0
@export var roll_speed := 10.0
@export var roll_duration := 0.45
@export var crouch_speed := 2.4

var sprinting := false
var crouching := false
var listening := false
var rolling := false
var thunder_charging := false
var thunder_charge := 0.0
var _roll_time_left := 0.0

var hero_visual: Node3D
var hero_skeleton: Skeleton3D
var hero_animation_player: AnimationPlayer
var hero_visual_ready := false

func _ready() -> void:
    bind_real_hero_visual()

## Called by the asset bridge after the real hero scene is instantiated.
## Main.tscn keeps one canonical Hero gameplay body; the imported visual is
## attached to that body at runtime, so binding must happen after attachment.
func bind_real_hero_visual() -> void:
    hero_visual = null
    hero_skeleton = null
    hero_animation_player = null
    hero_visual_ready = false

    for child in get_children():
        if child is Node3D:
            var candidate := child as Node3D
            if candidate.name.to_lower().begins_with("hero_real_"):
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
    if not hero_visual_ready:
        bind_real_hero_visual()

    _update_movement_state(delta)
    _update_vertical_motion(delta)
    move_and_slide()

func _update_movement_state(delta: float) -> void:
    var input_vector := Input.get_vector("move_left", "move_right", "move_forward", "move_back")
    var input_strength := minf(input_vector.length(), 1.0)

    if Input.is_action_just_pressed("crouch") and not rolling:
        crouching = not crouching
        if crouching:
            sprinting = false

    if Input.is_action_just_pressed("listen") and not rolling:
        listening = not listening
        if listening:
            sprinting = false

    if not Input.is_action_pressed("sprint") and sprinting:
        sprinting = false

    if rolling:
        _roll_time_left = maxf(_roll_time_left - delta, 0.0)
        if _roll_time_left <= 0.0:
            rolling = false
    elif Input.is_action_just_pressed("roll") and is_on_floor() and input_strength > 0.05:
        start_roll(input_vector)

    if rolling:
        return

    var direction := _camera_relative_direction(input_vector)
    var speed := walk_speed
    if listening:
        speed = crouch_speed
    elif crouching:
        speed = crouch_speed
    elif sprinting and input_strength > 0.05:
        speed = sprint_speed
    elif input_strength > 0.05:
        speed = jog_speed

    var target_velocity := direction * speed * input_strength
    var rate := acceleration if is_on_floor() else air_acceleration
    if input_strength <= 0.05:
        rate = braking if is_on_floor() else air_acceleration

    velocity.x = move_toward(velocity.x, target_velocity.x, rate * delta)
    velocity.z = move_toward(velocity.z, target_velocity.z, rate * delta)

    if direction.length_squared() > 0.01:
        var target_angle := atan2(-direction.x, -direction.z)
        rotation.y = lerp_angle(rotation.y, target_angle, minf(turn_speed * delta, 1.0))

func _camera_relative_direction(input_vector: Vector2) -> Vector3:
    if input_vector.length_squared() <= 0.0001:
        return Vector3.ZERO

    var camera := get_viewport().get_camera_3d()
    if camera == null:
        return Vector3(input_vector.x, 0.0, input_vector.y).normalized()

    var forward := -camera.global_transform.basis.z
    var right := camera.global_transform.basis.x
    forward.y = 0.0
    right.y = 0.0
    forward = forward.normalized()
    right = right.normalized()

    return (right * input_vector.x + forward * input_vector.y).normalized()

func _update_vertical_motion(delta: float) -> void:
    if is_on_floor():
        if velocity.y < 0.0:
            velocity.y = -0.5
        return

    velocity.y = maxf(velocity.y - gravity * delta, -max_fall_speed)

func start_roll(input_vector: Vector2 = Vector2.ZERO) -> void:
    if rolling or not is_on_floor():
        return

    var direction := _camera_relative_direction(input_vector)
    if direction.length_squared() <= 0.01:
        direction = -global_transform.basis.z
        direction.y = 0.0
        direction = direction.normalized()

    rolling = true
    sprinting = false
    listening = false
    velocity.x = direction.x * roll_speed
    velocity.z = direction.z * roll_speed
    _roll_time_left = roll_duration

func start_sprint() -> void:
    if not crouching and not listening and not rolling:
        sprinting = true

func stop_sprint() -> void:
    sprinting = false

func toggle_crouch() -> void:
    if rolling:
        return
    crouching = not crouching
    if crouching:
        sprinting = false

func toggle_listen() -> void:
    if rolling:
        return
    listening = not listening
    if listening:
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
