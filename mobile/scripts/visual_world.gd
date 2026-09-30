extends Node3D

## Procedural visual blockout used until the licensed/owned production meshes are
## imported. It deliberately establishes composition, lighting, color language,
## mine silhouettes, worker/enemy placement and cinematic atmosphere.

@export var worker_count := 5
@export var enemy_count := 9

func _ready() -> void:
    _build_environment()
    _build_mine()
    _build_characters()

func _mat(color: Color, roughness := 0.75, metallic := 0.0) -> StandardMaterial3D:
    var m := StandardMaterial3D.new()
    m.albedo_color = color
    m.roughness = roughness
    m.metallic = metallic
    return m

func _mesh(parent: Node3D, mesh: PrimitiveMesh, pos: Vector3, material: Material, scale := Vector3.ONE) -> MeshInstance3D:
    var n := MeshInstance3D.new()
    n.mesh = mesh
    n.position = pos
    n.scale = scale
    n.material_override = material
    parent.add_child(n)
    return n

func _build_environment() -> void:
    var env := WorldEnvironment.new()
    var e := Environment.new()
    e.background_mode = Environment.BG_COLOR
    e.background_color = Color("07111c")
    e.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
    e.ambient_light_color = Color("6e86a1")
    e.ambient_light_energy = 0.38
    e.tonemap_mode = Environment.TONE_MAPPER_FILMIC
    e.glow_enabled = true
    e.glow_intensity = 0.9
    e.glow_strength = 1.1
    e.fog_enabled = true
    e.fog_light_color = Color("17283a")
    e.fog_density = 0.018
    env.environment = e
    add_child(env)

    var moon := DirectionalLight3D.new()
    moon.rotation_degrees = Vector3(-52, -28, 0)
    moon.light_color = Color("9bb8d7")
    moon.light_energy = 1.15
    moon.shadow_enabled = true
    add_child(moon)

func _build_mine() -> void:
    var stone := _mat(Color("242a31"))
    var dark_stone := _mat(Color("111820"))
    var timber := _mat(Color("5a3d29"))
    var metal := _mat(Color("59636d"), 0.35, 0.55)
    var ember := _mat(Color("ff7b35"), 0.45, 0.0)
    ember.emission_enabled = true
    ember.emission = Color("ff5b20")
    ember.emission_energy_multiplier = 4.0

    var floor := BoxMesh.new()
    floor.size = Vector3(46, 0.5, 34)
    _mesh(self, floor, Vector3(0,-1,0), dark_stone)

    for z in [-15.0, 15.0]:
        var wall := BoxMesh.new()
        wall.size = Vector3(46, 12, 1.5)
        _mesh(self, wall, Vector3(0,5,z), stone)

    for x in [-22.0, 22.0]:
        var wall := BoxMesh.new()
        wall.size = Vector3(1.5, 12, 34)
        _mesh(self, wall, Vector3(x,5,0), stone)

    for x in [-15.0, -5.0, 5.0, 15.0]:
        for z in [-10.0, 10.0]:
            var post := BoxMesh.new()
            post.size = Vector3(0.8, 9, 0.8)
            _mesh(self, post, Vector3(x,3.5,z), timber)
            var beam := BoxMesh.new()
            beam.size = Vector3(11, 0.7, 0.8)
            _mesh(self, beam, Vector3(x,7.7,z), timber)

    for p in [Vector3(-13,1,-4),Vector3(12,1,4),Vector3(-2,1,8)]:
        var lamp := SphereMesh.new()
        lamp.radius = 0.45
        lamp.height = 0.9
        _mesh(self, lamp, p + Vector3(0,2.8,0), ember)

func _build_characters() -> void:
    var hero_mat := _mat(Color("174a72"), 0.55, 0.15)
    var skin := _mat(Color("b87955"))
    var worker_mat := _mat(Color("a67b52"))
    var enemy_mat := _mat(Color("4d2025"), 0.65, 0.1)
    var weapon_mat := _mat(Color("9da7af"), 0.25, 0.75)

    var hero := get_node_or_null("../Hero")
    if hero:
        var body := CapsuleMesh.new()
        body.radius = 0.55
        body.height = 1.8
        _mesh(hero, body, Vector3(0,1,0), hero_mat)
        var head := SphereMesh.new()
        head.radius = 0.42
        head.height = 0.84
        _mesh(hero, head, Vector3(0,2.25,0), skin)

    for i in worker_count:
        var worker := Node3D.new()
        worker.name = "Worker_%02d" % (i + 1)
        add_child(worker)
        var x := -14.0 + float(i) * 4.8
        _mesh(worker, CapsuleMesh.new(), Vector3(x,1,3.5), worker_mat, Vector3(0.7,1.0,0.7))
        var h := SphereMesh.new()
        h.radius = 0.38
        h.height = 0.76
        _mesh(worker, h, Vector3(x,2.25,3.5), skin)

    for i in enemy_count:
        var enemy := Node3D.new()
        enemy.name = "MineGuard_%02d" % (i + 1)
        add_child(enemy)
        var angle := TAU * float(i) / float(enemy_count)
        var radius := 10.0 + float(i % 3) * 2.0
        var pos := Vector3(cos(angle) * radius, 1.0, sin(angle) * radius)
        _mesh(enemy, CapsuleMesh.new(), pos, enemy_mat, Vector3(0.72,1.05,0.72))
        var blade := BoxMesh.new()
        blade.size = Vector3(0.12,1.8,0.32)
        _mesh(enemy, blade, pos + Vector3(0.8,0.25,0), weapon_mat, Vector3.ONE)

