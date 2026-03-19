/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

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

#include <QPainter>

#include "SliderIgnoreScroll.h"


class AbsoluteSlider : public SliderIgnoreScroll {
    Q_OBJECT
    Q_PROPERTY(QColor tickColor READ getTickColor WRITE setTickColor DESIGNABLE true)

public:
    AbsoluteSlider(QWidget *parent = nullptr);
    AbsoluteSlider(Qt::Orientation orientation, QWidget *parent = nullptr);

    bool getDisplayTicks() const;
    void setDisplayTicks(bool display);

    QColor getTickColor() const;
    void setTickColor(QColor c);

    signals:
        void absoluteSliderHovered(int value);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual bool eventFilter(QObject *obj, QEvent *event) override;

    int posToRangeValue(QMouseEvent *event);

    virtual void paintEvent(QPaintEvent *event) override;

private:
    bool dragging = false;
    bool displayTicks = false;

    QColor tickColor;

};
