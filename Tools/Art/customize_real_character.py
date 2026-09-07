"""ALSAEQA Blender helper for unique character variants.

Works only on existing imported mesh objects. It does not generate primitive
body parts. Run inside Blender's Python environment after importing a real,
rigged character/outfit pack.
"""
import bpy
import os

ROLES = {
    "Hero": {
        "collection": "ALSAEQA_Hero",
        "prefix": "ALSAEQA_Hero",
        "description": "Lean survivor silhouette with asymmetric thunder armor and mining-origin details",
    },
    "Companion": {
        "collection": "ALSAEQA_Companion",
        "prefix": "ALSAEQA_Companion",
        "description": "Distinct female travel/light-armour silhouette with family-symbol accessory",
    },
    "Enemy_MineOverseer": {
        "collection": "ALSAEQA_Enemy_MineOverseer",
        "prefix": "ALSAEQA_Enemy_MineOverseer",
        "description": "Industrial armor, lamp and mining-tool silhouette",
    },
    "Enemy_TyrantSoldier": {
        "collection": "ALSAEQA_Enemy_TyrantSoldier",
        "prefix": "ALSAEQA_Enemy_TyrantSoldier",
        "description": "Disciplined armor, shield and polearm silhouette",
    },
    "Enemy_StormHunter": {
        "collection": "ALSAEQA_Enemy_StormHunter",
        "prefix": "ALSAEQA_Enemy_StormHunter",
        "description": "Insulated anti-thunder equipment and ranged weapon silhouette",
    },
    "Enemy_FortressElite": {
        "collection": "ALSAEQA_Enemy_FortressElite",
        "prefix": "ALSAEQA_Enemy_FortressElite",
        "description": "Heavy ornate fortress armor",
    },
    "Enemy_DesertRaider": {
        "collection": "ALSAEQA_Enemy_DesertRaider",
        "prefix": "ALSAEQA_Enemy_DesertRaider",
        "description": "Layered desert cloth, mask and curved weapon silhouette",
    },
    "Enemy_IceGuard": {
        "collection": "ALSAEQA_Enemy_IceGuard",
        "prefix": "ALSAEQA_Enemy_IceGuard",
        "description": "Winter armor and frozen-metal weapon silhouette",
    },
    "Boss": {
        "collection": "ALSAEQA_Boss",
        "prefix": "ALSAEQA_Boss",
        "description": "Unique boss silhouette and equipment language",
    },
    "GiantSnake": {
        "collection": "ALSAEQA_GiantSnake",
        "prefix": "ALSAEQA_GiantSnake",
        "description": "Continuous authored creature mesh; preserve real creature topology",
    },
    "Mount_ThunderBeast": {
        "collection": "ALSAEQA_Mount_ThunderBeast",
        "prefix": "ALSAEQA_Mount_ThunderBeast",
        "description": "Distinct quadruped with horns/crest and thunder-reactive details",
    },
    "Mount_FlyingBeast": {
        "collection": "ALSAEQA_Mount_FlyingBeast",
        "prefix": "ALSAEQA_Mount_FlyingBeast",
        "description": "Winged authored creature with flight-ready rig",
    },
}


def mesh_objects():
    return [o for o in bpy.context.scene.objects if o.type == 'MESH']


def require_real_meshes():
    meshes = mesh_objects()
    if not meshes:
        raise RuntimeError("No imported mesh found. Import a real rigged asset first; this tool never fabricates a body.")
    return meshes


def make_collection(name):
    col = bpy.data.collections.get(name)
    if col is None:
        col = bpy.data.collections.new(name)
        bpy.context.scene.collection.children.link(col)
    return col


def move_to_collection(obj, col):
    for c in list(obj.users_collection):
        c.objects.unlink(obj)
    col.objects.link(obj)


def prepare_variant(role, variant="A"):
    if role not in ROLES:
        raise ValueError(f"Unknown role: {role}")
    meshes = require_real_meshes()
    spec = ROLES[role]
    col = make_collection(spec["collection"])

    for index, obj in enumerate(meshes, 1):
        obj.name = f"{spec['prefix']}_{variant}_{index:02d}"
        obj["ALSAEQA_Role"] = role
        obj["ALSAEQA_Variant"] = variant
        obj["ALSAEQA_ArtRule"] = "REAL_IMPORTED_MESH_ONLY"
        obj["ALSAEQA_DesignBrief"] = spec["description"]
        move_to_collection(obj, col)

    # Preserve armature modifiers and weights. Apply no destructive transforms.
    for obj in meshes:
        if obj.parent and obj.parent.type == 'ARMATURE':
            obj["ALSAEQA_RigPreserved"] = True

    print(f"ALSAEQA: prepared {len(meshes)} imported mesh objects for {role} variant {variant}")
    return col


def save_variant(filepath):
    os.makedirs(os.path.dirname(os.path.abspath(filepath)), exist_ok=True)
    bpy.ops.wm.save_as_mainfile(filepath=filepath)


if __name__ == "__main__":
    # Edit these two values before running, or call prepare_variant() from Blender's console.
    ROLE = "Hero"
    VARIANT = "A"
    prepare_variant(ROLE, VARIANT)
    print("ALSAEQA: customization staging complete. Continue art direction using real modular parts/materials.")
