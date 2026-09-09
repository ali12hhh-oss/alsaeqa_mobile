extends CharacterBody3D

## Mobile hero controller: touch/virtual controls can drive the same actions as
## keyboard input. The implementation is intentionally animation-ready.

@export var walk_speed := 4.0
@export var sprint_speed := 7.0
@export var acceleration := 18.0
@export var gravity := 18.0
@export var roll_speed := 10.0

var sprinting := false
var rolling := false
var thunder_charging := false
var thunder_charge := 0.0

func _physics_process(delta: float) -> void:
    if not is_on_floor():
        velocity.y -= gravity * delta

    var input_vector := Input.get_vector("move_left", "move_right", "move_forward", "move_back")
    var direction := Vector3(input_vector.x, 0.0, input_vector.y)
    if direction.length() > 1.0:
        direction = direction.normalized()

    if not rolling:
        var speed := sprint_speed if sprinting else walk_speed
        var target := direction * speed
        velocity.x = move_toward(velocity.x, target.x, acceleration * delta)
        velocity.z = move_toward(velocity.z, target.z, acceleration * delta)
        if direction.length_squared() > 0.01:
            look_at(global_position + Vector3(direction.x, 0.0, direction.z), Vector3.UP)

    move_and_slide()

func start_sprint() -> void:
    sprinting = true

func stop_sprint() -> void:
    sprinting = false

func light_attack() -> void:
    CinematicDirector.combat_impact(false)

func heavy_attack() -> void:
    CinematicDirector.combat_impact(true)

func start_thunder_charge() -> void:
    thunder_charging = true
    thunder_charge = 0.0

func release_thunder() -> void:
    if not thunder_charging:
        return
    thunder_charging = false
    var ratio := clamp(thunder_charge, 0.0, 1.0)
    GameState.thunder_charge = ratio
    CinematicDirector.thunder_impact(ratio)
    thunder_charge = 0.0

func cancel_thunder() -> void:
    thunder_charging = false
    thunder_charge = 0.0

func _process(delta: float) -> void:
    if thunder_charging:
        thunder_charge = min(thunder_charge + delta * 0.55, 1.0)
