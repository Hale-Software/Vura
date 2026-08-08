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

#include <QObject>
#include <QWidget>
#include <QListView>
#include <QStackedWidget>
#include <QStringList>
#include <QMenu>
#include <QAction>


class QRandomGenerator;
class QFileDialog;
class QDir;
class QFileInfo;
class QSettings;
class QStandardPaths;
class QFile;
class QTextStream;
class QMessageBox;
class QUrl;
class QDesktopServices;
class QDebug;

class PlaylistModel;

class PlaylistController : public QObject {
    Q_OBJECT

public:
    enum PlaybackMode
    {
        DoNotLoopPlaylist,
        LoopPlaylist,
        LoopCurrentVideo,
        Shuffle
    };
    Q_ENUM(PlaybackMode)

    enum PlaylistViewMode
    {
        Icons,
        DetailedList,
        List
    };
    Q_ENUM(PlaylistViewMode)

    explicit PlaylistController(QListView* view,
                                QWidget* emptyPlaylistWidget,
                                QStackedWidget* container,
                                QAction* togglePlaylistAction,
                                QObject* parent = nullptr);

    PlaylistModel* getModel() const { return m_model; }
    bool isPlaylistVisible() const { return m_container->isVisible(); }
    PlaybackMode playbackMode() const { return m_playbackMode; };
    void setPlaybackMode(PlaybackMode mode);

public slots:
    bool isLastTrack() const;
    void showContextMenu(const QPoint& pos);
    void requestFileImport();
    void requestMultipleFileImport();
    void requestFolderImport();
    void filesDropped(const QStringList &filePaths, bool autoPlay = false);
    void addNetworkVideo(const QString &mediaUrl);
    void clearPlaylist() const;
    void hidePlaylist() const;
    void showPlaylist() const;
    void togglePlaylist() const;
    void nextTrack();
    void previousTrack();
    void savePlaylistAs() const;
    void loadPlaylistFile();
    void contextMenuNoLoop();
    void contextMenuLoopTrack();
    void contextMenuLoopPlaylist();
    void contextMenuShuffle();

signals:
    void playTrackRequested(const QUrl &mediaUrl);
    void playlistUpdated(int mediaCount);
    void playbackModeChanged(PlaybackMode mode);
    void playlistCleared();

private slots:
    void updateEmptyState();
    void handleItemDoubleClicked(const QModelIndex &index);
    void playAction();
    void streamAction();
    void saveAction();
    void informationAction();
    void showContainingFolderAction();
    void removeSelectedAction();
    void addFileAction();
    void addFolderAction();
    void savePlaylistAction();
    void clearPlaylistAction();
    void sortByTitleAscendingAction();
    void sortByTitleDescendingAction();
    void sortByTrackNumberAscendingAction();
    void sortByTrackNumberDescendingAction();
    void displaySizeIncreaseAction();
    void displaySizeDecreaseAction();
    void playlistViewIconsAction();
    void playlistViewDetailedListAction();
    void playlistViewListAction();

private:
    void processFilePaths(const QStringList &paths, bool autoPlay, bool isLocalFile = true);
    bool saveToFile(const QString &filePath) const;
    bool loadFromFile(const QString &filePath);
    void resetUnplayedIndexes();
    int getNextShuffleIndex(int currentIndex);

    PlaylistModel* m_model;
    PlaybackMode m_playbackMode = DoNotLoopPlaylist;
    PlaylistViewMode m_playlistViewMode = DetailedList;
    QList<int> m_shuffledIndexes;
    QList<int> m_unplayedIndexes;
    QListView* m_view;
    QWidget* m_emptyPlaylistWidget;
    QStackedWidget* m_container;
    QAction* m_togglePlaylistAction;
    int selectedIndex = -1;

    QMenu* m_contextMenu = nullptr;
    QAction* m_playAction = nullptr;
    QAction* m_streamAction = nullptr;
    QAction* m_saveAction = nullptr;
    QAction* m_informationAction = nullptr;
    QAction* m_showContainingFolderAction = nullptr;
    QAction* m_removeSelectedAction = nullptr;

    QAction* m_addFileAction = nullptr;
    QAction* m_addFolderAction = nullptr;
    QAction* m_savePlaylistAction = nullptr;
    QAction* m_clearPlaylistAction = nullptr;

    QMenu* m_sortByMenu = nullptr;
    QAction* m_sortByTitleAscendingAction = nullptr;
    QAction* m_sortByTitleDescendingAction = nullptr;
    QAction* m_sortByTrackNumberAscendingAction = nullptr;
    QAction* m_sortByTrackNumberDescendingAction = nullptr;

    QMenu* m_playbackModeMenu = nullptr;
    QAction* m_noLoopAction = nullptr;
    QAction* m_loopPlaylistAction = nullptr;
    QAction* m_loopCurrentVideoAction = nullptr;
    QAction* m_shuffleAction = nullptr;

    QMenu* m_displaySizeMenu = nullptr;
    QAction* m_displaySizeIncreaseAction = nullptr;
    QAction* m_displaySizeDecreaseAction = nullptr;

    QMenu* m_playlistViewMenu = nullptr;
    QAction* m_playlistViewIconsAction = nullptr;
    QAction* m_playlistViewDetailedListAction = nullptr;
    QAction* m_playlistViewListAction = nullptr;

};
