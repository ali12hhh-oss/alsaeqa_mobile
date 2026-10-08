extends Node3D

## Runtime bridge for the real ALSAEQA asset library.
## Source packs remain editable; CI converts source models to GLB and this
## bridge binds logical roles without baking asset data into gameplay code.
## Stage-aware: the hero body is attached exactly once and persists across
## stages (its outfit/weapon visuals update through equipped gear, not by
## re-instantiating it); everything else (workers, guards, beasts,
## environment, chests) is cleared and rebuilt whenever GameState reports a
## stage change, since each stage is a different place with different
## content.

const ROOT := "res://assets/converted"
const ROLE_PATTERNS := {
    "hero": ["hero", "basecharacter", "character"],
    "worker": ["worker", "civilian", "villager", "farmer"],
    # NOTE: "medieval" was previously in this list, but the only pack whose
    # path contains that word is "Medieval Village MegaKit" — an environment
    # kit (buildings/props, zero characters). It was matching non-character
    # meshes as "guards". Removed; guard now correctly has no real dedicated
    # pack and uses the explicit fallback below instead of a false match.
    "guard": ["guard", "soldier", "warrior", "knight"],
    "beast": ["beast", "mount", "horse", "creature", "monster", "dragon", "snake"],
    # "dungeon" narrowed to "modular dungeon" (the real "Updated Modular
    # Dungeon" pack): the bare substring also matched the unrelated
    # "Bestiary - Dungeon Monsters Kit" character pack, silently leaking
    # Imp/Puglin monster meshes into "environment" prop scatter.
    "environment": ["ruin", "modular dungeon", "prison", "cave", "village", "farm", "wall", "prop", "nature"],
    # Stage 3 ("Echo Under Stone"): small ambient cave threats. Only the
    # Blob-family creatures (a small/simple silhouette, per explicit project
    # direction to keep the forest animal pack for Stage 6 instead) —
    # verified real filenames from ALSAEQA_EXTRA_MONSTERS.zip.zip's
    # Blob/FBX/ folder, not the whole folder (which also contains mundane
    # animals like Cat/Dog/Chicken that don't read as monsters).
    "cave_monster": ["pinkblob", "greenblob", "greenspikyblob", "mushnub"],
    # Stage 3's one mandatory boss. "/imp." (with the surrounding
    # separators) rather than a bare "imp" substring, so it can never
    # accidentally match an unrelated path containing that letter sequence.
    "dungeon_monster": ["/imp."]
}

const CANONICAL_HERO_HINTS := [
    "superhero_male_fullbody.glb",
    "base_character_male.glb",
    "basecharacter_male.glb",
    "male_fullbody.glb",
    "male_character.glb",
    "male_base_character.glb"
]

const WORKER_CAPTIVE_SCRIPT := preload("res://scripts/worker_captive.gd")
const GUARD_ENEMY_SCRIPT := preload("res://scripts/guard_enemy.gd")
const WEAPON_CHEST_SCRIPT := preload("res://scripts/weapon_chest.gd")
const CAVE_MONSTER_SCRIPT := preload("res://scripts/cave_monster.gd")
const DUNGEON_MONSTER_SCRIPT := preload("res://scripts/dungeon_monster.gd")
const CHECKPOINT_ZONE_SCRIPT := preload("res://scripts/checkpoint_zone.gd")
const GANG_MEMBER_SCRIPT := preload("res://scripts/gang_member.gd")
const GANG_ALARM_ZONE_SCRIPT := preload("res://scripts/gang_alarm_zone.gd")

## Roles that need an actual physics body (CharacterBody3D) wrapped around
## the imported visual, so the hero's melee shape-query and this creature's
## own contact-attack check can find each other as real PhysicsBody3D
## colliders. Keyed by role name so _spawn_role_variants stays one shared
## code path instead of repeating the same wrapper logic per role.
const PHYSICS_BODY_SCRIPTS := {
    "guard": GUARD_ENEMY_SCRIPT,
    "cave_monster": CAVE_MONSTER_SCRIPT,
    "dungeon_monster": DUNGEON_MONSTER_SCRIPT,
    "gang_member": GANG_MEMBER_SCRIPT
}

## Where the hero is placed when a stage begins (stage_advanced fires, or
## the game first loads already on that stage). Without this, the hero
## simply stayed wherever the previous stage left it while the world
## rebuilt around it — harmless when Stage 2's small area happened to
## overlap Stage 1's, but Stage 4 deliberately continues far past Stage
## 3's chamber (x=38), so stage transitions now reposition the hero
## explicitly instead of relying on coincidence.
const STAGE_START_TRANSFORMS := {
    1: {"position": Vector3.ZERO, "rotation_y": 0.0},
    2: {"position": Vector3(0, 0, 4), "rotation_y": 0.0},
    3: {"position": Vector3(0, 0, 0), "rotation_y": 0.0},
    4: {"position": Vector3(40, 0, 0), "rotation_y": -90.0}
}

@export var worker_count := 5
@export var guard_count := 9
@export var environment_count := 36

var _spawned_roles: Dictionary = {}
# KNOWN GAP: the currently downloaded source packs contain no dedicated
# "worker" or "guard" body mesh — only the Superhero_Male/Female_FullBody
# generic bodies exist. Both roles fall back to reusing the canonical hero
# body (with its attached outfit) as a stand-in, and both report this
# loudly rather than silently spawning zero instances or matching the wrong
# asset. Tracked per-role so the coverage report is accurate for each.
var _role_uses_body_fallback: Dictionary = {}
var _hero_bound := false
var _cached_assets: Array[String] = []

func _ready() -> void:
    _cached_assets = _find_glb_files(ROOT)
    if _cached_assets.is_empty():
        push_error("ALSAEQA real asset library is missing from the mobile build")
        return
    _ensure_hero_bound()
    GameState.stage_advanced.connect(_on_stage_advanced)
    rebuild_for_stage(GameState.current_stage)

func _on_stage_advanced(new_stage: int) -> void:
    rebuild_for_stage(new_stage)
    _reposition_hero_for_stage(new_stage)

func _reposition_hero_for_stage(stage: int) -> void:
    var start_data = STAGE_START_TRANSFORMS.get(stage)
    if start_data == null:
        return
    var hero: Node3D = get_parent().get_node_or_null("Hero")
    if hero == null:
        return
    hero.global_position = start_data["position"]
    hero.rotation.y = deg_to_rad(start_data["rotation_y"])
    GameState.set_checkpoint(hero.global_position, hero.rotation.y)

## Spawns/binds the canonical hero exactly once. The hero body ($Hero in
## Main.tscn) persists for the entire game; only its equipped gear visuals
## change between stages, so it must never be re-instantiated on a stage
## rebuild the way role content below is.
func _ensure_hero_bound() -> void:
    if _hero_bound:
        return
    _spawn_role_variants("hero", _cached_assets, 1, Vector3.ZERO, 2.0)
    _hero_bound = true

## Clears every previously spawned role instance (workers, guards, beasts,
## environment, chests — anything parented under this node) and rebuilds
## the content for the given stage. The hero itself is untouched, since it
## lives under $Hero, a sibling of this node, not a child of it.
func rebuild_for_stage(stage: int) -> void:
    for child in get_children():
        child.queue_free()
    _spawned_roles.clear()
    _role_uses_body_fallback.clear()

    match stage:
        1:
            _build_stage_1()
        2:
            _build_stage_2()
        3:
            _build_stage_3()
        4:
            _build_stage_4()
        _:
            push_warning("real_asset_world.gd has no world content defined yet for stage %d — leaving the area empty rather than reusing Stage 1/2 content, since neither is what this stage is meant to look like." % stage)
    _report_role_coverage(_cached_assets)

func _build_stage_1() -> void:
    _spawn_role_variants("worker", _cached_assets, worker_count, Vector3(-10, 0, 4), 1.9)
    _spawn_role_variants("guard", _cached_assets, guard_count, Vector3.ZERO, 2.0)
    _spawn_role_variants("beast", _cached_assets, 4, Vector3(18, 0, 8), 2.4)
    _spawn_role_variants("environment", _cached_assets, environment_count, Vector3.ZERO, 8.0)

## Stage 2 ("The Hidden Mark"): a smaller rocky stretch just outside the
## mine, not the forest (that opens in Stage 6 per the project bible) — a
## handful of environment pieces for rocky/rubble dressing, two guards, and
## the weapon cache chest.
func _build_stage_2() -> void:
    _spawn_role_variants("guard", _cached_assets, 2, Vector3(6, 0, -4), 2.0)
    _spawn_role_variants("environment", _cached_assets, 14, Vector3.ZERO, 8.0)
    _spawn_weapon_chest()

## HONEST GAP: no verified real "chest" or "crate" prop from the downloaded
## packs has been identified yet (would need to inspect the Fantasy Props
## kit's exact file list to pick one correctly, per the project's rule
## against guessing asset paths). Rather than fake a primitive box mesh as
## a stand-in, this is an invisible interact volume for now — the "press to
## open" prompt still works, but there's nothing to see until a real prop
## path is confirmed and wired in here.
func _spawn_weapon_chest() -> void:
    var chest := Node3D.new()
    chest.name = "WeaponChest"
    chest.set_script(WEAPON_CHEST_SCRIPT)
    add_child(chest)
    chest.position = Vector3(3, 0, 7)

const RUINS_FBX := "res://assets/converted/ALSAEQA_EXTRA_ULTIMATE_MODULAR_RUINS.zip/FBX/"

func _build_stage_3() -> void:
    _spawn_stage3_cave_structure()
    _spawn_role_variants("cave_monster", _cached_assets, 4, Vector3(6, 0, 0), 1.3)
    _spawn_cave_checkpoint()
    _spawn_role_variants("dungeon_monster", _cached_assets, 1, Vector3(34, 0, 0), 2.0)

## Loads and places one real, exactly-named piece (not role-pattern matched)
## at an authored position/rotation, keeping its native imported scale —
## used only for Stage 3's hand-built cave structure below, where the
## layout depends on knowing exactly which piece sits where, unlike the
## role-based scatter every other environment call uses.
func _spawn_named_piece(path: String, position: Vector3, rotation_y_degrees: float = 0.0, scale_mult: float = 1.0) -> void:
    var packed := load(path) as PackedScene
    if packed == null:
        push_warning("Stage 3 cave structure: missing expected real asset %s" % path)
        return
    var inst := packed.instantiate() as Node3D
    add_child(inst)
    inst.position = position
    inst.rotation_degrees.y = rotation_y_degrees
    if scale_mult != 1.0:
        inst.scale *= scale_mult

## Stage 3's cave, hand-authored on a 2m grid rather than the generic
## perimeter-scatter every other stage's environment uses — a straight
## walled tunnel (hero start -> x=20) opening through an arch into a
## walled chamber (x=24..38) where the DungeonMonster boss waits.
## Piece sizes are REAL measured values (Blender bound-box, meters), not
## guessed: Wall = 2.0 x 2.0m tile (0.29m thick), Column_Round = 0.66m
## across x 3.99m tall, Arch_Round = 3.09m wide x 3.53m tall. See
## Docs/PROJECT_CONTINUITY.md for the full measured table and how it was
## obtained (a local Blender bounding-box pass over the real FBX source,
## not an assumption). The whole layout was verified by an actual local
## render (Godot + Xvfb/Mesa software GL), not assumed to look right —
## see Docs/PROJECT_CONTINUITY.md for what that found and fixed.
##
## HONEST LIMITATION: each piece's own authored "front" direction (which
## way a wall's decorative face points, whether Arch_Round's opening runs
## along local X or Z) was read from a real render of this exact layout,
## not guessed — but this is still a first structured pass. Fine seam
## alignment between adjacent tiles needs a visual pass in the Editor once
## the full real asset set (not this session's curated subset) is in CI.
func _spawn_stage3_cave_structure() -> void:
    var wall := RUINS_FBX + "Wall.glb"
    var wall_broken := RUINS_FBX + "Wall_Broken.glb"
    var wall_overgrown := RUINS_FBX + "Wall_Overgrown.glb"
    var wall_hole := RUINS_FBX + "Wall_Hole.glb"
    var column := RUINS_FBX + "Column_Round.glb"
    var arch := RUINS_FBX + "Arch_Round.glb"
    var torch := RUINS_FBX + "Torch.glb"

    # Tunnel: two rows of wall tiles, 2m apart, x = 2..20. A few damaged/
    # overgrown variants break up the repetition instead of one tile
    # copy-pasted end to end.
    var variants := [wall, wall, wall_broken, wall, wall_overgrown, wall, wall_hole, wall, wall, wall]
    var i := 0
    var x := 2.0
    while x <= 20.0:
        _spawn_named_piece(variants[i % variants.size()], Vector3(x, 0, -2.0), 0.0)
        _spawn_named_piece(variants[(i + 4) % variants.size()], Vector3(x, 0, 2.0), 180.0)
        x += 2.0
        i += 1
    _spawn_named_piece(torch, Vector3(8.0, 1.6, -1.85), 0.0)
    _spawn_named_piece(torch, Vector3(8.0, 1.6, 1.85), 180.0)

    # Threshold arch between the tunnel and the chamber. Centered in the
    # x=20..24 gap; scaled 1.6x — verified by real render against the 2m-
    # tall tunnel walls, since a real render showed the arch's native
    # import scale reading noticeably shorter than the walls either side of
    # it despite its measured (Blender-space) height being taller, which
    # points at a units mismatch somewhere in this pack's export rather
    # than a measuring error; scaling it up to visually match is the
    # honest fix until that's root-caused.
    _spawn_named_piece(arch, Vector3(22.0, 0, 0), 90.0, 1.6)

    # Chamber: x = 24..38, z = -8..8, walled on the left/right/far edges
    # (the tunnel arch is the only opening, on the near edge).
    var cz := -8.0
    while cz <= 8.0:
        _spawn_named_piece(wall, Vector3(24.0, 0, cz), 90.0)
        _spawn_named_piece(wall, Vector3(38.0, 0, cz), 90.0)
        cz += 2.0
    var cx := 26.0
    while cx <= 36.0:
        _spawn_named_piece(wall, Vector3(cx, 0, -8.0), 0.0)
        _spawn_named_piece(wall, Vector3(cx, 0, 8.0), 180.0)
        cx += 2.0

    for corner in [Vector3(27, 0, -6.5), Vector3(27, 0, 6.5), Vector3(35, 0, -6.5), Vector3(35, 0, 6.5)]:
        _spawn_named_piece(column, corner, 0.0)

func _spawn_cave_checkpoint() -> void:
    var checkpoint := Area3D.new()
    checkpoint.name = "CaveCheckpoint"
    checkpoint.set_script(CHECKPOINT_ZONE_SCRIPT)
    checkpoint.collision_layer = 0
    checkpoint.collision_mask = 1

    var shape := CollisionShape3D.new()
    var box := BoxShape3D.new()
    box.size = Vector3(4, 3, 4)
    shape.shape = box
    checkpoint.add_child(shape)

    add_child(checkpoint)
    # At the tunnel/chamber threshold (arch is at x=21), matching the "same
    # point or slightly before it, not the stage start" requirement.
    checkpoint.position = Vector3(19, 0, 0)

const GANG_TINT := Color(0.55, 0.26, 0.2)

## Stage 4 continues the same corridor theme past Stage 3's chamber
## (x=38): x=40..55 is a rock-to-forest transition (ruin pieces thinning
## out, trees/bushes increasing), x=55..70 is the observation approach
## (GangAlarmZone covers it), the gang tableau sits around x=58, and the
## cave entrance with its 3 door guards is at x=78.
##
## HONEST SIMPLIFICATION: the brief describes the gang actively walking
## the captive companion to the cave. Simulating that as real pathfinding
## AI was out of scope for this pass; the gang/companion are placed as a
## static tableau the hero observes from a distance, and the 3 door guards
## are presented as already in position at the cave mouth beyond it
## (narrative compression, not a live escort sequence).
##
## HONEST ASSET GAP: no "bandit"/"leader" outfit exists in the real packs
## (only Peasant and Ranger — verified by opening the actual archive, not
## guessed). Gang figures use the same hero-body fallback workers/guards
## already use, distinguished by a colour tint instead of real clothing,
## since no general runtime "equip any NPC" system exists yet (only the
## hero's own equipment pipeline does). This is a documented gap, not a
## final look.
func _build_stage_4() -> void:
    _spawn_stage4_transition_terrain()
    _spawn_stage4_tableau()
    _spawn_stage4_alarm_zone()
    _spawn_stage4_checkpoint()
    _spawn_role_variants("guard", _cached_assets, 3, Vector3(78, 0, 0), 2.0)

func _spawn_stage4_transition_terrain() -> void:
    # Thin out ruin-style rock environment pieces across x=40..55, and
    # layer in real vegetation (Tree_1-3, Bush_*, Grass, DeadTree_1-2 —
    # already verified real assets from Stage 3's vegetation pass) more
    # densely as x increases, reading as the rocky area giving way to
    # forest edge rather than a hard cut.
    var rock_pieces := [
        RUINS_FBX + "Wall_Broken.glb", RUINS_FBX + "Wall_Overgrown.glb",
        RUINS_FBX + "Floor_Standard.glb", RUINS_FBX + "Floor_Hole_Corner.glb"
    ]
    var veg_pieces := [
        RUINS_FBX + "Tree_1.glb", RUINS_FBX + "Tree_2.glb", RUINS_FBX + "Tree_3.glb",
        RUINS_FBX + "DeadTree_1.glb", RUINS_FBX + "Bush_Large.glb",
        RUINS_FBX + "Bush_1x1.glb", RUINS_FBX + "Bush_Round.glb", RUINS_FBX + "Grass.glb"
    ]
    var rng := RandomNumberGenerator.new()
    rng.seed = 404  # deterministic layout, not a different one on every rebuild
    var x := 40.0
    var i := 0
    while x <= 55.0:
        var veg_chance: float = (x - 40.0) / 15.0  # 0 near the rocks, ~1 near the forest edge
        var z := rng.randf_range(-7.0, 7.0)
        var pool := veg_pieces if rng.randf() < veg_chance else rock_pieces
        _spawn_named_piece(pool[i % pool.size()], Vector3(x, 0, z), rng.randf_range(0.0, 360.0))
        x += rng.randf_range(2.0, 3.5)
        i += 1
    # A denser treeline either side of the approach corridor from x=55 on,
    # framing it without blocking the straight path down the centre.
    var tx := 56.0
    while tx <= 76.0:
        _spawn_named_piece(veg_pieces[int(tx) % veg_pieces.size()], Vector3(tx, 0, -6.0 - rng.randf_range(0.0, 3.0)), rng.randf_range(0.0, 360.0))
        _spawn_named_piece(veg_pieces[(int(tx) + 2) % veg_pieces.size()], Vector3(tx, 0, 6.0 + rng.randf_range(0.0, 3.0)), rng.randf_range(0.0, 360.0))
        tx += 4.0

## Static observation tableau: the companion, captive, with the gang
## (leader + second-in-command + 4 members) gathered around her, roughly
## at x=58. Purely decorative (plain Node3D, not HostileCreature) — the
## hero is only meant to watch here, matching "stage 4 only watches, does
## not engage them yet".
func _spawn_stage4_tableau() -> void:
    var hero_files := _hero_files(_cached_assets)
    if hero_files.is_empty():
        push_warning("Stage 4 tableau: no hero-body fallback asset available to represent the gang/companion")
        return
    var body_path: String = hero_files[0]

    _spawn_tinted_figure(body_path, Vector3(58, 0, 3), 180.0, Color(1.0, 0.92, 0.8))  # companion — lighter tint to stand apart
    _spawn_tinted_figure(body_path, Vector3(56, 0, 1), 160.0, GANG_TINT)   # leader
    _spawn_tinted_figure(body_path, Vector3(56, 0, 5), 200.0, GANG_TINT)   # second-in-command
    var member_offsets := [Vector3(54, 0, 2), Vector3(54, 0, 4), Vector3(59, 0, 0), Vector3(59, 0, 6)]
    for offset in member_offsets:
        _spawn_tinted_figure(body_path, offset, rng_angle(), GANG_TINT)

func rng_angle() -> float:
    return randf() * 360.0

func _spawn_tinted_figure(path: String, position: Vector3, rotation_y_degrees: float, tint: Color) -> void:
    var packed := load(path) as PackedScene
    if packed == null:
        return
    var inst := packed.instantiate() as Node3D
    add_child(inst)
    inst.position = position
    inst.rotation_degrees.y = rotation_y_degrees
    _normalize_height(inst, 1.9)
    _apply_tint(inst, tint)

func _apply_tint(node: Node, tint: Color) -> void:
    if node is MeshInstance3D:
        var mesh_instance := node as MeshInstance3D
        if mesh_instance.mesh != null:
            for i in mesh_instance.mesh.get_surface_count():
                var mat := mesh_instance.get_active_material(i)
                if mat is StandardMaterial3D:
                    var dup := (mat as StandardMaterial3D).duplicate() as StandardMaterial3D
                    dup.albedo_color = dup.albedo_color * tint
                    mesh_instance.set_surface_override_material(i, dup)
    for child in node.get_children():
        _apply_tint(child, tint)

## Covers the approach from the transition terrain to the tableau
## (x=55..70). Entering it without crouching/listening spawns the ambush.
func _spawn_stage4_alarm_zone() -> void:
    var zone := Area3D.new()
    zone.name = "GangAlarmZone"
    zone.set_script(GANG_ALARM_ZONE_SCRIPT)
    zone.collision_layer = 0
    zone.collision_mask = 1
    var shape := CollisionShape3D.new()
    var box := BoxShape3D.new()
    box.size = Vector3(16, 4, 16)
    shape.shape = box
    zone.add_child(shape)
    add_child(zone)
    zone.position = Vector3(62, 0, 2)
    zone.connect("detected", Callable(self, "_on_stage4_detected"))

func _on_stage4_detected() -> void:
    push_warning("Stage 4: hero detected during the observation approach — spawning the full-gang ambush (intentionally overwhelming; not an alternate clear route)")
    var hero: Node3D = get_parent().get_node_or_null("Hero")
    var center: Vector3 = hero.global_position if hero != null else Vector3(60, 0, 2)
    for i in 6:
        var angle := TAU * float(i) / 6.0
        var offset := Vector3(cos(angle) * 3.0, 0, sin(angle) * 3.0)
        _spawn_role_variants("gang_member", _cached_assets, 1, center + offset, 1.9)

func _spawn_stage4_checkpoint() -> void:
    var checkpoint := Area3D.new()
    checkpoint.name = "GangCaveCheckpoint"
    checkpoint.set_script(CHECKPOINT_ZONE_SCRIPT)
    checkpoint.collision_layer = 0
    checkpoint.collision_mask = 1
    var shape := CollisionShape3D.new()
    var box := BoxShape3D.new()
    box.size = Vector3(4, 3, 4)
    shape.shape = box
    checkpoint.add_child(shape)
    add_child(checkpoint)
    # Just before the 3 door guards at x=78, matching the "same point or
    # slightly before it, not the stage start" requirement given explicitly
    # for Stage 4's death/respawn behaviour.
    checkpoint.position = Vector3(74, 0, 0)

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

## Roles that fall back to reusing the canonical hero body (with its
## attached outfit) when no dedicated real asset exists for them.
## "gang_member" added for Stage 4's ambush — same no-dedicated-pack gap as
## worker/guard, honestly reported the same way.
const BODY_FALLBACK_ROLES := ["worker", "guard", "gang_member"]

func _role_files(role: String, assets: Array[String]) -> Array[String]:
    var matches: Array[String] = []
    for asset in assets:
        if _matches_role(role, asset):
            matches.append(asset)
    matches.sort()

    if role in BODY_FALLBACK_ROLES and matches.is_empty():
        var body_fallback := _hero_files(assets)
        if not body_fallback.is_empty():
            _role_uses_body_fallback[role] = true
            # NOTE: the whole concatenated string must be wrapped in
            # parentheses before applying the % operator — without them,
            # % binds only to the last string literal (which has no %d in
            # it), so the format argument is never consumed and Godot raises
            # "String formatting error: not all arguments converted".
            push_warning(
                ("No dedicated '%s' asset in source packs; reusing the " +
                "canonical hero body as a placeholder (%d candidate(s)). " +
                "Add a distinct pack for this role to replace this fallback.")
                % [role, body_fallback.size()]
            )
            return body_fallback

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

        if role == "worker":
            # WorkerCaptive extends Node3D, the same base class the glTF
            # import root already is, so replacing the script is safe and
            # keeps the imported mesh/skeleton hierarchy intact.
            instance.set_script(WORKER_CAPTIVE_SCRIPT)
            add_child(instance)
            instance.position = _role_position(role, i, count, origin)
            _normalize_height(instance, target_height)
            instance.worker_index = i
            continue

        if role in PHYSICS_BODY_SCRIPTS:
            # Every HostileCreature (GuardEnemy, CaveMonster, DungeonMonster)
            # extends CharacterBody3D so the hero's melee shape query (which
            # only finds actual PhysicsBody3D colliders) — and this
            # creature's own contact-attack check against the hero — can
            # find each other. The imported visual itself has no physics
            # body, so it is reparented as a child of a new physics-enabled
            # wrapper rather than added to the scene directly.
            var body := CharacterBody3D.new()
            body.name = "%s_Real_%02d" % [role, i + 1]
            body.set_script(PHYSICS_BODY_SCRIPTS[role])

            var collision := CollisionShape3D.new()
            var capsule := CapsuleShape3D.new()
            capsule.height = target_height
            capsule.radius = target_height * 0.18
            collision.shape = capsule
            collision.position = Vector3.UP * (target_height * 0.5)
            body.add_child(collision)

            add_child(body)
            body.position = _role_position(role, i, count, origin)
            instance.name = "%sVisual" % role.capitalize()
            body.add_child(instance)
            _normalize_height(instance, target_height)
            if role == "gang_member":
                _apply_tint(instance, GANG_TINT)
            continue

        add_child(instance)
        instance.position = _role_position(role, i, count, origin)
        if role == "environment":
            # Forcing every environment piece to one uniform height distorts
            # small props into giant shapes and shrinks large ruin sections
            # to nothing, since a single wall/floor/barrel/pillar pack mixes
            # wildly different real-world sizes. Environment kits are
            # generally authored at correct real-world scale already, so
            # they are placed at their native imported scale instead.
            pass
        else:
            _normalize_height(instance, target_height)

func _report_role_coverage(assets: Array[String]) -> void:
    for role in ROLE_PATTERNS.keys():
        var matches := _hero_files(assets) if role == "hero" else _role_files(role, assets)
        if matches.is_empty():
            push_warning("Real asset coverage missing for logical role: %s" % role)
        else:
            var suffix := " (using base-character fallback)" if _role_uses_body_fallback.get(role, false) else ""
            print("ALSAEQA real asset coverage | %s: %d source-derived GLB candidates%s" % [role, matches.size(), suffix])

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
    if role == "cave_monster":
        return origin + Vector3(float(index % 2) * 6.0 - 3.0, 0, float(index / 2) * 5.0)
    if role == "dungeon_monster":
        # A single boss placed exactly at the point it was given, not offset
        # into a grid cell meant for spawning several instances.
        return origin
    if role == "environment":
        return origin + _mine_perimeter_position(index, count)
    var row := index / 4
    var col := index % 4
    return origin + Vector3(float(col - 1) * 12.0, 0, float(row - 1) * 10.0)

## Places environment pieces around an enclosed boundary (perimeter walls
## plus an inner scatter ring for props/rubble) instead of a plain open
## grid, so the real modular dungeon/ruins/rock assets read as an actual
## contained space, for both Stage 1's mine and Stage 2's rocky ambush spot.
##
## HONEST LIMITATION: this places whole environment GLBs at even intervals
## along the boundary by their bounding-box center; it does not know each
## piece's actual door/socket connectors, so seams between adjacent modular
## pieces are not guaranteed to align perfectly. That level of precision
## needs visual review/tuning inside the Editor once real screenshots are
## available — this is a first structured pass, not a finished hand-authored
## level.
func _mine_perimeter_position(index: int, count: int) -> Vector3:
    var perimeter_count: int = int(ceil(float(count) * 0.7))
    var scatter_count: int = count - perimeter_count

    if index < perimeter_count:
        var half_width := 21.0
        var half_depth := 14.0
        var perimeter_length := (half_width * 2.0) * 2.0 + (half_depth * 2.0) * 2.0
        var distance: float = (float(index) / float(max(perimeter_count, 1))) * perimeter_length

        if distance < half_width * 2.0:
            return Vector3(-half_width + distance, 0, -half_depth)
        distance -= half_width * 2.0
        if distance < half_depth * 2.0:
            return Vector3(half_width, 0, -half_depth + distance)
        distance -= half_depth * 2.0
        if distance < half_width * 2.0:
            return Vector3(half_width - distance, 0, half_depth)
        distance -= half_width * 2.0
        return Vector3(-half_width, 0, half_depth - distance)

    var scatter_index := index - perimeter_count
    var angle := TAU * float(scatter_index) / float(max(scatter_count, 1))
    var radius := 6.0 + float(scatter_index % 3) * 3.5
    return Vector3(cos(angle) * radius, 0, sin(angle) * radius)

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
