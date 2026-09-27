extends HostileCreature
class_name CaveMonster

## Small ambient threat scattered through Stage 3's widened mine/cave
## terrain (the Blob-family creatures — PinkBlob/GreenBlob/GreenSpikyBlob/
## Mushnub — a small, simple silhouette distinct from the forest animal
## pack reserved for Stage 6, per explicit project direction). These are
## real threats — they can hurt the hero exactly like a guard can — but
## defeating them is NOT required to clear Stage 3; only the one
## DungeonMonster boss gates stage completion. That's the only difference
## from GuardEnemy: counts_toward_stage_clear = false.

func _ready() -> void:
    max_health = 30.0
    attack_damage = 8.0
    attack_range = 1.6
    attack_cooldown = 1.2
    counts_toward_stage_clear = false
    super._ready()
