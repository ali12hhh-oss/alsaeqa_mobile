extends Control

## Touch joystick that drives the same movement actions used by the hero.
## It is intentionally resolution-independent and keeps the visual control
## compact so it does not collide with the action cluster.

@export var radius := 82.0
@export var knob_radius := 34.0
@export var deadzone := 0.16
@export var opacity := 0.78

var _touch_id := -1
var _vector := Vector2.ZERO
var _center := Vector2.ZERO

func _ready() -> void:
    mouse_filter = Control.MOUSE_FILTER_STOP
    _center = size * 0.5
    queue_redraw()

func _notification(what: int) -> void:
    if what == NOTIFICATION_RESIZED:
        _center = size * 0.5
        queue_redraw()

func _gui_input(event: InputEvent) -> void:
    if event is InputEventScreenTouch:
        var touch := event as InputEventScreenTouch
        if touch.pressed and _touch_id == -1:
            _touch_id = touch.index
            _set_vector(touch.position)
            accept_event()
        elif not touch.pressed and touch.index == _touch_id:
            _touch_id = -1
            _set_vector(_center)
            accept_event()
    elif event is InputEventScreenDrag:
        var drag := event as InputEventScreenDrag
        if drag.index == _touch_id:
            _set_vector(drag.position)
            accept_event()

func _set_vector(position: Vector2) -> void:
    var delta := position - _center
    if delta.length() > radius:
        delta = delta.normalized() * radius
    _vector = delta / radius
    if _vector.length() < deadzone:
        _vector = Vector2.ZERO
    else:
        var direction := _vector.normalized()
        var strength := inverse_lerp(deadzone, 1.0, _vector.length())
        _vector = direction * strength
    _apply_actions()
    queue_redraw()

func _apply_actions() -> void:
    Input.action_release("move_left")
    Input.action_release("move_right")
    Input.action_release("move_forward")
    Input.action_release("move_back")
    if _vector.x < -0.01:
        Input.action_press("move_left", absf(_vector.x))
    elif _vector.x > 0.01:
        Input.action_press("move_right", absf(_vector.x))
    if _vector.y < -0.01:
        Input.action_press("move_forward", absf(_vector.y))
    elif _vector.y > 0.01:
        Input.action_press("move_back", absf(_vector.y))

func _exit_tree() -> void:
    Input.action_release("move_left")
    Input.action_release("move_right")
    Input.action_release("move_forward")
    Input.action_release("move_back")

func _draw() -> void:
    var c := _center
    draw_circle(c, radius + 10.0, Color(0.03, 0.05, 0.08, opacity * 0.32))
    draw_arc(c, radius, 0.0, TAU, 64, Color(0.55, 0.72, 0.92, opacity * 0.72), 3.0, true)
    draw_circle(c, radius * 0.68, Color(0.06, 0.09, 0.14, opacity * 0.34))
    var knob := c + _vector * radius * 0.72
    draw_circle(knob + Vector2(0, 4), knob_radius + 3.0, Color(0.0, 0.0, 0.0, opacity * 0.25))
    draw_circle(knob, knob_radius, Color(0.22, 0.55, 0.88, opacity * 0.72))
    draw_arc(knob, knob_radius, 0.0, TAU, 48, Color(0.78, 0.91, 1.0, opacity), 2.0, true)
