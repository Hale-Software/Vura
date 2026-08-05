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
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>


class PlaylistModel;


class PlaylistController : public QObject {
    Q_OBJECT

public:
    explicit PlaylistController(QListView* view,
                                QWidget* emptyPlaylistWidget,
                                QStackedWidget* container,
                                QAction* togglePlaylistAction,
                                QObject* parent = nullptr);

    PlaylistModel* getModel() const { return m_model; }
    bool isPlaylistVisible() const { return m_container->isVisible(); }

public slots:
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

signals:
    void playTrackRequested(const QUrl &mediaUrl);
    void playlistUpdated(int mediaCount);

private slots:
    void updateEmptyState();
    void handleItemDoubleClicked(const QModelIndex &index);

private:
    void processFilePaths(const QStringList &paths, bool autoPlay, bool isLocalFile = true);

    PlaylistModel* m_model;
    QListView* m_view;
    QWidget* m_emptyPlaylistWidget;
    QStackedWidget* m_container;
    QAction* m_togglePlaylistAction;

    QMenu* m_contextMenu = nullptr;
    QAction* m_addFileAction = nullptr;
    QAction* m_addFolderAction = nullptr;
    QAction* m_clearPlaylistAction = nullptr;
    QAction* m_loadPlaylistAction = nullptr;
    QAction* m_savePlaylistAction = nullptr;
    QAction* m_videoInformationAction = nullptr;
    QAction* m_showFolderAction = nullptr;
    QAction* m_removeSelectedAction = nullptr;

    bool saveToFile(const QString &filePath) const;
    bool loadFromFile(const QString &filePath);

};
