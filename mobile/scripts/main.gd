extends Node3D

@onready var objective_label: Label = $MobileHUD/Objective
@onready var stage1 = $Stage1

func _ready() -> void:
    stage1.rescue_progress.connect(_on_rescue_progress)
    stage1.stage_ready.connect(_on_stage_ready)
    _refresh_hud()

func _process(_delta: float) -> void:
    _refresh_hud()

func _refresh_hud() -> void:
    objective_label.text = "إنقاذ العمال: %d/5    هزيمة زعيم المستعبدين: %d/1" % [GameState.rescued_workers, GameState.defeated_slavers]

func _on_rescue_progress(_current: int, _required: int) -> void:
    _refresh_hud()

func _on_stage_ready() -> void:
    _refresh_hud()
    CinematicDirector.start_story_beat("StageTransition", 1.25)
