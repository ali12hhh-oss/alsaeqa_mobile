import hashlib
import json
import os
import sys
import urllib.request

with open("Tools/Assets/assets-manifest.json", encoding="utf-8") as manifest_file:
    manifest = json.load(manifest_file)

release_tag = manifest["release_tag"]
repository = os.environ.get("GITHUB_REPOSITORY", "ali12hhh-oss/alsaeqa_mobile")
repositories = [repository, "ali12hhh-oss/alsaeqa"]
os.makedirs("Downloads", exist_ok=True)

for asset in manifest["assets"]:
    filename = asset["file"]
    expected_sha256 = asset["sha256"]
    destination = os.path.join("Downloads", filename)
    downloaded = False

    for candidate in repositories:
        url = f"https://github.com/{candidate}/releases/download/{release_tag}/{filename}"
        try:
            print(f"Trying {url}")
            urllib.request.urlretrieve(url, destination)
            downloaded = True
            break
        except Exception as error:
            print(f"  failed: {error}")

    if not downloaded:
        sys.exit(f"Could not download {filename} from any candidate repo")

    digest = hashlib.sha256()
    with open(destination, "rb") as downloaded_file:
        for chunk in iter(lambda: downloaded_file.read(1 << 20), b""):
            digest.update(chunk)

    actual_sha256 = digest.hexdigest()
    if actual_sha256 != expected_sha256:
        sys.exit(
            f"Checksum mismatch for {filename}: expected {expected_sha256}, got {actual_sha256}"
        )
    print(f"OK  {filename}  sha256={actual_sha256}")
