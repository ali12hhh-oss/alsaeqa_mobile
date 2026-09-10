extends Camera3D

## Third-person gameplay camera foundation.
## It follows the canonical Hero body, stays behind the hero's facing direction,
## and changes distance/FOV gently for crouch, sprint and normal traversal.

@export var target_path: NodePath = NodePath("../Hero")
@export var follow_height := 3.6
@export var follow_distance := 7.5
@export var crouch_height := 2.9
@export var crouch_distance := 6.7
@export var smoothing := 8.0
@export var rotation_smoothing := 10.0
@export var look_height := 1.25
@export var look_ahead := 0.7
@export var normal_fov := 70.0
@export var sprint_fov := 74.0
@export var crouch_fov := 66.0

var target: Node3D
var _look_point := Vector3.ZERO

func _ready() -> void:
    target = get_node_or_null(target_path)
    current = true
    fov = normal_fov
    if target:
        _look_point = target.global_position + Vector3.UP * look_height

func _process(delta: float) -> void:
    if not is_instance_valid(target):
        target = get_node_or_null(target_path)
        if not target:
            return

    var hero := target as CharacterBody3D
    var is_crouched: bool = hero != null and (hero.crouching or hero.listening)
    var is_sprinting: bool = hero != null and hero.sprinting and not is_crouched

    var distance := crouch_distance if is_crouched else follow_distance
    var height := crouch_height if is_crouched else follow_height

    var forward := -target.global_transform.basis.z
    forward.y = 0.0
    if forward.length_squared() < 0.01:
        forward = Vector3.FORWARD
    forward = forward.normalized()

    var desired := target.global_position - forward * distance + Vector3.UP * height
    var position_weight := 1.0 - exp(-smoothing * delta)
    global_position = global_position.lerp(desired, position_weight)

    var speed := Vector2(target.velocity.x, target.velocity.z).length() if hero else 0.0
    var ahead := clampf(speed / 7.0, 0.0, 1.0) * look_ahead
    var desired_look := target.global_position + Vector3.UP * (1.15 if is_crouched else look_height) + forward * ahead
    _look_point = _look_point.lerp(desired_look, 1.0 - exp(-rotation_smoothing * delta))
    look_at(_look_point, Vector3.UP)

    var target_fov := crouch_fov if is_crouched else (sprint_fov if is_sprinting else normal_fov)
    fov = lerpf(fov, target_fov, 1.0 - exp(-6.0 * delta))
