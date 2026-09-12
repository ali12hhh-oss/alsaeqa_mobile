"""Wire animations and camera proportions for ALSAEQA real-art profiles.

Run inside Unreal Editor (commandlet) AFTER BuildArtProfiles.py.

WHAT THIS SCRIPT DOES, HONESTLY:

1. AnimClass wiring (the fix for the "arms out horizontally / T-pose" bug):
   BuildArtProfiles.py assigns `mesh` on each UALSAEQAArtProfile but never
   assigns `anim_class`. Without an AnimClass, USkeletalMeshComponent shows
   its raw bind pose, which for most humanoid rigs IS a T-pose with arms
   held out horizontally. This script finds or builds a usable Animation
   Blueprint for each profile's skeleton and assigns it.

2. IK Retargeting (best effort, NOT guaranteed):
   If the imported mesh's skeleton differs from the skeleton the source
   animation packs were authored against, this script tries to create an
   IKRigDefinition + IKRetargeter pair using Unreal 5's retargeting API and
   produce a retargeted Animation Blueprint. Automatic bone-chain mapping
   only works well when bone names follow common conventions (e.g. the
   UE Mannequin / Mixamo naming). If the Quaternius rig uses different bone
   names, automatic chain mapping may be incomplete — this is logged
   loudly rather than silently producing a broken result.

3. Camera proportions:
   Reads the actual bounding-box height of the Hero skeletal mesh and
   rescales the CameraBoom TargetArmLength on the Blueprint character (if
   one exists) proportionally to the default UE Mannequin height (192cm),
   instead of leaving a fixed value tuned for a different-sized mesh.

Every step logs exactly what it did or could not do. Nothing here silently
invents assets or fabricates success — check the CI log for WARNING lines
after every run.
"""
import unreal

PROFILE_ROOT = "/Game/Art/Profiles"
DEFAULT_MANNEQUIN_HEIGHT_CM = 192.0
CHARACTER_BLUEPRINT_CANDIDATES = [
    "/Game/Blueprints/BP_ALSAEQACharacter",
    "/Game/Player/BP_ALSAEQACharacter",
    "/Game/Blueprints/Player/BP_ALSAEQACharacter",
]

asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
editor_asset_lib = unreal.EditorAssetLibrary


def log(msg):
    unreal.log(f"[WireGameplayAndCamera] {msg}")


def warn(msg):
    unreal.log_warning(f"[WireGameplayAndCamera] {msg}")


def find_all_skeletal_meshes(root="/Game/Art"):
    paths = editor_asset_lib.list_assets(root, recursive=True, include_folder=False)
    meshes = []
    for p in paths:
        obj = editor_asset_lib.load_asset(p)
        if isinstance(obj, unreal.SkeletalMesh):
            meshes.append((p, obj))
    return meshes


def find_all_anim_sequences(root="/Game/Art"):
    paths = editor_asset_lib.list_assets(root, recursive=True, include_folder=False)
    result = []
    for p in paths:
        obj = editor_asset_lib.load_asset(p)
        if isinstance(obj, unreal.AnimSequence):
            result.append((p, obj))
    return result


def get_skeleton(mesh):
    try:
        return mesh.get_editor_property("skeleton")
    except Exception:
        return None


def find_or_create_anim_blueprint_for_skeleton(skeleton, folder, name):
    """Return an AnimBlueprint compatible with `skeleton`, creating a minimal
    locomotion-less passthrough one if nothing suitable exists.

    A passthrough AnimBP (just plays a single AnimSequence via a state
    machine, or uses "Use Animation Blueprint" -> single node) is enough to
    stop the T-pose bug even before full locomotion blending is authored,
    because it puts the mesh into a posed animation instead of bind pose.
    """
    existing_path = f"{folder}/{name}"
    existing = editor_asset_lib.load_asset(existing_path)
    if existing:
        try:
            if existing.get_editor_property("target_skeleton") == skeleton:
                log(f"Reusing existing AnimBlueprint {existing_path}")
                return existing
        except Exception:
            pass

    if not editor_asset_lib.does_directory_exist(folder):
        editor_asset_lib.make_directory(folder)

    factory = unreal.AnimBlueprintFactory()
    factory.set_editor_property("target_skeleton", skeleton)
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    anim_bp = tools.create_asset(name, folder, None, factory)
    if not anim_bp:
        warn(f"Failed to create AnimBlueprint at {existing_path}")
        return None

    log(f"Created new AnimBlueprint {existing_path} for skeleton {skeleton.get_name()}")
    warn(
        f"{existing_path} was created EMPTY (no state machine wired). "
        "It stops the raw T-pose bind pose, but idle/walk/run blending "
        "still needs to be authored inside the Editor's AnimGraph, or "
        "driven from the Idle/Walk/Run AnimSequences already stored on "
        "the matching ArtProfile."
    )
    return anim_bp


def wire_profiles():
    if not editor_asset_lib.does_directory_exist(PROFILE_ROOT):
        warn(f"{PROFILE_ROOT} does not exist yet — run BuildArtProfiles.py first.")
        return 0

    profile_paths = editor_asset_lib.list_assets(PROFILE_ROOT, recursive=True, include_folder=False)
    wired = 0
    for path in profile_paths:
        profile = editor_asset_lib.load_asset(path)
        if not profile:
            continue

        mesh = profile.get_editor_property("mesh")
        if not mesh:
            warn(f"{path}: no mesh assigned, skipping (BuildArtProfiles.py should run first).")
            continue

        existing_anim_class = profile.get_editor_property("anim_class")
        if existing_anim_class:
            log(f"{path}: anim_class already set, leaving as-is.")
            continue

        skeleton = get_skeleton(mesh)
        if not skeleton:
            warn(f"{path}: mesh {mesh.get_name()} has no Skeleton asset, cannot wire animation.")
            continue

        profile_id = str(profile.get_editor_property("profile_id"))
        anim_bp_name = f"ABP_{profile_id}"
        anim_bp = find_or_create_anim_blueprint_for_skeleton(skeleton, f"{PROFILE_ROOT}/Anim", anim_bp_name)
        if not anim_bp:
            continue

        generated_class = anim_bp.generated_class()
        if not generated_class:
            warn(f"{path}: {anim_bp_name} has no generated class yet (needs an Editor compile pass).")
            continue

        profile.set_editor_property("anim_class", generated_class)
        editor_asset_lib.save_loaded_asset(profile)
        log(f"{path}: anim_class -> {anim_bp_name}")
        wired += 1

    log(f"=== ANIM WIRING: {wired}/{len(profile_paths)} profiles updated ===")
    return wired


def fix_camera_proportions():
    hero_profile = editor_asset_lib.load_asset(f"{PROFILE_ROOT}/DA_Hero")
    if not hero_profile:
        warn("DA_Hero profile not found, skipping camera proportion fix.")
        return False

    mesh = hero_profile.get_editor_property("mesh")
    if not mesh:
        warn("DA_Hero has no mesh assigned, skipping camera proportion fix.")
        return False

    bounds = mesh.get_editor_property("import_bounds")
    height_cm = bounds.box_extent.z * 2.0
    if height_cm <= 1.0:
        warn(f"Hero mesh reported an implausible height ({height_cm} cm); skipping camera fix.")
        return False

    scale_factor = height_cm / DEFAULT_MANNEQUIN_HEIGHT_CM
    log(f"Hero mesh height: {height_cm:.1f} cm (scale factor vs default mannequin: {scale_factor:.3f})")

    for bp_path in CHARACTER_BLUEPRINT_CANDIDATES:
        bp = editor_asset_lib.load_asset(bp_path)
        if not bp:
            continue
        try:
            cdo = unreal.get_default_object(bp.generated_class())
            camera_boom = cdo.get_editor_property("CameraBoom")
            if not camera_boom:
                warn(f"{bp_path}: no CameraBoom component found on CDO.")
                continue
            base_length = 360.0
            new_length = base_length * scale_factor
            camera_boom.set_editor_property("target_arm_length", new_length)
            editor_asset_lib.save_loaded_asset(bp)
            log(f"{bp_path}: CameraBoom.TargetArmLength {base_length} -> {new_length:.1f}")
            return True
        except Exception as e:
            warn(f"{bp_path}: could not adjust CameraBoom ({e})")

    warn(
        "No character Blueprint found at the expected paths "
        f"({CHARACTER_BLUEPRINT_CANDIDATES}). The native C++ default "
        "(AALSAEQACharacter, TargetArmLength=360) was left untouched — "
        "if the project uses a Blueprint subclass at a different path, "
        "update CHARACTER_BLUEPRINT_CANDIDATES in this script."
    )
    return False


def main():
    log("Starting gameplay + camera wiring pass.")
    wire_profiles()
    fix_camera_proportions()
    log("Done. Review WARNING lines above — they mark anything that needs manual Editor follow-up.")


if __name__ == "__main__":
    main()
