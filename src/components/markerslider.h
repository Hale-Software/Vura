#ifndef MARKERSLIDER_H
#define MARKERSLIDER_H

#include <QSlider>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QToolTip>
#include <QMouseEvent>
#include <QDebug>


class MarkerSlider : public QSlider {
    Q_OBJECT

public:
    explicit MarkerSlider(QWidget *parent = nullptr);

    void setMarkers(QMap<QString,QList<double>> markers);
    void jumpToNextMarker(double currentPercent);
    void jumpToPreviousMarker(double currentPercent);

    bool showMarkers = true;
    bool showCumshotMarkers = true;
    bool showCyanMarkers = true;
    bool showDialogMarkers = true;
    bool showMagentaMarkers = true;
    bool showOrangeMarkers = true;
    bool showSceneMarkers = true;
    bool showStripMarkers = true;

signals:
    void sliderClicked(int mseconds);
    void markerSelected(int mseconds);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QMap<QString,QList<double>> m_markers;

};

#endif // MARKERSLIDER_H
