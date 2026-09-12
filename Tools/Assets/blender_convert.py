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
IMAGE_EXTS = (".png", ".jpg", ".jpeg", ".tga", ".bmp", ".tif", ".tiff")

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


def find_sidecar_image(source_dir: Path, material_name: str):
    """Unity-authored asset packs frequently ship textures as loose files
    sitting next to the .fbx rather than embedded/referenced inside it, so
    Blender's FBX importer never creates an Image Texture node at all for
    that material (there is nothing to link, not just something unlinked).
    This searches the source file's own directory (and one level down, for
    packs that nest a Textures/ subfolder) for an image whose filename
    contains the material name, case-insensitively.
    """
    candidates = []
    search_dirs = [source_dir]
    for sub in ("Textures", "textures", "Texture", "Materials"):
        d = source_dir / sub
        if d.is_dir():
            search_dirs.append(d)

    name_lower = material_name.lower()
    for d in search_dirs:
        try:
            for f in d.iterdir():
                if f.is_file() and f.suffix.lower() in IMAGE_EXTS and name_lower in f.name.lower():
                    candidates.append(f)
        except OSError:
            continue
    return candidates[0] if candidates else None


def repair_material_base_color_links(source_dir: Path):
    """Two-part repair for the common flat-grey-material failure mode:

    1. An Image Texture node exists in the material but was never wired
       into the Principled BSDF's Base Color input (texture loads fine at
       runtime, but nothing displays it).
    2. No Image Texture node exists at all, because the FBX never embedded
       or referenced one (common with Unity asset-store exports that ship
       textures as loose sidecar files). In that case, search the source
       folder for a same-named image and wire it in from scratch.

    Both cases are reported explicitly in the CI log so a silently
    untextured mesh is never shipped without a trace.
    """
    repaired_linked = 0
    repaired_sidecar = 0
    unresolved = 0
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

        if image_nodes:
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
            repaired_linked += 1
            print(f"[ALSAEQA] repaired Base Color link: material='{mat.name}' image='{chosen.image.name}'")
            continue

        # No texture node exists at all — look for a sidecar image file.
        sidecar = find_sidecar_image(source_dir, mat.name)
        if sidecar is not None:
            try:
                img = bpy.data.images.load(str(sidecar), check_existing=True)
                tex_node = nodes.new("ShaderNodeTexImage")
                tex_node.image = img
                tex_node.location = (principled.location.x - 300, principled.location.y)
                links.new(tex_node.outputs["Color"], base_color_input)
                repaired_sidecar += 1
                print(f"[ALSAEQA] wired sidecar texture: material='{mat.name}' image='{sidecar.name}'")
            except Exception as exc:
                unresolved += 1
                print(f"[ALSAEQA] WARNING: found sidecar '{sidecar}' for material='{mat.name}' but failed to load: {exc}")
        else:
            unresolved += 1
            print(f"[ALSAEQA] WARNING: material='{mat.name}' has no texture node and no matching sidecar image was found near {source_dir} — will export flat/untextured")

    if checked:
        print(f"[ALSAEQA] material repair pass: {repaired_linked} linked, {repaired_sidecar} sidecar-wired, {unresolved} unresolved out of {checked} materials")


for src in sorted(src_root.rglob("*")):
    if not src.is_file() or src.suffix.lower() not in EXTS:
        continue
    try:
        if not import_source(src):
            continue
        repair_material_base_color_links(src.parent)
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
