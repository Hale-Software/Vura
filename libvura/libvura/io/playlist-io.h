#pragma once

#include "../models/playlist.h"   // PlaylistItem, Playlist, media::Msec

#include <QList>
#include <QString>
#include <QStringList>
#include <QUrl>

#include <optional>

/// Reading and writing of the playlist container formats a desktop player is
/// expected to understand. Qt 6 dropped QMediaPlaylist along with the parsers
/// that used to live behind it, so all of this is ours now.
///
/// Nothing here touches QMediaPlayer: a playlist file is just a list of URLs
/// plus some optional metadata, and QMediaPlayer only ever wants
/// setSource(QUrl) for one track at a time.
namespace playlistio {

enum class Format {
    M3u,    ///< Legacy Winamp/Nullsoft list. Plain text, no declared encoding.
    M3u8,   ///< Same grammar, UTF-8 by definition.
    Xspf,   ///< XML. The only one of the five with an actual specification.
    Asx,    ///< Windows Media metafile. XML-shaped, frequently not well formed.
    B4s,    ///< Winamp 3. XML inside <WinampXML>, Windows paths inside that.
};

struct Entry {
    QUrl url;
    QString title;
    media::Msec duration = -1;   ///< milliseconds, -1 when unknown
};

struct ReadOptions {
    /// When a stored absolute path does not exist, look for the file next to
    /// the playlist. This is what makes a playlist written on Windows (with
    /// `C:\Music\...` paths) usable after copying the folder to Linux or macOS.
    bool relocateMissingFiles = true;

    /// Keep entries whose target cannot be found on disk. Leaving them in is
    /// usually kinder than silently dropping tracks; the view can grey them out.
    bool keepMissingFiles = true;
};

struct WriteOptions {
    enum PathStyle {
        Absolute,   ///< always full paths
        Relative,   ///< relative to the playlist, when a relative path exists
        Auto,       ///< relative if the file lives under the playlist's folder
    };
    PathStyle pathStyle = Auto;

    /// Emit #EXTM3U/#EXTINF lines. Only affects the two M3U flavours.
    bool writeExtendedInfo = true;
};

/// Suffix -> format. Recognises the .wax/.wvx/.wmx aliases of ASX.
std::optional<Format> formatForFile(const QString &path);
/// Content sniffing, for files that arrive without a usable suffix.
std::optional<Format> formatForContent(const QByteArray &bytes);
QString defaultSuffix(Format format);

/// Ready-made QFileDialog filters.
QString openFileFilter();
QStringList saveFileFilters();

/// In-memory conversion. `base` is the location of the playlist file itself and
/// is what relative entries are resolved against; pass an invalid QUrl if the
/// data did not come from a file (relative entries are then dropped).
QList<Entry> parse(const QByteArray &bytes, Format format, const QUrl &base,
                   const ReadOptions &options = {});
QByteArray serialize(const QList<Entry> &entries, Format format, const QUrl &base,
                     const WriteOptions &options = {});

/// Filesystem wrappers. Both return false and fill `error` on failure. Saving
/// goes through QSaveFile, so an interrupted write cannot shred an existing
/// playlist.
bool load(const QString &path, QList<Entry> *out, QString *error = nullptr,
          const ReadOptions &options = {});
bool save(const QString &path, const QList<Entry> &entries, QString *error = nullptr,
          const WriteOptions &options = {});

/// Convenience glue for the Playlist model. These use nothing but its public
/// API, so Playlist itself needs no changes.
bool loadInto(Playlist *playlist, const QString &path, QString *error = nullptr,
              const ReadOptions &options = {});
bool saveFrom(const Playlist *playlist, const QString &path, QString *error = nullptr,
              const WriteOptions &options = {});

} // namespace playlistio
