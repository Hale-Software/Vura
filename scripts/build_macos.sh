#!/usr/bin/env bash
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
# build_macos.sh - Build a Vura release for macOS, bundle Qt with macdeployqt,
# sign (and optionally notarize) it, package it as a DMG, hash it, and upload
# it to Cloudflare R2 via publish.py.
#
# macOS counterpart to build_windows.ps1.
#
# Usage:
#   ./scripts/build_macos.sh --channel stable --version v1.4.0
#   ./scripts/build_macos.sh --skip-upload --ffmpeg
#   ./scripts/build_macos.sh --channel stable \
#       --sign-identity "Developer ID Application: Andrew Hale (TEAMID)" \
#       --notary-profile vura-notary
#
# Options:
#   --channel stable|beta     Release channel (default: beta, so accidental
#                             runs never land on stable)
#   --version VERSION         e.g. v1.4.0 or v1.4.0-beta.1
#                             (default: git describe --tags --abbrev=0)
#   --[no-]qtmultimedia       MEDIA_ENABLE_QTMULTIMEDIA (default: on)
#   --[no-]opengl             MEDIA_ENABLE_OPENGL       (default: off)
#   --[no-]mpv                MEDIA_ENABLE_MPV          (default: off)
#   --[no-]ffmpeg             ENABLE_FFMPEG             (default: off)
#   --sign-identity NAME      codesign identity. Without it the app is
#                             ad-hoc signed (runs locally, Gatekeeper will
#                             warn on other machines).
#   --notary-profile NAME     notarytool keychain profile; requires
#                             --sign-identity. Create one once with:
#                               xcrun notarytool store-credentials NAME \
#                                 --apple-id you@example.com --team-id TEAMID
#   --skip-upload             Build and package, but don't publish to R2
#   -h, --help                Show this help
#
# Environment overrides:
#   QT_DIR          Qt kit for macOS (default: ~/Qt/6.12.0/macos)
#   FFMPEG_PREFIX   FFmpeg install prefix (default: Homebrew's ffmpeg)
#   PYTHON          Python interpreter (default: python3)

set -euo pipefail

# ---- paths matching your local environment ---------------------------------
QT_DIR="${QT_DIR:-$HOME/Qt/6.12.0/macos}"
FFMPEG_PREFIX="${FFMPEG_PREFIX:-$(brew --prefix ffmpeg 2>/dev/null || echo /opt/homebrew/opt/ffmpeg)}"
#FFMPEG_PREFIX="../deps/ffmpeg"
PYTHON="${PYTHON:-python3}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$REPO_ROOT/build/release"
INSTALLER_DIR="$REPO_ROOT/installer"
DEPLOY_DIR="$REPO_ROOT/build/deploy/dist_macos"
DMG_STAGING="$REPO_ROOT/build/deploy/dmg_staging"
APP_NAME="vura"
ENTITLEMENTS="$INSTALLER_DIR/vura.entitlements"   # used only if it exists

# ---- defaults ---------------------------------------------------------------
CHANNEL="beta"
VERSION=""
ENABLE_QTMULTIMEDIA=1
ENABLE_OPENGL=0
ENABLE_MPV=0
ENABLE_FFMPEG=0
SIGN_IDENTITY=""
NOTARY_PROFILE=""
SKIP_UPLOAD=0

# ---- helpers ----------------------------------------------------------------
info() { printf '\033[36m%s\033[0m\n' "$*"; }
ok()   { printf '\033[32m%s\033[0m\n' "$*"; }
warn() { printf '\033[33m%s\033[0m\n' "$*"; }
die()  { printf '\033[31merror: %s\033[0m\n' "$*" >&2; exit 1; }
on_off() { [[ "$1" == 1 ]] && echo ON || echo OFF; }
usage() { sed -n '/^# Usage:/,/^# Environment/p' "$0" | sed 's/^# \{0,1\}//'; }

# ---- parse arguments --------------------------------------------------------
while [[ $# -gt 0 ]]; do
    case "$1" in
        --channel)          CHANNEL="${2:?--channel needs a value}"; shift 2 ;;
        --version)          VERSION="${2:?--version needs a value}"; shift 2 ;;
        --qtmultimedia)     ENABLE_QTMULTIMEDIA=1; shift ;;
        --no-qtmultimedia)  ENABLE_QTMULTIMEDIA=0; shift ;;
        --opengl)           ENABLE_OPENGL=1; shift ;;
        --no-opengl)        ENABLE_OPENGL=0; shift ;;
        --mpv)              ENABLE_MPV=1; shift ;;
        --no-mpv)           ENABLE_MPV=0; shift ;;
        --ffmpeg)           ENABLE_FFMPEG=1; shift ;;
        --no-ffmpeg)        ENABLE_FFMPEG=0; shift ;;
        --sign-identity)    SIGN_IDENTITY="${2:?--sign-identity needs a value}"; shift 2 ;;
        --notary-profile)   NOTARY_PROFILE="${2:?--notary-profile needs a value}"; shift 2 ;;
        --skip-upload)      SKIP_UPLOAD=1; shift ;;
        -h|--help)          usage; exit 0 ;;
        *)                  die "unknown option: $1 (see --help)" ;;
    esac
done

[[ "$CHANNEL" == "stable" || "$CHANNEL" == "beta" ]] || die "--channel must be 'stable' or 'beta'"
if [[ -n "$NOTARY_PROFILE" && -z "$SIGN_IDENTITY" ]]; then
    die "--notary-profile requires --sign-identity"
fi
[[ -x "$QT_DIR/bin/macdeployqt" ]] || die "macdeployqt not found in $QT_DIR/bin (set QT_DIR)"

if [[ -z "$VERSION" ]]; then
    VERSION="$(git -C "$REPO_ROOT" describe --tags --abbrev=0)"
fi
VERSION="$(echo "$VERSION" | tr -d '[:space:]')"
[[ "$VERSION" == v* ]] || VERSION="v$VERSION"
VERSION_NUMERIC="${VERSION#v}"

info "== Building Vura $VERSION ($CHANNEL) for macOS =="

# ---- 1. Clean build directories ---------------------------------------------
echo "Cleaning build directories..."
rm -rf "$BUILD_DIR" "$DEPLOY_DIR" "$DMG_STAGING"
mkdir -p "$DEPLOY_DIR" "$DMG_STAGING" "$INSTALLER_DIR"

# ---- 2. Configure + build (Release) -----------------------------------------
PREFIX_PATH="$QT_DIR"
if [[ "$ENABLE_FFMPEG" == 1 ]]; then
    [[ -d "$FFMPEG_PREFIX" ]] || die "FFmpeg not found at $FFMPEG_PREFIX (brew install ffmpeg, or set FFMPEG_PREFIX)"
    PREFIX_PATH="$PREFIX_PATH;$FFMPEG_PREFIX"
fi

cmake -S "$REPO_ROOT" -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$PREFIX_PATH" \
    -DMEDIA_ENABLE_QTMULTIMEDIA="$(on_off $ENABLE_QTMULTIMEDIA)" \
    -DMEDIA_ENABLE_OPENGL="$(on_off $ENABLE_OPENGL)" \
    -DMEDIA_ENABLE_MPV="$(on_off $ENABLE_MPV)" \
    -DENABLE_FFMPEG="$(on_off $ENABLE_FFMPEG)"

cmake --build "$BUILD_DIR" --config Release --parallel

# ---- 3. Stage the app bundle ------------------------------------------------
APP_SRC="$BUILD_DIR/bin/$APP_NAME.app"
if [[ ! -d "$APP_SRC" ]]; then
    APP_SRC="$(find "$BUILD_DIR" -maxdepth 3 -type d -name "$APP_NAME.app" -print -quit)"
fi
[[ -n "$APP_SRC" && -d "$APP_SRC" ]] || die "could not find $APP_NAME.app under $BUILD_DIR (is MACOSX_BUNDLE set?)"

APP="$DEPLOY_DIR/$APP_NAME.app"
ditto "$APP_SRC" "$APP"   # ditto preserves symlinks, xattrs and permissions

# macdeployqt copies Qt frameworks + plugins and any non-system dylibs the
# binary links (Homebrew FFmpeg/mpv included), rewriting install names.
DEPLOY_ARGS=("$APP" -always-overwrite -verbose=1)
if [[ -n "$SIGN_IDENTITY" ]]; then
    DEPLOY_ARGS+=(-codesign="$SIGN_IDENTITY" -hardened-runtime -timestamp)
fi
"$QT_DIR/bin/macdeployqt" "${DEPLOY_ARGS[@]}"

echo "Deployment bundle staged in $DEPLOY_DIR"

# ---- 4. Code signing --------------------------------------------------------
# macdeployqt rewrites install names, which invalidates the linker's ad-hoc
# signature. Apple Silicon refuses to run unsigned code, so always re-sign.
if [[ -n "$SIGN_IDENTITY" ]]; then
    info "Signing with: $SIGN_IDENTITY"
    SIGN_ARGS=(--force --options runtime --timestamp --sign "$SIGN_IDENTITY")
    [[ -f "$ENTITLEMENTS" ]] && SIGN_ARGS+=(--entitlements "$ENTITLEMENTS")
    # Nested code was signed by macdeployqt; seal the outer bundle last.
    codesign "${SIGN_ARGS[@]}" "$APP"
else
    warn "No --sign-identity: ad-hoc signing (Gatekeeper will warn on other Macs)."
    codesign --force --deep --sign - "$APP"
fi
codesign --verify --deep --strict --verbose=2 "$APP"

# ---- 5. Package as a DMG ----------------------------------------------------
DMG_FILE="$INSTALLER_DIR/$APP_NAME-$VERSION_NUMERIC-macos.dmg"
rm -f "$DMG_FILE"

ditto "$APP" "$DMG_STAGING/$APP_NAME.app"
ln -s /Applications "$DMG_STAGING/Applications"   # drag-to-install target

hdiutil create \
    -volname "Vura $VERSION_NUMERIC" \
    -srcfolder "$DMG_STAGING" \
    -fs HFS+ \
    -format UDZO \
    -ov \
    "$DMG_FILE"

[[ -f "$DMG_FILE" ]] || die "expected DMG not found: $DMG_FILE"

if [[ -n "$SIGN_IDENTITY" ]]; then
    codesign --force --timestamp --sign "$SIGN_IDENTITY" "$DMG_FILE"
fi

# ---- 6. Notarize + staple (optional) ----------------------------------------
if [[ -n "$NOTARY_PROFILE" ]]; then
    info "Submitting to Apple notary service (this can take a few minutes)..."
    xcrun notarytool submit "$DMG_FILE" --keychain-profile "$NOTARY_PROFILE" --wait
    xcrun stapler staple "$DMG_FILE"
    # Notarizing the DMG also covers the app inside it, so the ticket can be
    # stapled to the loose bundle that publish.py uploads file-by-file.
    xcrun stapler staple "$APP" || warn "could not staple ticket to $APP"
    spctl --assess --type open --context context:primary-signature --verbose "$DMG_FILE" || true
fi

ok "Built installer: $DMG_FILE"

# ---- 7. Hash + upload to R2 -------------------------------------------------
if [[ "$SKIP_UPLOAD" == 1 ]]; then
    warn "Skipping upload to R2 (--skip-upload set)."
else
    # publish.py calls load_dotenv(".env") relative to the working directory,
    # so run it from the scripts folder where .env lives.
    (
        cd "$SCRIPT_DIR"
        "$PYTHON" publish.py \
            --channel "$CHANNEL" \
            --version "$VERSION" \
            --platform mac \
            --dir "$DEPLOY_DIR" \
            --installer "$DMG_FILE"
    ) || die "publish.py failed"
fi

info "== Done: $VERSION ($CHANNEL) published for macOS =="
