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


def repair_material_base_color_links():
    """FBX imports (especially from Unity-authored asset packs) frequently
    leave an Image Texture node present in the material's node tree WITHOUT
    it being wired into the Principled BSDF's Base Color input. The texture
    still gets picked up as a dependency (so it exports and loads fine at
    runtime), but nothing ever displays it, producing a flat grey/default
    material. This repairs that specific, common failure mode.

    Reported once per material so failures are visible in CI logs instead of
    silently shipping an untextured mesh.
    """
    repaired = 0
    checked = 0
    for mat in bpy.data.materials:
        if mat is None or not mat.use_nodes or mat.node_tree is None:
            continue
        checked += 1
        nodes = mat.node_tree.nodes
        links = mat.node_tree.links

        principled = next((n for n in nodes if n.type == "BSDF_PRINCIPLED"), None)
        if principled is None:
            continue

        base_color_input = principled.inputs.get("Base Color")
        if base_color_input is None:
            continue
        if base_color_input.is_linked:
            continue  # already wired correctly, nothing to do

        image_nodes = [n for n in nodes if n.type == "TEX_IMAGE" and n.image is not None]
        if not image_nodes:
            continue

        # Prefer a texture whose image name suggests it is a base/albedo/
        # diffuse map over normal/roughness/etc. Fall back to the first one.
        def score(node):
            name = node.image.name.lower()
            if any(k in name for k in ("normal", "nrm", "_n.", "roughness", "metallic", "ao", "occlusion")):
                return -1
            if any(k in name for k in ("albedo", "basecolor", "base_color", "diffuse", "color")):
                return 2
            return 1

        image_nodes.sort(key=score, reverse=True)
        chosen = image_nodes[0]
        links.new(chosen.outputs["Color"], base_color_input)
        repaired += 1
        print(f"[ALSAEQA] repaired Base Color link: material='{mat.name}' image='{chosen.image.name}'")

    if checked:
        print(f"[ALSAEQA] material repair pass: {repaired}/{checked} materials needed a Base Color fix")


for src in sorted(src_root.rglob("*")):
    if not src.is_file() or src.suffix.lower() not in EXTS:
        continue
    try:
        if not import_source(src):
            continue
        repair_material_base_color_links()
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
