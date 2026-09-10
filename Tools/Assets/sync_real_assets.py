#!/usr/bin/env python3
import hashlib
import json
import os
import shutil
import subprocess
import sys
import urllib.error
import urllib.request
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / "build" / "source_assets"
CONVERTED = ROOT / "mobile" / "assets" / "converted"
MANIFEST = ROOT / "mobile" / "assets" / "asset_manifest.json"
SOURCE_REPO = "ali12hhh-oss/alsaeqa"
RELEASE = "assets-v1"
BASE_URL = f"https://github.com/{SOURCE_REPO}/releases/download/{RELEASE}"
RELEASE_API_URL = f"https://api.github.com/repos/{SOURCE_REPO}/releases/tags/{RELEASE}"


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def download(url: str, target: Path) -> None:
    target.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(["curl", "-L", "--fail", "--retry", "5", "--retry-all-errors", url, "-o", str(target)], check=True)


def release_digests() -> dict[str, str]:
    headers = {
        "Accept": "application/vnd.github+json",
        "User-Agent": "alsaeqa-mobile-assets",
        "X-GitHub-Api-Version": "2022-11-28",
    }
    token = os.environ.get("GITHUB_TOKEN") or os.environ.get("GH_TOKEN")
    if token:
        headers["Authorization"] = f"Bearer {token}"
    request = urllib.request.Request(RELEASE_API_URL, headers=headers)
    try:
        with urllib.request.urlopen(request, timeout=30) as response:
            payload = json.load(response)
    except urllib.error.HTTPError as exc:
        if exc.code in (403, 429):
            print(f"WARNING: GitHub release API rate-limited (HTTP {exc.code}); using manifest SHA-256 values for verification")
            return {}
        raise
    except urllib.error.URLError as exc:
        print(f"WARNING: GitHub release API unavailable ({exc.reason}); using manifest SHA-256 values for verification")
        return {}
    return {
        a["name"]: a["digest"].removeprefix("sha256:").lower()
        for a in payload.get("assets", [])
        if a.get("name") and isinstance(a.get("digest"), str) and a["digest"].startswith("sha256:")
    }


def extract_nested(zip_path: Path, destination: Path) -> None:
    destination.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path) as z:
        z.extractall(destination)
    processed = set()
    while True:
        nested_archives = [p for p in destination.rglob("*.zip") if p.is_file() and p not in processed]
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
                continue
        if not changed:
            break


source_exts = {".fbx", ".obj", ".dae", ".gltf", ".glb", ".blend", ".bin", ".mtl", ".png", ".jpg", ".jpeg", ".webp", ".tga", ".wav", ".ogg", ".mp3", ".json"}
runtime_copy_exts = {".gltf", ".glb", ".bin", ".mtl", ".png", ".jpg", ".jpeg", ".webp", ".tga", ".wav", ".ogg", ".mp3", ".json"}


def classify(path: str) -> str:
    value = path.lower()
    rules = {
        "monster_or_creature": ("monster", "dragon", "bee", "creature", "beast", "snake"),
        "character": ("character", "hero", "woman", "female", "male", "man", "person", "worker", "civilian", "villager", "guard", "soldier", "warrior", "knight"),
        "environment_ruins_caves": ("ruin", "cave", "dungeon", "wall", "rock", "cliff"),
        "environment": ("environment", "nature", "tree", "grass", "village", "farm", "prop"),
        "animation": ("animation", "anim", "walk", "run", "attack", "idle"),
        "weapon": ("weapon", "sword", "axe", "spear", "bow", "chain"),
        "audio": ("sound", "audio", "voice", "music"),
        "texture_material": ("texture", "material", "normal", "roughness"),
    }
    for category, tokens in rules.items():
        if any(token in value for token in tokens):
            return category
    return "other"


def main() -> int:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    BUILD.mkdir(parents=True, exist_ok=True)
    CONVERTED.mkdir(parents=True, exist_ok=True)
    remote_digests = release_digests()
    inventory = []
    category_counts: dict[str, int] = {}

    for pack in data["packs"]:
        name = pack["file"]
        archive = BUILD / "_downloads" / name
        if not archive.exists():
            download(f"{BASE_URL}/{name}", archive)
        actual = sha256(archive).strip().lower()
        expected = str(pack.get("sha256", "")).strip().lower()
        published = remote_digests.get(name)
        if published:
            if actual != published:
                raise RuntimeError(f"SHA-256 mismatch for {name}: downloaded={actual}, published={published}")
            if expected != published:
                print(f"WARNING: manifest SHA-256 for {name} is stale; using published release digest {published}")
        elif actual != expected:
            raise RuntimeError(f"SHA-256 mismatch for {name}: downloaded={actual}, manifest={expected}")
        extract_root = BUILD / Path(name).stem
        if not extract_root.exists():
            extract_nested(archive, extract_root)

    for src in sorted(BUILD.rglob("*")):
        if not src.is_file() or "_downloads" in src.parts or src.suffix.lower() not in source_exts:
            continue
        rel = src.relative_to(BUILD).as_posix()
        category = classify(rel)
        category_counts[category] = category_counts.get(category, 0) + 1
        inventory.append({"source": rel, "extension": src.suffix.lower(), "size": src.stat().st_size, "sha256": sha256(src), "category": category})
        if src.suffix.lower() in runtime_copy_exts:
            dst = CONVERTED / rel
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(src, dst)

    missing_bins = []
    for gltf in CONVERTED.rglob("*.gltf"):
        try:
            document = json.loads(gltf.read_text(encoding="utf-8"))
        except (OSError, UnicodeDecodeError, json.JSONDecodeError):
            continue
        for buffer in document.get("buffers", []):
            uri = buffer.get("uri")
            if not uri or uri.startswith("data:"):
                continue
            sidecar = (gltf.parent / uri).resolve()
            if not sidecar.is_file():
                missing_bins.append(str(sidecar.relative_to(CONVERTED)))
    if missing_bins:
        raise RuntimeError(f"Missing glTF sidecar files after asset sync: {len(missing_bins)} (examples: {', '.join(missing_bins[:10])})")

    required = ("character", "monster_or_creature", "environment", "environment_ruins_caves", "weapon", "animation")
    missing_categories = [name for name in required if category_counts.get(name, 0) == 0]
    if missing_categories:
        raise RuntimeError(f"Real asset release is missing required logical categories: {', '.join(missing_categories)}")

    (CONVERTED / "asset_inventory.json").write_text(json.dumps({"source": "ALSAEQA releases", "release": RELEASE, "assets": inventory, "category_counts": category_counts, "required_categories": list(required)}, ensure_ascii=False, indent=2), encoding="utf-8")
    print(f"Indexed {len(inventory)} real source assets")
    print("Verified glTF sidecar files (.bin/.mtl) are present")
    print("Verified required real-asset categories: " + ", ".join(required))
    return 0


if __name__ == "__main__":
    sys.exit(main())
