extends Area3D
class_name CheckpointZone

## Reusable checkpoint trigger. A stage places one of these anywhere along
## its path (e.g. right before a dangerous fight); when the hero enters it,
## GameState remembers this position and facing as where the hero respawns
## on death, instead of always sending the player back to the stage's
## original start point. A stage can place several of these — the hero
## always respawns at the most recently entered one.

func _ready() -> void:
    monitoring = true
    body_entered.connect(_on_body_entered)

func _on_body_entered(body: Node) -> void:
    if not body.is_in_group("alsaeqa_hero") or not (body is Node3D):
        return
    var hero := body as Node3D
    GameState.set_checkpoint(hero.global_position, hero.rotation.y)
