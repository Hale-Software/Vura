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

#include "SliderIgnoreScroll.h"


SliderIgnoreScroll::SliderIgnoreScroll(QWidget *parent) : QSlider(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

SliderIgnoreScroll::SliderIgnoreScroll(Qt::Orientation orientation, QWidget *parent) : QSlider(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setOrientation(orientation);
}

void SliderIgnoreScroll::wheelEvent(QWheelEvent *event)
{
    if (!hasFocus())
        event->ignore();
    else
        QSlider::wheelEvent(event);
}

void SliderIgnoreClick::mousePressEvent(QMouseEvent *event)
{
    QStyleOptionSlider styleOption;
    initStyleOption(&styleOption);
    QRect handle = style()->subControlRect(QStyle::CC_Slider, &styleOption, QStyle::SC_SliderHandle, this);
    if (handle.contains(event->position().toPoint())) {
        SliderIgnoreScroll::mousePressEvent(event);
        dragging = true;
    } else {
        event->accept();
    }
}

void SliderIgnoreClick::mouseReleaseEvent(QMouseEvent *event)
{
    dragging = false;
    SliderIgnoreScroll::mouseReleaseEvent(event);
}

void SliderIgnoreClick::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging) {
        SliderIgnoreScroll::mouseMoveEvent(event);
    } else {
        event->accept();
    }
}