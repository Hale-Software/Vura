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

    m_view->setDragEnabled(true);
    m_view->setAcceptDrops(true);
    m_view->setDropIndicatorShown(true);
    m_view->setDragDropMode(QAbstractItemView::InternalMove);

    //connect(m_view, &QListView::clicked, this, &PlaylistController::itemClicked);
    connect(m_view, &QListView::doubleClicked, this, &PlaylistController::handleItemDoubleClicked);
    connect(m_view, &QListView::customContextMenuRequested, this, &PlaylistController::showContextMenu);

    connect(m_model, &PlaylistModel::rowsInserted, this, &PlaylistController::updateEmptyState);
    connect(m_model, &PlaylistModel::rowsRemoved, this, &PlaylistController::updateEmptyState);
    connect(m_model, &PlaylistModel::modelReset, this, &PlaylistController::updateEmptyState);
    updateEmptyState();
}

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
    m_contextMenu->addAction(m_addFileAction);

    m_addFolderAction = new QAction(tr("Add Folder"), m_contextMenu);
    connect(m_addFolderAction, &QAction::triggered, this, &PlaylistController::requestFolderImport);
    m_contextMenu->addAction(m_addFolderAction);

    m_contextMenu->addSeparator();

    m_loadPlaylistAction = new QAction(tr("Load Playlist..."), m_contextMenu);
    connect(m_loadPlaylistAction, &QAction::triggered, this, &PlaylistController::loadPlaylistFile);
    m_contextMenu->addAction(m_loadPlaylistAction);

    m_savePlaylistAction = new QAction(tr("Save Playlist As..."), m_contextMenu);
    connect(m_savePlaylistAction, &QAction::triggered, this, &PlaylistController::savePlaylistAs);
    m_contextMenu->addAction(m_savePlaylistAction);

    m_contextMenu->addSeparator();

    m_clearPlaylistAction = new QAction(tr("Clear Playlist"), m_contextMenu);
    connect(m_clearPlaylistAction, &QAction::triggered, this, &PlaylistController::clearPlaylist);
    m_contextMenu->addAction(m_clearPlaylistAction);

    m_contextMenu->addSeparator();

    m_videoInformationAction = new QAction(tr("Video Information"), m_contextMenu);
    m_contextMenu->addAction(m_videoInformationAction);

    m_showFolderAction = new QAction(tr("Show Folder"), m_contextMenu);
    m_contextMenu->addAction(m_showFolderAction);

    m_contextMenu->addSeparator();

    m_removeSelectedAction = new QAction(tr("Remove Selected"), m_contextMenu);
    m_contextMenu->addAction(m_removeSelectedAction);

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
    QSettings settings;
    QStringList fileList;

    const QString fileName = QFileDialog::getOpenFileName(
        m_view,
        tr("Open File"),
        settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
        "All Files (*)");

    if (!fileName.isEmpty()) {
        fileList << fileName;
        settings.setValue("lastFileDirectory", QFileInfo(fileName).path());
        processFilePaths(fileList, true);
    }
}

void PlaylistController::requestMultipleFileImport()
{
    QSettings settings;
    QStringList fileList;

    const QStringList files = QFileDialog::getOpenFileNames(
            m_view,
            tr("Open Media Files"),
            settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
            "All Files (*)"
        );

    if (!files.isEmpty()) {
        for (const QString& fileName : files) {
            fileList << fileName;
        }

        const QString& lastFile = files.last();
        settings.setValue("lastFileDirectory", QFileInfo(lastFile).path());

        processFilePaths(fileList, true);
    }
}

void PlaylistController::requestFolderImport()
{
    QSettings settings;
    QStringList fileList;

    const QString dir = QFileDialog::getExistingDirectory(
            m_view,
            tr("Open Folder"),
            settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (!dir.isEmpty()) {
        const QDir directory(dir);
        QStringList filters;
        filters << "*.mp4" << "*.mkv" << "*.avi" << "*.mp3" << "*.wav" << "*.flac";

        QFileInfoList fileInfoList = directory.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);
        for (const QFileInfo& fileInfo : fileInfoList) {
            fileList << fileInfo.absoluteFilePath();
        }
        settings.setValue("lastFileDirectory", QFileInfo(dir).path());

        if (!fileList.isEmpty())
            processFilePaths(fileList, true);
    }
}

void PlaylistController::filesDropped(const QStringList &filePaths, const bool autoPlay)
{
    processFilePaths(filePaths, autoPlay);
}

void PlaylistController::processFilePaths(const QStringList &paths, bool autoPlay)
{
    const int previousCount = m_model->rowCount();

    if (paths.isEmpty()) return;

    for (const QString& path : paths) {
        QFileInfo fileInfo(path);
        m_model->addItem({fileInfo.baseName(), path, 0, 0});
    }

    // Always auto-play if the playlist was completely empty
    if (previousCount == 0 && m_model->rowCount() > 0) {
        m_view->setCurrentIndex(m_model->index(0, 0));
        emit playTrackRequested(QUrl::fromLocalFile(m_model->currentURL(0)));
    }
    // Otherwise, only interrupt playback and jump if autoPlay is explicitly true
    else if (autoPlay && m_model->rowCount() > previousCount) {
        m_view->setCurrentIndex(m_model->index(previousCount, 0));
        emit playTrackRequested(QUrl::fromLocalFile(m_model->currentURL(previousCount)));
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

void PlaylistController::savePlaylistAs()
{
    if (m_model->rowCount() == 0) {
        QMessageBox::information(m_view, tr("Save Playlist"), tr("The playlist is empty."));
        return;
    }

    const QString filePath = QFileDialog::getSaveFileName(
        m_view,
        tr("Save Playlist"),
        QDir::homePath(),
        tr("Playlist Files (*.m3u);;All Files (*.*)")
    );

    if (!filePath.isEmpty()) {
        if (!saveToFile(filePath)) {
            QMessageBox::critical(m_view, tr("Error"), tr("Could not save playlist file."));
        }
    }
}

void PlaylistController::loadPlaylistFile()
{
    const QString filePath = QFileDialog::getOpenFileName(
        m_view,
        tr("Open Playlist"),
        QDir::homePath(),
        tr("Playlist Files (*.m3u *.m3u8);;All Files (*.*)")
    );

    if (!filePath.isEmpty()) {
        if (!loadFromFile(filePath)) {
            QMessageBox::critical(m_view, tr("Error"), tr("Could not read playlist file."));
        }
    }
}

bool PlaylistController::saveToFile(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << "#EXTM3U\n"; // Standard M3U header

    // Iterate through the model and write each file path
    for (int i = 0; i < m_model->rowCount(); ++i) {
        QString mediaPath = m_model->currentURL(i);
        if (!mediaPath.isEmpty()) {
            out << mediaPath << "\n";
        }
    }

    file.close();
    return true;
}

bool PlaylistController::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    QStringList pathsToAdd;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        // Skip empty lines and M3U comments/metadata (lines starting with #)
        if (!line.isEmpty() && !line.startsWith("#")) {
            pathsToAdd << line;
        }
    }

    file.close();

    // Use our existing method to add the files without interrupting current playback
    if (!pathsToAdd.isEmpty()) {
        processFilePaths(pathsToAdd, false);
        return true;
    }

    return false;
}
