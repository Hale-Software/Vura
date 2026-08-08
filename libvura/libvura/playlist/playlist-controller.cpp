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

#include <QRandomGenerator>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QUrl>
#include <QDesktopServices>
#include <QDebug>

#include "playlist-controller.h"
#include "../models/playlist-model.h"
#include "playlist-delegate.h"


PlaylistController::PlaylistController(QListView* view, QWidget* emptyPlaylistWidget, QStackedWidget* container, QAction* togglePlaylistAction, QObject* parent)
    : QObject(parent),
      m_model(new PlaylistModel()),
      m_view(view),
      m_emptyPlaylistWidget(emptyPlaylistWidget),
      m_container(container),
      m_togglePlaylistAction(togglePlaylistAction)
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(new PlaylistDelegate());

    m_view->setDragEnabled(true);
    m_view->setAcceptDrops(true);
    m_view->setDropIndicatorShown(true);
    m_view->setDragDropMode(QAbstractItemView::InternalMove);

    connect(m_view, &QListView::doubleClicked, this, &PlaylistController::handleItemDoubleClicked);
    connect(m_view, &QListView::customContextMenuRequested, this, &PlaylistController::showContextMenu);
    connect(m_model, &PlaylistModel::rowsInserted, this, &PlaylistController::updateEmptyState);
    connect(m_model, &PlaylistModel::rowsRemoved, this, &PlaylistController::updateEmptyState);
    connect(m_model, &PlaylistModel::modelReset, this, &PlaylistController::updateEmptyState);
    connect(m_model, &PlaylistModel::rowsInserted, this, &PlaylistController::resetUnplayedIndexes);
    connect(m_model, &PlaylistModel::rowsRemoved, this, &PlaylistController::resetUnplayedIndexes);
    connect(m_model, &PlaylistModel::modelReset, this, &PlaylistController::resetUnplayedIndexes);

    updateEmptyState();
}

void PlaylistController::setPlaybackMode(const PlaybackMode mode)
{
    if (m_playbackMode == mode) return;
    m_playbackMode = mode;

    if (m_playbackMode == Shuffle) {
        resetUnplayedIndexes();
        int currentIndex = m_view->currentIndex().row();
        if (currentIndex >= 0) {
            m_unplayedIndexes.removeAll(currentIndex);
        }
    }

    emit playbackModeChanged(mode);
}

bool PlaylistController::isLastTrack() const
{
    if (!m_model || m_model->rowCount() == 0)
        return false;

    const int currentIndex = m_view->currentIndex().row();
    return (currentIndex >= m_model->rowCount() - 1);
}

void PlaylistController::showContextMenu(const QPoint &pos)
{
    const QModelIndex index = m_view->indexAt(pos);
    m_contextMenu = new QMenu(tr("Context Menu"), m_view);

    if (index.isValid()) {
        selectedIndex = index.row();

        m_playAction = new QAction(tr("Play"), m_contextMenu);
        connect(m_playAction, &QAction::triggered, this, &PlaylistController::playAction);
        m_contextMenu->addAction(m_playAction);

        m_streamAction = new QAction(tr("Stream..."), m_contextMenu);
        connect(m_streamAction, &QAction::triggered, this, &PlaylistController::streamAction);
        m_contextMenu->addAction(m_streamAction);

        m_saveAction = new QAction(tr("Save..."), m_contextMenu);
        connect(m_saveAction, &QAction::triggered, this, &PlaylistController::saveAction);
        m_contextMenu->addAction(m_saveAction);

        m_informationAction = new QAction(tr("Information..."), m_contextMenu);
        connect(m_informationAction, &QAction::triggered, this, &PlaylistController::informationAction);
        m_contextMenu->addAction(m_informationAction);

        m_contextMenu->addSeparator();

        m_showContainingFolderAction = new QAction(tr("Show Containing Folder..."), m_contextMenu);
        connect(m_showContainingFolderAction, &QAction::triggered, this, &PlaylistController::showContainingFolderAction);
        m_contextMenu->addAction(m_showContainingFolderAction);

        m_contextMenu->addSeparator();
    } else {
        selectedIndex = -1;
    }

    m_addFileAction = new QAction(tr("Add File..."), m_contextMenu);
    connect(m_addFileAction, &QAction::triggered, this, &PlaylistController::requestFileImport);
    m_contextMenu->addAction(m_addFileAction);

    m_addFolderAction = new QAction(tr("Add Folder..."), m_contextMenu);
    connect(m_addFolderAction, &QAction::triggered, this, &PlaylistController::requestFolderImport);
    m_contextMenu->addAction(m_addFolderAction);

    m_contextMenu->addSeparator();

    m_savePlaylistAction = new QAction(tr("Save Playlist to File..."), m_contextMenu);
    connect(m_savePlaylistAction, &QAction::triggered, this, &PlaylistController::savePlaylistAs);
    m_contextMenu->addAction(m_savePlaylistAction);

    m_contextMenu->addSeparator();

    if (index.isValid()) {
        m_removeSelectedAction = new QAction(tr("Remove Selected"), m_contextMenu);
        connect(m_removeSelectedAction, &QAction::triggered, this, &PlaylistController::removeSelectedAction);
        m_contextMenu->addAction(m_removeSelectedAction);
    }
    m_clearPlaylistAction = new QAction(tr("Clear Playlist"), m_contextMenu);
    connect(m_clearPlaylistAction, &QAction::triggered, this, &PlaylistController::clearPlaylistAction);
    m_contextMenu->addAction(m_clearPlaylistAction);

    m_contextMenu->addSeparator();

    m_sortByMenu = new QMenu(tr("Sort By"), m_contextMenu);

    m_sortByTitleAscendingAction = new QAction(tr("Title Ascending"), m_sortByMenu);
    connect(m_sortByTitleAscendingAction, &QAction::triggered, this, &PlaylistController::sortByTitleAscendingAction);
    m_sortByMenu->addAction(m_sortByTitleAscendingAction);

    m_sortByTitleDescendingAction = new QAction(tr("Title Descending"), m_sortByMenu);
    connect(m_sortByTitleDescendingAction, &QAction::triggered, this, &PlaylistController::sortByTitleDescendingAction);
    m_sortByMenu->addAction(m_sortByTitleDescendingAction);

    m_sortByTrackNumberAscendingAction = new QAction(tr("Track Number Ascending"), m_sortByMenu);
    connect(m_sortByTrackNumberAscendingAction, &QAction::triggered, this, &PlaylistController::sortByTrackNumberAscendingAction);
    m_sortByMenu->addAction(m_sortByTrackNumberAscendingAction);

    m_sortByTrackNumberDescendingAction = new QAction(tr("Track Number Descending"), m_sortByMenu);
    connect(m_sortByTrackNumberDescendingAction, &QAction::triggered, this, &PlaylistController::sortByTrackNumberDescendingAction);
    m_sortByMenu->addAction(m_sortByTrackNumberDescendingAction);

    m_contextMenu->addMenu(m_sortByMenu);

    m_playbackModeMenu = new QMenu(tr("Playback Mode"), m_contextMenu);

    m_noLoopAction = new QAction(tr("No Loop"), m_playbackModeMenu);
    m_noLoopAction->setCheckable(true);
    m_noLoopAction->setChecked(m_playbackMode == DoNotLoopPlaylist);
    connect(m_noLoopAction, &QAction::triggered, this, &PlaylistController::contextMenuNoLoop);
    m_playbackModeMenu->addAction(m_noLoopAction);

    m_loopPlaylistAction = new QAction(tr("Loop Playlist"), m_playbackModeMenu);
    m_loopPlaylistAction->setCheckable(true);
    m_loopPlaylistAction->setChecked(m_playbackMode == LoopPlaylist);
    connect(m_loopPlaylistAction, &QAction::triggered, this, &PlaylistController::contextMenuLoopPlaylist);
    m_playbackModeMenu->addAction(m_loopPlaylistAction);

    m_loopCurrentVideoAction = new QAction(tr("Loop Current Video"), m_playbackModeMenu);
    m_loopCurrentVideoAction->setCheckable(true);
    m_loopCurrentVideoAction->setChecked(m_playbackMode == LoopCurrentVideo);
    connect(m_loopCurrentVideoAction, &QAction::triggered, this, &PlaylistController::contextMenuLoopTrack);
    m_playbackModeMenu->addAction(m_loopCurrentVideoAction);

    m_shuffleAction = new QAction(tr("Shuffle"), m_playbackModeMenu);
    m_shuffleAction->setCheckable(true);
    m_shuffleAction->setChecked(m_playbackMode == Shuffle);
    connect(m_shuffleAction, &QAction::triggered, this, &PlaylistController::contextMenuShuffle);
    m_playbackModeMenu->addAction(m_shuffleAction);

    m_contextMenu->addMenu(m_playbackModeMenu);

    m_displaySizeMenu = new QMenu(tr("Display Size"), m_contextMenu);

    m_displaySizeIncreaseAction = new QAction(tr("Increase"), m_displaySizeMenu);
    connect(m_displaySizeIncreaseAction, &QAction::triggered, this, &PlaylistController::displaySizeIncreaseAction);
    m_displaySizeMenu->addAction(m_displaySizeIncreaseAction);

    m_displaySizeDecreaseAction = new QAction(tr("Decrease"), m_displaySizeMenu);
    connect(m_displaySizeDecreaseAction, &QAction::triggered, this, &PlaylistController::displaySizeDecreaseAction);
    m_displaySizeMenu->addAction(m_displaySizeDecreaseAction);

    m_contextMenu->addMenu(m_displaySizeMenu);

    m_playlistViewMenu = new QMenu(tr("Playlist View"), m_contextMenu);

    m_playlistViewIconsAction = new QAction(tr("Icons"), m_playlistViewMenu);
    m_playlistViewIconsAction->setCheckable(true);
    m_playlistViewIconsAction->setChecked(m_playlistViewMode == Icons);
    connect(m_playlistViewIconsAction, &QAction::triggered, this, &PlaylistController::playlistViewIconsAction);
    m_playlistViewMenu->addAction(m_playlistViewIconsAction);

    m_playlistViewDetailedListAction = new QAction(tr("Detailed List"), m_playlistViewMenu);
    m_playlistViewDetailedListAction->setCheckable(true);
    m_playlistViewDetailedListAction->setChecked(m_playlistViewMode == DetailedList);
    connect(m_playlistViewDetailedListAction, &QAction::triggered, this, &PlaylistController::playlistViewDetailedListAction);
    m_playlistViewMenu->addAction(m_playlistViewDetailedListAction);

    m_playlistViewListAction = new QAction(tr("List"), m_playlistViewMenu);
    m_playlistViewListAction->setCheckable(true);
    m_playlistViewListAction->setChecked(m_playlistViewMode == List);
    connect(m_playlistViewListAction, &QAction::triggered, this, &PlaylistController::playlistViewListAction);
    m_playlistViewMenu->addAction(m_playlistViewListAction);

    m_contextMenu->addMenu(m_playlistViewMenu);

    m_contextMenu->exec(m_view->mapToGlobal(pos));
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

void PlaylistController::addNetworkVideo(const QString &mediaUrl)
{
    QStringList urlList;
    urlList.append(mediaUrl);

    processFilePaths(urlList, true, false);
}

void PlaylistController::clearPlaylist() const
{
    m_model->clear();
}

void PlaylistController::hidePlaylist() const
{
    m_togglePlaylistAction->setChecked(false);
    m_container->hide();
}

void PlaylistController::showPlaylist() const
{
    m_togglePlaylistAction->setChecked(true);
    m_container->show();
}

void PlaylistController::togglePlaylist() const
{
    if (m_container->isVisible()) {
        hidePlaylist();
    } else {
        showPlaylist();
    }
}

void PlaylistController::nextTrack()
{
    const int count = m_model->rowCount();
    if (count == 0) return;

    int currentIndex = m_view->currentIndex().row();
    if (currentIndex < 0) currentIndex = 0;

    int nextIndex = currentIndex;

    switch (m_playbackMode) {
        case LoopCurrentVideo:
            nextIndex = currentIndex;
            break;

        case Shuffle:
            nextIndex = getNextShuffleIndex(currentIndex);
            break;

        case DoNotLoopPlaylist:
            if (currentIndex >= count - 1) return;
            nextIndex = currentIndex + 1;
            break;

        case LoopPlaylist:
        default:
            nextIndex = (currentIndex + 1) % count;
            break;
    }

    const QString filePath = m_model->currentURL(nextIndex);
    if (!filePath.isEmpty()) {
        m_view->setCurrentIndex(m_model->index(nextIndex, 0));
        if (m_model->getItemAt(nextIndex).isLocalFile) {
            emit playTrackRequested(QUrl::fromLocalFile(filePath));
        } else {
            emit playTrackRequested(QUrl(filePath));
        }
    }
}

void PlaylistController::previousTrack()
{
    const int count = m_model->rowCount();
    if (count == 0) return;

    int currentIndex = m_view->currentIndex().row();
    if (currentIndex < 0) currentIndex = 0;

    int previousIndex = currentIndex;

    switch (m_playbackMode) {
        case LoopCurrentVideo:
            previousIndex = currentIndex;
            break;

        case Shuffle: {
            if (count > 1) {
                do {
                    previousIndex = QRandomGenerator::global()->bounded(count);
                } while (previousIndex == currentIndex);
            } else {
                previousIndex = 0;
            }
            break;
        }

        case DoNotLoopPlaylist:
            if (currentIndex <= 0) {
                return;
            }
            previousIndex = currentIndex - 1;
            break;

        case LoopPlaylist:
        default:
            previousIndex = (currentIndex - 1 + count) % count;
            break;
    }

    const QString filePath = m_model->currentURL(previousIndex);
    if (!filePath.isEmpty()) {
        m_view->setCurrentIndex(m_model->index(previousIndex, 0));
        if (m_model->getItemAt(previousIndex).isLocalFile) {
            emit playTrackRequested(QUrl::fromLocalFile(filePath));
        } else {
            emit playTrackRequested(QUrl(filePath));
        }
    }
}

void PlaylistController::savePlaylistAs() const
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

void PlaylistController::contextMenuNoLoop()
{
    if (m_playbackMode == DoNotLoopPlaylist) {
        m_noLoopAction->setChecked(true);
        return;
    }

    m_playbackMode = DoNotLoopPlaylist;
    m_noLoopAction->setChecked(true);
    m_loopPlaylistAction->setChecked(false);
    m_loopCurrentVideoAction->setChecked(false);
    m_shuffleAction->setChecked(false);
    emit playbackModeChanged(m_playbackMode);
}

void PlaylistController::contextMenuLoopTrack()
{
    if (m_playbackMode == LoopCurrentVideo) {
        m_loopCurrentVideoAction->setChecked(true);
        return;
    }

    m_playbackMode = LoopCurrentVideo;
    m_noLoopAction->setChecked(false);
    m_loopPlaylistAction->setChecked(false);
    m_loopCurrentVideoAction->setChecked(true);
    m_shuffleAction->setChecked(false);
    emit playbackModeChanged(m_playbackMode);
}

void PlaylistController::contextMenuLoopPlaylist()
{
    if (m_playbackMode == LoopPlaylist) {
        m_loopPlaylistAction->setChecked(true);
        return;
    }

    m_playbackMode = LoopPlaylist;
    m_noLoopAction->setChecked(false);
    m_loopPlaylistAction->setChecked(true);
    m_loopCurrentVideoAction->setChecked(false);
    m_shuffleAction->setChecked(false);
    emit playbackModeChanged(m_playbackMode);
}

void PlaylistController::contextMenuShuffle()
{
    if (m_playbackMode == Shuffle) {
        m_shuffleAction->setChecked(true);
        return;
    }

    m_playbackMode = Shuffle;
    m_noLoopAction->setChecked(false);
    m_loopPlaylistAction->setChecked(false);
    m_loopCurrentVideoAction->setChecked(false);
    m_shuffleAction->setChecked(true);
    emit playbackModeChanged(m_playbackMode);
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

void PlaylistController::handleItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    const QString filePath = m_model->currentURL(index.row());

    if (!filePath.isEmpty()) {
        if (m_model->getItemAt(index.row()).isLocalFile) {
            emit playTrackRequested(QUrl::fromLocalFile(filePath));
        } else {
            emit playTrackRequested(QUrl(filePath));
        }
    }
}

void PlaylistController::playAction()
{
    if (selectedIndex == -1) return;
    if (m_model->getItemAt(selectedIndex).isLocalFile) {
        emit playTrackRequested(QUrl::fromLocalFile(m_model->currentURL(selectedIndex)));
    } else {
        emit playTrackRequested(QUrl(m_model->currentURL(selectedIndex)));
    }
}

void PlaylistController::streamAction()
{
    QMessageBox::information(m_view, tr("Stream"), tr("This feature is not yet implemented."));
}

void PlaylistController::saveAction()
{
    QMessageBox::information(m_view, tr("Save"), tr("This feature is not yet implemented."));
}

void PlaylistController::informationAction()
{
    QMessageBox::information(m_view, tr("Information"), tr("This feature is not yet implemented."));
}

void PlaylistController::showContainingFolderAction()
{
    if (selectedIndex == -1) return;
    if (m_model->getItemAt(selectedIndex).isLocalFile) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_model->currentURL(selectedIndex)).toLocalFile());
    } else {
        QMessageBox::warning(m_view, tr("Open Containing Folder"), tr("This file is not a local file."));
    }
}

void PlaylistController::removeSelectedAction()
{
    if (selectedIndex == -1) return;
    m_model->removeRow(selectedIndex);
    updateEmptyState();
}

void PlaylistController::addFileAction()
{
    requestFileImport();
}

void PlaylistController::addFolderAction()
{
    requestFolderImport();
}

void PlaylistController::savePlaylistAction()
{
    savePlaylistAs();
}

void PlaylistController::clearPlaylistAction()
{
    m_model->clear();
    emit playlistCleared();
    updateEmptyState();
}

void PlaylistController::sortByTitleAscendingAction() {}

void PlaylistController::sortByTitleDescendingAction() {}

void PlaylistController::sortByTrackNumberAscendingAction() {}

void PlaylistController::sortByTrackNumberDescendingAction() {}

void PlaylistController::displaySizeIncreaseAction() {}

void PlaylistController::displaySizeDecreaseAction() {}

void PlaylistController::playlistViewIconsAction() {}

void PlaylistController::playlistViewDetailedListAction() {}

void PlaylistController::playlistViewListAction() {}

void PlaylistController::processFilePaths(const QStringList &paths, const bool autoPlay, const bool isLocalFile)
{
    const int previousCount = m_model->rowCount();

    if (paths.isEmpty()) return;

    for (const QString& path : paths) {
        QFileInfo fileInfo(path);
        m_model->addItem({.title = fileInfo.baseName(), .filePath = path, .duration = 0, .progress = 0, .isLocalFile = isLocalFile});
    }

    if (previousCount == 0 && m_model->rowCount() > 0) {
        hidePlaylist();
        m_view->setCurrentIndex(m_model->index(0, 0));
        if (isLocalFile) {
            emit playTrackRequested(QUrl::fromLocalFile(m_model->currentURL(0)));
        } else {
            emit playTrackRequested(QUrl(m_model->currentURL(0)));
        }
    }
    else if (autoPlay && m_model->rowCount() > previousCount) {
        m_view->setCurrentIndex(m_model->index(previousCount, 0));
        if (isLocalFile) {
            emit playTrackRequested(QUrl::fromLocalFile(m_model->currentURL(previousCount)));
        } else {
            emit playTrackRequested(QUrl(m_model->currentURL(previousCount)));
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
    out << "#EXTM3U\n";

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

        if (!line.isEmpty() && !line.startsWith("#")) {
            pathsToAdd << line;
        }
    }

    file.close();

    if (!pathsToAdd.isEmpty()) {
        processFilePaths(pathsToAdd, false);
        return true;
    }

    return false;
}

void PlaylistController::resetUnplayedIndexes()
{
    m_unplayedIndexes.clear();
    const int count = m_model->rowCount();
    for (int i = 0; i < count; ++i) {
        m_unplayedIndexes.append(i);
    }
}

int PlaylistController::getNextShuffleIndex(int currentIndex)
{
    const int count = m_model->rowCount();
    if (count == 0) return 0;

    if (m_unplayedIndexes.isEmpty()) {
        resetUnplayedIndexes();
        if (count > 1) {
            m_unplayedIndexes.removeAll(currentIndex);
        }
    }

    int randomIndex = QRandomGenerator::global()->bounded(m_unplayedIndexes.size());
    int selectedTrackIndex = m_unplayedIndexes.takeAt(randomIndex);

    return selectedTrackIndex;
}
