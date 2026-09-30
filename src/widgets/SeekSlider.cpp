#include "SeekSlider.h"

#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionSlider>

SeekSlider::SeekSlider(QWidget *parent)
    : QSlider(Qt::Horizontal, parent)
{
    setRange(0, 0);
    setSingleStep(5000);
    setPageStep(30000);
    setTracking(true);

    connect(this, &QSlider::sliderPressed, this, [this] { m_scrubbing = true; });
    connect(this, &QSlider::valueChanged, this, [this](int value) {
        if (m_scrubbing)
            emit scrubbed(value);
    });
    connect(this, &QSlider::sliderReleased, this, [this] {
        m_scrubbing = false;
        emit scrubFinished(value());
    });
}

void SeekSlider::setPositionFromEngine(qint64 ms)
{
    if (m_scrubbing)
        return;

    QSignalBlocker blocker(this);
    const qint64 lo = minimum();
    const qint64 hi = maximum();
    setValue(int(qBound(lo, ms, hi)));
}

int SeekSlider::valueForPosition(int x) const
{
    QStyleOptionSlider option;
    initStyleOption(&option);

    const QRect groove = style()->subControlRect(QStyle::CC_Slider, &option,
                                                 QStyle::SC_SliderGroove, this);
    const QRect handle = style()->subControlRect(QStyle::CC_Slider, &option,
                                                 QStyle::SC_SliderHandle, this);

    const int span = groove.width() - handle.width();
    if (span <= 0)
        return minimum();

    const int pos = x - groove.left() - handle.width() / 2;
    return QStyle::sliderValueFromPosition(minimum(), maximum(), pos, span, option.upsideDown);
}

void SeekSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || maximum() == minimum()) {
        QSlider::mousePressEvent(event);
        return;
    }

    // Land on the click rather than paging toward it.
    m_scrubbing = true;
    setValue(valueForPosition(int(event->position().x())));
    emit scrubbed(value());
    event->accept();

    // Hand the synthetic press to QSlider so dragging continues normally.
    QSlider::mousePressEvent(event);
}

void SeekSlider::mouseReleaseEvent(QMouseEvent *event)
{
    QSlider::mouseReleaseEvent(event);

    if (m_scrubbing) {
        m_scrubbing = false;
        emit scrubFinished(value());
    }
}
