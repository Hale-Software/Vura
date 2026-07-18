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

#include "PlaylistController.h"
#include "PlaylistEmptyStateWidget.h"
#include <libvura/playlist/playlist-model.h>
#include <libvura/playlist/playlist-delegate.h>


PlaylistController::PlaylistController(QListView* view, PlaylistEmptyStateWidget* emptyPlaylistWidget, QStackedWidget* container, QObject* parent) :
        QObject(parent),
        m_model(new PlaylistModel()),
        m_view(view),
        m_emptyPlaylistWidget(emptyPlaylistWidget),
        m_container(container)
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(new PlaylistDelegate());

    //connect(m_view, &QListView::clicked, this, &PlaylistController::itemClicked);
    connect(m_view, &QListView::doubleClicked, this, &PlaylistController::handleItemDoubleClicked);
    connect(m_view, &QListView::customContextMenuRequested, this, &PlaylistController::showContextMenu);

    connect(m_model, &PlaylistModel::rowsInserted, this, &PlaylistController::updateEmptyState);
    connect(m_model, &PlaylistModel::rowsRemoved, this, &PlaylistController::updateEmptyState);
    connect(m_model, &PlaylistModel::modelReset, this, &PlaylistController::updateEmptyState);
    updateEmptyState();
}

PlaylistController::~PlaylistController() {}

void PlaylistController::itemClicked(const QModelIndex &index)
{
    //if (index.isValid())
    //    emit playTrackRequested(index.row());
}

void PlaylistController::handleItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString filePath = m_model->currentURL(index.row());

    if (!filePath.isEmpty())
        emit playTrackRequested(QUrl::fromLocalFile(filePath));
}

void PlaylistController::nextTrack()
{
    if (m_model->rowCount() == 0) return;

    int currentIndex = m_view->currentIndex().row();

    if (currentIndex < 0) {
        currentIndex = 0;
    }

    int nextIndex = currentIndex + 1;

    if (nextIndex >= m_model->rowCount()) {
        nextIndex = 0;
    }

    QString filePath = m_model->currentURL(nextIndex);

    if (!filePath.isEmpty()) {
        m_view->setCurrentIndex(m_model->index(nextIndex, 0));
        emit playTrackRequested(QUrl::fromLocalFile(filePath));
    }
}

void PlaylistController::previousTrack()
{
    if (m_model->rowCount() == 0) return;

    int currentIndex = m_view->currentIndex().row();

    if (currentIndex < 0) {
        currentIndex = 0;
    }

    int previousIndex = currentIndex - 1;

    if (previousIndex < 0) {
        previousIndex = m_model->rowCount() - 1;
    }

    QString filePath = m_model->currentURL(previousIndex);

    if (!filePath.isEmpty()) {
        m_view->setCurrentIndex(m_model->index(previousIndex, 0));
        emit playTrackRequested(QUrl::fromLocalFile(filePath));
    }
}

void PlaylistController::showContextMenu(const QPoint &pos)
{
    QModelIndex index = m_view->indexAt(pos);
    m_contextMenu = new QMenu(tr("Context Menu"), m_view);

    m_addFileAction = new QAction(tr("Add File"), m_contextMenu);
    connect(m_addFileAction, &QAction::triggered, this, &PlaylistController::requestFileImport);

    m_addFolderAction = new QAction(tr("Add Folder"), m_contextMenu);
    connect(m_addFolderAction, &QAction::triggered, this, &PlaylistController::addFolder);

    m_clearPlaylistAction = new QAction(tr("Clear Playlist"), m_contextMenu);
    connect(m_clearPlaylistAction, &QAction::triggered, this, &PlaylistController::clearPlaylist);

    m_contextMenu->exec(m_view->mapToGlobal(pos));
}

void PlaylistController::updateEmptyState()
{
    const int trackCount = m_model->rowCount();
    if (trackCount == 0) {
        m_container->setCurrentIndex(1);
    } else {
        m_container->setCurrentIndex(0);
    }

    emit playlistUpdated(trackCount);
}

void PlaylistController::requestFileImport()
{
    const QStringList files = QFileDialog::getOpenFileNames(
            m_view,
            tr("Open Media Files"),
            QDir::homePath(),
            tr("Media Files (*.mp4 *.mkv *.avi *.mp3 *.wav *.flac);;All Files (*.*)")
        );

    if (!files.isEmpty())
        processFilePaths(files);
}

void PlaylistController::addFolder()
{
    const QString dir = QFileDialog::getExistingDirectory(
            m_view,
            tr("Open Directory"),
            QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (!dir.isEmpty()) {
        QStringList filePaths;
        const QDir directory(dir);
        QStringList filters;
        filters << "*.mp4" << "*.mkv" << "*.avi" << "*.mp3" << "*.wav" << "*.flac";

        QFileInfoList fileInfoList = directory.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);
        for (const QFileInfo& fileInfo : fileInfoList) {
            filePaths << fileInfo.absoluteFilePath();
        }

        processFilePaths(filePaths);
    }
}

void PlaylistController::filesDropped(const QStringList &filePaths)
{
    processFilePaths(filePaths);
}

void PlaylistController::processFilePaths(const QStringList &paths)
{
    const int previousCount = m_model->rowCount();

    for (const QString& path : paths) {
        QFileInfo fileInfo(path);
        m_model->addItem({fileInfo.baseName(), path, 0, 0});
    }

    // If the playlist was empty before adding these files, auto-play the first one
    if (previousCount == 0 && m_model->rowCount() > 0) {
        m_view->setCurrentIndex(m_model->index(0, 0));
        emit playTrackRequested(QUrl::fromLocalFile(m_model->currentURL(0)));
    }
}

void PlaylistController::clearPlaylist()
{
    m_model->clear();
}

void PlaylistController::hidePlaylist()
{
    m_container->hide();
}

void PlaylistController::showPlaylist()
{
    m_container->show();
}

void PlaylistController::togglePlaylist()
{
    if (m_container->isVisible()) {
        hidePlaylist();
    } else {
        showPlaylist();
    }
}
