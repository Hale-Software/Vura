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

#include "PlaylistEmptyStateWidget.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QUrl>


PlaylistEmptyStateWidget::PlaylistEmptyStateWidget(QWidget *parent) : QWidget(parent)
{
    setAcceptDrops(true);
}

void PlaylistEmptyStateWidget::addVideoFile_Clicked()
{
    emit requestFileImport();
}

void PlaylistEmptyStateWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void PlaylistEmptyStateWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    QStringList filePaths;
    for (const QUrl &url : urls) {
        filePaths.append(url.toLocalFile());
    }

    emit filesDropped(filePaths);
}
