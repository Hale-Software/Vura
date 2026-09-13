/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

// SPDX-License-Identifier: MIT
#pragma once

#include <QMetaType>
#include <QObject>
#include <QString>
#include <QVector>

#include <utility>

namespace media {

Q_NAMESPACE

/// What the transport is doing right now.
/// Deliberately kept separate from MediaStatus: "playing" and "buffering"
/// are orthogonal, and folding them into one enum is a trap you cannot
/// undo later without touching every call site.
enum class PlaybackState {
    Stopped,
    Playing,
    Paused,
};
Q_ENUM_NS(PlaybackState)

/// Where the current media is in its lifecycle.
enum class MediaStatus {
    NoMedia,
    Loading,
    Loaded,
    Buffering,
    Buffered,
    EndOfMedia,
    Invalid,
};
Q_ENUM_NS(MediaStatus)

/// Error categories the UI can act on. The distinction that matters is
/// recoverable (retry) versus terminal (skip to the next item).
enum class ErrorKind {
    None,
    NotFound,
    AccessDenied,
    UnsupportedFormat,
    MissingCodec,
    NetworkFailure,
    DecodeFailure,
    Internal,
};
Q_ENUM_NS(ErrorKind)

enum class TrackType {
    Video,
    Audio,
    Subtitle,
};
Q_ENUM_NS(TrackType)

/// True for errors where retrying the same source can plausibly succeed.
bool isRecoverable(ErrorKind kind);

QString describe(ErrorKind kind);

/// A selectable stream inside the current media.
///
/// `id` is an opaque, engine-assigned string and never a list index.
/// Qt Multimedia numbers tracks by position in its own list, mpv uses
/// sparse `aid`/`sid` integers, and libVLC hands out its own ids. Typing
/// this as an int silently commits the whole application to one backend's
/// numbering, and the bug surfaces later as "picking subtitle 2 selects 3".
struct TrackInfo {
    QString id;
    QString title;
    QString language;
    QString codec;
    bool isDefault = false;

    /// Human-readable label for a menu entry.
    QString displayName(int fallbackIndex) const;

    /// Lets adapters compare a freshly parsed track list against the cached
    /// one and stay quiet when nothing changed. Backends re-report the list
    /// often, and rebuilding the track menus on every report makes them
    /// unusable while a menu is open.
    friend bool operator==(const TrackInfo &lhs, const TrackInfo &rhs)
    {
        return lhs.id == rhs.id && lhs.title == rhs.title && lhs.language == rhs.language
                && lhs.codec == rhs.codec && lhs.isDefault == rhs.isDefault;
    }
    friend bool operator!=(const TrackInfo &lhs, const TrackInfo &rhs) { return !(lhs == rhs); }
};

/// Backends are not interchangeable in what they can do, and pretending
/// otherwise turns a backend swap into a set of silent no-ops. The
/// controller publishes this so the UI can disable what is unavailable.
struct Capabilities {
    bool variableRate = false;
    bool pitchCorrection = false;
    bool externalSubtitles = false;
    bool gapless = false;
    bool hardwareDecode = false;
    bool videoTrackSelection = false;
    bool audioTrackSelection = false;
    bool subtitleTrackSelection = false;
    bool video = false;
    std::pair<qreal, qreal> rateRange{1.0, 1.0};
};

/// Canonical metadata keys. Backends report wildly different key names,
/// so each adapter normalises into these before emitting.
namespace meta {
inline constexpr auto Title = "title";
inline constexpr auto Artist = "artist";
inline constexpr auto Album = "album";
inline constexpr auto CoverArt = "coverArt"; // QImage
} // namespace meta

/// Milliseconds are the unit at every boundary in this codebase.
/// mpv reports doubles in seconds; that conversion belongs inside the
/// mpv adapter, not in the controller and certainly not in the UI.
using Msec = qint64;

QString formatTime(Msec ms);

} // namespace media

Q_DECLARE_METATYPE(media::TrackInfo)
