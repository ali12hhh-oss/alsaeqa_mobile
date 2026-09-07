extends Node3D

## Runtime bridge for the real ALSAEQA asset library.
## The source packs stay editable; CI converts source models to GLB and this
## scene binds logical roles without baking asset data into gameplay code.

const ROOT := "res://assets/converted"
const ROLE_PATTERNS := {
    "hero": ["hero", "character", "basecharacter"],
    "worker": ["worker", "civilian", "villager", "farmer"],
    "guard": ["guard", "soldier", "warrior", "knight", "medieval"],
    "beast": ["beast", "mount", "horse", "creature", "monster", "dragon", "snake"],
    "environment": ["ruin", "dungeon", "prison", "cave", "village", "farm", "wall", "prop", "nature"]
}

@export var worker_count := 5
@export var guard_count := 9

func _ready() -> void:
    var assets := _find_glb_files(ROOT)
    if assets.is_empty():
        push_error("ALSAEQA real asset library is missing from the mobile build")
        return

    _spawn_role_variants("hero", assets, 1, Vector3(0, 0, 0), 2.0)
    _spawn_role_variants("worker", assets, worker_count, Vector3(-10, 0, 4), 1.9)
    _spawn_role_variants("guard", assets, guard_count, Vector3(0, 0, 0), 2.0)
    _spawn_role_variants("environment", assets, 8, Vector3.ZERO, 8.0)

func _find_glb_files(path: String) -> Array[String]:
    var result: Array[String] = []
    var dir := DirAccess.open(path)
    if dir == null:
        return result
    dir.list_dir_begin()
    while true:
        var name := dir.get_next()
        if name.is_empty():
            break
        if name.begins_with("."):
            continue
        var full := path.path_join(name)
        if dir.current_is_dir():
            result.append_array(_find_glb_files(full))
        elif name.to_lower().ends_with(".glb"):
            result.append(full)
    dir.list_dir_end()
    return result

func _role_files(role: String, assets: Array[String]) -> Array[String]:
    var patterns: Array = ROLE_PATTERNS.get(role, [])
    var matches: Array[String] = []
    for asset in assets:
        var lower := asset.to_lower()
        for pattern in patterns:
            if lower.contains(pattern):
                matches.append(asset)
                break
    return matches

func _spawn_role_variants(role: String, assets: Array[String], count: int, origin: Vector3, target_height: float) -> void:
    var candidates := _role_files(role, assets)
    if candidates.is_empty():
        push_warning("No converted real assets matched role: %s" % role)
        return
    for i in count:
        var path: String = candidates[i % candidates.size()]
        var packed := load(path) as PackedScene
        if packed == null:
            push_warning("Unable to load real asset: %s" % path)
            continue
        var instance := packed.instantiate()
        instance.name = "%s_Real_%02d" % [role, i + 1]
        add_child(instance)
        instance.position = _role_position(role, i, count, origin)
        _normalize_height(instance, target_height)

func _role_position(role: String, index: int, count: int, origin: Vector3) -> Vector3:
    if role == "hero":
        return origin
    if role == "worker":
        return origin + Vector3(float(index % 5) * 4.0, 0, float(index / 5) * 3.0)
    if role == "guard":
        var angle := TAU * float(index) / float(max(count, 1))
        return origin + Vector3(cos(angle) * 11.0, 0, sin(angle) * 11.0)
    var row := index / 4
    var col := index % 4
    return origin + Vector3(float(col - 1) * 12.0, 0, float(row - 1) * 10.0)

func _normalize_height(node: Node, target_height: float) -> void:
    var bounds := _node_bounds(node)
    if bounds.size.y <= 0.001:
        return
    var factor := target_height / bounds.size.y
    if factor <= 0.0:
        return
    if node is Node3D:
        (node as Node3D).scale *= factor

func _node_bounds(node: Node) -> AABB:
    var found := false
    var bounds := AABB()
    for child in node.get_children():
        if child is VisualInstance3D:
            var a := (child as VisualInstance3D).get_aabb()
            if not found:
                bounds = a
                found = true
            else:
                bounds = bounds.merge(a)
        if child.get_child_count() > 0:
            var child_bounds := _node_bounds(child)
            if child_bounds.size != Vector3.ZERO:
                if not found:
                    bounds = child_bounds
                    found = true
                else:
                    bounds = bounds.merge(child_bounds)
    return bounds
