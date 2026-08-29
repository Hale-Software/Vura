<#
.SYNOPSIS
  Build a Vura release for Windows, package it with NSIS, hash it, and
  upload it to Cloudflare R2 via publish.py.

.DESCRIPTION
  This replaces / extends the old deploy_win.bat: it still does the
  windeployqt + MinGW/FFmpeg DLL staging + makensis packaging, then adds
  SHA-256 hashing and the R2 upload + manifest update.

.PARAMETER Channel
  "stable" or "beta". Defaults to "beta" for anything that isn't an
  annotated tag build, so accidental pushes never land on stable.

.PARAMETER Version
  Version string, e.g. v1.4.0 or v1.4.0-beta.1. Defaults to `git describe`.

.EXAMPLE
  ./build_windows.ps1 -Channel stable -Version v1.4.0
#>

param(
    [ValidateSet("stable", "beta")]
    [string]$Channel = "beta",

    [string]$Version = "",

    [switch]$SkipUpload
)

$ErrorActionPreference = "Stop"

# ---- paths matching your local environment -------------------------------
$QT_BIN      = "C:\Qt\6.11.1\mingw_64\bin"
$MINGW_BIN   = "C:\Qt\Tools\mingw1310_64\bin"
$FFMPEG_BIN  = "C:\Users\halea\bin\ffmpeg-8.1.1-full_build-shared\bin"
$NSIS_PATH   = "C:\Program Files (x86)\NSIS\makensis.exe"

$RepoRoot     = Resolve-Path "$PSScriptRoot\.."
$BuildDir     = Join-Path $RepoRoot "build\release"
$InstallerDir = Join-Path $RepoRoot "installer"
$DeployDir    = Join-Path $InstallerDir "deploy\dist_windows"

if (-not $Version) {
    $Version = (git -C $RepoRoot describe --tags --abbrev=0).Trim()
}
if (-not $Version.StartsWith("v")) { $Version = "v$Version" }

Write-Host "== Building Vura $Version ($Channel) for Windows ==" -ForegroundColor Cyan

# ---- 1. Configure + build (Release) ---------------------------------------
cmake -S $RepoRoot -B $BuildDir -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) { throw "cmake configure failed" }

cmake --build $BuildDir --config Release --parallel
if ($LASTEXITCODE -ne 0) { throw "cmake build failed" }

# ---- 2. Stage the deploy directory -----------------------------------------
if (Test-Path $DeployDir) { Remove-Item -Recurse -Force $DeployDir }
New-Item -ItemType Directory -Path $DeployDir | Out-Null

Copy-Item "$BuildDir\src\vura.exe" $DeployDir
Copy-Item "$RepoRoot\assets" "$DeployDir\assets" -Recurse

& "$QT_BIN\windeployqt.exe" --dir $DeployDir "$DeployDir\vura.exe"

Copy-Item "$MINGW_BIN\libgcc_s_seh-1.dll"   $DeployDir
Copy-Item "$MINGW_BIN\libstdc++-6.dll"      $DeployDir
Copy-Item "$MINGW_BIN\libwinpthread-1.dll"  $DeployDir

Copy-Item "$FFMPEG_BIN\avcodec-*.dll"  $DeployDir
Copy-Item "$FFMPEG_BIN\avformat-*.dll" $DeployDir
Copy-Item "$FFMPEG_BIN\avutil-*.dll"   $DeployDir
Copy-Item "$FFMPEG_BIN\swscale-*.dll"  $DeployDir

Write-Host "Deployment package staged in $DeployDir"

# ---- 3. Package with NSIS --------------------------------------------------
$VersionNumeric = $Version.TrimStart("v")
Push-Location $InstallerDir
try {
    & $NSIS_PATH /V1 "/DVERSION=$VersionNumeric" "installer.nsi"
    if ($LASTEXITCODE -ne 0) { throw "makensis failed with exit code $LASTEXITCODE" }
} finally {
    Pop-Location
}

$InstallerFile = Join-Path $InstallerDir "vura-$VersionNumeric-windows.exe"
if (-not (Test-Path $InstallerFile)) { throw "expected installer not found: $InstallerFile" }

Write-Host "Built installer: $InstallerFile" -ForegroundColor Green

# ---- 4. Hash + upload to R2 ------------------------------------------------
if ($SkipUpload) {
    Write-Host "Skipping upload (-SkipUpload set)."
    exit 0
}

python "$PSScriptRoot\publish.py" `
    --channel $Channel `
    --version $Version `
    --platform windows `
    --file $InstallerFile

if ($LASTEXITCODE -ne 0) { throw "publish.py failed" }

Write-Host "== Done: $Version ($Channel) published for Windows ==" -ForegroundColor Cyan
