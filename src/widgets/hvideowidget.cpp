#include "hvideowidget.h"

#include <QKeyEvent>

HVideoWidget::HVideoWidget(QWidget *parent) : QVideoWidget(parent)
{

}

void HVideoWidget::keyPressEvent(QKeyEvent *event)
{
    //emit hKeyPress(event);
}
