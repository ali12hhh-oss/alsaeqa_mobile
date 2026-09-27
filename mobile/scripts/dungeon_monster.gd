extends HostileCreature
class_name DungeonMonster

## Stage 3's one mandatory gate: the single boss creature (Imp, from the
## real "Bestiary - Dungeon Monsters Kit") placed inside the main cave.
## Defeating it is what advances the game to Stage 4 — every other Stage 3
## threat (CaveMonster) is optional; this one is not.

func _ready() -> void:
    max_health = 140.0
    attack_damage = 20.0
    attack_range = 2.6
    attack_cooldown = 1.8
    counts_toward_stage_clear = true
    super._ready()
