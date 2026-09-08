#!/usr/bin/env python3
"""Inspect ALSAEQA Release archives, including ZIPs nested at any depth.

The report is deliberately separate from the runtime asset conversion pipeline:
we inspect archive structure without committing the large source archives.
"""
from __future__ import annotations

import hashlib
import io
import json
import re
import shutil
import tempfile
import zipfile
from collections import Counter
from pathlib import Path
from urllib.request import Request, urlopen

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / "build" / "asset_inspection"
DOWNLOADS = ROOT / "build" / "source_assets" / "_downloads"
MANIFEST = ROOT / "mobile" / "assets" / "asset_manifest.json"
SOURCE_REPO = "ali12hhh-oss/alsaeqa"
RELEASE = "assets-v1"
BASE_URL = f"https://github.com/{SOURCE_REPO}/releases/download/{RELEASE}"


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def download(url: str, target: Path) -> None:
    target.parent.mkdir(parents=True, exist_ok=True)
    request = Request(url, headers={"User-Agent": "ALSAEQA-Mobile-Asset-Inspector/1.0"})
    with urlopen(request) as response, target.open("wb") as output:
        shutil.copyfileobj(response, output, length=1024 * 1024)


def is_zip_name(name: str) -> bool:
    return name.lower().endswith(".zip")


def safe_member_name(name: str) -> bool:
    # Inspection must not extract anything, but flag unsafe paths explicitly.
    normalized = name.replace("\\", "/")
    parts = [p for p in normalized.split("/") if p]
    return not normalized.startswith("/") and ".." not in parts


def classify(path: str) -> str:
    value = path.lower()
    rules = {
        "monster_or_creature": ("monster", "dragon", "bee", "creature", "beast", "snake"),
        "character": ("character", "hero", "woman", "female", "male", "man", "person"),
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


def inspect_zip(path: Path, root_label: str, report: dict, depth: int = 0) -> None:
    with zipfile.ZipFile(path) as archive:
        entries = archive.infolist()
        archive_record = {
            "archive": root_label,
            "depth": depth,
            "entry_count": len(entries),
            "compressed_bytes": sum(item.compress_size for item in entries),
            "uncompressed_bytes": sum(item.file_size for item in entries),
            "nested_archives": [],
            "unsafe_paths": [],
            "suffix_counts": Counter(),
            "categories": Counter(),
        }

        for item in entries:
            if item.is_dir():
                continue
            name = item.filename
            suffix = Path(name).suffix.lower() or "[none]"
            archive_record["suffix_counts"][suffix] += 1
            archive_record["categories"][classify(name)] += 1
            if not safe_member_name(name):
                archive_record["unsafe_paths"].append(name)

            if is_zip_name(name):
                archive_record["nested_archives"].append({
                    "path": name,
                    "compressed_bytes": item.compress_size,
                    "uncompressed_bytes": item.file_size,
                })
                # Recursively inspect the nested archive without extracting the
                # complete parent archive to disk. Streaming to a temp file keeps
                # memory bounded even for large nested archives.
                with archive.open(item, "r") as source, tempfile.NamedTemporaryFile(suffix=".zip") as tmp:
                    shutil.copyfileobj(source, tmp, length=1024 * 1024)
                    tmp.flush()
                    try:
                        inspect_zip(Path(tmp.name), f"{root_label}!/{name}", report, depth + 1)
                    except zipfile.BadZipFile:
                        archive_record.setdefault("zip_suffix_non_archives", []).append(name)

        archive_record["suffix_counts"] = dict(sorted(archive_record["suffix_counts"].items()))
        archive_record["categories"] = dict(sorted(archive_record["categories"].items()))
        report["archives"].append(archive_record)


def main() -> int:
    BUILD.mkdir(parents=True, exist_ok=True)
    DOWNLOADS.mkdir(parents=True, exist_ok=True)
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    report = {
        "source_repository": SOURCE_REPO,
        "release": RELEASE,
        "archives": [],
        "summary": {},
    }

    for pack in manifest["packs"]:
        target = DOWNLOADS / pack["file"]
        if not target.exists():
            print(f"Downloading {pack['file']} ...")
            download(f"{BASE_URL}/{pack['file']}", target)
        actual = sha256(target)
        if actual.lower() != pack["sha256"].lower():
            raise RuntimeError(f"SHA-256 mismatch for {pack['file']}: {actual}")
        print(f"Verified {pack['file']} ({target.stat().st_size:,} bytes)")
        inspect_zip(target, pack["file"], report)

    total_entries = sum(item["entry_count"] for item in report["archives"])
    max_depth = max((item["depth"] for item in report["archives"]), default=0)
    nested_count = sum(len(item["nested_archives"]) for item in report["archives"])
    report["summary"] = {
        "archive_count_including_nested": len(report["archives"]),
        "total_entries_across_archives": total_entries,
        "nested_archive_count": nested_count,
        "maximum_nested_depth": max_depth,
        "unsafe_paths_count": sum(len(item["unsafe_paths"]) for item in report["archives"]),
    }

    json_path = BUILD / "release_archive_inventory.json"
    json_path.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")

    lines = [
        "ALSAEQA REAL ASSETS — RELEASE ARCHIVE INSPECTION",
        f"Repository: {SOURCE_REPO}",
        f"Release: {RELEASE}",
        "",
        f"Archives inspected (including nested): {len(report['archives'])}",
        f"Nested ZIP files discovered: {nested_count}",
        f"Maximum nested ZIP depth: {max_depth}",
        f"Unsafe archive paths: {report['summary']['unsafe_paths_count']}",
        "",
    ]
    for item in report["archives"]:
        lines.extend([
            f"[{item['depth']}] {item['archive']}",
            f"  entries: {item['entry_count']}",
            f"  uncompressed: {item['uncompressed_bytes']:,} bytes",
            f"  nested ZIPs: {len(item['nested_archives'])}",
            f"  categories: {json.dumps(item['categories'], ensure_ascii=False, sort_keys=True)}",
            f"  extensions: {json.dumps(item['suffix_counts'], ensure_ascii=False, sort_keys=True)}",
        ])
        if item.get("unsafe_paths"):
            lines.append(f"  UNSAFE PATHS: {len(item['unsafe_paths'])}")
        if item.get("zip_suffix_non_archives"):
            lines.append(f"  .ZIP suffix but invalid archive: {len(item['zip_suffix_non_archives'])}")
        lines.append("")

    text_path = BUILD / "release_archive_inventory.txt"
    text_path.write_text("\n".join(lines), encoding="utf-8")
    print("Wrote:", json_path)
    print("Wrote:", text_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
