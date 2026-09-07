import bpy
import os
import sys
from pathlib import Path

argv = sys.argv
try:
    marker = argv.index("--")
    src_root = Path(argv[marker + 1]).resolve()
    dst_root = Path(argv[marker + 2]).resolve()
except (ValueError, IndexError):
    raise SystemExit("usage: blender --background --python blender_convert.py -- SOURCE_ROOT DEST_ROOT")

EXTS = {".fbx", ".obj", ".dae", ".blend"}

def reset_scene():
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for datablocks in (bpy.data.meshes, bpy.data.curves, bpy.data.materials, bpy.data.cameras, bpy.data.lights):
        for block in list(datablocks):
            if block.users == 0:
                datablocks.remove(block)

def import_source(path: Path):
    reset_scene()
    ext = path.suffix.lower()
    if ext == ".fbx":
        bpy.ops.import_scene.fbx(filepath=str(path), use_anim=True)
    elif ext == ".obj":
        bpy.ops.wm.obj_import(filepath=str(path))
    elif ext == ".dae":
        bpy.ops.wm.collada_import(filepath=str(path))
    elif ext == ".blend":
        bpy.ops.wm.open_mainfile(filepath=str(path))
    else:
        return False
    return True

for src in sorted(src_root.rglob("*")):
    if not src.is_file() or src.suffix.lower() not in EXTS:
        continue
    try:
        if not import_source(src):
            continue
        rel = src.relative_to(src_root).with_suffix(".glb")
        dst = dst_root / rel
        dst.parent.mkdir(parents=True, exist_ok=True)
        bpy.ops.export_scene.gltf(
            filepath=str(dst),
            export_format="GLB",
            export_image_format="AUTO",
            export_materials="EXPORT",
            export_animations=True,
            export_skins=True,
            export_morph=True,
            export_apply=False,
        )
        print(f"[ALSAEQA] converted {src} -> {dst}")
    except Exception as exc:
        print(f"[ALSAEQA] conversion failed: {src}: {exc}")

print("[ALSAEQA] source conversion pass complete")
