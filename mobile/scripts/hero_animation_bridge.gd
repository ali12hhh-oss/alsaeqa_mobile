extends Node

## Retargets the real Quaternius Universal Animation Library onto the real
## hero Skeleton3D. This keeps animation authored-data driven even when the
## selected hero mesh GLB is a static character variant.

@export var animation_blend := 0.12

var hero: CharacterBody3D
var hero_skeleton: Skeleton3D
var driver_skeleton: Skeleton3D
var driver_player: AnimationPlayer
var _bone_pairs: Array[Vector2i] = []
var _animations: Array[String] = []
var _current := ""
var _action_left := 0.0
var _last_attack_active := false
var _last_thunder_charging := false
var _ready_for_runtime := false

func _ready() -> void:
    hero = get_parent() as CharacterBody3D
    call_deferred("_setup")

func _setup() -> void:
    if hero == null:
        return
    var visual := hero.get_node_or_null("Hero_Real_01")
    var driver := hero.get_node_or_null("HeroAnimationDriver")
    if visual == null or driver == null:
        push_warning("Hero animation bridge is waiting for real visual and animation driver")
        return

    hero_skeleton = _find_first_skeleton(visual)
    driver_skeleton = _find_first_skeleton(driver)
    driver_player = _find_first_animation_player(driver)
    if hero_skeleton == null or driver_skeleton == null or driver_player == null:
        push_error("Hero animation bridge could not find both real skeletons and the authored AnimationPlayer")
        return

    _build_bone_map()
    for name in driver_player.get_animation_list():
        if str(name) != "RESET":
            _animations.append(str(name))
    _animations.sort()
    if _animations.is_empty():
        push_error("Universal Animation Library driver contains no usable animations")
        return

    driver_player.playback_default_blend_time = animation_blend
    _ready_for_runtime = _bone_pairs.size() >= 4
    print("ALSAEQA hero animation bridge | animations=%d matched_bones=%d" % [_animations.size(), _bone_pairs.size()])
    _play_best(["idle", "breathing", "stand"], true)

func _process(delta: float) -> void:
    if not _ready_for_runtime:
        return
    _action_left = maxf(_action_left - delta, 0.0)
    _update_animation_choice()
    _copy_driver_pose_to_hero()

func _update_animation_choice() -> void:
    var attack_active := hero.is_attack_active()
    var heavy := hero.attack_is_heavy()
    if attack_active and not _last_attack_active:
        _action_left = 0.55 if heavy else 0.36
        _play_best(["heavy_attack", "heavy", "power_attack", "attack_2", "attack2"] if heavy else ["light_attack", "attack_1", "attack1", "slash", "attack"], false)
    _last_attack_active = attack_active

    var charging: bool = hero.thunder_charging
    if charging:
        _play_best(["thunder_charge", "charge", "power_charge"], true)
    elif _last_thunder_charging:
        _action_left = 0.72
        _play_best(["thunder_release", "thunder_attack", "thunder", "release", "attack"], false)
    _last_thunder_charging = charging

    if _action_left > 0.0 or attack_active or charging:
        return
    if hero.rolling:
        _play_best(["roll", "dodge", "evade"], true)
        return
    if not hero.is_on_floor():
        if hero.velocity.y < -1.5:
            _play_best(["fall", "falling", "airborne"], true)
        else:
            _play_best(["jump", "airborne"], true)
        return

    var speed := Vector2(hero.velocity.x, hero.velocity.z).length()
    if speed < 0.15:
        if hero.listening:
            _play_best(["listen", "listening", "stealth"], true)
        elif hero.crouching:
            _play_best(["crouch_idle", "idle_crouch", "crouch"], true)
        else:
            _play_best(["idle", "breathing", "stand"], true)
    elif hero.crouching or hero.listening:
        _play_best(["crouch_walk", "stealth_walk", "walk"], true)
    elif hero.sprinting:
        _play_best(["sprint", "run", "jog"], true)
    elif speed < 4.5:
        _play_best(["walk", "jog", "locomotion"], true)
    else:
        _play_best(["jog", "run", "locomotion"], true)

func _play_best(tokens: Array[String], looping: bool) -> void:
    var name := _find_animation(tokens)
    if name.is_empty() or driver_player == null:
        return
    if _current == name and driver_player.is_playing():
        return
    _current = name
    driver_player.play(name, animation_blend, 1.0)
    var animation := driver_player.get_animation(name)
    if animation != null:
        animation.loop_mode = Animation.LOOP_LINEAR if looping else Animation.LOOP_NONE

func _find_animation(tokens: Array[String]) -> String:
    if _animations.is_empty():
        return ""
    for token in tokens:
        var normalized_token := token.to_lower().replace(" ", "_").replace("-", "_")
        for name in _animations:
            var normalized := name.to_lower().replace(" ", "_").replace("-", "_")
            if normalized == normalized_token or normalized.contains(normalized_token):
                return name
    return ""

func _build_bone_map() -> void:
    _bone_pairs.clear()
    for hero_index in range(hero_skeleton.get_bone_count()):
        var name := hero_skeleton.get_bone_name(hero_index)
        var driver_index := driver_skeleton.find_bone(name)
        if driver_index >= 0:
            _bone_pairs.append(Vector2i(hero_index, driver_index))

func _copy_driver_pose_to_hero() -> void:
    for pair in _bone_pairs:
        var hero_index := pair.x
        var driver_index := pair.y
        hero_skeleton.set_bone_pose_position(hero_index, driver_skeleton.get_bone_pose_position(driver_index))
        hero_skeleton.set_bone_pose_rotation(hero_index, driver_skeleton.get_bone_pose_rotation(driver_index))
        hero_skeleton.set_bone_pose_scale(hero_index, driver_skeleton.get_bone_pose_scale(driver_index))

func _find_first_skeleton(node: Node) -> Skeleton3D:
    if node is Skeleton3D:
        return node as Skeleton3D
    for child in node.get_children():
        var nested := _find_first_skeleton(child)
        if nested != null:
            return nested
    return null

func _find_first_animation_player(node: Node) -> AnimationPlayer:
    if node is AnimationPlayer:
        return node as AnimationPlayer
    for child in node.get_children():
        var nested := _find_first_animation_player(child)
        if nested != null:
            return nested
    return null
