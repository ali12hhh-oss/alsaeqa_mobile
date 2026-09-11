#!/usr/bin/env python3
"""Transfer real Quaternius Universal Animation Library clips to the fixed ALSAEQA hero."""
from __future__ import annotations

import shutil
import sys
from pathlib import Path

import bpy


def fail(message: str) -> None:
    raise SystemExit(f"[ALSAEQA][hero-animation] ERROR: {message}")


def reset_scene() -> None:
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for datablocks in (bpy.data.meshes, bpy.data.curves, bpy.data.materials, bpy.data.cameras, bpy.data.lights, bpy.data.armatures):
        for block in list(datablocks):
            if block.users == 0:
                datablocks.remove(block)


def import_glb(path: Path) -> set[bpy.types.Object]:
    before = set(bpy.data.objects)
    result = bpy.ops.import_scene.gltf(filepath=str(path))
    if result != {'FINISHED'}:
        fail(f"Blender could not import GLB: {path}")
    return set(bpy.data.objects) - before


def pick_armature(objects: set[bpy.types.Object], animated: bool = False):
    candidates = [obj for obj in objects if obj.type == "ARMATURE"]
    if animated:
        candidates = [
            obj for obj in candidates
            if obj.animation_data
            and (
                obj.animation_data.action is not None
                or any(strip.action is not None for track in obj.animation_data.nla_tracks for strip in track.strips)
            )
        ]
    return max(candidates, key=lambda obj: len(obj.data.bones)) if candidates else None


def collect_actions(armature) -> list[bpy.types.Action]:
    """Collect every authored action imported with the donor GLB.

    Blender's glTF importer can create all animation clips as Action datablocks
    while assigning only one of them to the donor armature. Looking only at
    armature.animation_data therefore loses the rest of a multi-clip library.
    We first collect assigned/NLA actions, then include imported global actions
    whose F-curves actually target bones present on the donor rig.
    """
    actions: list[bpy.types.Action] = []
    seen: set[int] = set()
    if armature.animation_data:
        if armature.animation_data.action is not None:
            action = armature.animation_data.action
            actions.append(action)
            seen.add(action.as_pointer())
        for track in armature.animation_data.nla_tracks:
            for strip in track.strips:
                if strip.action is not None and strip.action.as_pointer() not in seen:
                    actions.append(strip.action)
                    seen.add(strip.action.as_pointer())

    donor_bones = {bone.name for bone in armature.data.bones}
    for action in list(bpy.data.actions):
        if action.as_pointer() in seen:
            continue
        bones = action_bones(action)
        if bones and (bones & donor_bones):
            actions.append(action)
            seen.add(action.as_pointer())

    return actions


def action_bones(action) -> set[str]:
    names: set[str] = set()
    for curve in action.fcurves:
        marker = 'pose.bones["'
        if marker in curve.data_path:
            names.add(curve.data_path.split(marker, 1)[1].split('"', 1)[0])
    return names


def safe_name(value: str) -> str:
    allowed = set("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-")
    cleaned = "".join(ch if ch in allowed else "_" for ch in value).strip("_")
    return cleaned or "ALSAEQA_Animation"


def find_animation_glb(converted_root: Path, hero_path: Path) -> Path:
    candidates = []
    for path in sorted(converted_root.rglob("*.glb")):
        if path.resolve() == hero_path.resolve():
            continue
        haystack = str(path).lower()
        if "animationlibrary" in haystack or "universal_animation_library" in haystack or "universal animation library" in haystack:
            candidates.append(path)
    if not candidates:
        fail("no converted Universal Animation Library GLB was found; the conversion step must produce the animation pack before transfer")

    print(f"[ALSAEQA][hero-animation] animation GLB candidates: {len(candidates)}")
    for path in candidates:
        print(f"[ALSAEQA][hero-animation] probing animation source: {path}")
        reset_scene()
        objects = import_glb(path)
        armature = pick_armature(objects, animated=True)
        if armature and collect_actions(armature):
            print(f"[ALSAEQA][hero-animation] selected animated source: {path}")
            return path
    fail("Universal Animation Library GLBs were found, but none contains an imported animated armature")


def main() -> int:
    if "--" not in sys.argv:
        fail("usage: blender --background --python attach_quaternius_hero_animations.py -- SOURCE_ROOT CONVERTED_ROOT")
    marker = sys.argv.index("--")
    if len(sys.argv) <= marker + 2:
        fail("missing SOURCE_ROOT/CONVERTED_ROOT")
    converted_root = Path(sys.argv[marker + 2]).resolve()

    hero_candidates = sorted(converted_root.rglob("Superhero_Male_FullBody.glb"))
    if not hero_candidates:
        fail("canonical Superhero_Male_FullBody.glb was not produced")
    hero_path = hero_candidates[0]

    animation_path = find_animation_glb(converted_root, hero_path)

    reset_scene()
    hero_objects = import_glb(hero_path)
    hero_armature = pick_armature(hero_objects)
    if hero_armature is None:
        fail("canonical hero has no armature")
    hero_bones = {bone.name for bone in hero_armature.data.bones}
    if len(hero_bones) < 10:
        fail(f"canonical hero armature has only {len(hero_bones)} bones")

    donor_objects = import_glb(animation_path)
    donor_armature = pick_armature(donor_objects, animated=True)
    if donor_armature is None:
        fail("selected animation source has no animated armature")
    donor_bones = {bone.name for bone in donor_armature.data.bones}
    common_ratio = len(hero_bones & donor_bones) / max(1, len(donor_bones))
    print(f"[ALSAEQA][hero-animation] rig compatibility hero={len(hero_bones)} donor={len(donor_bones)} common={len(hero_bones & donor_bones)} ratio={common_ratio:.3f}")
    if common_ratio < 0.80:
        fail("hero and animation library rigs are not compatible enough for direct transfer")

    donor_actions = collect_actions(donor_armature)
    print(f"[ALSAEQA][hero-animation] imported donor action datablocks: {len(donor_actions)}")
    compatible = []
    for action in donor_actions:
        bones = action_bones(action)
        ratio = len(bones & hero_bones) / max(1, len(bones))
        if bones and ratio >= 0.80:
            compatible.append(action)
            print(f"[ALSAEQA][hero-animation] compatible clip: {action.name} bones={len(bones)} ratio={ratio:.3f}")
    if len(compatible) < 3:
        fail(f"only {len(compatible)} compatible authored animation clips were found")
    print(f"[ALSAEQA][hero-animation] compatible authored clips: {len(compatible)}")

    hero_armature.animation_data_create()
    hero_armature.animation_data.action = None
    for track in list(hero_armature.animation_data.nla_tracks):
        hero_armature.animation_data.nla_tracks.remove(track)

    names: set[str] = set()
    for source_action in compatible:
        copied = source_action.copy()
        base = safe_name(source_action.name)
        name = base
        index = 2
        while name in names:
            name = f"{base}_{index}"
            index += 1
        names.add(name)
        copied.name = name
        track = hero_armature.animation_data.nla_tracks.new()
        track.name = name
        strip = track.strips.new(name, float(copied.frame_range[0]), copied)
        strip.action_frame_start = float(copied.frame_range[0])
        strip.action_frame_end = float(copied.frame_range[1])
        strip.blend_type = "REPLACE"
        strip.extrapolation = "NOTHING"

    hero_armature.animation_data.use_nla = True
    first_strip = hero_armature.animation_data.nla_tracks[0].strips[0]
    scene = bpy.context.scene
    scene.frame_set(int(first_strip.action_frame_start))
    before = {bone.name: bone.matrix_basis.copy() for bone in hero_armature.pose.bones}
    end_frame = int(min(first_strip.action_frame_end, first_strip.action_frame_start + 10))
    scene.frame_set(end_frame)
    changed = sum(1 for bone in hero_armature.pose.bones if bone.name in before and bone.matrix_basis != before[bone.name])
    if changed == 0:
        fail("Blender pose verification found zero changed bones in the transferred clip")
    print(f"[ALSAEQA][hero-animation] pose verification changed bones: {changed}")

    for obj in list(donor_objects):
        if obj.name in bpy.data.objects:
            bpy.data.objects.remove(obj, do_unlink=True)

    for obj in bpy.context.selected_objects:
        obj.select_set(False)
    for obj in hero_objects:
        if obj.name in bpy.data.objects:
            obj.select_set(True)
    bpy.context.view_layer.objects.active = hero_armature

    output = hero_path.with_name(hero_path.stem + ".animated.glb")
    backup = hero_path.with_name(hero_path.stem + ".static-backup.glb")
    if output.exists():
        output.unlink()
    if backup.exists():
        backup.unlink()

    result = bpy.ops.export_scene.gltf(
        filepath=str(output),
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
        export_rest_position_armature=True,
        export_reset_pose_bones=True,
        export_anim_single_armature=True,
    )
    if result != {'FINISHED'} or not output.is_file() or output.stat().st_size < 10000:
        fail("Blender export did not produce a valid animated canonical hero GLB")

    shutil.move(str(hero_path), str(backup))
    shutil.move(str(output), str(hero_path))
    print(f"[ALSAEQA][hero-animation] SUCCESS: canonical hero contains {len(names)} real authored animation clips")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
