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
    _spawn_role_variants("beast", assets, 4, Vector3(18, 0, 8), 2.4)
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

## Select the canonical story hero from the real character library.
## The previous alphabetical selection could pick a female superhero or a
## visually unrelated character. Scoring is deliberately conservative: a
## suitable male/worker/civilian character wins, while explicit female,
## superhero, monster and non-character assets are strongly rejected.
func _hero_files(assets: Array[String]) -> Array[String]:
    var scored: Array = []
    for asset in assets:
        var lower := asset.to_lower()
        var score := _hero_score(lower)
        if score <= -1000:
            continue
        scored.append({"path": asset, "score": score})

    scored.sort_custom(func(a: Dictionary, b: Dictionary) -> bool:
        if a["score"] == b["score"]:
            return a["path"] < b["path"]
        return a["score"] > b["score"]
    )

    var result: Array[String] = []
    for item in scored:
        result.append(item["path"])
    return result

func _hero_score(lower: String) -> int:
    var score := 0
    var base_name := lower.get_file()

    # Must look like a character asset, not an environment/prop/weapon file.
    if not (lower.contains("character") or lower.contains("basecharacter") or lower.contains("hero") or lower.contains("worker") or lower.contains("civilian") or lower.contains("villager") or lower.contains("farmer")):
        return -1001
    if lower.contains("weapon") or lower.contains("prop") or lower.contains("environment") or lower.contains("building"):
        return -1001

    # Strong positive signals for the Stage 1 young male worker/civilian role.
    if lower.contains("worker"):
        score += 100
    if lower.contains("civilian"):
        score += 85
    if lower.contains("villager"):
        score += 80
    if lower.contains("farmer"):
        score += 75
    if lower.contains("male") or lower.contains("man") or lower.contains("boy"):
        score += 55
    if lower.contains("young"):
        score += 20
    if lower.contains("character") or lower.contains("basecharacter"):
        score += 20
    if lower.contains("hero"):
        score += 10

    # Explicitly avoid the unrelated female superhero that previously became
    # the runtime hero, plus other obviously wrong role families.
    if lower.contains("female") or lower.contains("woman") or lower.contains("girl"):
        score -= 700
    if lower.contains("superhero") or lower.contains("super_hero"):
        score -= 600
    if lower.contains("supervillain") or lower.contains("villain"):
        score -= 500
    if lower.contains("monster") or lower.contains("creature") or lower.contains("beast"):
        score -= 900
    if lower.contains("robot") or lower.contains("zombie") or lower.contains("skeleton"):
        score -= 900

    # Keep a small preference for ordinary base-character assets over special
    # costumes/variants when several real male characters score similarly.
    if base_name.contains("base"):
        score += 8
    if base_name.contains("fullbody"):
        score += 3
    return score

func _spawn_role_variants(role: String, assets: Array[String], count: int, origin: Vector3, target_height: float) -> void:
    var candidates := _hero_files(assets) if role == "hero" else _role_files(role, assets)
    if candidates.is_empty():
        push_warning("No converted real assets matched role: %s" % role)
        _spawned_roles[role] = 0
        return
    _spawned_roles[role] = min(count, candidates.size())
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
            print("ALSAEQA hero selected: %s" % path)
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
    if role == "beast":
        return origin + Vector3(float(index % 2) * 7.0, 0, float(index / 2) * 6.0)
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
