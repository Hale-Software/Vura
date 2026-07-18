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
#include <QKeyEvent>
#include <QDebug>

#include <libvura/constants.h>
#include <libvura/logger.h>
#include <libvura/settings.h>
#include <libvura/ErrorService.h>
#include <libvura/util/blogger.h>
#include <libvura/data/video-markers.h>
#include <libvura/playlist/playlist-model.h>
#include <libvura/playlist/playlist-delegate.h>

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

    void setMainWindowVisibility(bool state);
    void openFile(const QString &file) const;
    void openFolder(const QString &path) const;

protected:
    void closeEvent(QCloseEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void updateVideoSlider();
    void quitProgram();

private slots:
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
    void actionToggleVideoControls();


public slots:
    void sourceChanged(const QUrl &source);
    void errorOccurred(const QString &errorMessage);

private:
    Ui::VuraMainWindow *ui;

    void setTrackInfo(const QString &trackInfo);
    static QString trackName(const QMediaMetaData &metaData, int index);
    void setApplicationWindowTitle();

    QList<VuraVideoMarker> m_videoMarkers;
    VideoSlider *m_videoSlider = nullptr;
    VideoSliderWidget *m_videoSliderWidget = nullptr;
    VideoControlWidget *m_videoControlWidget = nullptr;

    PlaylistController *m_playlistController = nullptr;
    PlaybackController *m_playbackController = nullptr;
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
    bool m_wasPlaylistShowing = false;

};
