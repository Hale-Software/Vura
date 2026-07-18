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

#include "playlist-delegate.h"


void PlaylistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 1. Background Logic
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, QColor("#1E90FF"));
    } else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, QColor("#333333"));
    }

    // 2. Custom Progress Bar
    const qint64 progress = index.data(Qt::UserRole).toLongLong();
    const int barHeight = 4;
    const QRect progressRect(option.rect.left(), option.rect.bottom() - barHeight,
                       option.rect.width() * (progress / 100.0), barHeight);
    painter->fillRect(progressRect, QColor("#00FF00"));

    // 3. Text
    painter->setPen(Qt::white);
    painter->drawText(option.rect.adjusted(20, 0, 0, 0), Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());

    painter->restore();
}

QSize PlaylistDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), 40);
}
