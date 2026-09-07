"""Import authored ALSAEQA source art downloaded from the Release.

No primitives, placeholders, procedural meshes, or fake final art are created.
"""
import json
import os
import re
import unreal

SOURCE_ROOTS_FILE = os.environ.get("ALSAEQA_ASSET_SOURCE_ROOTS", "")
DEST_ROOT = "/Game/Art"


def clean_name(value):
    value = re.sub(r"[^A-Za-z0-9_]+", "_", value)
    return value.strip("_") or "Imported"


def destination_for(source_path, source_root):
    rel = os.path.relpath(source_path, source_root).replace("\\", "/")
    parts = rel.split("/")
    group = clean_name(parts[0]) if len(parts) > 1 else "SourceAssets"
    return DEST_ROOT + "/" + group


def main():
    if not SOURCE_ROOTS_FILE or not os.path.isfile(SOURCE_ROOTS_FILE):
        raise RuntimeError("ALSAEQA_ASSET_SOURCE_ROOTS is missing or invalid.")

    with open(SOURCE_ROOTS_FILE, "r", encoding="utf-8-sig") as handle:
        source_roots = json.load(handle)
    source_roots = [p for p in source_roots if isinstance(p, str) and os.path.isdir(p)]
    if not source_roots:
        raise RuntimeError("No extracted authored-art source roots were found.")

    supported = {".fbx", ".obj", ".gltf", ".glb"}
    files = []
    for source_root in source_roots:
        for root, _, names in os.walk(source_root):
            for name in names:
                if os.path.splitext(name)[1].lower() in supported:
                    files.append((os.path.join(root, name), source_root))

    unreal.log("ALSAEQA authored source files discovered: %d across %d packs" % (len(files), len(source_roots)))
    if not files:
        raise RuntimeError("No supported authored 3D source files were found.")

    tools = unreal.AssetToolsHelpers.get_asset_tools()
    imported = 0
    failed = 0

    for source, source_root in sorted(files):
        dest = destination_for(source, source_root)
        unreal.EditorAssetLibrary.make_directory(dest)

        task = unreal.AssetImportTask()
        task.filename = source
        task.destination_path = dest
        task.automated = True
        task.replace_existing = False
        task.save = True

        try:
            tools.import_asset_tasks([task])
            count = len(task.imported_object_paths or [])
            if count:
                imported += count
            else:
                unreal.log_warning("No Unreal object produced: " + source)
                failed += 1
        except Exception as exc:
            unreal.log_error("Import failed: %s :: %s" % (source, exc))
            failed += 1

    unreal.log("=== ALSAEQA RELEASE ART IMPORT ===")
    unreal.log("Source files: %d" % len(files))
    unreal.log("Imported objects: %d" % imported)
    unreal.log("Failed files: %d" % failed)

    if imported == 0:
        raise RuntimeError("No authored assets were imported into Unreal.")


if __name__ == "__main__":
    main()
