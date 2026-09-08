#!/usr/bin/env python3
import hashlib
import json
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / "build" / "source_assets"
CONVERTED = ROOT / "mobile" / "assets" / "converted"
MANIFEST = ROOT / "mobile" / "assets" / "asset_manifest.json"
SOURCE_REPO = "ali12hhh-oss/alsaeqa"
RELEASE = "assets-v1"
BASE_URL = f"https://github.com/{SOURCE_REPO}/releases/download/{RELEASE}"


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def download(url: str, target: Path) -> None:
    target.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(
        ["curl", "-L", "--fail", "--retry", "5", "--retry-all-errors", url, "-o", str(target)],
        check=True,
    )


def extract_nested(zip_path: Path, destination: Path) -> None:
    """Extract an archive and any ZIP files contained in it."""
    destination.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path) as z:
        z.extractall(destination)

    processed = set()
    while True:
        nested_archives = [
            p for p in destination.rglob("*.zip")
            if p.is_file() and p != zip_path and p not in processed
        ]
        if not nested_archives:
            break

        changed = False
        for nested in nested_archives:
            processed.add(nested)
            out = nested.with_suffix("")
            out.mkdir(parents=True, exist_ok=True)
            try:
                with zipfile.ZipFile(nested) as z:
                    z.extractall(out)
                nested.unlink()
                changed = True
            except zipfile.BadZipFile:
                # Keep non-ZIP files that merely use a .zip suffix for inventory.
                continue

        if not changed:
            break


def main() -> int:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    BUILD.mkdir(parents=True, exist_ok=True)
    CONVERTED.mkdir(parents=True, exist_ok=True)
    inventory = []

    for pack in data["packs"]:
        name = pack["file"]
        archive = BUILD / "_downloads" / name
        if not archive.exists():
            download(f"{BASE_URL}/{name}", archive)
        actual = sha256(archive)
        if actual.lower() != pack["sha256"].lower():
            raise RuntimeError(f"SHA-256 mismatch for {name}: {actual}")
        extract_root = BUILD / Path(name).stem
        if not extract_root.exists():
            extract_nested(archive, extract_root)

    source_exts = {
        ".fbx", ".obj", ".dae", ".gltf", ".glb", ".blend",
        ".png", ".jpg", ".jpeg", ".webp", ".tga",
        ".wav", ".ogg", ".mp3", ".json",
    }
    for src in sorted(BUILD.rglob("*")):
        if not src.is_file() or src.parent.name == "_downloads":
            continue
        if src.suffix.lower() not in source_exts:
            continue
        rel = src.relative_to(BUILD).as_posix()
        item = {
            "source": rel,
            "extension": src.suffix.lower(),
            "size": src.stat().st_size,
            "sha256": sha256(src),
        }
        inventory.append(item)
        if src.suffix.lower() in {
            ".gltf", ".glb", ".png", ".jpg", ".jpeg", ".webp", ".tga",
            ".wav", ".ogg", ".mp3", ".json",
        }:
            dst = CONVERTED / rel
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(src, dst)

    (CONVERTED / "asset_inventory.json").write_text(
        json.dumps(
            {"source": "ALSAEQA releases", "release": RELEASE, "assets": inventory},
            ensure_ascii=False,
            indent=2,
        ),
        encoding="utf-8",
    )
    print(f"Indexed {len(inventory)} real source assets")
    return 0


if __name__ == "__main__":
    sys.exit(main())
