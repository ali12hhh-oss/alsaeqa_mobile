extends Node3D

@onready var hero: CharacterBody3D = $Hero
@onready var camera: Camera3D = $Camera3D
@onready var objective_label: Label = $MobileHUD/Objective
@onready var stage1 = $Stage1
@onready var mobile_controls: Control = $MobileHUD/MobileControls
@onready var home_panel: Panel = $MobileHUD/HomePanel
@onready var start_button: Button = $MobileHUD/HomePanel/StartButton
@onready var stage_intro: Control = $MobileHUD/StageIntro
@onready var stage_intro_label: Label = $MobileHUD/StageIntro/StageNumber

var gameplay_started := false
var home_time := 0.0
var stage_intro_time_left := 0.0
var home_hero_visual: Node3D
var home_animation_started := false

func _ready() -> void:
    stage1.rescue_progress.connect(_on_rescue_progress)
    stage1.stage_ready.connect(_on_stage_ready)
    start_button.pressed.connect(_start_adventure)

    gameplay_started = false
    mobile_controls.visible = false
    stage_intro.visible = false
    _set_gameplay_hud_visible(false)

    hero.set_physics_process(false)
    hero.set_process(false)
    camera.set_process(false)
    camera.current = true

    call_deferred("_prepare_cinematic_home")
    _refresh_hud()

func _process(delta: float) -> void:
    if gameplay_started:
        _update_stage_intro(delta)
        return

    home_time += delta
    _update_cinematic_home(delta)

func _prepare_cinematic_home() -> void:
    if not is_instance_valid(hero):
        return

    home_hero_visual = hero.get_node_or_null("HeroRealVisual")
    if home_hero_visual == null:
        for child in hero.get_children():
            if child is Node3D and child.name.to_lower().begins_with("hero_real_"):
                home_hero_visual = child as Node3D
                break

    if hero.has_method("bind_real_hero_visual"):
        hero.bind_real_hero_visual()

    if hero.has_method("_play_best_animation"):
        var idle_tokens: Array[String] = ["idle", "breathing", "stand"]
        hero.call("_play_best_animation", idle_tokens, true)
        home_animation_started = true

    _update_cinematic_home(0.0)

func _update_cinematic_home(_delta: float) -> void:
    if not is_instance_valid(hero) or not is_instance_valid(camera):
        return

    var target := hero.global_position + Vector3.UP * 1.35
    var drift := Vector3(
        sin(home_time * 0.23) * 0.65,
        2.65 + sin(home_time * 0.31) * 0.10,
        6.8 + cos(home_time * 0.19) * 0.55
    )
    camera.global_position = hero.global_position + drift
    camera.look_at(target, Vector3.UP)
    camera.fov = 58.0 + sin(home_time * 0.17) * 1.5

    if is_instance_valid(home_hero_visual):
        if not home_animation_started and hero.has_method("_play_best_animation"):
            var idle_tokens: Array[String] = ["idle", "breathing", "stand"]
            hero.call("_play_best_animation", idle_tokens, true)
            home_animation_started = true
        var sway := sin(home_time * 0.85) * 0.012
        home_hero_visual.rotation.y = sway
        home_hero_visual.position.y = sin(home_time * 1.15) * 0.012

func _start_adventure() -> void:
    if gameplay_started:
        return

    gameplay_started = true
    home_panel.visible = false
    mobile_controls.visible = true
    _set_gameplay_hud_visible(true)

    hero.set_process(true)
    hero.set_physics_process(true)
    camera.set_process(true)

    _show_stage_intro(GameState.current_stage)
    _refresh_hud()

func _show_stage_intro(stage_number: int) -> void:
    stage_intro_label.text = "المرحلة %s" % _to_arabic_digits(stage_number)
    stage_intro_time_left = 3.0
    stage_intro.visible = true
    stage_intro.modulate.a = 1.0

func _update_stage_intro(delta: float) -> void:
    if not stage_intro.visible:
        return
    stage_intro_time_left = maxf(stage_intro_time_left - delta, 0.0)
    if stage_intro_time_left <= 0.0:
        stage_intro.visible = false
    elif stage_intro_time_left < 0.55:
        stage_intro.modulate.a = stage_intro_time_left / 0.55

func _set_gameplay_hud_visible(value: bool) -> void:
    $MobileHUD/TitlePanel.visible = value
    $MobileHUD/Objective.visible = value
    $MobileHUD/Hint.visible = value

func _refresh_hud() -> void:
    objective_label.text = "إنقاذ العمال: %d/5    هزيمة زعيم المستعبدين: %d/1" % [GameState.rescued_workers, GameState.defeated_slavers]

func _on_rescue_progress(_current: int, _required: int) -> void:
    _refresh_hud()

func _on_stage_ready() -> void:
    _refresh_hud()
    CinematicDirector.start_story_beat("StageTransition", 1.25)

func _to_arabic_digits(value: int) -> String:
    var source := str(value)
    var result := ""
    var arabic := ["٠", "١", "٢", "٣", "٤", "٥", "٦", "٧", "٨", "٩"]
    for character in source:
        var digit := int(character)
        result += arabic[digit]
    return result
