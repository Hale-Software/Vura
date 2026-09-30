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
stable/beta channel manifest that the in-app updater reads.

Typical usage (one call per platform, e.g. one per CI runner):

    python publish.py \\
        --channel stable \\
        --version v1.0.0 \\
        --platform windows \\
        --installer ../installer/deploy/dist_windows/vura-1.0.0-windows.exe \\
        --dir ../installer/deploy/dist_windows \\
        --changelog-url https://vura.hale-software.com/changelog/v1.0.0.md

    python publish.py \\
        --channel stable \\
        --version v1.0.0 \\
        --platform mac \\
        --installer ../installer/deploy/dist_macos/vura-1.0.0-macos.dmg \\
        --dir ../installer/deploy/dist_macos \\
        --changelog-url https://vura.hale-software.com/changelog/v1.0.0.md

    python publish.py \\
        --channel stable \\
        --version v1.0.0 \\
        --platform linux \\
        --installer ../installer/deploy/dist_linux/vura-1.0.0-linux.AppImage \\
        --dir ../installer/deploy/dist_linux \\
        --changelog-url https://vura.hale-software.com/changelog/v1.0.0.md

Multiple platforms for the same version/channel can be published in any
order or from separate CI jobs — the manifest is merged, not overwritten,
as long as --version matches.
"""

from __future__ import annotations

import os
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


def process_file(path: Path, name: str) -> dir:
    data = {}
    data["path"] = path
    data["name"] = name
    data["hash"] = sha256_file(path)
    data["size"] = path.stat().st_size
    return data


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--channel", required=True, choices=["stable", "beta"], help="Release channel/branch - matches stable.json / beta.json")
    parser.add_argument("--version", required=True, help="Release version, e.g. v1.0.0 or 1.0.0-beta.1")
    parser.add_argument("--platform", required=True, choices=["windows", "mac", "linux"])
    parser.add_argument("--installer", required=True, type=Path, help="Path to the built installer/artifact")
    parser.add_argument("--dir", required=True, type=str, help="Path to the directory to publish")
    parser.add_argument("--changelog-url", default="", help="Optional changelog URL for this release")
    parser.add_argument("--force", action="store_true", help="Re-upload even if an object already exists at that key")
    parser.add_argument("--dry-run", action="store_true", help="Do everything except actually contact R2; prints what would happen")
    args = parser.parse_args()

    files = []
    file_list = []

    for root, dirs, filenames in os.walk(args.dir):
        for filename in filenames:
            data = {}
            path = os.path.join(root, filename)
            data["path"] = path
            name = path.replace(args.dir + "\\", "")
            data["name"] = name.replace("\\", "/")
            files.append(data)

    for file in files:
        file_list.append(process_file(Path(file["path"]), file["name"]))

    version = normalize_version(args.version)
    is_beta = args.channel == "beta" or "beta" in version.lower()
    installer = {}
    installer["name"] = Path(args.installer).name
    installer["hash"] = sha256_file(args.installer)
    installer["size"] = args.installer.stat().st_size

    cfg = R2Config.from_env()
    client = R2Client(cfg)

    for f in file_list:
        if args.dry_run:
            del f["path"]
            continue

        key = f"releases/{args.channel}/{version}/{f['name']}"
        content_type = guess_content_type(Path(f["path"]))

        if not args.force and client.exists(key):
            print(f"  note: {key} already exists in R2 — skipping upload (use --force to overwrite)")
            public_url = client.public_url(key)
        else:
            public_url = client.upload_file(f["path"], key, content_type=content_type)

        del f["path"]

    if not args.dry_run:
        key = f"installers/{installer['name']}"
        content_type = guess_content_type(args.installer)

        if not args.force and client.exists(key):
            print(f"  note: {key} already exists in R2 - skipping upload (use --force to overwrite)")
            public_url = client.public_url(key)
        else:
            public_url = client.upload_file(args.installer, key, content_type=content_type)

    manifest_key = f"{args.channel}.json"
    existing = client.download_json(manifest_key)

    manifest = merge_manifest(
        existing,
        version=version,
        is_beta=is_beta,
        changelog_url=args.changelog_url,
        installer=installer,
        platform_updates={args.platform: file_list},
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
