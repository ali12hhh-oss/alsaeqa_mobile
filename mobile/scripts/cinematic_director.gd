extends Node

## Mobile cinematic director. Gameplay systems emit named story/action beats;
## presentation remains data-driven so real camera assets can replace placeholders.

signal story_beat_started(event_id: String)
signal story_beat_finished(event_id: String)

var active_event: String = ""
var is_playing: bool = false

func start_story_beat(event_id: String, duration: float = 0.8) -> void:
    if is_playing:
        return
    active_event = event_id
    is_playing = true
    story_beat_started.emit(event_id)
    await get_tree().create_timer(max(duration, 0.05)).timeout
    is_playing = false
    story_beat_finished.emit(event_id)
    active_event = ""

func combat_impact(heavy: bool = false) -> void:
    await start_story_beat("CombatFinisher" if heavy else "CombatImpact", 0.25 if heavy else 0.12)

func thunder_impact(charge_ratio: float) -> void:
    if charge_ratio >= 0.85:
        await start_story_beat("AbilityImpact", 0.35)
