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

const CANONICAL_HERO_HINTS := [
    "superhero_male_fullbody.glb",
    "base_character_male.glb",
    "basecharacter_male.glb",
    "male_fullbody.glb",
    "male_character.glb",
    "male_base_character.glb"
]

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

## Select the single canonical story hero from the real character library.
## The hero is never selected per stage and never selected by alphabetical
## order. We first resolve the explicit male identity contract; only if a pack
## uses an alternate filename do we use a deterministic scored fallback.
func _hero_files(assets: Array[String]) -> Array[String]:
    var by_name: Dictionary = {}
    for asset in assets:
        by_name[asset.get_file().to_lower()] = asset

    for hint in CANONICAL_HERO_HINTS:
        var exact: String = by_name.get(hint, "")
        if not exact.is_empty() and _is_valid_hero_asset(exact.to_lower()):
            return [exact]

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

func _is_valid_hero_asset(lower: String) -> bool:
    if lower.contains("female") or lower.contains("woman") or lower.contains("girl"):
        return false
    if lower.contains("supervillain") or lower.contains("villain"):
        return false
    if lower.contains("monster") or lower.contains("creature") or lower.contains("beast"):
        return false
    if lower.contains("robot") or lower.contains("zombie") or lower.contains("skeleton"):
        return false
    return true

func _hero_score(lower: String) -> int:
    var score := 0
    var base_name := lower.get_file()

    if not (lower.contains("character") or lower.contains("basecharacter") or lower.contains("hero") or lower.contains("worker") or lower.contains("civilian") or lower.contains("villager") or lower.contains("farmer")):
        return -1001
    if lower.contains("weapon") or lower.contains("prop") or lower.contains("environment") or lower.contains("building"):
        return -1001

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

    if base_name.contains("base"):
        score += 8
    if base_name.contains("fullbody"):
        score += 3
    return score

## Prefer a real imported hero that contains a usable animation player. This
## keeps the canonical male identity while avoiding a static T-pose asset when
## the converted library also contains an animated variant.
func _select_runtime_hero(candidates: Array[String]) -> Dictionary:
    var fallback := {}
    for path in candidates:
        var packed := load(path) as PackedScene
        if packed == null:
            continue
        var probe := packed.instantiate()
        var has_mesh := _find_first_mesh(probe) != null
        var has_skeleton := _find_first_skeleton(probe) != null
        var has_animation := _find_first_animation_player(probe) != null
        probe.free()
        if not has_mesh or not has_skeleton:
            continue
        if fallback.is_empty():
            fallback = {"path": path, "packed": packed, "animated": has_animation}
        if has_animation:
            return {"path": path, "packed": packed, "animated": true}
    return fallback

func _spawn_role_variants(role: String, assets: Array[String], count: int, origin: Vector3, target_height: float) -> void:
    var candidates := _hero_files(assets) if role == "hero" else _role_files(role, assets)
    if candidates.is_empty():
        push_warning("No converted real assets matched role: %s" % role)
        _spawned_roles[role] = 0
        return

    if role == "hero":
        var selected := _select_runtime_hero(candidates)
        if selected.is_empty():
            push_error("No renderable canonical hero asset could be loaded")
            _spawned_roles[role] = 0
            return
        _spawned_roles[role] = 1
        var hero_instance := (selected["packed"] as PackedScene).instantiate()
        hero_instance.name = "hero_Real_01"
        var hero_parent: Node = get_parent().get_node_or_null("Hero")
        if hero_parent == null:
            hero_parent = self
        hero_parent.add_child(hero_instance)
        hero_instance.position = Vector3.ZERO
        _normalize_height(hero_instance, target_height)
        print("ALSAEQA canonical hero selected: %s | animated=%s" % [selected["path"], selected["animated"]])
        _validate_hero_runtime(hero_instance, selected["path"])
        if hero_parent.has_method("bind_real_hero_visual"):
            hero_parent.call_deferred("bind_real_hero_visual")
        if not selected["animated"]:
            push_warning("Canonical hero is renderable but has no AnimationPlayer: %s" % selected["path"])
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
        add_child(instance)
        instance.position = _role_position(role, i, count, origin)
        _normalize_height(instance, target_height)

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