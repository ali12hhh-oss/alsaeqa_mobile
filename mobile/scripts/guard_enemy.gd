extends HostileCreature
class_name GuardEnemy

## Stage guard (Stage 1 mine guards, Stage 2 weapon-chest guards). All
## behaviour — health, receive_damage, defeat, melee contact damage against
## the hero, and registering with the active stage controller — lives in
## the shared HostileCreature base now; this file only sets the guard's own
## tuning and confirms it counts toward the stage's clear condition.

func _ready() -> void:
    max_health = 60.0
    attack_damage = 14.0
    attack_range = 2.0
    attack_cooldown = 1.5
    counts_toward_stage_clear = true
    super._ready()
