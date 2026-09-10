extends Node

## CI-only runtime viewport capture.
## Captures three frames after the real scene has had time to finish loading.
const CAPTURE_TIMES := [3.0, 7.0, 11.0]

var _capture_finished := false

func _ready() -> void:
    var capture_dir := OS.get_environment("ALSAEQA_CAPTURE_DIR")
    if capture_dir.is_empty():
        return
    DirAccess.make_dir_recursive_absolute(capture_dir)
    call_deferred("_capture_sequence", capture_dir)

func _capture_sequence(capture_dir: String) -> void:
    var elapsed := 0.0
    for capture_time in CAPTURE_TIMES:
        var wait_time := maxf(capture_time - elapsed, 0.0)
        if wait_time > 0.0:
            await get_tree().create_timer(wait_time).timeout

        # Wait for an actual rendered frame rather than only a process tick.
        await RenderingServer.frame_post_draw
        await get_tree().process_frame

        var viewport := get_viewport()
        if viewport == null or viewport.get_texture() == null:
            push_error("Runtime capture viewport/texture is unavailable")
            return

        var image := viewport.get_texture().get_image()
        if image == null or image.is_empty():
            push_error("Runtime capture produced an empty viewport image")
            return

        var output := "%s/runtime_%02ds.png" % [capture_dir, int(capture_time)]
        var error := image.save_png(output)
        print("RUNTIME_VIEWPORT_CAPTURE path=%s size=%dx%d error=%s" % [output, image.get_width(), image.get_height(), error])
        if error != OK:
            push_error("Runtime viewport capture failed: %s" % output)
            return

        elapsed = capture_time

    _capture_finished = true
    # Let the final PNG write and renderer finish before terminating Godot.
    await RenderingServer.frame_post_draw
    get_tree().quit()
