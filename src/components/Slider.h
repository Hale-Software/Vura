#pragma once

#include <QSlider>


class Slider : public QSlider
{
    Q_OBJECT
public:
    explicit Slider(QWidget *parent = nullptr);

    bool isScrubbing() const { return m_scrubbing; }
    void setPositionFromEngine(qint64 ms);

signals:
    void scrubbed(qint64 ms);
    void scrubFinished(qint64 ms);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int valueForPosition(int x) const;

    bool m_scrubbing = false;

};
