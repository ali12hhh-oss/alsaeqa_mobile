extends Node3D

## Runtime bridge for the real ALSAEQA asset library.
## The mobile runtime selects authored assets by semantic role, avoids random
## modular pieces, and attaches the real Universal Animation Library to the
## approved humanoid hero rig when the hero mesh itself is static.

const ROOT := "res://assets/converted"
const ROLE_PATTERNS := {
    "hero": ["hero", "basecharacter", "character"],
    "worker": ["worker", "civilian", "villager", "farmer"],
    "guard": ["guard", "soldier", "warrior", "knight"],
    "beast": ["beast", "mount", "horse", "creature", "monster", "dragon", "snake"],
    "environment": ["ruin", "dungeon", "prison", "cave", "mine", "wall", "rock", "cliff", "floor", "arch", "village", "farm", "nature", "building", "house", "tree"]
}

const ENVIRONMENT_PRIORITY := [
    "mine", "cave", "dungeon", "prison", "rock", "cliff", "wall", "floor", "arch", "ruin",
    "gate", "building", "house", "roof", "road", "path", "tree", "well", "fence", "barrel", "crate"
]

@export var worker_count := 5
@export var guard_count := 7
@export var beast_count := 2
@export var environment_count := 14

var _spawned_roles: Dictionary = {}
var _animation_driver_attached := false

func _ready() -> void:
    var assets := _find_glb_files(ROOT)
    if assets.is_empty():
        push_error("ALSAEQA real asset library is missing from the mobile build")
        return
    _spawn_hero(assets)
    _spawn_workers(assets)
    _spawn_guards(assets)
    _spawn_beasts(assets)
    _spawn_environment(assets)
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

func _character_files(assets: Array[String]) -> Array[String]:
    var result: Array[String] = []
    for asset in assets:
        var lower := asset.to_lower()
        if lower.contains("character") or lower.contains("basecharacter") or lower.contains("hero"):
            if not lower.contains("animation") and not lower.contains("environment"):
                result.append(asset)
    result.sort()
    return result

func _hero_files(assets: Array[String]) -> Array[String]:
    var scored: Array = []
    for asset in assets:
        var lower := asset.to_lower()
        var score := 0
        if lower.contains("hero"):
            score += 120
        if lower.contains("male"):
            score += 100
        if lower.contains("man"):
            score += 55
        if lower.contains("worker") or lower.contains("farmer") or lower.contains("civilian"):
            score += 70
        if lower.contains("basecharacter"):
            score += 30
        if lower.contains("female") or lower.contains("woman"):
            score -= 80
        if lower.contains("superhero"):
            score -= 40
        if score > 0:
            scored.append([score, asset])
    scored.sort_custom(func(a, b): return a[0] > b[0])
    var result: Array[String] = []
    for item in scored:
        result.append(item[1])
    if result.is_empty():
        return _character_files(assets)
    return result

func _worker_files(assets: Array[String]) -> Array[String]:
    var explicit: Array[String] = []
    for asset in assets:
        var lower := asset.to_lower()
        if lower.contains("worker") or lower.contains("civilian") or lower.contains("villager") or lower.contains("farmer"):
            explicit.append(asset)
    explicit.sort()
    if explicit.size() >= worker_count:
        return explicit
    var hero_candidates := _hero_files(assets)
    var fallback: Array[String] = []
    for asset in _character_files(assets):
        if not hero_candidates.has(asset):
            fallback.append(asset)
    explicit.append_array(fallback)
    return explicit

func _guard_files(assets: Array[String]) -> Array[String]:
    var result: Array[String] = []
    for asset in assets:
        var lower := asset.to_lower()
        if lower.contains("guard") or lower.contains("soldier") or lower.contains("warrior") or lower.contains("knight"):
            result.append(asset)
    result.sort()
    if result.is_empty():
        for asset in _character_files(assets):
            if lower_character_score(asset) > 0:
                result.append(asset)
    return result

func lower_character_score(asset: String) -> int:
    var lower := asset.to_lower()
    var score := 0
    if lower.contains("armor") or lower.contains("armour"):
        score += 5
    if lower.contains("male") or lower.contains("man"):
        score += 2
    return score

func _beast_files(assets: Array[String]) -> Array[String]:
    var result: Array[String] = []
    for asset in assets:
        var lower := asset.to_lower()
        if lower.contains("beast") or lower.contains("mount") or lower.contains("horse") or lower.contains("creature") or lower.contains("monster") or lower.contains("dragon") or lower.contains("snake"):
            result.append(asset)
    result.sort()
    return result

func _environment_files(assets: Array[String]) -> Array[String]:
    var scored: Array = []
    for asset in assets:
        var lower := asset.to_lower()
        if lower.contains("character") or lower.contains("basecharacter") or lower.contains("weapon") or lower.contains("monster"):
            continue
        var score := 0
        for i in range(ENVIRONMENT_PRIORITY.size()):
            var token: String = ENVIRONMENT_PRIORITY[i]
            if lower.contains(token):
                score += 100 - i
        if lower.contains("medieval village"):
            score += 8
        if lower.contains("stylized nature"):
            score += 6
        if lower.contains("ultimate modular ruins"):
            score += 16
        if lower.contains("updated modular dungeon"):
            score += 24
        if score > 0:
            scored.append([score, asset])
    scored.sort_custom(func(a, b):
        if a[0] == b[0]:
            return a[1] < b[1]
        return a[0] > b[0]
    )
    var result: Array[String] = []
    for item in scored:
        if not result.has(item[1]):
            result.append(item[1])
    return result

func _spawn_hero(assets: Array[String]) -> void:
    var candidates := _hero_files(assets)
    if candidates.is_empty():
        push_error("No real humanoid hero candidates were found")
        return
    var parent := get_parent().get_node_or_null("Hero")
    if parent == null:
        push_error("Canonical Hero gameplay body is missing")
        return
    var selected := ""
    var selected_instance: Node = null
    for path in candidates:
        var packed := load(path) as PackedScene
        if packed == null:
            continue
        var instance := packed.instantiate()
        if _find_first_mesh(instance) == null or _find_first_skeleton(instance) == null:
            instance.queue_free()
            continue
        parent.add_child(instance)
        instance.name = "Hero_Real_01"
        _normalize_height(instance, 1.78)
        selected = path
        selected_instance = instance
        break
    if selected_instance == null:
        push_error("No usable real humanoid hero mesh/skeleton was found")
        return
    _attach_animation_driver(parent, assets)
    _validate_hero_runtime(selected_instance, selected)
    parent.call_deferred("bind_real_hero_visual")
    _spawned_roles["hero"] = 1
    print("ALSAEQA hero selected: %s" % selected)

func _attach_animation_driver(hero_parent: Node, assets: Array[String]) -> void:
    if _animation_driver_attached:
        return
    var candidates: Array[String] = []
    for asset in assets:
        var lower := asset.to_lower()
        if lower.contains("universal animation library"):
            candidates.append(asset)
    candidates.sort()
    for path in candidates:
        var packed := load(path) as PackedScene
        if packed == null:
            continue
        var instance := packed.instantiate()
        var player := _find_first_animation_player(instance)
        var skeleton := _find_first_skeleton(instance)
        if player == null or skeleton == null:
            instance.queue_free()
            continue
        instance.name = "HeroAnimationDriver"
        hero_parent.add_child(instance)
        if instance is Node3D:
            (instance as Node3D).visible = false
        player.stop()
        _animation_driver_attached = true
        print("ALSAEQA hero animation driver selected: %s" % path)
        return
    push_warning("No real Universal Animation Library scene with Skeleton3D + AnimationPlayer was found")

func _spawn_workers(assets: Array[String]) -> void:
    var candidates := _worker_files(assets)
    if candidates.is_empty():
        push_warning("No explicit worker assets; using real character variants for rescued workers")
        _spawned_roles["worker"] = 0
        return
    var count := min(worker_count, candidates.size())
    for i in range(count):
        _spawn_scene(candidates[i], "Worker_Real_%02d" % (i + 1), _worker_position(i), 1.68)
    _spawned_roles["worker"] = count

func _spawn_guards(assets: Array[String]) -> void:
    var candidates := _guard_files(assets)
    if candidates.is_empty():
        push_warning("No real guard candidates were found")
        _spawned_roles["guard"] = 0
        return
    var count := min(guard_count, candidates.size())
    for i in range(count):
        _spawn_scene(candidates[i], "Guard_Real_%02d" % (i + 1), _guard_position(i, count), 1.8)
    _spawned_roles["guard"] = count

func _spawn_beasts(assets: Array[String]) -> void:
    var candidates := _beast_files(assets)
    if candidates.is_empty():
        _spawned_roles["beast"] = 0
        return
    var count := min(beast_count, candidates.size())
    for i in range(count):
        _spawn_scene(candidates[i], "Beast_Real_%02d" % (i + 1), Vector3(7.0 + i * 5.0, 0, -7.0), 2.6)
    _spawned_roles["beast"] = count

func _spawn_environment(assets: Array[String]) -> void:
    var candidates := _environment_files(assets)
    if candidates.is_empty():
        push_error("No usable real environment candidates were found")
        _spawned_roles["environment"] = 0
        return
    var placements := [
        Vector3(-10, 0, -14), Vector3(10, 0, -14), Vector3(-18, 0, -5), Vector3(18, 0, -5),
        Vector3(-16, 0, 8), Vector3(16, 0, 8), Vector3(-9, 0, 15), Vector3(9, 0, 15),
        Vector3(-24, 0, 15), Vector3(24, 0, 15), Vector3(-25, 0, -12), Vector3(25, 0, -12),
        Vector3(-4, 0, -20), Vector3(4, 0, -20)
    ]
    var count := min(environment_count, min(candidates.size(), placements.size()))
    for i in range(count):
        var path: String = candidates[i]
        var instance := _spawn_scene(path, "Environment_Real_%02d" % (i + 1), placements[i], 5.0)
        if instance == null:
            continue
        if i % 3 == 0 and instance is Node3D:
            (instance as Node3D).rotation.y = deg_to_rad(float((i * 37) % 360))
    _spawned_roles["environment"] = count

func _spawn_scene(path: String, node_name: String, position: Vector3, target_height: float) -> Node:
    var packed := load(path) as PackedScene
    if packed == null:
        push_warning("Unable to load real asset: %s" % path)
        return null
    var instance := packed.instantiate()
    instance.name = node_name
    add_child(instance)
    if instance is Node3D:
        (instance as Node3D).position = position
    _normalize_height(instance, target_height)
    return instance

func _worker_position(index: int) -> Vector3:
    var positions := [Vector3(-6, 0, 1), Vector3(-3, 0, -2), Vector3(3, 0, -2), Vector3(6, 0, 1), Vector3(0, 0, -5)]
    return positions[index % positions.size()]

func _guard_position(index: int, count: int) -> Vector3:
    var angle := TAU * float(index) / float(max(count, 1))
    return Vector3(cos(angle) * 11.0, 0, sin(angle) * 11.0 - 2.0)

func _report_role_coverage(assets: Array[String]) -> void:
    print("ALSAEQA real asset coverage | hero: %d candidates" % _hero_files(assets).size())
    print("ALSAEQA real asset coverage | workers: %d candidates" % _worker_files(assets).size())
    print("ALSAEQA real asset coverage | guards: %d candidates" % _guard_files(assets).size())
    print("ALSAEQA real asset coverage | beasts: %d candidates" % _beast_files(assets).size())
    print("ALSAEQA real asset coverage | environment: %d candidates" % _environment_files(assets).size())

func _validate_hero_runtime(instance: Node, source_path: String) -> void:
    if _find_first_mesh(instance) == null:
        push_error("Selected hero asset has no renderable mesh: %s" % source_path)
    if _find_first_skeleton(instance) == null:
        push_error("Selected hero asset has no Skeleton3D: %s" % source_path)
    if _find_first_animation_player(instance) == null:
        push_warning("Hero mesh has no local AnimationPlayer; the Universal Animation Library driver will provide authored motion when compatible")

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

func _normalize_height(node: Node, target_height: float) -> void:
    if not node is Node3D:
        return
    var bounds := _node_bounds(node)
    if bounds.size.y <= 0.001:
        return
    var factor := clampf(target_height / bounds.size.y, 0.05, 20.0)
    (node as Node3D).scale *= factor

func _node_bounds(node: Node) -> AABB:
    var found := false
    var bounds := AABB()
    for child in node.get_children():
        if child is VisualInstance3D:
            var visual := child as VisualInstance3D
            var local := visual.get_aabb()
            var transform := visual.global_transform
            var transformed := AABB()
            for corner in range(8):
                var world_p := transform * local.get_endpoint(corner)
                transformed = AABB(world_p, Vector3.ZERO) if corner == 0 else transformed.expand(world_p)
            bounds = transformed if not found else bounds.merge(transformed)
            found = true
        var nested := _node_bounds(child)
        if nested.size != Vector3.ZERO:
            bounds = nested if not found else bounds.merge(nested)
            found = true
    return bounds
