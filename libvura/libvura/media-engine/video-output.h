#pragma once

#include <QSize>
#include <QString>

#include <functional>

QT_BEGIN_NAMESPACE
class QVideoSink;
QT_END_NAMESPACE

namespace media {

enum class OutputKind
{
    None,
    VideoSink,
    OpenGL,
    Software
};

class VideoOutput
{
public:
    virtual ~VideoOutput() = default;

    virtual OutputKind kind() const = 0;

    virtual QSize surfaceSize() const = 0;

    virtual void requestRedraw() = 0;

    virtual void clearSurface() {}
};

class VideoSinkOutput : public VideoOutput
{
public:
    OutputKind kind() const override { return OutputKind::VideoSink; }
    virtual QVideoSink *videoSink() const = 0;
};

class GLVideoOutput : public VideoOutput
{
public:
    using RenderFn = std::function<void(const QSize &sizeInPixels, unsigned int fbo)>;

    OutputKind kind() const override { return OutputKind::OpenGL; }

    virtual void executeWithContext(const std::function<void()> &fn) = 0;

    virtual void *glProcAddress(const char *name) = 0;

    virtual void setRenderCallback(RenderFn fn) = 0;
};

}
