#!/usr/bin/env python3
"""Attach the real Quaternius Universal Animation Library to the canonical hero.

The mobile project intentionally keeps Superhero_Male_FullBody as the one visible
hero. The base-character pack and animation pack use the same universal humanoid
rig, so this step combines the authored mesh/materials with the authored
animation actions before Godot imports the result.
"""

from __future__ import annotations

import shutil
import sys
from pathlib import Path

import bpy


def die(message: str) -> None:
    raise SystemExit(f"[ALSAEQA][hero-animation] ERROR: {message}")


def reset_scene() -> None:
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for datablocks in (
        bpy.data.meshes,
        bpy.data.curves,
        bpy.data.materials,
        bpy.data.cameras,
        bpy.data.lights,
        bpy.data.armatures,
    ):
        for block in list(datablocks):
            if block.users == 0:
                datablocks.remove(block)


def import_glb(path: Path) -> set[bpy.types.Object]:
    before = set(bpy.data.objects)
    bpy.ops.import_scene.gltf(filepath=str(path))
    return set(bpy.data.objects) - before


def choose_armature(objects: set[bpy.types.Object], require_animation: bool = False):
    candidates = [obj for obj in objects if obj.type == "ARMATURE"]
    if require_animation:
        animated = []
        for obj in candidates:
            if not obj.animation_data:
                continue
            if obj.animation_data.action is not None or any(
                strip.action
                for track in obj.animation_data.nla_tracks
                for strip in track.strips
            ):
                animated.append(obj)
        candidates = animated
    if not candidates:
        return None
    return max(candidates, key=lambda obj: len(obj.data.bones))


def collect_animation_actions(armature) -> list[bpy.types.Action]:
    result: list[bpy.types.Action] = []
    seen: set[int] = set()
    if armature.animation_data:
        if armature.animation_data.action is not None:
            action = armature.animation_data.action
            seen.add(action.as_pointer())
            result.append(action)
        for track in armature.animation_data.nla_tracks:
            for strip in track.strips:
                if strip.action is not None and strip.action.as_pointer() not in seen:
                    seen.add(strip.action.as_pointer())
                    result.append(strip.action)
    return result


def animated_bone_names(action) -> set[str]:
    names: set[str] = set()
    for curve in action.fcurves:
        path = curve.data_path
        marker = 'pose.bones["'
        if marker in path:
            tail = path.split(marker, 1)[1]
            name = tail.split('"', 1)[0]
            if name:
                names.add(name)
    return names


def sanitize_name(value: str) -> str:
    allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-"
    return "".join(ch if ch in allowed else "_" for ch in value).strip("_") or "ALSAEQA_Animation"


def main() -> int:
    if "--" not in sys.argv:
        die("usage: blender --background --python attach_quaternius_hero_animations.py -- SOURCE_ROOT CONVERTED_ROOT")
    marker = sys.argv.index("--")
    if len(sys.argv) <= marker + 2:
        die("missing SOURCE_ROOT/CONVERTED_ROOT")

    source_root = Path(sys.argv[marker + 1]).resolve()
    converted_root = Path(sys.argv[marker + 2]).resolve()

    hero_candidates = sorted(converted_root.rglob("Superhero_Male_FullBody.glb"))
    if not hero_candidates:
        die("canonical Superhero_Male_FullBody.glb was not produced by the real-asset conversion")
    hero_path = hero_candidates[0]

    animation_candidates = sorted(
        p
        for p in source_root.rglob("*.glb")
        if "animationlibrary" in p.name.lower()
        or "universalanimationlibrary" in p.name.lower()
    )
    if not animation_candidates:
        die("Universal Animation Library GLB was not found in the verified real-asset release")
    standard = [p for p in animation_candidates if "standard" in p.name.lower()]
    animation_path = standard[0] if standard else animation_candidates[0]

    print(f"[ALSAEQA][hero-animation] canonical hero: {hero_path}")
    print(f"[ALSAEQA][hero-animation] animation source: {animation_path}")

    reset_scene()
    hero_objects = import_glb(hero_path)
    hero_armature = choose_armature(hero_objects)
    if hero_armature is None:
        die("canonical hero has no armature after GLB import")
    hero_bones = {bone.name for bone in hero_armature.data.bones}
    if len(hero_bones) < 10:
        die(f"canonical hero armature has unexpectedly few bones: {len(hero_bones)}")

    source_objects = import_glb(animation_path)
    source_armature = choose_armature(source_objects, require_animation=True)
    if source_armature is None:
        die("Universal Animation Library contains no animated armature")

    source_bones = {bone.name for bone in source_armature.data.bones}
    common = hero_bones & source_bones
    ratio = len(common) / max(1, len(source_bones))
    print(
        f"[ALSAEQA][hero-animation] rig compatibility: hero={len(hero_bones)} "
        f"source={len(source_bones)} common={len(common)} ratio={ratio:.3f}"
    )
    if ratio < 0.80:
        die("hero and Universal Animation Library rigs are not compatible enough for a safe direct animation transfer")

    source_actions = collect_animation_actions(source_armature)
    if not source_actions:
        die("Universal Animation Library imported, but no animation actions were found")

    # Keep only actions that actually animate bones shared by the canonical rig.
    compatible: list[bpy.types.Action] = []
    for action in source_actions:
        bones = animated_bone_names(action)
        if not bones:
            continue
        action_ratio = len(bones & hero_bones) / max(1, len(bones))
        if action_ratio >= 0.80:
            compatible.append(action)

    if not compatible:
        die("none of the imported authored animation actions target the canonical hero rig")

    print(f"[ALSAEQA][hero-animation] compatible authored actions: {len(compatible)}")

    # The source animation armature is only a donor. Copy its authored actions
    # onto the canonical hero armature and expose them as separate NLA tracks.
    hero_armature.animation_data_create()
    hero_armature.animation_data.action = None
    for track in list(hero_armature.animation_data.nla_tracks):
        hero_armature.animation_data.nla_tracks.remove(track)

    used_names: set[str] = set()
    for source_action in compatible:
        copied = source_action.copy()
        base = sanitize_name(source_action.name)
        name = base
        suffix = 2
        while name in used_names:
            name = f"{base}_{suffix}"
            suffix += 1
        copied.name = name
        used_names.add(name)

        track = hero_armature.animation_data.nla_tracks.new()
        track.name = name
        start = float(copied.frame_range[0])
        strip = track.strips.new(name=name, start=start, action=copied)
        strip.action_frame_start = float(copied.frame_range[0])
        strip.action_frame_end = float(copied.frame_range[1])
        strip.blend_type = "REPLACE"
        strip.extrapolation = "NOTHING"
        if strip.action_suitable_slots:
            strip.action_slot = strip.action_suitable_slots[0]

    if len(used_names) < 3:
        die(f"animation transfer produced too few usable clips: {len(used_names)}")

    # Verify the first authored clip actually changes a pose bone on the target.
    first_track = hero_armature.animation_data.nla_tracks[0]
    first_strip = first_track.strips[0]
    hero_armature.animation_data.use_nla = True
    scene = bpy.context.scene
    scene.frame_set(int(first_strip.action_frame_start))
    before_pose = {
        bone.name: bone.matrix_basis.copy()
        for bone in hero_armature.pose.bones
    }
    scene.frame_set(int(min(first_strip.action_frame_end, first_strip.action_frame_start + 10)))
    changed = 0
    for bone in hero_armature.pose.bones:
        previous = before_pose.get(bone.name)
        if previous is not None and not bone.matrix_basis == previous:
            changed += 1
    if changed == 0:
        die("transferred animation clip did not change the canonical hero pose during Blender verification")
    print(f"[ALSAEQA][hero-animation] pose verification changed bones: {changed}")

    # Remove the donor armature and its meshes before export. Only the canonical
    # hero and its real materials/textures remain in the output.
    for obj in list(source_objects):
        if obj.name in bpy.data.objects:
            bpy.data.objects.remove(obj, do_unlink=True)

    for obj in bpy.context.selected_objects:
        obj.select_set(False)
    for obj in hero_objects:
        if obj.name in bpy.data.objects:
            obj.select_set(True)
    bpy.context.view_layer.objects.active = hero_armature

    output = hero_path.with_name(hero_path.stem + ".animated.glb")
    if output.exists():
        output.unlink()

    bpy.ops.export_scene.gltf(
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

    if not output.is_file() or output.stat().st_size < 10000:
        die("Blender reported success but the animated canonical hero GLB is missing or implausibly small")

    backup = hero_path.with_name(hero_path.stem + ".static-backup.glb")
    if backup.exists():
        backup.unlink()
    shutil.move(str(hero_path), str(backup))
    shutil.move(str(output), str(hero_path))
    print(
        f"[ALSAEQA][hero-animation] SUCCESS: canonical hero now contains "
        f"{len(used_names)} real Universal Animation Library clips"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
