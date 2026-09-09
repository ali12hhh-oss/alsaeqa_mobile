extends CharacterBody3D

## Canonical runtime hero controller.
## The real imported character remains the visual child; this body owns
## movement state, physics and mobile/desktop gameplay input.
## Animation is driven from the real AnimationPlayer exposed by the imported
## hero asset; no duplicate animation asset is generated here.

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
@export var animation_blend := 0.12

var sprinting := false
var crouching := false
var listening := false
var rolling := false
var thunder_charging := false
var thunder_charge := 0.0
var _roll_time_left := 0.0
var _landing_time_left := 0.0
var _action_time_left := 0.0
var _was_on_floor := true
var _animation_warning_emitted := false
var _current_animation := ""
var _combat_cooldown := 0.0
var _combat_window := 0.0
var _combo_step := 0
var _combo_reset_time := 0.0
var _attack_active := false
var _attack_heavy := false

var hero_visual: Node3D
var hero_skeleton: Skeleton3D
var hero_animation_player: AnimationPlayer
var hero_visual_ready := false
var _animation_names: Array[String] = []

func _ready() -> void:
    bind_real_hero_visual()
    _was_on_floor = is_on_floor()

## Called by the asset bridge after the real hero scene is instantiated.
## Main.tscn keeps one canonical Hero gameplay body; the imported visual is
## attached to that body at runtime, so binding must happen after attachment.
func bind_real_hero_visual() -> void:
    hero_visual = null
    hero_skeleton = null
    hero_animation_player = null
    hero_visual_ready = false
    _animation_names.clear()

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
    _cache_animation_names()

    if not hero_visual_ready:
        push_error("Hero visual is attached but contains no renderable real mesh")
    if hero_animation_player == null and not _animation_warning_emitted:
        push_warning("Real hero visual has no AnimationPlayer; movement remains playable but animation is unavailable")
        _animation_warning_emitted = true

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

func _cache_animation_names() -> void:
    _animation_names.clear()
    if hero_animation_player == null:
        return
    for animation_name in hero_animation_player.get_animation_list():
        if animation_name != "RESET":
            _animation_names.append(str(animation_name))
    _animation_names.sort()

func _physics_process(delta: float) -> void:
    if not hero_visual_ready:
        bind_real_hero_visual()

    var was_on_floor := _was_on_floor
    _update_action_timers(delta)
    _update_movement_state(delta)
    _update_vertical_motion(delta)
    move_and_slide()
    _update_landing_state(was_on_floor)
    _update_animation_state()
    _was_on_floor = is_on_floor()

func _update_action_timers(delta: float) -> void:
    _roll_time_left = maxf(_roll_time_left - delta, 0.0)
    _landing_time_left = maxf(_landing_time_left - delta, 0.0)
    _action_time_left = maxf(_action_time_left - delta, 0.0)
    _combat_cooldown = maxf(_combat_cooldown - delta, 0.0)
    _combat_window = maxf(_combat_window - delta, 0.0)
    _combo_reset_time = maxf(_combo_reset_time - delta, 0.0)
    if _combo_reset_time <= 0.0:
        _combo_step = 0
    if rolling and _roll_time_left <= 0.0:
        rolling = false

func _update_movement_state(delta: float) -> void:
    var input_vector := Input.get_vector("move_left", "move_right", "move_forward", "move_back")
    var input_strength := minf(input_vector.length(), 1.0)

    if Input.is_action_just_pressed("crouch") and not rolling:
        toggle_crouch()

    if Input.is_action_just_pressed("listen") and not rolling:
        toggle_listen()

    if not Input.is_action_pressed("sprint") and sprinting:
        sprinting = false

    if rolling:
        return

    if _action_time_left > 0.0:
        velocity.x = move_toward(velocity.x, 0.0, braking * delta)
        velocity.z = move_toward(velocity.z, 0.0, braking * delta)
        return

    if Input.is_action_just_pressed("roll") and is_on_floor() and input_strength > 0.05:
        start_roll(input_vector)
        return

    var direction := _camera_relative_direction(input_vector)
    var speed := walk_speed
    if listening or crouching:
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

func _update_landing_state(was_on_floor: bool) -> void:
    if not was_on_floor and is_on_floor():
        _landing_time_left = 0.28

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
    crouching = false
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
        listening = false

func toggle_listen() -> void:
    if rolling:
        return
    listening = not listening
    if listening:
        sprinting = false
        crouching = false

func light_attack() -> void:
    if rolling or _combat_cooldown > 0.0 or thunder_charging:
        return
    _begin_attack(false)

func heavy_attack() -> void:
    if rolling or _combat_cooldown > 0.0 or thunder_charging:
        return
    _begin_attack(true)

func _begin_attack(heavy: bool) -> void:
    if _combo_reset_time <= 0.0:
        _combo_step = 0
    _combo_step = (_combo_step + 1) if not heavy else 1
    var tokens: Array[String]
    if heavy:
        tokens = ["heavy_attack", "heavy", "power_attack", "attack_2", "attack2"]
    elif _combo_step >= 3:
        tokens = ["light_attack_3", "attack_3", "attack3", "combo_3", "slash_3", "light_attack"]
    elif _combo_step == 2:
        tokens = ["light_attack_2", "attack_2", "attack2", "combo_2", "slash_2", "light_attack"]
    else:
        tokens = ["light_attack_1", "attack_1", "attack1", "combo_1", "slash", "light_attack"]
    _attack_heavy = heavy
    _attack_active = true
    _combat_window = 0.18 if heavy else 0.12
    _combat_cooldown = 0.62 if heavy else 0.38
    _combo_reset_time = 0.9
    _play_action_animation(tokens, 0.52 if heavy else 0.34)
    CinematicDirector.combat_impact(heavy)

func is_attack_active() -> bool:
    return _attack_active and _combat_window > 0.0

func attack_is_heavy() -> bool:
    return _attack_heavy

func end_attack_window() -> void:
    _attack_active = false

func start_thunder_charge() -> void:
    if rolling:
        return
    thunder_charging = true
    thunder_charge = 0.0
    _play_action_animation(["thunder_charge", "charge", "power_charge"], 0.2)

func release_thunder() -> void:
    if not thunder_charging:
        return
    thunder_charging = false
    var ratio: float = clampf(thunder_charge, 0.0, 1.0)
    GameState.thunder_charge = ratio
    _play_action_animation(["thunder_release", "thunder_attack", "thunder", "release"], 0.7)
    CinematicDirector.thunder_impact(ratio)
    thunder_charge = 0.0

func cancel_thunder() -> void:
    thunder_charging = false
    thunder_charge = 0.0

func _process(delta: float) -> void:
    if thunder_charging:
        thunder_charge = min(thunder_charge + delta * 0.55, 1.0)

func _update_animation_state() -> void:
    if hero_animation_player == null or _animation_names.is_empty():
        return
    if _action_time_left > 0.0:
        return
    if thunder_charging:
        _play_best_animation(["thunder_charge", "charge", "power_charge"], true)
        return
    if rolling:
        _play_best_animation(["roll", "dodge", "evade"], true)
        return
    if _landing_time_left > 0.0:
        _play_best_animation(["landing", "land"], false)
        return
    if not is_on_floor():
        if velocity.y < -1.5:
            _play_best_animation(["fall", "airborne", "falling"], true)
        else:
            _play_best_animation(["jump", "airborne", "falling"], true)
        return
    var horizontal_speed := Vector2(velocity.x, velocity.z).length()
    if horizontal_speed < 0.15:
        if listening:
            _play_best_animation(["listen", "listening", "stealth"], true)
        elif crouching:
            _play_best_animation(["crouch_idle", "crouch", "idle_crouch"], true)
        else:
            _play_best_animation(["idle", "breathing", "stand"], true)
    elif listening or crouching:
        _play_best_animation(["crouch_walk", "crouch", "stealth_walk", "walk"], true)
    elif sprinting:
        _play_best_animation(["sprint", "run", "jog"], true)
    elif horizontal_speed < (walk_speed + jog_speed) * 0.5:
        _play_best_animation(["walk", "locomotion"], true)
    else:
        _play_best_animation(["jog", "run", "locomotion"], true)

func _play_action_animation(tokens: Array[String], lock_time: float) -> void:
    var animation_name := _find_animation_name(tokens)
    if animation_name.is_empty():
        return
    _action_time_left = lock_time
    _play_animation(animation_name, false)

func _play_best_animation(tokens: Array[String], looping: bool) -> void:
    var animation_name := _find_animation_name(tokens)
    if animation_name.is_empty():
        return
    _play_animation(animation_name, looping)

func _find_animation_name(tokens: Array[String]) -> String:
    if _animation_names.is_empty():
        return ""
    var normalized_tokens: Array[String] = []
    for token in tokens:
        normalized_tokens.append(str(token).to_lower().replace(" ", "_").replace("-", "_"))

    for animation_name in _animation_names:
        var normalized := animation_name.to_lower().replace(" ", "_").replace("-", "_")
        for token in normalized_tokens:
            if normalized == token or normalized.ends_with("/" + token) or normalized.ends_with("|" + token):
                return animation_name
    for animation_name in _animation_names:
        var normalized := animation_name.to_lower().replace(" ", "_").replace("-", "_")
        for token in normalized_tokens:
            if normalized.contains(token):
                return animation_name
    return ""

func _play_animation(animation_name: String, looping: bool) -> void:
    if hero_animation_player == null or not hero_animation_player.has_animation(animation_name):
        return
    if _current_animation == animation_name and hero_animation_player.is_playing():
        return
    hero_animation_player.play(animation_name, animation_blend)
    var animation := hero_animation_player.get_animation(animation_name)
    if animation != null:
        animation.loop_mode = Animation.LOOP_LINEAR if looping else Animation.LOOP_NONE
    _current_animation = animation_name
