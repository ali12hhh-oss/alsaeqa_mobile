extends CharacterBody3D
class_name HostileCreature

## Shared base for every hostile that can hurt/be hurt by the hero: mine/
## stage guards (guard_enemy.gd), small ambient cave threats
## (cave_monster.gd) and stage boss creatures (dungeon_monster.gd). Holds
## the health/receive_damage/defeat contract every StageNController already
## expects (register_guard()/notify_guard_defeated() via the "stage_controller"
## group), plus melee-range contact damage against the hero, which no
## enemy in the project could deal before this — the hero could only ever
## deal damage, never take it.
##
## Subclasses set their own max_health/attack_damage/attack_range and
## counts_toward_stage_clear in _ready() *before* calling super._ready(),
## then add no further behaviour of their own beyond that.

@export var max_health: float = 60.0
@export var attack_damage: float = 12.0
@export var attack_range: float = 2.0
@export var attack_cooldown: float = 1.6

## Whether defeating this creature counts toward the active stage's clear
## condition (mine guards, stage bosses) or is a purely ambient/optional
## threat that does not gate progression (small cave monsters). Subclasses
## must set this before calling super._ready().
var counts_toward_stage_clear := true

var health: float
var _defeated := false
var _stage_controller: Node
var _attack_cooldown_left := 0.0

signal defeated

func _ready() -> void:
    add_to_group("hostile_creature")
    health = max_health
    if counts_toward_stage_clear:
        _stage_controller = get_tree().get_first_node_in_group("stage_controller")
        if _stage_controller != null and _stage_controller.has_method("register_guard"):
            _stage_controller.call("register_guard")
        else:
            push_warning("%s could not find an active stage controller in group 'stage_controller' to register itself" % name)

func _physics_process(delta: float) -> void:
    if _defeated:
        return
    if not is_on_floor():
        velocity.y -= 18.0 * delta
    else:
        velocity.y = -0.5
    move_and_slide()

    _attack_cooldown_left = maxf(_attack_cooldown_left - delta, 0.0)
    _try_attack_hero()

func _try_attack_hero() -> void:
    if _attack_cooldown_left > 0.0:
        return
    var hero := get_tree().get_first_node_in_group("alsaeqa_hero")
    if hero == null or not (hero is Node3D):
        return
    var hero_node := hero as Node3D
    if global_position.distance_to(hero_node.global_position) > attack_range:
        return
    if hero.has_method("receive_damage"):
        hero.call("receive_damage", attack_damage, global_position, false)
        _attack_cooldown_left = attack_cooldown

func receive_damage(amount: float, _source_position: Vector3, _heavy: bool) -> void:
    if _defeated:
        return
    health = maxf(health - amount, 0.0)
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
            var lowered := String(animation_name).to_lower()
            if lowered.contains("death") or lowered.contains("die"):
                player.play(animation_name)
                played_death_animation = true
                break
    if not played_death_animation:
        # No matching death animation on this creature's real asset — hide
        # rather than leave it standing frozen, which would read as a bug.
        visible = false

    if counts_toward_stage_clear:
        if _stage_controller != null and _stage_controller.has_method("notify_guard_defeated"):
            _stage_controller.call("notify_guard_defeated")
    else:
        defeated.emit()

func _find_first_animation_player(node: Node) -> AnimationPlayer:
    for child in node.get_children():
        if child is AnimationPlayer:
            return child as AnimationPlayer
        var nested := _find_first_animation_player(child)
        if nested != null:
            return nested
    return null
