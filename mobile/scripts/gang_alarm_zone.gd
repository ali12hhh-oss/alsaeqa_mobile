extends Area3D
class_name GangAlarmZone

## Stage 4's stealth check. Reuses the hero's existing crouch/listen
## stealth states (player.gd) rather than inventing a new mechanic.
## Entering this zone without being in either state means the gang
## notices him; per explicit project direction ("if they sense Alsaeqa
## watching them before entering the cave, make them gather to fight and
## finish him off, since the companion can't be freed in Stage 4"), this
## triggers an intentionally overwhelming ambush — the honest expectation
## is the hero dies and respawns at the checkpoint, not that it's meant to
## be won.

signal detected

var _triggered := false

func _ready() -> void:
    monitoring = true
    body_entered.connect(_on_body_entered)

func _on_body_entered(body: Node) -> void:
    if _triggered or not body.is_in_group("alsaeqa_hero"):
        return
    if body.get("crouching") == true or body.get("listening") == true:
        return
    _triggered = true
    detected.emit()
