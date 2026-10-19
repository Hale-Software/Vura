# Vura Release Pipeline (build -> package -> hash -> upload to R2)

This adds a repeatable release pipeline.

## One-time setup

1. Create an R2 bucket, e.g. `vura-releases`, and attach a custom domain
(`vura.hale-software.com`) or note its `r2.dev` public URL.
2. Create an R2 API token scoped to that bucket (Object Read & Write).
3. `pip install -r requirements.txt`
4. `cp .env.example .env` and fill in the R2 values, then
   `set -a; source .env; set +a` (bash/zsh) before running the scripts,
   or export the same variables as CI secrets.

## Creating a stable release

Run the relevant script on each platform (a GitHub Actions matrix works
well here - see below). Each one builds, packages, hashes, and uploads:

```bash
# Windows (PowerShell)
PowerShell.exe -ExecutionPolicy Bypass -File ./build_windows.ps1 -Channel stable -Version v1.0.0

# macOS
./build_macos.sh --channel stable --version v1.0.0

# Linux
./build_linux.sh --channel stable --version v1.0.0
```

## Creating a beta release

Run the relevant script on each platform (a GitHub Actions matrix works
well here - see below). Each one builds, packages, hashes, and uploads:

```bash
# Windows (PowerShell)
PowerShell.exe -ExecutionPolicy Bypass -File ./build_windows.ps1 -Channel beta -Version v1.0.0

# macOS
./build_macos.sh --channel beta --version v1.0.0

# Linux
./build_linux.sh --channel beta --version v1.0.0
```

### Example GitHub Actions matrix

```yaml
name: release
on:
  push:
    tags: ["v*"]

jobs:
  build:
    strategy:
      matrix:
        include:
          - os: windows-latest
            script: pwsh scripts/build_windows.ps1 -Channel stable -Version ${{ github.ref_name }}
          - os: macos-latest
            script: bash scripts/build_macos.sh --channel stable --version ${{ github.ref_name }}
          - os: ubuntu-latest
            script: bash scripts/build_linux.sh --channel stable --version ${{ github.ref_name }}
    runs-on: ${{ matrix.os }}
    env:
      R2_ACCOUNT_ID: ${{ secrets.R2_ACCOUNT_ID }}
      R2_ACCESS_KEY_ID: ${{ secrets.R2_ACCESS_KEY_ID }}
      R2_SECRET_ACCESS_KEY: ${{ secrets.R2_SECRET_ACCESS_KEY }}
      R2_BUCKET: vura-releases
      R2_PUBLIC_BASE_URL: https://vura.hale-software.com
    steps:
      - uses: actions/checkout@v4
      - run: pip install -r scripts/requirements.txt
      - run: ${{ matrix.script }}
```

Push a tag like `v1.4.0` for a stable release; for a beta, tag
`v1.4.0-beta.1` and pass `-Channel beta` / `--channel beta` (or drop the
`-Channel stable` overrides in the matrix and let the tag name decide).

## Manifest schema

### Stable

Served at `https://vura.hale-software.com/stable.json`

```json
{
  "version": "v1.0.0",
  "is_beta": false,
  "release_date": "2026-01-20",
  "changelog_url": "https://vura.hale-software.com/changelog/v1.0.0.md",
  "platforms": {
    "windows": { "url": "https://vura.hale-software.com/releases/stable/v1.0.0/vura-1.0.0-windows.exe", "sha256": "..." },
    "mac":     { "url": "https://vura.hale-software.com/releases/stable/v1.0.0/vura-1.4.0-macos.dmg",    "sha256": "..." },
    "linux":   { "url": "https://vura.hale-software.com/releases/stable/v1.0.0/vura-1.4.0-linux.AppImage", "sha256": "..." }
  }
}
```

### Beta

Served at `https://vura.hale-software.com/beta.json`

```json
{
  "version": "v1.0.0",
  "is_beta": true,
  "release_date": "2026-01-20",
  "changelog_url": "https://vura.hale-software.com/changelog/v1.0.0.md",
  "platforms": {
    "windows": { "url": "https://vura.hale-software.com/releases/beta/v1.0.0/vura-1.0.0-windows.exe", "sha256": "..." },
    "mac":     { "url": "https://vura.hale-software.com/releases/beta/v1.0.0/vura-1.4.0-macos.dmg",    "sha256": "..." },
    "linux":   { "url": "https://vura.hale-software.com/releases/beta/v1.0.0/vura-1.4.0-linux.AppImage", "sha256": "..." }
  }
}
```

## Security notes

- Every artifact is SHA-256 hashed at publish time and re-verified by
  the client in `Updater::verifyFileIntegrity()` before the installer is
  ever executed — keep that check in place; don't let `startDownload()`
  be called with an empty `expectedHash`.
- Manifests are uploaded with `Cache-Control: no-cache` specifically so
  a compromised or rolled-back release can be corrected immediately
  without waiting on CDN cache expiry.
- Consider also publishing a detached `SHA256SUMS.txt` per version for
  anyone verifying manually outside the app.