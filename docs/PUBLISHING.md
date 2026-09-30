# Publishing a Release

## One-time setup

1. Create an R2 bucket, e.g. `vura-releases`, and attach a custom domain
   (`storage.hale-tech.net`) or note its `r2.dev` public URL.
2. Create an R2 API token scoped to that bucket (Object Read & Write).
3. `pip install -r requirements.txt`
4. `cp .env.example .env` and fill in the R2 values, then
   `set -a; source .env; set +a` (bash/zsh) before running the scripts,
   or export the same variables as CI secrets.

## Cutting a release

### GitHub Release

Push a tag like `1.4.0` for a stable release; for a beta, tag `1.4.0-beta.1`
and pass `-Channel beta` / `--channel beta` (or drop the `-Channel stable`
overrides in the matrix and let the tag name decide).

### Manual Release

Run the relevant script on each platform. Each one builds, packages, hashes, and uploads:

```bash
# Windows (PowerShell)
./build_windows.ps1 -Channel stable -Version v1.4.0

# macOS
./build_macos.sh --channel stable --version v1.4.0

# Linux
./build_linux.sh --channel stable --version v1.4.0
```

Use `--channel beta` for pre-releases. The three platform builds can run
in any order, from different machines/CI jobs — `publish.py` merges
each platform's `{url, sha256}` into the same `stable.json`/`beta.json`
manifest object instead of overwriting it, keyed by matching `version`.

Under the hood each build script ends by calling:

```bash
python publish.py --channel stable --version v1.4.0 \
    --platform windows --file path/to/installer.exe
```

which:
1. SHA-256 hashes the artifact (streamed, so multi-GB installers are fine)
2. uploads it to `r2://<bucket>/releases/<channel>/<version>/<filename>`
   with a long-lived immutable cache header (the key is versioned, so
   that's always safe)
3. downloads the current `<channel>.json`, merges in this platform's
   `{url, sha256}`, and re-uploads it with `Cache-Control: no-cache` so
   clients never see a stale manifest

## Manifest schema

This is exactly what `VuraMainWindow::updateCheckReplyFinished` already
parses — the scripts don't invent a new format:

```json
{
  "version": "v1.4.0",
  "is_beta": false,
  "release_date": "2026-08-20",
  "changelog_url": "https://storage.hale-tech.net/changelog/v1.4.0.md",
  "platforms": {
    "windows": { "url": "https://storage.hale-tech.net/releases/stable/v1.4.0/vura-1.4.0-windows.exe", "sha256": "..." },
    "mac":     { "url": "https://storage.hale-tech.net/releases/stable/v1.4.0/vura-1.4.0-macos.dmg",    "sha256": "..." },
    "linux":   { "url": "https://storage.hale-tech.net/releases/stable/v1.4.0/vura-1.4.0-linux.AppImage", "sha256": "..." }
  }
}
```

Served at `https://storage.hale-tech.net/stable.json` and
`https://storage.hale-tech.net/beta.json`.
