#ifndef HVIDEOWIDGET_H
#define HVIDEOWIDGET_H

#include <QApplication>
#include <QVideoWidget>
#include <QWidget>

class HVideoWidget : public QVideoWidget
{
    Q_OBJECT

public:
    explicit HVideoWidget(QWidget *parent = nullptr);

signals:
    void hKeyPress(QKeyEvent *event);

protected:
    void keyPressEvent(QKeyEvent *event) override;

};

#endif // HVIDEOWIDGET_H
