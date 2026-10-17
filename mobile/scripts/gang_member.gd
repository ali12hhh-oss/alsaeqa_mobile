extends HostileCreature
class_name GangMember

## Stage 4's detection-triggered ambush only (see gang_alarm_zone.gd) —
## spawned exclusively if the hero is spotted during the observation
## sequence. An intentionally overwhelming fight meant to punish detection,
## not an alternate way to clear the stage, so it never counts toward
## Stage 4's clear condition (only the 3 door guards, GuardEnemy, do).

func _ready() -> void:
    max_health = 50.0
    attack_damage = 16.0
    attack_range = 2.0
    attack_cooldown = 1.3
    counts_toward_stage_clear = false
    super._ready()
