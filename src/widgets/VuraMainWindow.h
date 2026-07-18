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
#include <QDirIterator>
#include <QCloseEvent>
#include <QAbstractItemView>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoSink>
#include <QDebug>

#include <libvura/constants.h>
#include <libvura/logger.h>
#include <libvura/settings.h>
#include <libvura/ErrorService.h>
#include <libvura/util/blogger.h>
#include <libvura/data/video-markers.h>
#include <libvura/models/playlistmodel.h>
#include <libvura/playlist/playlist.h>
//#include <libvura/playlist/MediaItem.h>
//#include <libvura/playlist/PlaylistModel.h>
//#include <libvura/playlist/PlaylistDelegate.h>

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

//#include "PlaybackController.h"
//#include "PlaylistController.h"


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

    void setMainWindowVisibility(bool state);
    void openFolderContextMenu(const QString &path);
    void openFileContextMenu(const QString &file);
    void addFileToPlaylistContextMenu(const QString &file) const;
    void addFolderToPlaylistContextMenu(const QString &path);

protected:
    void closeEvent(QCloseEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void updateVideoSlider();
    void quitProgram();

private slots:
    void playlistPositionChanged(int index);

    static void actionTestFunction();
    void actionOpenFile();
    void actionOpenFolder();
    void actionOpenMultipleFiles();
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
    void actionNext();
    void actionPrevious();
    void actionJumpForwardExtraLarge();
    void actionJumpBackwardExtraLarge();
    void actionJumpForwardLarge();
    void actionJumpBackwardLarge();
    void actionJumpForwardMedium();
    void actionJumpBackwardMedium();
    void actionJumpForwardSmall();
    void actionJumpBackwardSmall();
    void actionJumpForwardExtraSmall();
    void actionJumpBackwardExtraSmall();
    void actionTogglePlay();
    void actionRestart();


public slots:
    void sourceChanged(const QUrl &source);
    void errorOccurred(const QString &errorMessage);

private slots:
    void jump(const QModelIndex &index) const;
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void tracksChanged();
    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(float progress);
    void displayErrorMessage();
    void playbackRateChanged(qreal rate);
    void videoFrameChanged(const QVideoFrame &frame);
    void seek(int mseconds);

private:
    Ui::VuraMainWindow *ui;

    void setTrackInfo(const QString &trackInfo);
    static QString trackName(const QMediaMetaData &metaData, int index);
    void setApplicationWindowTitle();

    QVideoSink *m_videoSink = nullptr;
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;

    QList<VuraVideoMarker> m_videoMarkers;
    VideoSlider *m_videoSlider = nullptr;
    VideoSliderWidget *m_videoSliderWidget = nullptr;
    VideoControlWidget *m_videoControlWidget = nullptr;

    Playlist *m_playlist = nullptr;
    PlaylistModel *m_playlistModel = nullptr;

    //PlaylistController *m_playlistController;
    //PlaybackController *m_playbackController;
    SystemTrayWidget *m_systemTray = nullptr;

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
