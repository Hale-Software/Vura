#pragma once

#include <QSlider>

/// A slider that jumps to where you click and stops arguing with the engine
/// while you drag.
///
/// The default QSlider pages toward the click instead of landing on it, and
/// a naive position binding fights the pointer: the engine reports the old
/// position mid-drag, the handle snaps back, and scrubbing feels broken.
/// setPositionFromEngine() is ignored while the user is holding the handle.
class SeekSlider : public QSlider
{
    Q_OBJECT

public:
    explicit SeekSlider(QWidget *parent = nullptr);

    bool isScrubbing() const { return m_scrubbing; }
    void setPositionFromEngine(qint64 ms);

    signals:
        /// Emitted continuously while dragging, for preview seeking.
        void scrubbed(qint64 ms);
    /// Emitted once when the handle is released, for the final exact seek.
    void scrubFinished(qint64 ms);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int valueForPosition(int x) const;

    bool m_scrubbing = false;
};
