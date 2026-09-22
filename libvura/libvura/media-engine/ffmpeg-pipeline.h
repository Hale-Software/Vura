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

#pragma once

#include "../models/types.h"

#include <QString>
#include <QUrl>
#include <QVariantMap>
#include <QVector>

#include <atomic>
#include <functional>
#include <memory>

// Forward declarations keep FFmpeg headers out of everything that includes
// this, the same way GLVideoWidget keeps mpv out of the UI.
struct AVFrame;

namespace media::ffmpeg {

/// ADJUST: map these onto the enumerators your models/types.h defines.
/// They are the only place this backend names an ErrorKind. Network should
/// be one your isRecoverable() accepts, so the controller retries it once.
namespace errors {
inline constexpr ErrorKind OpenFailed = ErrorKind::Resource;  // local file missing / unreadable
inline constexpr ErrorKind Network = ErrorKind::NetworkFailure;      // remote URL failed to open
inline constexpr ErrorKind Unsupported = ErrorKind::UnsupportedFormat;   // no streams, no decoder
inline constexpr ErrorKind Renderer = ErrorKind::Resource;    // GL setup failed
}

/// A decoded picture, always YUV420P (8-bit, three planes). Anything the
/// decoder produces in another format is converted before it gets here, so
/// the renderer only ever has to understand one layout.
struct VideoFrame
{
    AVFrame *frame = nullptr;
    double pts = 0.0;       ///< seconds, relative to the start of the file
    double duration = 0.0;  ///< seconds
    int serial = 0;         ///< seek generation this frame belongs to
    bool fullRange = false; ///< JPEG/full range rather than limited (16-235)
    bool bt709 = false;     ///< BT.709 matrix rather than BT.601
    double aspect = 1.0;    ///< display aspect ratio (width / height)

    VideoFrame() = default;
    ~VideoFrame();
    VideoFrame(const VideoFrame &) = delete;
    VideoFrame &operator=(const VideoFrame &) = delete;
};
using VideoFramePtr = std::unique_ptr<VideoFrame>;

struct StreamInfo
{
    int index = -1;
    TrackType type = TrackType::Video;
    QString title;
    QString language;
    QString codec;
};

struct OpenResult
{
    Msec duration = 0;
    bool seekable = false;
    int videoStream = -1;
    int audioStream = -1;
    QVector<StreamInfo> streams;
    QVariantMap metadata;
};

/// One opened source: a demux thread, a video decode thread and an audio
/// decode thread, plus the clock that keeps them in step.
///
/// A new Pipeline is built for every source (and rebuilt on track change),
/// which makes teardown trivial: destroy it and every thread is joined.
///
/// Threading contract:
///  - construct, destroy and call the public methods from the GUI thread;
///  - the callbacks fire on the demux thread and must be marshalled by the
///    receiver (OpenGLEngine does this with a queued invokeMethod).
class Pipeline
{
public:
    struct Callbacks
    {
        std::function<void(const OpenResult &)> opened;
        std::function<void(ErrorKind, const QString &)> failed;
    };

    /// @param preferredVideo / preferredAudio  stream index, or -1 for "best"
    Pipeline(const QUrl &url, Msec startAt, int preferredVideo, int preferredAudio, Callbacks callbacks);
    ~Pipeline();

    Pipeline(const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &) = delete;

    void start();

    void setPaused(bool paused);
    void seek(Msec ms);
    void setVolume(qreal linear);

    /// Master clock in seconds, or NaN while it is not yet known (right
    /// after a seek, before any audio is audible). Non-const because it
    /// samples the audio device's position.
    double clock();

    /// Current seek generation. Frames whose serial differs are stale.
    int serial() const;

    /// Returns the frame that should be on screen now, or nullptr if the one
    /// already displayed is still correct. With @p haveCurrent false the
    /// first queued frame is returned regardless of the clock, which is what
    /// makes a paused seek show a picture.
    VideoFramePtr takeFrame(double clock, bool haveCurrent);

    /// Timestamp of the next queued frame, or NaN when none is decoded yet.
    /// Lets the engine schedule its next repaint instead of spinning.
    double nextFrameDue();

    /// Discards frames that are already late while keeping the newest due
    /// frame for the renderer. Keeps decoding flowing when nothing is
    /// painting (window minimised, no output attached).
    void dropLateFrames(double clock);

    /// True once the source is fully demuxed, decoded and played out.
    bool finished();

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

}
