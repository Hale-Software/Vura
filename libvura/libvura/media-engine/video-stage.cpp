#include "video-stage.h"

#include "engine.h"

#include <QMetaObject>
#include <QPainter>
#include <QResizeEvent>

#ifdef MEDIA_HAVE_QTMULTIMEDIA
#include <QVideoSink>
#endif

#ifdef MEDIA_HAVE_MPV
#include <QOpenGLContext>
#endif

// ---------------------------------------------------------------- idle ----

IdleVideoWidget::IdleVideoWidget(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
    setMinimumSize(320, 180);
}

QSize IdleVideoWidget::surfaceSize() const
{
    return size() * devicePixelRatioF();
}

void IdleVideoWidget::requestRedraw()
{
    QMetaObject::invokeMethod(this, [this] { update(); }, Qt::QueuedConnection);
}

void IdleVideoWidget::clearSurface()
{
    m_cover = {};
    update();
}

void IdleVideoWidget::setCoverArt(const QImage &image)
{
    m_cover = image;
    update();
}

void IdleVideoWidget::setCaption(const QString &caption)
{
    m_caption = caption;
    update();
}

void IdleVideoWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    if (!m_cover.isNull()) {
        const QSize scaled = m_cover.size().scaled(size() * 0.8, Qt::KeepAspectRatio);
        const QRect target(QPoint((width() - scaled.width()) / 2,
                                  (height() - scaled.height()) / 2),
                           scaled);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawImage(target, m_cover);
        return;
    }

    if (m_caption.isEmpty())
        return;

    painter.setPen(QColor(150, 150, 150));
    painter.drawText(rect(), Qt::AlignCenter | Qt::TextWordWrap, m_caption);
}

// ---------------------------------------------------------------- sink ----

#ifdef MEDIA_HAVE_QTMULTIMEDIA
SinkVideoWidget::SinkVideoWidget(QWidget *parent)
    : QVideoWidget(parent)
{
    setMinimumSize(320, 180);
    setAspectRatioMode(Qt::KeepAspectRatio);
}

QVideoSink *SinkVideoWidget::videoSink() const
{
    return QVideoWidget::videoSink();
}

QSize SinkVideoWidget::surfaceSize() const
{
    return size() * devicePixelRatioF();
}

void SinkVideoWidget::requestRedraw()
{
    // Qt drives its own repaint when a frame reaches the sink; nothing to do.
}

void SinkVideoWidget::clearSurface()
{
    QMetaObject::invokeMethod(this, [this] { update(); }, Qt::QueuedConnection);
}
#endif

// ------------------------------------------------------------------ gl ----

#ifdef MEDIA_HAVE_MPV
GLVideoWidget::GLVideoWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setMinimumSize(320, 180);
    setUpdateBehavior(QOpenGLWidget::PartialUpdate);
}

GLVideoWidget::~GLVideoWidget() = default;

QSize GLVideoWidget::surfaceSize() const
{
    return size() * devicePixelRatioF();
}

void GLVideoWidget::requestRedraw()
{
    // Called from the engine's render thread, so it must be queued.
    QMetaObject::invokeMethod(this, [this] { update(); }, Qt::QueuedConnection);
}

void GLVideoWidget::clearSurface()
{
    QMetaObject::invokeMethod(this, [this] { update(); }, Qt::QueuedConnection);
}

void GLVideoWidget::executeWithContext(const std::function<void()> &fn)
{
    if (!m_initialized) {
        m_pendingContextOps.append(fn);
        // Force initializeGL() to run so an attach made before the first
        // show does not sit queued forever.
        QMetaObject::invokeMethod(this, [this] { update(); }, Qt::QueuedConnection);
        return;
    }

    makeCurrent();
    fn();
    doneCurrent();
}

void *GLVideoWidget::glProcAddress(const char *name)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context)
        return nullptr;
    return reinterpret_cast<void *>(context->getProcAddress(name));
}

void GLVideoWidget::setRenderCallback(RenderFn fn)
{
    m_render = std::move(fn);
}

void GLVideoWidget::initializeGL()
{
    m_initialized = true;

    const auto pending = m_pendingContextOps;
    m_pendingContextOps.clear();
    // Already current inside initializeGL(), so run these directly.
    for (const auto &op : pending)
        op();
}

void GLVideoWidget::paintGL()
{
    if (m_render)
        m_render(surfaceSize(), defaultFramebufferObject());
}
#endif

// --------------------------------------------------------------- stage ----

VideoStage::VideoStage(QWidget *parent)
    : QStackedWidget(parent)
{
    // Only the idle surface is built up front. A GL widget spins up a real
    // context the moment it is constructed, so building one for a backend
    // that may never be selected wastes a context on every launch -- and on
    // headless or software-GL platforms it can bring the process down.
    m_idle = new IdleVideoWidget(this);
    addWidget(m_idle);
    setCurrentWidget(m_idle);
}

media::VideoOutput *VideoStage::outputFor(media::Engine *engine)
{
    if (!engine)
        return showIdle();

    // Dispatch on the kind the engine declares, not on its name. A fourth
    // backend that also renders through a QVideoSink then needs no change
    // here at all.
    switch (engine->requiredOutput()) {
    case media::OutputKind::VideoSink:
#ifdef MEDIA_HAVE_QTMULTIMEDIA
        if (!m_sink) {
            m_sink = new SinkVideoWidget(this);
            addWidget(m_sink);
        }
        setCurrentWidget(m_sink);
        return m_sink;
#else
        break;
#endif

    case media::OutputKind::OpenGL:
#ifdef MEDIA_HAVE_MPV
        if (!m_gl) {
            m_gl = new GLVideoWidget(this);
            addWidget(m_gl);
        }
        setCurrentWidget(m_gl);
        return m_gl;
#else
        break;
#endif

    case media::OutputKind::None:
    case media::OutputKind::Software:
        break;
    }

    return showIdle();
}

media::VideoOutput *VideoStage::showIdle()
{
    setCurrentWidget(m_idle);
    return m_idle;
}

void VideoStage::setCoverArt(const QImage &image)
{
    m_idle->setCoverArt(image);
}

void VideoStage::setCaption(const QString &caption)
{
    m_idle->setCaption(caption);
}
