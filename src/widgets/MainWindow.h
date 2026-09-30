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

#include <libvura/media/types.h>
#include <libvura/media/engine-factory.h>

#include <QMainWindow>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QComboBox;
class QDockWidget;
class QLabel;
class QListView;
class QMenu;
class QSlider;
class QToolButton;
QT_END_NAMESPACE

class MediaController;
class SeekSlider;
class SleepInhibitor;
class VideoStage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(MediaController *controller, QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void openFiles();
    void openUrl();
    void openPaths(const QList<QUrl> &urls);
    void toggleFullScreen();

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void buildActions();
    void buildTransportBar();
    void buildMenus();
    void buildPlaylistDock();
    void connectController();

    void applyCapabilities(const media::Capabilities &capabilities);
    void rebuildTrackMenus();
    void updateTimeLabels();
    void updateWindowTitle();
    void showStatus(const QString &message, int timeoutMs = 4000);

    MediaController *m_controller = nullptr;
    SleepInhibitor *m_sleepInhibitor = nullptr;

    VideoStage *m_stage = nullptr;
    SeekSlider *m_seekSlider = nullptr;
    QSlider *m_volumeSlider = nullptr;
    QLabel *m_positionLabel = nullptr;
    QLabel *m_durationLabel = nullptr;
    QComboBox *m_rateBox = nullptr;

    QAction *m_playPauseAction = nullptr;
    QAction *m_stopAction = nullptr;
    QAction *m_nextAction = nullptr;
    QAction *m_previousAction = nullptr;
    QAction *m_muteAction = nullptr;
    QAction *m_fullScreenAction = nullptr;
    QAction *m_shuffleAction = nullptr;
    QAction *m_loadSubtitleAction = nullptr;

    QMenu *m_audioTrackMenu = nullptr;
    QMenu *m_subtitleTrackMenu = nullptr;
    QMenu *m_backendMenu = nullptr;
    QActionGroup *m_repeatGroup = nullptr;

    QDockWidget *m_playlistDock = nullptr;
    QListView *m_playlistView = nullptr;

    bool m_wasMaximized = false;
};
