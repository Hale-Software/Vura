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

#pragma once

#include <QSlider>
#include <QInputEvent>
#include <QtCore/QObject>
#include <QStyleOptionSlider>

class SliderIgnoreScroll : public QSlider {
    Q_OBJECT

public:
    SliderIgnoreScroll(QWidget *parent = nullptr);
    SliderIgnoreScroll(Qt::Orientation orientation, QWidget *parent = nullptr);

protected:
    virtual void wheelEvent(QWheelEvent *event) override;
};

class SliderIgnoreClick : public SliderIgnoreScroll {
public:
    inline SliderIgnoreClick(Qt::Orientation orientation, QWidget *parent = nullptr)
        : SliderIgnoreScroll(orientation, parent)
    {
    }

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

private:
    bool dragging = false;

};
