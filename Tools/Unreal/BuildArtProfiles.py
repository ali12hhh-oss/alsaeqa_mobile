"""Build ALSAEQA data-driven art profiles from imported real assets.

Run inside Unreal Editor after importing licensed assets into /Game/Art.
The script never generates geometry and never invents asset paths. It only
creates profile assets when a matching real SkeletalMesh is already present.
"""
import unreal

ROOT = "/Game/Art"
PROFILE_ROOT = "/Game/Art/Profiles"

ROLE_RULES = {
    "DA_Hero": ("Hero", ["Hero", "Player", "UniversalBase"]),
    "DA_Companion": ("Companion", ["Companion", "Female", "Woman"]),
    "DA_Enemy": ("Enemy", ["Enemy", "Monster", "Soldier"]),
    "DA_Boss": ("Boss", ["Boss", "Elite"]),
    "DA_GiantSnake": ("GiantSnake", ["Snake"]),
    "DA_Horse": ("Horse", ["Horse"]),
    "DA_Beast": ("Beast", ["Beast", "Wolf", "Bear"]),
    "DA_ThunderBeast": ("ThunderBeast", ["Thunder", "Beast"]),
    "DA_FlyingBeast": ("FlyingBeast", ["Flying", "Bird", "Dragon", "Eagle"]),
}


def ensure_folder(path):
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def find_mesh(tokens):
    assets = unreal.EditorAssetLibrary.list_assets(ROOT, recursive=True, include_folder=False)
    candidates = []
    for path in assets:
        obj = unreal.EditorAssetLibrary.load_asset(path)
        if not isinstance(obj, unreal.SkeletalMesh):
            continue
        name = path.rsplit("/", 1)[-1].lower()
        score = sum(1 for token in tokens if token.lower() in name)
        if score:
            candidates.append((score, path, obj))
    candidates.sort(key=lambda x: (-x[0], x[1]))
    return candidates[0][2] if candidates else None


def create_profile(asset_name, role, mesh):
    path = f"{PROFILE_ROOT}/{asset_name}"
    existing = unreal.EditorAssetLibrary.load_asset(path)
    if existing:
        profile = existing
    else:
        tools = unreal.AssetToolsHelpers.get_asset_tools()
        factory = unreal.DataAssetFactory()
        factory.set_editor_property("data_asset_class", unreal.load_class(None, "/Script/ALSAEQA.ALSAEQAArtProfile"))
        profile = tools.create_asset(asset_name, PROFILE_ROOT, None, factory)
    if not profile:
        unreal.log_warning(f"Could not create {path}")
        return False
    profile.set_editor_property("profile_id", asset_name)
    profile.set_editor_property("display_name", unreal.TextLibrary.conv_string_to_text(asset_name.replace("DA_", "")))
    profile.set_editor_property("mesh", mesh)
    role_enum = getattr(unreal, "ALSAEQAArtRole", None)
    if role_enum:
        try:
            profile.set_editor_property("role", role_enum[role])
        except Exception:
            pass
    unreal.EditorAssetLibrary.save_loaded_asset(profile)
    unreal.log(f"PROFILE READY: {path} -> {mesh.get_path_name()}")
    return True


def build():
    ensure_folder(PROFILE_ROOT)
    created = 0
    for asset_name, (role, tokens) in ROLE_RULES.items():
        mesh = find_mesh(tokens)
        if mesh and create_profile(asset_name, role, mesh):
            created += 1
        else:
            unreal.log_warning(f"No real SkeletalMesh found for {asset_name}; leaving profile unwired.")
    unreal.log(f"=== ALSAEQA ART PROFILE BUILD: {created}/{len(ROLE_RULES)} ===")
    return created


if __name__ == "__main__":
    build()
