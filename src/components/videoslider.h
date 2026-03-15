#ifndef HPLAYER_VIDEOSLIDER_H
#define HPLAYER_VIDEOSLIDER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QBrush>
#include <QEvent>
#include <QTimer>
#include <QMouseEvent>
#include <QDebug>


class VideoSlider : public QWidget
{
    Q_OBJECT

public:
    explicit VideoSlider(QWidget *parent = nullptr);

    void setMinimum(int mminimum);
    void setMaximum(int mmaximum);
    void setValue(int mvalue);
    void setVideoLoaded(bool isLoaded);
    void setMarkers(QMap<QString,QList<double>> markers);
    void jumpToNextMarker(double currentPercent);
    void jumpToPreviousMarker(double currentPercent);

    int minimum() const;
    int maximum() const;
    int value() const;
    bool isSliderDown() const;
    bool isVideoLoaded() const;
    bool showMarkers = true;
    bool showCumshotMarkers = true;
    bool showCyanMarkers = true;
    bool showDialogMarkers = true;
    bool showMagentaMarkers = true;
    bool showOrangeMarkers = true;
    bool showSceneMarkers = true;
    bool showStripMarkers = true;

signals:
    void valueChanged(int value);
    void sliderPressed();
    void sliderMoved(int value);
    void sliderReleased();
    void sliderClicked(int mseconds);
    void markerSelected(int mseconds);

private slots:
    void hideIndicator();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QTimer *timer;
    QMap<QString,QList<double>> m_markers;
    bool m_showingIndicator = false;
    int m_minimum = 0;
    int m_maximum = 100;
    int m_value = 0;
    bool m_isSliderDown = false;
    bool m_videoLoaded = false;

};

#endif //HPLAYER_VIDEOSLIDER_H