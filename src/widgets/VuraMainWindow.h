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

#include <QMainWindow>
#include <QWidget>
#include <QSettings>
#include <QStringList>
#include <QFileDialog>
#include <QMenuBar>
#include <QPointer>
#include <QSplitter>
#include <QItemSelectionModel>
#include <QMimeData>
#include <QDropEvent>
#include <QAction>
#include <QMessageBox>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>

#include <libvura/constants.h>
#include <libvura/logger.h>
#include <libvura/settings.h>
#include <libvura/ErrorService.h>
#include <libvura/util/blogger.h>
#include <libvura/data/video-markers.h>
#include <libvura/playlist/MediaItem.h>
#include <libvura/playlist/PlaylistModel.h>
#include <libvura/playlist/PlaylistDelegate.h>

#include "HelpDialog.h"
#include "AboutDialog.h"
#include "UpdateDialog.h"
#include "SettingsWindow.h"
#include "FeedbackDialog.h"
#include "LogViewerDialog.h"
#include "MarkerEditDialog.h"
#include "ConvertMediaDialog.h"
#include "MediaInformationDialog.h"
#include "UpdateChecker.h"

#include "SystemTrayWidget.h"
#include "VideoSliderWidget.h"
#include "VideoControlWidget.h"

#include "PlaybackController.h"
#include "PlaylistController.h"


namespace Ui {
class VuraMainWindow;
}


class VuraMainWindow : public QMainWindow
{
    Q_OBJECT

    friend class HelpDialog;
    friend class AboutDialog;
    friend class UpdateDialog;
    friend class SettingsWindow;
    friend class FeedbackDialog;
    friend class LogViewerDialog;
    friend class MarkerEditDialog;
    friend class ConvertMediaDialog;
    friend class MediaInformationDialog;

public:
    explicit VuraMainWindow(QWidget *parent = nullptr);

    //void setWindowVisibility(bool state);
    //void loadStylesheet();

    //void openFolder(const QString &path);
    //void openFile(const QUrl &path);
    //void openFile(const QString &path);

    //void loadPlaylist(const QUrl &url);
    //void loadPlaylist(const QString &path);
    //void savePlaylist(const QString &path);
    //void savePlaylist(const QUrl &url);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void updateVideoSlider();
    void quitProgram();

private slots:
    void actionTestFunction();
    void actionEmergencyClose();
    void actionShowLogViewer();
    void actionToggleFullscreen();
    void actionShowSettings();
    //void actionShowFeedback();
    //void actionShowAbout();
    void actionExit();
    void actionTogglePlaylist() const;
    void actionToggleVideoControls();
    void actionJumpForward();
    void actionJumpBackward();
    void actionVolumeUp();
    void actionVolumeDown();


public slots:
    void sourceChanged(const QUrl &source);
    void errorOccurred(const QString &errorMessage);

    //void showMediaInformationDialog();
    //void showMarkerEditDialog();
    //void showConvertMediaDialog();
    //void showLogViewerDialog();
    //void showSettingsDialog();
    //void showFeedbackDialog();
    //void showAboutDialog();
    //void showHelpDialog();
    //void showUpdateDialog();

    //void checkForUpdates();
    //void updateMarkerActionsEnabled();

    //void exitApplication();
    //void togglePlaylistVisibility();
    //void toggleVideoControlsVisibility();


private:
    Ui::VuraMainWindow *ui;

    void setTrackInfo(const QString &trackInfo);
    static QString trackName(const QMediaMetaData &metaData, int index);
    void setApplicationWindowTitle();

    //void initPlayback();
    //void initPlaylist();

    //void initializeApplication();
    //void initializeSystemTray();
    //void initializeMenuBar();
    //void initializeVideoSliderWidget();
    //void initializeVideoControlWidget();

    //double getSliderPercent();
    //VuraVideoMarker findNearestVisibleMarker(double sliderPercent, double markerRange);

    QList<VuraVideoMarker> m_videoMarkers;
    VideoSlider *m_videoSlider = nullptr;
    VideoSliderWidget *m_videoSliderWidget = nullptr;
    VideoControlWidget *m_videoControlWidget = nullptr;

    PlaylistController *m_playlistController;
    PlaybackController *m_playbackController;
    SystemTrayWidget *m_systemTray;

    QPointer<HelpDialog> m_helpDialog;
    QPointer<AboutDialog> m_aboutDialog;
    QPointer<UpdateDialog> m_updateDialog;
    QPointer<SettingsWindow> m_settingsWindow;
    QPointer<FeedbackDialog> m_feedbackDialog;
    QPointer<LogViewerDialog> m_logViewerDialog;
    QPointer<MarkerEditDialog> m_markerEditDialog;

    QString m_trackInfo;
    QString m_statusInfo;
    qint64 m_lastPosition = 0;
    bool m_showingVideoControls = false;

};
