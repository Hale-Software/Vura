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


PlaylistEmptyStateWidget::PlaylistEmptyStateWidget(QWidget *parent) : QWidget(parent)
{
    setAcceptDrops(true);

    auto *layout = new QVBoxLayout(this);

    // Icon (Using QIcon or a simple SVG)
    QLabel *icon = new QLabel("📁");
    icon->setAlignment(Qt::AlignCenter);

    // Text
    QLabel *title = new QLabel("Your library is empty");
    title->setAlignment(Qt::AlignCenter);

    // CTA Button
    QPushButton *btn = new QPushButton("Add Video Files");

    layout->addStretch();
    layout->addWidget(icon);
    layout->addWidget(title);
    layout->addWidget(btn);
    layout->addStretch();

    connect(btn, &QPushButton::clicked, this, &PlaylistEmptyStateWidget::addVideoFile_Clicked);
}

void PlaylistEmptyStateWidget::addVideoFile_Clicked()
{
    emit requestFileImport();
}

void PlaylistEmptyStateWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // Only accept the drop if it contains URLs (files)
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void PlaylistEmptyStateWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    QStringList filePaths;
    for (const QUrl &url : urls) {
        // Convert file URL to local path
        filePaths.append(url.toLocalFile());
    }

    // Notify the parent (or controller) that files were dropped
    emit filesDropped(filePaths);
}
