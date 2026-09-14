extends Control

## Opening story cinematic, played once after the player presses "ابدأ
## المغامرة" and before Stage 1 begins. Presents the hero's backstory as
## sequential fading text over a black screen with the canonical hero
## model visible and idling, driven by CinematicDirector story beats.
##
## MUSIC: no real music asset exists anywhere in the downloaded ALSAEQA
## asset packs (confirmed by inspecting the full release archive inventory
## — zero audio/music files of any kind). music_player below is wired and
## ready to play a real track the moment one is added to the project
## (assign an AudioStream to $MusicPlayer.stream, or call
## play_music(stream) before start()); until then it stays silent rather
## than shipping a placeholder beep or stock sound as if it were final
## music, per the project's no-placeholder rule.

signal finished

@export var lines: Array[String] = [
    "قبل أن يُسمّى بالصاعقة، كان طفلاً بعمر خمس سنين فقط.",
    "شاهد بعينيه مقتل أبيه وأخيه بيد تجّار الرقيق.",
    "اتُسر... واسترُقّ في مناجم الجبل المظلمة، سنة بعد سنة.",
    "حتى انهار المنجم يوماً — وأيقظ الانهيار قوة الرعد النائمة في دمه.",
    "الآن، بعمر اثنين وعشرين عاماً، يستيقظ من جديد.",
    "ذاكرته مكسورة... لكن القوة التي أيقظتها لن تُكسر.",
]

@export var line_duration: float = 3.2
@export var fade_duration: float = 0.6

@onready var label: Label = $StoryLabel
@onready var skip_button: Button = $SkipButton
@onready var music_player: AudioStreamPlayer = $MusicPlayer

var _index := 0
var _running := false

func _ready() -> void:
    visible = false
    label.modulate.a = 0.0
    skip_button.pressed.connect(_skip)

func start() -> void:
    if _running:
        return
    _running = true
    _index = 0
    visible = true
    modulate.a = 1.0

    if music_player.stream != null:
        music_player.play()
    else:
        push_warning("StoryIntro: no music stream assigned yet — playing the opening scene silently. Assign a real track to $MusicPlayer.stream when one is available.")

    CinematicDirector.start_story_beat("HeroOrigin", line_duration * lines.size())
    _play_next_line()

func play_music(stream: AudioStream) -> void:
    music_player.stream = stream

func _play_next_line() -> void:
    if not _running:
        return
    if _index >= lines.size():
        _finish()
        return
    label.text = lines[_index]
    _index += 1

    var tween := create_tween()
    tween.tween_property(label, "modulate:a", 1.0, fade_duration)
    tween.tween_interval(line_duration - fade_duration * 2.0)
    tween.tween_property(label, "modulate:a", 0.0, fade_duration)
    tween.finished.connect(_play_next_line)

func _skip() -> void:
    _finish()

func _finish() -> void:
    if not _running:
        return
    _running = false
    if music_player.playing:
        music_player.stop()
    var tween := create_tween()
    tween.tween_property(self, "modulate:a", 0.0, 0.4)
    tween.finished.connect(func():
        visible = false
        finished.emit()
    )
