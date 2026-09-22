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

#include "video-output.h"

#include <QStackedWidget>
#include <QWidget>

#ifdef VURA_HAVE_QTMULTIMEDIA
#include <QVideoWidget>
#endif

#ifdef VURA_HAVE_MPV
#include <QOpenGLWidget>
#endif

#ifdef VURA_HAVE_OPENGL

#endif

#include <QMutex>
#include <QVector>

#include <functional>

namespace media {
class Engine;
}

/// Placeholder surface for engines that render nothing, and the fallback
/// while no file is loaded. Shows cover art when there is any.
class IdleVideoWidget : public QWidget, public media::VideoOutput
{
    Q_OBJECT

public:
    explicit IdleVideoWidget(QWidget *parent = nullptr);

    media::OutputKind kind() const override { return media::OutputKind::None; }
    QSize surfaceSize() const override;
    void requestRedraw() override;
    void clearSurface() override;

    void setCoverArt(const QImage &image);
    void setCaption(const QString &caption);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage m_cover;
    QString m_caption;
};

#ifdef VURA_HAVE_QTMULTIMEDIA
/// Output for engines that push QVideoFrames into a sink Qt owns.
class SinkVideoWidget : public QVideoWidget, public media::VideoSinkOutput
{
    Q_OBJECT

public:
    explicit SinkVideoWidget(QWidget *parent = nullptr);

    QVideoSink *videoSink() const override;
    QSize surfaceSize() const override;
    void requestRedraw() override;
    void clearSurface() override;
};
#endif

#ifdef VURA_HAVE_MPV
/// Output for engines that render into our GL framebuffer themselves.
///
/// This widget knows nothing about mpv. The engine installs a render
/// callback on attach and we invoke it from paintGL(), which keeps
/// mpv/render_gl.h out of the UI translation units entirely.
class GLVideoWidget : public QOpenGLWidget, public media::GLVideoOutput
{
    Q_OBJECT

public:
    explicit GLVideoWidget(QWidget *parent = nullptr);
    ~GLVideoWidget() override;

    QSize surfaceSize() const override;
    void requestRedraw() override;
    void clearSurface() override;

    void executeWithContext(const std::function<void()> &fn) override;
    void *glProcAddress(const char *name) override;
    void setRenderCallback(RenderFn fn) override;

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    bool m_initialized = false;
    RenderFn m_render;

    /// Engines may attach before the widget has ever been shown, at which
    /// point there is no context to make current. Those operations are held
    /// until initializeGL() runs.
    QVector<std::function<void()>> m_pendingContextOps;
};
#endif

/// Holds one output widget per rendering model and shows whichever the
/// current engine needs.
///
/// One output implementation per backend is the honest outcome of the
/// VideoOutput design, and this is where that shows up. Widgets are built on
/// first use and keyed off Engine::requiredOutput(), so a backend that is
/// compiled in but never selected costs nothing at runtime.
class VideoStage : public QStackedWidget
{
    Q_OBJECT

public:
    explicit VideoStage(QWidget *parent = nullptr);

    /// Suitable as MediaController::setVideoOutputProvider(). Returns the
    /// output matching what the engine can drive, or the idle surface.
    media::VideoOutput *outputFor(media::Engine *engine);

    void setCoverArt(const QImage &image);
    void setCaption(const QString &caption);

private:
    /// Falls back to the placeholder surface, for audio-only engines and for
    /// kinds this build has no widget for.
    media::VideoOutput *showIdle();

    // Created on first use by outputFor(); see the note in the constructor.
    IdleVideoWidget *m_idle = nullptr;
#ifdef VURA_HAVE_QTMULTIMEDIA
    SinkVideoWidget *m_sink = nullptr;
#endif
#ifdef VURA_HAVE_MPV
    GLVideoWidget *m_gl = nullptr;
#endif
};
