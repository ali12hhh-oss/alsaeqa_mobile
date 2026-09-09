extends Camera3D

@export var target_path: NodePath = NodePath("../Hero")
@export var follow_height := 3.8
@export var follow_distance := 7.5
@export var smoothing := 7.0

var target: Node3D

func _ready() -> void:
    target = get_node_or_null(target_path)
    current = true

func _process(delta: float) -> void:
    if not target:
        return
    var desired := target.global_position + Vector3(0, follow_height, follow_distance)
    global_position = global_position.lerp(desired, 1.0 - exp(-smoothing * delta))
    look_at(target.global_position + Vector3(0,1.2,0), Vector3.UP)
