#!/usr/bin/env python3
import hashlib
import json
import shutil
import subprocess
import sys
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
    subprocess.run(
        ["curl", "-L", "--fail", "--retry", "5", "--retry-all-errors", url, "-o", str(target)],
        check=True,
    )


def release_digests() -> dict[str, str]:
    request = urllib.request.Request(
        RELEASE_API_URL,
        headers={"Accept": "application/vnd.github+json", "User-Agent": "alsaeqa-mobile-assets"},
    )
    with urllib.request.urlopen(request, timeout=30) as response:
        payload = json.load(response)

    digests = {}
    for asset in payload.get("assets", []):
        name = asset.get("name")
        digest = asset.get("digest")
        if name and isinstance(digest, str) and digest.startswith("sha256:"):
            digests[name] = digest.removeprefix("sha256:").lower()
    return digests


def extract_nested(zip_path: Path, destination: Path) -> None:
    """Extract an archive and ZIP files contained at any depth."""
    destination.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path) as z:
        z.extractall(destination)

    processed = set()
    while True:
        nested_archives = [
            p for p in destination.rglob("*.zip")
            if p.is_file() and p not in processed
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
                continue

        if not changed:
            break


# Files that can be referenced by a glTF/GLB or are otherwise required by
# the original asset package. In particular, .bin is mandatory for many
# Standard glTF files and must remain beside its .gltf file.
source_exts = {
    ".fbx", ".obj", ".dae", ".gltf", ".glb", ".blend",
    ".bin", ".mtl",
    ".png", ".jpg", ".jpeg", ".webp", ".tga",
    ".wav", ".ogg", ".mp3", ".json",
}
runtime_copy_exts = {
    ".gltf", ".glb", ".bin", ".mtl",
    ".png", ".jpg", ".jpeg", ".webp", ".tga",
    ".wav", ".ogg", ".mp3", ".json",
}


def main() -> int:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    BUILD.mkdir(parents=True, exist_ok=True)
    CONVERTED.mkdir(parents=True, exist_ok=True)

    remote_digests = release_digests()
    inventory = []

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
                raise RuntimeError(
                    f"SHA-256 mismatch for {name}: downloaded={actual}, published={published}"
                )
            if expected != published:
                print(
                    f"WARNING: manifest SHA-256 for {name} is stale; "
                    f"using published release digest {published}"
                )
        elif actual != expected:
            raise RuntimeError(
                f"SHA-256 mismatch for {name}: downloaded={actual}, manifest={expected}"
            )

        extract_root = BUILD / Path(name).stem
        if not extract_root.exists():
            extract_nested(archive, extract_root)

    for src in sorted(BUILD.rglob("*")):
        if not src.is_file() or "_downloads" in src.parts:
            continue
        suffix = src.suffix.lower()
        if suffix not in source_exts:
            continue

        rel = src.relative_to(BUILD).as_posix()
        item = {
            "source": rel,
            "extension": suffix,
            "size": src.stat().st_size,
            "sha256": sha256(src),
        }
        inventory.append(item)

        # Preserve glTF sidecars (.bin/.mtl) in exactly the same relative
        # location. This prevents Godot from importing a .gltf without its
        # referenced binary buffer/material files.
        if suffix in runtime_copy_exts:
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
        preview = ", ".join(missing_bins[:10])
        raise RuntimeError(
            f"Missing glTF sidecar files after asset sync: {len(missing_bins)} "
            f"(examples: {preview})"
        )

    (CONVERTED / "asset_inventory.json").write_text(
        json.dumps(
            {"source": "ALSAEQA releases", "release": RELEASE, "assets": inventory},
            ensure_ascii=False,
            indent=2,
        ),
        encoding="utf-8",
    )
    print(f"Indexed {len(inventory)} real source assets")
    print("Verified glTF sidecar files (.bin/.mtl) are present")
    return 0


if __name__ == "__main__":
    sys.exit(main())
