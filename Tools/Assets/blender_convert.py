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


GENERIC_TEXTURE_WORDS = {"texture", "tex", "mat", "material", "diffuse", "albedo", "color", "colour",
                         "basecolor", "base", "map", "main", "png", "jpg", "jpeg"}


def _name_tokens(name: str) -> set:
    """Normalises a material or image name to its distinctive words, so
    'Texture_Leaves' and 'Leaf_Texture.png' both reduce to {'leaf'}.
    Generic filler words are dropped (otherwise 'Bark_Texture' would match
    every material containing 'texture'), and a crude singularisation maps
    plural/irregular forms ('leaves' -> 'leaf', 'rocks' -> 'rock')."""
    import re
    raw = re.split(r"[^a-z0-9]+", Path(name).stem.lower())
    tokens = set()
    for tok in raw:
        if not tok or tok in GENERIC_TEXTURE_WORDS or tok.isdigit():
            continue
        if tok.endswith("ves") and len(tok) > 4:
            tok = tok[:-3] + "f"
        elif tok.endswith("s") and len(tok) > 3 and not tok.endswith("ss"):
            tok = tok[:-1]
        tokens.add(tok)
    return tokens


def find_sidecar_image(source_dir: Path, material_name: str):
    """Unity-authored asset packs frequently ship textures as loose files
    rather than embedded/referenced inside the .fbx, so Blender's FBX
    importer never creates an Image Texture node for that material (there
    is nothing to link, not just something unlinked).

    Searches, in order: the source file's own directory, a Textures/
    subfolder of it, AND a sibling/parent-level Textures/ folder — many
    packs (e.g. Quaternius' Ultimate Modular Ruins) keep FBX/ and Textures/
    side by side, which the original search missed entirely. A filename
    matches when it contains the material name, or when the two share the
    same distinctive words after normalisation (see _name_tokens).
    """
    search_dirs = [source_dir]
    subfolder_names = ("Textures", "textures", "Texture", "Materials")
    for base in (source_dir, source_dir.parent, source_dir.parent.parent):
        for sub in subfolder_names:
            d = base / sub
            if d.is_dir() and d not in search_dirs:
                search_dirs.append(d)

    name_lower = material_name.lower()
    wanted = _name_tokens(material_name)
    exact, fuzzy = [], []
    for d in search_dirs:
        try:
            for f in d.iterdir():
                if not (f.is_file() and f.suffix.lower() in IMAGE_EXTS):
                    continue
                if any(k in f.name.lower() for k in ("normal", "_nrm", "roughness", "metallic", "occlusion", "_ao.", "emissive")):
                    continue
                if name_lower in f.name.lower():
                    exact.append(f)
                elif wanted and wanted == _name_tokens(f.name):
                    fuzzy.append(f)
        except OSError:
            continue
    if exact:
        return exact[0]
    return fuzzy[0] if fuzzy else None


def sanitize_metallic(mat) -> bool:
    """FBX exports from some packs import as fully metallic (metallic=1.0)
    with no metallic texture behind it. Without an environment map to
    reflect, a fully metallic surface renders as blown-out chrome/silver —
    e.g. the ruins pack's tree canopies and trunks. If nothing drives the
    Metallic input and its default is high, treat it as an import artifact
    and reset it to a matte, non-metallic surface."""
    if mat is None or not mat.use_nodes or mat.node_tree is None:
        return False
    principled = next((n for n in mat.node_tree.nodes if n.type == "BSDF_PRINCIPLED"), None)
    if principled is None:
        return False
    metallic = principled.inputs.get("Metallic")
    if metallic is None or metallic.is_linked or metallic.default_value <= 0.5:
        return False
    metallic.default_value = 0.0
    rough = principled.inputs.get("Roughness")
    if rough is not None and not rough.is_linked:
        rough.default_value = max(rough.default_value, 0.85)
    print(f"[ALSAEQA] reset import-artifact metallic on material='{mat.name}'")
    return True


def find_file_by_name(start_dir: Path, filename: str, max_up: int = 4):
    """Walks upward from start_dir (and each level's Textures-style
    subfolder) looking for a file with this exact name. Used to repair an
    Image Texture node whose baked-in path does not resolve on this
    machine — either an absolute author-time path (e.g. Imp's
    'C:/Dropbox/...'), or a texture that is genuinely shared across many
    FBX files and lives above any single one of them (e.g. the Blob pack's
    one Atlas_Monsters.png at the whole pack's root, referenced by every
    creature FBX inside Blob/FBX/)."""
    d = start_dir
    for _ in range(max_up + 1):
        direct = d / filename
        if direct.is_file():
            return direct
        for sub in ("Textures", "textures", "Texture", "Materials"):
            nested = d / sub / filename
            if nested.is_file():
                return nested
        if d.parent == d:
            break
        d = d.parent
    return None


def repair_broken_image_textures(source_dir: Path) -> int:
    """Fixes a failure mode repair_material_base_color_links() cannot see:
    a material whose Base Color IS already linked to a real Image Texture
    node, but the image itself never actually loaded (has_data is False)
    because its baked-in filepath does not exist on this machine. Blender
    does not raise an error for this — the node imports fine and the
    material silently renders as flat grey/white in the export, which is
    easy to mistake for 'no texture available' rather than 'texture file
    not found'. Re-points every such broken image at a real file with the
    same name, searched for by walking up from source_dir."""
    fixed = 0
    checked = 0
    for img in bpy.data.images:
        if img.source != 'FILE' or img.has_data:
            continue
        checked += 1
        wanted_name = Path(img.filepath).name
        real = find_file_by_name(source_dir, wanted_name)
        if real is None:
            print(f"[ALSAEQA] WARNING: broken image reference '{img.name}' (wanted file '{wanted_name}') could not be located near {source_dir} — will export flat/untextured")
            continue
        # filepath_raw (not filepath) is required here: assigning .filepath
        # on some Blender versions does not actually repoint the image's
        # on-disk source before reload(), so the "candidate found" case was
        # still failing to load even though the real file plainly exists.
        img.filepath_raw = str(real)
        img.source = 'FILE'
        try:
            img.reload()
        except Exception as exc:
            print(f"[ALSAEQA] WARNING: found '{real}' for broken image '{img.name}' but reload failed: {exc}")
            continue
        # NOTE: Image.has_data is not a reliable success signal here — in
        # Blender's background/headless mode it can still read False right
        # after a successful reload() (pixel data isn't realized until
        # something forces it, e.g. accessing .pixels), even though
        # img.size already correctly reflects the real file's dimensions
        # and the glTF exporter reads the file from disk directly rather
        # than through this same has_data flag. img.size != (0, 0) is the
        # signal that actually correlates with a working export.
        if tuple(img.size) != (0, 0):
            fixed += 1
            print(f"[ALSAEQA] repaired broken image link: '{img.name}' -> {real} (size={tuple(img.size)})")
        else:
            print(f"[ALSAEQA] WARNING: found candidate '{real}' for broken image '{img.name}' but it still failed to load")
    if checked:
        print(f"[ALSAEQA] broken-image repair pass: {fixed}/{checked} re-linked near {source_dir}")
    return fixed


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
        sanitize_metallic(mat)
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
        repair_broken_image_textures(src.parent)
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
