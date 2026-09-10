extends Node

## Runtime capture is intentionally disabled in CI.
## GitHub Actions captures the real X11 framebuffer externally so capture
## cannot be interrupted by Godot timers, renderer shutdown, or SceneTree exit.

func _ready() -> void:
    pass
