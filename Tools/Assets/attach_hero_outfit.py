#!/usr/bin/env python3
"""Attach a real clothing outfit mesh onto the canonical ALSAEQA hero.

Run inside Blender (background), AFTER
attach_quaternius_hero_animations.py has already produced the animated
canonical hero GLB, using the same converted_root discovery pattern as
that script (no hardcoded paths in the CI workflow).

WHY THIS EXISTS:
The "Superhero_Male_FullBody" base body is a bare sculpt body by design
(confirmed by inspecting the source FBX directly: its material's Base
Color was already correctly linked to its one embedded texture, so the
plain grey/pale look is the base body's actual authored appearance, not a
material bug). Real clothing comes from a separate pack
("Modular Character Outfits - Fantasy") authored by a different source
and NOT guaranteed to share the same skeleton/bone names as the hero.

WHAT THIS SCRIPT DOES, HONESTLY:
1. Finds the already-animated hero GLB and the chosen outfit source under
   converted_root by filename, the same way attach_quaternius_hero_
   animations.py finds its animation source.
2. Checks bone-name compatibility between the two armatures. If compatible
   (>=80% common bone names), the outfit mesh is parented onto the HERO's
   armature so it animates together with the hero for free.
3. If NOT compatible, this fails loudly rather than shipping a silently
   detached/T-posed outfit mesh floating on top of the animated hero.
4. Exports a combined hero+outfit GLB, replacing the hero file in place
   (same static-backup/animated-swap convention as the animation script).
"""
from __future__ import annotations

import shutil
import sys
from pathlib import Path

import bpy

OUTFIT_FILENAME_CANDIDATES = [
    "Male_Peasant.glb",
    "Male_Ranger.glb",
]


def fail(message: str) -> None:
    raise SystemExit(f"[ALSAEQA][hero-outfit] ERROR: {message}")


def reset_scene() -> None:
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for datablocks in (
        bpy.data.meshes, bpy.data.curves, bpy.data.materials,
        bpy.data.cameras, bpy.data.lights, bpy.data.armatures,
    ):
        for block in list(datablocks):
            if block.users == 0:
                datablocks.remove(block)


def import_glb(path: Path) -> set:
    before = set(bpy.data.objects)
    result = bpy.ops.import_scene.gltf(filepath=str(path))
    if result != {'FINISHED'}:
        fail(f"could not import: {path}")
    return set(bpy.data.objects) - before


def pick_armature(objects: set):
    candidates = [o for o in objects if o.type == "ARMATURE"]
    return max(candidates, key=lambda o: len(o.data.bones)) if candidates else None


def pick_meshes(objects: set):
    return [o for o in objects if o.type == "MESH"]


def find_one(converted_root: Path, filename: str) -> Path | None:
    matches = sorted(converted_root.rglob(filename))
    return matches[0] if matches else None


def main() -> int:
    if "--" not in sys.argv:
        fail("usage: blender --background --python attach_hero_outfit.py -- CONVERTED_ROOT")
    marker = sys.argv.index("--")
    args = sys.argv[marker + 1:]
    if not args:
        fail("missing CONVERTED_ROOT")
    converted_root = Path(args[0]).resolve()

    hero_path = find_one(converted_root, "Superhero_Male_FullBody.glb")
    if hero_path is None:
        fail("canonical hero GLB not found under converted_root")

    outfit_path = None
    for candidate_name in OUTFIT_FILENAME_CANDIDATES:
        outfit_path = find_one(converted_root, candidate_name)
        if outfit_path is not None:
            print(f"[ALSAEQA][hero-outfit] selected outfit source: {outfit_path}")
            break
    if outfit_path is None:
        fail(f"no outfit GLB found under converted_root matching any of {OUTFIT_FILENAME_CANDIDATES}")

    reset_scene()
    hero_objects = import_glb(hero_path)
    hero_armature = pick_armature(hero_objects)
    if hero_armature is None:
        fail(f"hero GLB has no armature: {hero_path}")
    hero_bones = {b.name for b in hero_armature.data.bones}

    outfit_objects = import_glb(outfit_path)
    outfit_armature = pick_armature(outfit_objects)
    outfit_meshes = pick_meshes(outfit_objects)
    if not outfit_meshes:
        fail(f"outfit source has no mesh: {outfit_path}")

    if outfit_armature is not None:
        outfit_bones = {b.name for b in outfit_armature.data.bones}
        common = hero_bones & outfit_bones
        ratio = len(common) / max(1, len(outfit_bones))
        print(f"[ALSAEQA][hero-outfit] bone compatibility hero={len(hero_bones)} outfit={len(outfit_bones)} common={len(common)} ratio={ratio:.3f}")
        if ratio < 0.80:
            fail(
                f"outfit armature is not compatible enough with the hero armature "
                f"(ratio={ratio:.3f} < 0.80) \u2014 refusing to ship a detached/broken outfit. "
                f"Outfit source: {outfit_path}"
            )
    else:
        print("[ALSAEQA][hero-outfit] WARNING: outfit source has no separate armature; assuming its vertex groups reference the hero's own bone names directly.")

    for mesh_obj in outfit_meshes:
        mesh_obj.parent = hero_armature
        armature_mod = None
        for mod in mesh_obj.modifiers:
            if mod.type == "ARMATURE":
                armature_mod = mod
                break
        if armature_mod is None:
            armature_mod = mesh_obj.modifiers.new(name="Armature", type="ARMATURE")
        armature_mod.object = hero_armature
        print(f"[ALSAEQA][hero-outfit] attached outfit mesh '{mesh_obj.name}' to hero armature '{hero_armature.name}'")

    if outfit_armature is not None:
        bpy.data.objects.remove(outfit_armature, do_unlink=True)

    for obj in bpy.context.selected_objects:
        obj.select_set(False)
    for obj in hero_objects | set(outfit_meshes):
        if obj.name in bpy.data.objects:
            obj.select_set(True)
    bpy.context.view_layer.objects.active = hero_armature

    output_path = hero_path.with_name(hero_path.stem + ".with-outfit.glb")
    backup_path = hero_path.with_name(hero_path.stem + ".pre-outfit-backup.glb")
    if output_path.exists():
        output_path.unlink()
    if backup_path.exists():
        backup_path.unlink()

    result = bpy.ops.export_scene.gltf(
        filepath=str(output_path),
        export_format="GLB",
        export_image_format="AUTO",
        export_materials="EXPORT",
        export_animations=True,
        export_animation_mode="NLA_TRACKS",
        export_nla_strips=True,
        export_skins=True,
        export_morph=True,
        export_apply=False,
        export_yup=True,
    )
    if result != {'FINISHED'} or not output_path.is_file() or output_path.stat().st_size < 10000:
        fail("export did not produce a valid combined hero+outfit GLB")

    shutil.move(str(hero_path), str(backup_path))
    shutil.move(str(output_path), str(hero_path))
    print(f"[ALSAEQA][hero-outfit] SUCCESS: {hero_path} now contains hero body + {len(outfit_meshes)} outfit mesh(es)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
