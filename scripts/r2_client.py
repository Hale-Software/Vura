"""
r2_client.py
------------
Small helper around boto3 for talking to Cloudflare R2 (S3-compatible),
plus the SHA-256 hashing and manifest merge logic shared by the release
scripts.

R2 credentials are read from the environment (see .env.example):
    R2_ACCOUNT_ID          Cloudflare account id
    R2_ACCESS_KEY_ID       R2 API token access key
    R2_SECRET_ACCESS_KEY   R2 API token secret
    R2_BUCKET              bucket name, e.g. "vura-releases"
    R2_PUBLIC_BASE_URL     public URL the bucket is served from,
                            e.g. "https://storage.hale-software.com"
                            (a custom domain / public bucket dev URL)
"""

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

from __future__ import annotations

import hashlib
import json
import os
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Optional
from dotenv import load_dotenv

try:
    import boto3
    from botocore.client import Config
    from botocore.exceptions import ClientError
except ImportError:
    sys.exit(
        "Missing dependency: boto3.\n"
        "Install with:  pip3 install -r requirements.txt"
    )

load_dotenv(".env")

CHUNK_SIZE = 1024 * 1024  # 1 MiB streaming reads, matches Updater.cpp's approach


def sha256_file(path: Path) -> str:
    """Stream-hash a file so multi-GB installers don't need to fit in RAM."""
    hasher = hashlib.sha256()
    with open(path, "rb") as f:
        while chunk := f.read(CHUNK_SIZE):
            hasher.update(chunk)
    return hasher.hexdigest()


@dataclass
class R2Config:
    account_id: str
    access_key_id: str
    secret_access_key: str
    bucket: str
    public_base_url: str

    @classmethod
    def from_env(cls) -> "R2Config":
        required = [
            "R2_ACCOUNT_ID",
            "R2_ACCESS_KEY_ID",
            "R2_SECRET_ACCESS_KEY",
            "R2_BUCKET",
            "R2_PUBLIC_BASE_URL",
        ]
        missing = [k for k in required if not os.environ.get(k)]
        if missing:
            sys.exit(
                "Missing required environment variables: "
                + ", ".join(missing)
                + "\nCopy .env.example to .env and fill in your R2 credentials, "
                  "then `set -a; source .env; set +a` (or use your CI secret store)."
            )
        return cls(
            account_id=os.environ["R2_ACCOUNT_ID"],
            access_key_id=os.environ["R2_ACCESS_KEY_ID"],
            secret_access_key=os.environ["R2_SECRET_ACCESS_KEY"],
            bucket=os.environ["R2_BUCKET"],
            public_base_url=os.environ["R2_PUBLIC_BASE_URL"].rstrip("/"),
        )


class R2Client:
    def __init__(self, cfg: R2Config):
        self.cfg = cfg
        self.s3 = boto3.client(
            "s3",
            endpoint_url=f"https://{cfg.account_id}.r2.cloudflarestorage.com",
            aws_access_key_id=cfg.access_key_id,
            aws_secret_access_key=cfg.secret_access_key,
            config=Config(signature_version="s3v4"),
            region_name="auto",
        )

    def public_url(self, key: str) -> str:
        return f"{self.cfg.public_base_url}/{key.lstrip('/')}"

    def exists(self, key: str) -> bool:
        try:
            self.s3.head_object(Bucket=self.cfg.bucket, Key=key)
            return True
        except ClientError as e:
            if e.response["Error"]["Code"] in ("404", "NoSuchKey", "NotFound"):
                return False
            raise

    def upload_file(self, local_path: Path, key: str, content_type: str = "application/octet-stream",
                     cache_control: str = "public, max-age=31536000, immutable") -> str:
        """Upload a build artifact. Immutable cache headers are safe here because
        each release's key is versioned (channel/version/filename) and never reused."""
        print(f"  -> uploading {local_path.name} to r2://{self.cfg.bucket}/{key}")
        self.s3.upload_file(
            str(local_path),
            self.cfg.bucket,
            key,
            ExtraArgs={"ContentType": content_type, "CacheControl": cache_control},
        )
        return self.public_url(key)

    def upload_bytes(self, data: bytes, key: str, content_type: str = "application/json") -> str:
        # Manifests MUST NOT be cached, or clients keep seeing a stale channel
        # file after a new release is published.
        self.s3.put_object(
            Bucket=self.cfg.bucket,
            Key=key,
            Body=data,
            ContentType=content_type,
            CacheControl="no-cache, no-store, must-revalidate",
        )
        return self.public_url(key)

    def download_json(self, key: str) -> Optional[dict]:
        try:
            obj = self.s3.get_object(Bucket=self.cfg.bucket, Key=key)
            return json.loads(obj["Body"].read())
        except ClientError as e:
            if e.response["Error"]["Code"] in ("404", "NoSuchKey", "NotFound"):
                return None
            raise


PLATFORM_KEYS = ("windows", "mac", "linux")


def merge_manifest(existing: Optional[dict], *, version: str, is_beta: bool,
                    changelog_url: str, platform_updates: dict) -> dict:
    """
    Build the manifest object matching what VuraMainWindow::updateCheckReplyFinished
    parses:
        { "version", "is_beta", "release_date", "changelog_url",
          "platforms": { "windows": {"url","sha256"}, "mac": {...}, "linux": {...} } }

    `platform_updates` is e.g. {"windows": {"url": "...", "sha256": "..."}}.
    If the manifest already describes this exact version (e.g. a second CI job
    publishing the macOS artifact after Windows already published), platform
    entries are merged rather than clobbered so multi-platform releases can be
    published incrementally.
    """
    if existing and existing.get("version") == version:
        manifest = dict(existing)
    else:
        manifest = {
            "version": version,
            "is_beta": is_beta,
            "release_date": datetime.now(timezone.utc).strftime("%Y-%m-%d"),
            "changelog_url": changelog_url,
            "platforms": {},
        }

    manifest["is_beta"] = is_beta
    if changelog_url:
        manifest["changelog_url"] = changelog_url
    manifest.setdefault("platforms", {})

    for plat, info in platform_updates.items():
        if plat not in PLATFORM_KEYS:
            raise ValueError(f"Unknown platform key '{plat}', expected one of {PLATFORM_KEYS}")
        manifest["platforms"][plat] = info

    return manifest
