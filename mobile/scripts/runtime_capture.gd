extends Node

const CAPTURE_TIMES := [2.5, 5.5, 8.5]

func _ready() -> void:
    var capture_dir := OS.get_environment("ALSAEQA_CAPTURE_DIR")
    if capture_dir.is_empty():
        return
    DirAccess.make_dir_recursive_absolute(capture_dir)
    _capture_sequence(capture_dir)

func _capture_sequence(capture_dir: String) -> void:
    var elapsed := 0.0
    for capture_time in CAPTURE_TIMES:
        var wait_time := maxf(capture_time - elapsed, 0.0)
        if wait_time > 0.0:
            await get_tree().create_timer(wait_time).timeout
        await get_tree().process_frame
        await get_tree().process_frame
        var image := get_viewport().get_texture().get_image()
        var output := "%s/runtime_%02ds.png" % [capture_dir, int(capture_time)]
        var error := image.save_png(output)
        print("RUNTIME_VIEWPORT_CAPTURE path=%s size=%dx%d error=%s" % [output, image.get_width(), image.get_height(), error])
        elapsed = capture_time
