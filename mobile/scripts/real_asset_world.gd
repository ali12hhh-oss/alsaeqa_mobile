extends Node3D

## Runtime bridge for the real ALSAEQA asset library.
## Source packs remain editable; CI converts source models to GLB and this
## bridge binds logical roles without baking asset data into gameplay code.

const ROOT := "res://assets/converted"
const ROLE_PATTERNS := {
    "hero": ["hero", "basecharacter", "character"],
    "worker": ["worker", "civilian", "villager", "farmer"],
    "guard": ["guard", "soldier", "warrior", "knight", "medieval"],
    "beast": ["beast", "mount", "horse", "creature", "monster", "dragon", "snake"],
    "environment": ["ruin", "dungeon", "prison", "cave", "village", "farm", "wall", "prop", "nature"]
}

@export var worker_count := 5
@export var guard_count := 9
@export var environment_count := 8

var _spawned_roles: Dictionary = {}

func _ready() -> void:
    var assets := _find_glb_files(ROOT)
    if assets.is_empty():
        push_error("ALSAEQA real asset library is missing from the mobile build")
        return
    _spawn_role_variants("hero", assets, 1, Vector3.ZERO, 2.0)
    _spawn_role_variants("worker", assets, worker_count, Vector3(-10, 0, 4), 1.9)
    _spawn_role_variants("guard", assets, guard_count, Vector3.ZERO, 2.0)
    _spawn_role_variants("environment", assets, environment_count, Vector3.ZERO, 8.0)
    _report_role_coverage(assets)

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
    var matches: Array[String] = []
    for asset in assets:
        if _matches_role(role, asset):
            matches.append(asset)
    matches.sort()
    return matches

func _matches_role(role: String, asset: String) -> bool:
    var lower := asset.to_lower()
    var patterns: Array = ROLE_PATTERNS.get(role, [])
    for pattern in patterns:
        if lower.contains(pattern):
            return true
    return false

func _hero_files(assets: Array[String]) -> Array[String]:
    var exact: Array[String] = []
    var broad: Array[String] = []
    for asset in assets:
        var lower := asset.to_lower()
        if lower.contains("hero"):
            exact.append(asset)
        elif lower.contains("basecharacter"):
            broad.append(asset)
        elif lower.contains("character"):
            broad.append(asset)
    exact.sort()
    broad.sort()
    exact.append_array(broad)
    return exact

func _spawn_role_variants(role: String, assets: Array[String], count: int, origin: Vector3, target_height: float) -> void:
    var candidates := _hero_files(assets) if role == "hero" else _role_files(role, assets)
    if candidates.is_empty():
        push_warning("No converted real assets matched role: %s" % role)
        _spawned_roles[role] = 0
        return
    _spawned_roles[role] = min(count, candidates.size()) if role == "hero" else count
    for i in count:
        var path: String = candidates[i % candidates.size()]
        var packed := load(path) as PackedScene
        if packed == null:
            push_warning("Unable to load real asset: %s" % path)
            continue
        var instance := packed.instantiate()
        instance.name = "%s_Real_%02d" % [role, i + 1]
        var parent: Node = self
        if role == "hero":
            parent = get_parent().get_node_or_null("Hero")
            if parent == null:
                parent = self
        parent.add_child(instance)
        instance.position = Vector3.ZERO if role == "hero" else _role_position(role, i, count, origin)
        _normalize_height(instance, target_height)
        if role == "hero":
            _validate_hero_runtime(instance, path)
            if parent.has_method("bind_real_hero_visual"):
                parent.call_deferred("bind_real_hero_visual")

func _report_role_coverage(assets: Array[String]) -> void:
    for role in ROLE_PATTERNS.keys():
        var matches := _hero_files(assets) if role == "hero" else _role_files(role, assets)
        if matches.is_empty():
            push_warning("Real asset coverage missing for logical role: %s" % role)
        else:
            print("ALSAEQA real asset coverage | %s: %d source-derived GLB candidates" % [role, matches.size()])

func _validate_hero_runtime(instance: Node, source_path: String) -> void:
    if _find_first_mesh(instance) == null:
        push_error("Selected hero asset has no renderable mesh: %s" % source_path)
        return
    if _find_first_skeleton(instance) == null:
        push_warning("Selected hero asset has no Skeleton3D yet: %s" % source_path)
    if _find_first_animation_player(instance) == null:
        push_warning("Selected hero asset has no AnimationPlayer yet: %s" % source_path)

func _find_first_mesh(node: Node) -> MeshInstance3D:
    for child in node.get_children():
        if child is MeshInstance3D and (child as MeshInstance3D).mesh != null:
            return child as MeshInstance3D
        var nested := _find_first_mesh(child)
        if nested != null:
            return nested
    return null

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

func _role_position(role: String, index: int, count: int, origin: Vector3) -> Vector3:
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
    if factor > 0.0 and node is Node3D:
        (node as Node3D).scale *= factor

func _node_bounds(node: Node) -> AABB:
    var found := false
    var bounds := AABB()
    for child in node.get_children():
        if child is VisualInstance3D:
            var a := (child as VisualInstance3D).get_aabb()
            bounds = a if not found else bounds.merge(a)
            found = true
        if child.get_child_count() > 0:
            var child_bounds := _node_bounds(child)
            if child_bounds.size != Vector3.ZERO:
                bounds = child_bounds if not found else bounds.merge(child_bounds)
                found = true
    return bounds
