extends Node3D

@onready var objective_label: Label = $MobileHUD/Objective
@onready var stage1 = $Stage1
@onready var mobile_controls: Control = $MobileHUD/MobileControls
@onready var home_panel: Panel = $MobileHUD/HomePanel
@onready var start_button: Button = $MobileHUD/HomePanel/StartButton

var gameplay_started := false

func _ready() -> void:
    stage1.rescue_progress.connect(_on_rescue_progress)
    stage1.stage_ready.connect(_on_stage_ready)
    start_button.pressed.connect(_start_gameplay)
    mobile_controls.visible = false
    _set_gameplay_hud_visible(false)
    _refresh_hud()

func _process(_delta: float) -> void:
    _refresh_hud()

func _start_gameplay() -> void:
    if gameplay_started:
        return
    gameplay_started = true
    home_panel.visible = false
    mobile_controls.visible = true
    _set_gameplay_hud_visible(true)

func _set_gameplay_hud_visible(value: bool) -> void:
    $MobileHUD/Objective.visible = value
    $MobileHUD/Hint.visible = value

func _refresh_hud() -> void:
    objective_label.text = "إنقاذ العمال: %d/5    هزيمة زعيم المستعبدين: %d/1" % [GameState.rescued_workers, GameState.defeated_slavers]

func _on_rescue_progress(_current: int, _required: int) -> void:
    _refresh_hud()

func _on_stage_ready() -> void:
    _refresh_hud()
    CinematicDirector.start_story_beat("StageTransition", 1.25)
