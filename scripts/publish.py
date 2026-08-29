#!/usr/bin/env python3
#      Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>
#
#      This program is free software: you can redistribute it and/or modify
#      it under the terms of the GNU General Public License as published by
#      the Free Software Foundation, either version 3 of the License, or
#      (at your option) any later version.
#
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#      GNU General Public License for more details.
#
#      You should have received a copy of the GNU General Public License
#      along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
#      This program is free software: you can redistribute it and/or modify
#      it under the terms of the GNU General Public License as published by
#      the Free Software Foundation, either version 3 of the License, or
#      (at your option) any later version.
#
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#      GNU General Public License for more details.
#
#      You should have received a copy of the GNU General Public License
#      along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

"""
publish.py — upload built Vura installers to Cloudflare R2 and update the
stable/beta channel manifest that the in-app updater (UpdateChecker /
VuraMainWindow::actionHelpCheckForUpdates) reads.

Typical usage (one call per platform, e.g. one per CI runner):

    python publish.py \\
        --channel stable \\
        --version v1.4.0 \\
        --platform windows \\
        --file dist/vura-1.4.0-windows.exe \\
        --changelog-url https://storage.hale-software.com/changelog/v1.4.0.md

    python publish.py --channel stable --version v1.4.0 --platform mac \\
        --file dist/vura-1.4.0-macos.dmg

    python publish.py --channel stable --version v1.4.0 --platform linux \\
        --file dist/vura-1.4.0-linux.AppImage

Each call:
  1. sha256-hashes the artifact
  2. uploads it to  r2://<bucket>/releases/<channel>/<version>/<filename>
  3. downloads the current <channel>.json manifest (if any)
  4. merges in this platform's {url, sha256} and re-uploads the manifest
     to r2://<bucket>/<channel>.json  (served at <public-base>/stable.json
     or <public-base>/beta.json, no-cache so clients see it immediately)

Multiple platforms for the same version/channel can be published in any
order or from separate CI jobs — the manifest is merged, not overwritten,
as long as --version matches.
"""

from __future__ import annotations

import argparse
import mimetypes
import sys
from pathlib import Path

from r2_client import R2Client, R2Config, merge_manifest, sha256_file


CONTENT_TYPES = {
    ".exe": "application/x-msdownload",
    ".dmg": "application/x-apple-diskimage",
    ".pkg": "application/x-newton-compatible-pkg",
    ".appimage": "application/x-executable",
    ".deb": "application/vnd.debian.binary-package",
    ".rpm": "application/x-rpm",
    ".tar.gz": "application/gzip",
    ".zip": "application/zip",
}


def guess_content_type(path: Path) -> str:
    suffix = "".join(path.suffixes[-2:]) if path.suffixes[-2:] == [".tar", ".gz"] else path.suffix
    return CONTENT_TYPES.get(suffix.lower()) or mimetypes.guess_type(str(path))[0] or "application/octet-stream"


def normalize_version(v: str) -> str:
    """The client compares this string exactly against VURA_VERSION_STRING,
    which is always 'v' + canonical version (see config_h.in). Normalize so
    a caller passing '1.4.0' or 'v1.4.0' both work."""
    v = v.strip()
    return v if v.startswith("v") else f"v{v}"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--channel", required=True, choices=["stable", "beta"], help="Release channel/branch — matches stable.json / beta.json")
    parser.add_argument("--version", required=True, help="Release version, e.g. v1.4.0 or 1.4.0-beta.1")
    parser.add_argument("--platform", required=True, choices=["windows", "mac", "linux"])
    parser.add_argument("--file", required=False, type=Path, help="Path to the built installer/artifact")
    parser.add_argument("--dir", required=True, type=Path, help="Path to the directory to publish")
    parser.add_argument("--changelog-url", default="", help="Optional changelog URL for this release")
    parser.add_argument("--force", action="store_true", help="Re-upload even if an object already exists at that key")
    parser.add_argument("--dry-run", action="store_true", help="Do everything except actually contact R2; prints what would happen")
    args = parser.parse_args()

    if not args.dir.is_dir():
        sys.exit(f"error: no such directory: {args.dir}")

    file_list = []

    version = normalize_version(args.version)
    is_beta = args.channel == "beta" or "beta" in version.lower()

    print(f"Hashing {args.file} ...")
    digest = sha256_file(args.file)
    print(f"  sha256: {digest}")

    key = f"releases/{args.channel}/{version}/{args.file.name}"
    content_type = guess_content_type(args.file)

    if args.dry_run:
        print(f"[dry-run] would upload to key: {key} (content-type: {content_type})")
        print(f"[dry-run] would merge platform '{args.platform}' into {args.channel}.json")
        return 0

    cfg = R2Config.from_env()
    client = R2Client(cfg)

    if not args.force and client.exists(key):
        print(f"  note: {key} already exists in R2 — skipping upload (use --force to overwrite)")
        public_url = client.public_url(key)
    else:
        public_url = client.upload_file(args.file, key, content_type=content_type)

    print(f"  public url: {public_url}")

    manifest_key = f"{args.channel}.json"
    existing = client.download_json(manifest_key)

    manifest = merge_manifest(
        existing,
        version=version,
        is_beta=is_beta,
        changelog_url=args.changelog_url,
        platform_updates={args.platform: {"url": public_url, "sha256": digest}},
    )

    import json
    manifest_url = client.upload_bytes(
        json.dumps(manifest, indent=2).encode("utf-8"), manifest_key
    )

    print(f"\nUpdated manifest: {manifest_url}")
    print(json.dumps(manifest, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
