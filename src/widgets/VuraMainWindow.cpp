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

#include "VuraMainWindow.h"
#include "ui_VuraMainWindow.h"

#include "PlaylistEmptyStateWidget.h"

#include <ui-config.h>

#include <libvura/io/playlist-io.h>
#include <libvura/logging/logger.h>
#include <libvura/media-controller.h>
#include <libvura/models/playlist.h>
#include <libvura/media-engine/video-stage.h>
#include <libvura/platform/platform.h>

#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QListView>
#include <QMenuBar>
#include <QMimeData>
#include <QSettings>
#include <QSlider>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QStyle>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtMath>
#include <QProgressDialog>
#include <QDir>
#include <QDirIterator>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>


#ifdef MEDIA_HAVE_QTMULTIMEDIA
#include <QAudio>
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#endif


namespace {

/// A volume slider must be perceptual or the top quarter of its travel does
/// nothing audible. Qt ships the conversion; without Qt Multimedia we use
/// the same cubic curve by hand.
qreal sliderToLinear(int sliderValue)
{
    const qreal fraction = qreal(sliderValue) / 100.0;
#ifdef MEDIA_HAVE_QTMULTIMEDIA
    return QAudio::convertVolume(fraction, QAudio::LogarithmicVolumeScale,
                                 QAudio::LinearVolumeScale);
#else
    return fraction * fraction * fraction;
#endif
}

int linearToSlider(qreal linear)
{
#ifdef MEDIA_HAVE_QTMULTIMEDIA
    const qreal fraction = QAudio::convertVolume(linear, QAudio::LinearVolumeScale,
                                                 QAudio::LogarithmicVolumeScale);
#else
    const qreal fraction = std::cbrt(linear);
#endif
    return int(qRound(fraction * 100.0));
}

}


VuraMainWindow::VuraMainWindow(MediaController *controller, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::VuraMainWindow),
      m_controller(controller),
      m_sleepInhibitor(new SleepInhibitor(this))
{
    ui->setupUi(this);
    setAcceptDrops(true);

    m_stage = new VideoStage(this);
    ui->verticalLayout->addWidget(m_stage);

    m_controller->setVideoOutputProvider([this](media::Engine *engine) { return m_stage->outputFor(engine); });

    initSystemTray();
    buildMenus();
    buildPlaylistDock();
    initUI();
    connectController();
    initMisc();

    applyCapabilities(m_controller->capabilities());

    HotkeyManager::instance()->registerWindow(this);
    HotkeyManager::instance()->load();

    qCDebug(Core) << "Application Initialized!";
    qCInfo(Core) << "Vura Version: " << VURA_VERSION_STRING;
}

VuraMainWindow::~VuraMainWindow() = default;

// Application Events
void VuraMainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    m_videoMarkerController->saveVideoMarkers();
    event->accept();
}

bool VuraMainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    if (HotkeyManager::instance()->handleNativeEvent(eventType, message, result))
        return true;

#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        const auto msg = static_cast<MSG *>(message);
        if (msg->message == WM_NCLBUTTONDBLCLK)
        {
            resize(887, 530);
            return true;
        }
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void VuraMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void VuraMainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    if (m_replacePlaylist)
        openPaths(urls);
    else
        m_controller->enqueue(urls);

    event->acceptProposedAction();
}

void VuraMainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Escape:
            if (isFullScreen()) {
                actionToggleFullscreen();
                return;
            }
            break;

        default:
            break;
    }
    QMainWindow::keyPressEvent(event);
}

void VuraMainWindow::changeEvent(QEvent *event)
{
    const QSettings settings;

    if (event->type() == QEvent::WindowStateChange) {
        const auto *stateEvent = dynamic_cast<QWindowStateChangeEvent*>(event);
        if (!(stateEvent->oldState() & Qt::WindowMinimized) && (windowState() & Qt::WindowMinimized)) {
            if (m_controller && settings.value("pausePlaybackWhenMinimized", true).toBool())
                m_controller->pause();
        }
    }
    QMainWindow::changeEvent(event);
}

bool VuraMainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        const auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);

        QPointF localPos = mouseEvent->position();

        this->unsetCursor();
        m_videoSliderWidget->show();
        if (m_controller->playbackState() == media::PlaybackState::Playing) {
            m_videoSliderHideTimer->start();
        }
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        if (m_controller->playbackState() == media::PlaybackState::Playing) {
            m_controller->pause();
        } else if (m_controller->playbackState() == media::PlaybackState::Paused) {
            m_controller->play();
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void VuraMainWindow::maximized()
{
    setWindowState(windowState() | Qt::WindowMaximized);
}

void VuraMainWindow::setMainWindowVisibility(const bool state)
{
    if (state) {
        show();
        showNormal();
        raise();
        activateWindow();
    }
}

void VuraMainWindow::openFile(const QString &file)
{
    if (file.isEmpty()) {
        qDebug() << "No file specified.";
        return;
    }
    qDebug() << "Open file requested. File: " << file;

    if (m_replacePlaylist)
        openPaths({QUrl::fromLocalFile(file)});
    else
        m_controller->enqueue({QUrl::fromLocalFile(file)});
}

void VuraMainWindow::openFolder(const QString &path)
{
    if (path.isEmpty()) {
        qDebug() << "No path specified.";
        return;
    }
    qDebug() << "Open folder requested. Path: " << path;

    QList<QUrl> urls;
    QDirIterator folderIterator(path, QDir::Files | QDir::NoDotAndDotDot);
    while (folderIterator.hasNext()) {
        folderIterator.next();
        urls.append(QUrl::fromLocalFile(folderIterator.filePath()));
    }

    if (m_replacePlaylist)
        openPaths(urls);
    else
        m_controller->enqueue(urls);
}

void VuraMainWindow::openNetworkStream(const QString& networkUrl)
{
    if (networkUrl.isEmpty()) {
        qDebug() << "No network URL specified.";
        return;
    }
    qDebug() << "Open with network stream requested. Network URL: " << networkUrl;

    m_playlistDock->hide();
    const QString formattedUrl = Helpers::networkUrlFormatter(networkUrl);
    qDebug() << "Cleaned Network URL: " << formattedUrl;

    if (m_replacePlaylist)
        openPaths({QUrl(formattedUrl)});
    else
        m_controller->enqueue({QUrl(formattedUrl)});
}

void VuraMainWindow::stateChanged(const media::PlaybackState state)
{
    if (state == media::PlaybackState::Playing && !m_videoSliderHideTimer->isActive()) {
        m_videoSliderHideTimer->start();
    } else {
        m_videoSliderHideTimer->stop();
        m_videoSliderWidget->show();
        this->unsetCursor();
    }
}

void VuraMainWindow::sourceChanged(const QUrl &source)
{
    qCDebug(Core) << "Source changed to: " << source.toString();

    QSettings settings;

    m_currentSource = source;
    m_videoMarkerController->loadVideoMarkers(source);
    setApplicationWindowTitle();

    // Media Change Alert
    const int showMediaChangeNotification = settings.value("showMediaChangeNotification", 1).toInt();
    switch (showMediaChangeNotification) {
        // Never alert
        case 0:
            break;
            // Alert when minimized
        case 1:
            if (windowState() & Qt::WindowMinimized) {
                QApplication::alert(this);
            }
            break;
            // Always alert
        case 2:
            QApplication::alert(this);
            break;
        default:
            break;
    }

    // Continue playback
    //if (source.isLocalFile()) {
    //    const QByteArray pathBytes = source.toLocalFile().toUtf8();
    //    const QString fileHash = QCryptographicHash::hash(pathBytes, QCryptographicHash::Md5).toHex();
    //
    //    settings.beginGroup("ResumeData");
    //    const qint64 savedPosition = settings.value(fileHash, 0).toLongLong();
    //    settings.endGroup();
    //
    //    if (savedPosition > 5000) {
    //        const int continuePlayback = settings.value("continuePlayback", 1).toInt();
    //        switch (continuePlayback) {
    //            // Never continue
    //            case 0:
    //                break;
    //            // Ask user
    //            case 1:
    //                showResumeOverlay(savedPosition);
    //                break;
    //            // Always continue
    //            case 2:
    //                m_playbackController->seek(savedPosition);
    //                break;
    //            default:
    //                break;
    //        }
    //    }
    //}
}

void VuraMainWindow::errorOccurred(const QString &errorMessage)
{
    qCCritical(Core) << "MediaPlayer Error: " << errorMessage;
    QMessageBox::critical(this, "Media Player Error", errorMessage);
}

void VuraMainWindow::hideVideoSlider()
{
    const QSettings settings;

    if (m_controller->playbackState() != media::PlaybackState::Playing)
        return;

    int autohideSlider = settings.value("autohideSlider", 1).toInt();
    switch (autohideSlider) {
        case 0:
            break;
        case 1:
            if (isFullScreen()) {
                m_videoSliderWidget->hide();
                setCursor(Qt::BlankCursor);
            }
            break;
        case 2:
            if (isMaximized()) {
                m_videoSliderWidget->hide();
                setCursor(Qt::BlankCursor);
            }
            break;
        case 3:
            m_videoSliderWidget->hide();
            setCursor(Qt::BlankCursor);
            break;
        default:
            break;
    }
}

void VuraMainWindow::resetVideoSliderVisibility()
{
    m_videoSliderHideTimer->stop();
    m_videoSliderWidget->show();
    unsetCursor();

    if (m_controller->playbackState() == media::PlaybackState::Playing && !m_videoSliderHideTimer->isActive())
        m_videoSliderHideTimer->start();
}

void VuraMainWindow::updateCheckReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Network Error:" << reply->errorString();
        return;
    }

    const QByteArray response = reply->readAll();
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();

    bool isRemoteBeta = jsonObj["is_beta"].toBool();
    const QString remoteVersion = jsonObj["version"].toString();
    const QString releaseDate = jsonObj["release_date"].toString();

    if (remoteVersion != VURA_VERSION_STRING) {
        QJsonObject platforms = jsonObj["platforms"].toObject();

#if defined(Q_OS_WIN)
        QJsonObject currentPlatform = platforms["windows"].toObject();
#elif defined(Q_OS_MAC)
        QJsonObject currentPlatform = platforms["mac"].toObject();
#else
        QJsonObject currentPlatform = platforms["linux"].toObject();
#endif

        const QString downloadUrl = currentPlatform["url"].toString();
        const QString expectedHash = currentPlatform["sha256"].toString();
        const QString changelogUrl = jsonObj["changelog_url"].toString();


        const QSettings settings;
        const QString lastCheckedVersion = settings.value("lastCheckedVersion", "").toString();
        if (remoteVersion != lastCheckedVersion) {
            if (m_updateDialog)
                m_updateDialog->close();

            m_updateDialog = new UpdateDialog(remoteVersion, releaseDate, downloadUrl, changelogUrl, expectedHash, this);
            connect(m_updateDialog, &UpdateDialog::updateNow, this, &VuraMainWindow::onUpdateConfirmed);
            m_updateDialog->show();
            m_updateDialog->setAttribute(Qt::WA_DeleteOnClose, true);
        }
    }

    reply->deleteLater();
}

void VuraMainWindow::openRecentFile()
{
    if (QAction *action = qobject_cast<QAction *>(sender())) {
        QString fileName = action->data().toString();
        if (fileName.isEmpty()) {
            qDebug() << "No file name specified.";
            return;
        }
        qDebug() << "Open recent file requested. File: " << fileName;

        if (m_replacePlaylist)
            openPaths({QUrl::fromLocalFile(fileName)});
        else
            m_controller->enqueue({QUrl::fromLocalFile(fileName)});
    }
}

void VuraMainWindow::updateRecentFileActions() const
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    const int numRecentFiles = qMin(files.size(), static_cast<int>(MaxRecentFiles));

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());

        m_recentFileActions[i]->setText(text);
        m_recentFileActions[i]->setData(files[i]);
        m_recentFileActions[i]->setVisible(true);
    }

    for (int j = numRecentFiles; j < MaxRecentFiles; ++j) {
        m_recentFileActions[j]->setVisible(false);
    }

    const bool hasRecentFiles = (numRecentFiles > 0);
    m_recentFilesSeparator->setVisible(hasRecentFiles);
    ui->menuFileOpenRecent->setEnabled(hasRecentFiles);
}

void VuraMainWindow::actionFileOpenFile()
{
    QSettings settings;

    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
        "All Files (*)");

    if (!fileName.isEmpty()) {
        settings.setValue("lastFileDirectory", QFileInfo(fileName).path());

        if (m_replacePlaylist)
            openPaths({QUrl::fromLocalFile(fileName)});
        else
            m_controller->enqueue({QUrl::fromLocalFile(fileName)});
    }
}

void VuraMainWindow::actionFileOpenMultipleFiles()
{
    QSettings settings;
    QList<QUrl> urls;

    const QStringList files = QFileDialog::getOpenFileNames(
            this,
            tr("Open Media Files"),
            settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
            "All Files (*)"
        );

    if (!files.isEmpty()) {
        for (const QString& fileName : files) {
            urls << QUrl::fromLocalFile(fileName);
        }

        const QString& lastFile = files.last();
        settings.setValue("lastFileDirectory", QFileInfo(lastFile).path());

        if (m_replacePlaylist)
            openPaths(urls);
        else
            m_controller->enqueue(urls);
    }
}

void VuraMainWindow::actionFileOpenFolder()
{
    QSettings settings;
    QList<QUrl> urls;

    const QString dir = QFileDialog::getExistingDirectory(
            this,
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
            urls << QUrl::fromLocalFile(fileInfo.absoluteFilePath());
        }
        settings.setValue("lastFileDirectory", QFileInfo(dir).path());

        if (m_replacePlaylist)
            openPaths(urls);
        else
            m_controller->enqueue(urls);
    }
}

void VuraMainWindow::actionFileOpenPlaylist()
{
    QSettings settings;

    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
        "XSPF playlist (*.xspf);;M3U playlist (*.m3u);;M3U8 playlist (*.m3u8);;All Files (*.*)");

    if (!fileName.isEmpty()) {
        settings.setValue("lastFileDirectory", QFileInfo(fileName).path());

        QString error;
        if (!playlistio::loadInto(m_controller->playlist(), fileName, &error)) {
            qWarning() << "Error loading playlist: " << error;
        }
    }
}

void VuraMainWindow::actionFileSavePlaylist()
{
    QSettings settings;

    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Playlist As"),
        settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
        tr("XSPF playlist (*.xspf);;M3U playlist (*.m3u);;M3U8 playlist (*.m3u8);;All Files (*.*)"));

    if (!fileName.isEmpty()) {
        QString error;
        if (!playlistio::saveFrom(m_controller->playlist(), fileName, &error)) {
            qWarning() << "Error saving playlist: " << error;
        }
    }
}

void VuraMainWindow::actionFileOpenRecentClear()
{
    QSettings settings;
    settings.remove("recentFileList");
    updateRecentFileActions();
}

void VuraMainWindow::actionHelpCheckForUpdates()
{
    const QSettings settings;
    QString manifestFile = "stable.json";

    int updateBranch = settings.value("updateBranch", 0).toInt();
    if (updateBranch == 1) {
        manifestFile = "beta.json";
    }

    m_updateNetworkManager = new QNetworkAccessManager(this);
    connect(m_updateNetworkManager, &QNetworkAccessManager::finished, this, &VuraMainWindow::updateCheckReplyFinished);

    const QUrl url(QString("https://vura.hale-software.com/%1").arg(manifestFile));
    const QNetworkRequest request(url);

    qDebug() << "Checking for updates using URL: " << url << "...";
    m_updateNetworkManager->get(request);
}

void VuraMainWindow::actionTestFunction()
{
    //qDebug() << "Current Height: " << QString::number(height()) << ". Current Width: " << QString::number(width());
    if (m_replacePlaylist)
        openPaths({QUrl::fromLocalFile("C:\\Users\\halea\\Vura-Testing\\test1.mp4")});
    else
        m_controller->enqueue({QUrl::fromLocalFile("C:\\Users\\halea\\Vura-Testing\\test1.mp4")});
}

void VuraMainWindow::actionOpenNetworkStream()
{
    bool ok;
    const QString networkUrl = QInputDialog::getText(this,
            tr("Open Network Stream"),
            tr("Network Stream URL:"),
            QLineEdit::Normal,
            QString(),
            &ok);

    if (ok && !networkUrl.isEmpty()) {
        if (m_replacePlaylist)
            openPaths({QUrl(networkUrl)});
        else
            m_controller->enqueue({QUrl(networkUrl)});
    }
}

void VuraMainWindow::actionEmergencyClose()
{
    m_controller->pause();
    setWindowState(Qt::WindowMinimized);
}

void VuraMainWindow::actionShowLogViewer()
{
    if (m_logViewerDialog)
        m_logViewerDialog->close();

    m_logViewerDialog = new LogViewerDialog(this);
    m_logViewerDialog->show();
    m_logViewerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

void VuraMainWindow::actionToggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
        if (m_wasMaximized)
            showMaximized();

        unsetCursor();
        ui->menubar->show();
        m_videoSliderWidget->show();
        if (m_wasPlaylistShowing)
            m_playlistDock->show();

        return;
    }

    m_wasMaximized = isMaximized();
    m_wasPlaylistShowing = m_playlistDock->isVisible();
    ui->menubar->hide();
    m_videoSliderWidget->hide();
    m_playlistDock->hide();
    showFullScreen();
}

void VuraMainWindow::actionShowSettings()
{
    if (m_settingsDialog)
        m_settingsDialog->close();

    m_settingsDialog = new SettingsDialog(this);
    m_settingsDialog->show();
    m_settingsDialog->setAttribute(Qt::WA_DeleteOnClose, true);

    connect(m_settingsDialog, &SettingsDialog::updateRequested, this, &VuraMainWindow::onUpdateConfirmed);
    connect(m_settingsDialog, &SettingsDialog::requiresRestart, this, [this]() {
        restartApplication();
    });
}

void VuraMainWindow::actionShowConvertMedia()
{
    if (m_convertMediaDialog)
        m_convertMediaDialog->close();

    m_convertMediaDialog = new ConvertMediaDialog(this);
    m_convertMediaDialog->show();
    m_convertMediaDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

void VuraMainWindow::actionExit()
{
    const QMessageBox::StandardButton confirmationBox = QMessageBox::question(this,
        tr("Exit Application"),
        tr("Are you sure you want to exit?"),
        QMessageBox::Yes | QMessageBox::No);

    if (confirmationBox == QMessageBox::Yes) {
        this->close();
    }
}

void VuraMainWindow::actionViewTogglePlaylist()
{
    if (m_playlistDock->isVisible())
        m_playlistDock->hide();
    else
        m_playlistDock->show();

    ui->actionViewTogglePlaylist->setChecked(m_playlistDock->isVisible());
}

void VuraMainWindow::actionToggleVideoControls()
{
    if (m_showingVideoControls) {
        ui->verticalLayout->removeWidget(m_videoControlWidget);
        delete m_videoControlWidget;
        m_showingVideoControls = false;
    } else {
        m_videoControlWidget = new VideoControlWidget(this);
        m_videoControlWidget->setMuted(m_controller->isMuted());
        m_videoControlWidget->setVolume(m_controller->volume());

        ui->verticalLayout->addWidget(m_videoControlWidget);

        connect(m_controller, &MediaController::playbackStateChanged, m_videoControlWidget, &VideoControlWidget::setState);
        connect(m_videoControlWidget, &VideoControlWidget::play, m_controller, &MediaController::play);
        connect(m_videoControlWidget, &VideoControlWidget::pause, m_controller, &MediaController::pause);
        connect(m_videoControlWidget, &VideoControlWidget::stop, m_controller, &MediaController::stop);
        connect(m_videoControlWidget, &VideoControlWidget::changeVolume, this, [this](int value) {
            m_controller->setVolume(sliderToLinear(value));
        });

        m_showingVideoControls = true;
    }
    ui->actionViewToggleVideoControls->setChecked(m_showingVideoControls);
}

void VuraMainWindow::actionViewToggleStatusBar() const
{
    if (ui->statusBar->isVisible()) {
        ui->statusBar->hide();
    } else {
        ui->statusBar->show();
    }
    ui->actionViewToggleStatusBar->setChecked(ui->statusBar->isVisible());
}

void VuraMainWindow::actionViewToggleVideoResolution()
{
    QSettings settings;
    settings.setValue("showVideoResolutionOnStart", ui->actionViewToggleVideoResolution->isChecked());
    setApplicationWindowTitle();
}

void VuraMainWindow::actionViewMediaInformation()
{
    QMessageBox::information(this, tr("Information"), tr("This function is not implemented yet."));
    return;
    if (m_mediaInformationDialog)
        m_mediaInformationDialog->close();

    m_mediaInformationDialog = new MediaInformationDialog(this);

    //if (!m_controller->currentTitle().isEmpty())
    //    m_mediaInformationDialog->setMetaData(*m_playbackController->getMetadata());

    m_mediaInformationDialog->show();
}

void VuraMainWindow::actionMarkersClearIn() {}

void VuraMainWindow::actionMarkersClearInOut() {}

void VuraMainWindow::actionMarkersClearMarkers()
{
    const QMessageBox::StandardButton confirmationBox = QMessageBox::question(
        this,
        tr("Clear Markers"),
        tr("Are you sure you want to clear all markers? This cannot be undone."),
        QMessageBox::Yes | QMessageBox::No
        );

    if (confirmationBox == QMessageBox::Yes) {
        m_videoMarkerController->clearMarkers();
    }
}

void VuraMainWindow::actionMarkersClearOut() {}

void VuraMainWindow::actionMarkersEditSelectedMarker()
{
    const VideoMarkerRecord marker = m_videoMarkerController->getSelectedMarker();
    if (marker.id <= 0)
        return;

    if (m_markerEditDialog)
        m_markerEditDialog->close();

    m_markerEditDialog = new MarkerEditDialog(marker, m_controller->duration(), this);
    m_markerEditDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    m_markerEditDialog->show();

    connect(m_markerEditDialog, &MarkerEditDialog::markerEdited, this, [this](const VideoMarkerRecord &videoMarker) {
        m_videoMarkerController->addVideoMarker(videoMarker);
    });

    connect(m_markerEditDialog, &MarkerEditDialog::markerDeleted, this, [this](const VideoMarkerRecord &videoMarker) {
        m_videoMarkerController->deleteVideoMarker(videoMarker);
    });
}

void VuraMainWindow::actionMarkersGoToIn() {}

void VuraMainWindow::actionMarkersGoToOut() {}

void VuraMainWindow::actionMarkersMarkIn() {}

void VuraMainWindow::actionMarkersMarkOut() {}

void VuraMainWindow::actionSubtitlesOpenSubtitlesFile()
{
    QSettings settings;

    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Subtitle File"),
        settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
        tr("Subtitles (*.srt *.ass *.ssa *.vtt *.sub);;All files (*)"));

    if (fileName.isEmpty())
        return;

    if (m_controller->loadExternalSubtitle(QUrl::fromLocalFile(fileName)))
        qDebug() << "Loaded external subtitle file: " << fileName;
    else
        qWarning() << "Failed to load external subtitle file: " << fileName;

    settings.setValue("lastFileDirectory", QFileInfo(fileName).path());
}

void VuraMainWindow::actionSubtitlesToggleSubtitles(bool checked)
{
    m_subtitlesEnabled = checked;
}

void VuraMainWindow::setCurrentFile(const QUrl &mediaUrl)
{
    if (mediaUrl.isLocalFile()) {
        updateRecentFilesList(mediaUrl.toLocalFile());
    } else {
        updateRecentFilesList(mediaUrl.toString());
    }
}

void VuraMainWindow::restartApplication()
{
    qInfo() << "Restarting application...";
    qApp->exit(0xA1);
}

void VuraMainWindow::openPaths(const QList<QUrl> &urls)
{
    if (urls.isEmpty())
        return;
    m_controller->openUrls(urls);
}

void VuraMainWindow::initSystemTray()
{
    QSettings settings;

    m_systemTray = new SystemTrayWidget(this);

    connect(m_systemTray, &SystemTrayWidget::clicked, this, &VuraMainWindow::systemTray_Clicked);
    connect(m_systemTray, &SystemTrayWidget::hiding, this, &VuraMainWindow::systemTray_Hide);
    connect(m_systemTray, &SystemTrayWidget::togglePlayPause, m_controller, &MediaController::togglePlayPause);
    connect(m_systemTray, &SystemTrayWidget::stop, m_controller, &MediaController::stop);
    connect(m_systemTray, &SystemTrayWidget::nextVideo, m_controller, &MediaController::next);
    connect(m_systemTray, &SystemTrayWidget::previousVideo, m_controller, &MediaController::previous);
    connect(m_systemTray, &SystemTrayWidget::setMuted, m_controller, &MediaController::setMuted);
    connect(m_systemTray, &SystemTrayWidget::toggleFullscreen, this, &VuraMainWindow::actionToggleFullscreen);
    connect(m_systemTray, &SystemTrayWidget::exit, this, &VuraMainWindow::actionExit);

    bool systemTrayIcon = settings.value("systemTrayIcon", true).toBool();
    m_systemTray->setVisibility(systemTrayIcon);
}

void VuraMainWindow::buildMenus()
{
    const QSettings settings;

    m_recentFilesSeparator = ui->menuFileOpenRecent->addSeparator();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        m_recentFileActions[i] = new QAction(this);
        m_recentFileActions[i]->setVisible(false);

        connect(m_recentFileActions[i], &QAction::triggered, this, &VuraMainWindow::openRecentFile);

        ui->menuFileOpenRecent->addAction(m_recentFileActions[i]);
    }

    connect(ui->menuFileOpenRecent, &QMenu::aboutToShow, this, &VuraMainWindow::updateRecentFileActions);

    updateRecentFileActions();
    
    connect(ui->actionPlaybackModeDoNotLoopPlaylist, &QAction::toggled, this, [this](bool checked) {
        if (checked) {
            ui->actionPlaybackModeLoopCurrentTrack->setChecked(false);
            ui->actionPlaybackModeLoopPlaylist->setEnabled(false);
            m_controller->playlist()->setRepeatMode(Playlist::RepeatMode::RepeatNone);
        }
    });
    this->addAction(ui->actionPlaybackModeDoNotLoopPlaylist);
    ui->actionPlaybackModeDoNotLoopPlaylist->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackModeLoopCurrentTrack, &QAction::toggled, this, [this](bool checked) {
        if (checked) {
            ui->actionPlaybackModeDoNotLoopPlaylist->setChecked(false);
            ui->actionPlaybackModeLoopPlaylist->setEnabled(false);
            m_controller->playlist()->setRepeatMode(Playlist::RepeatMode::RepeatOne);
        }
    });
    this->addAction(ui->actionPlaybackModeLoopCurrentTrack);
    ui->actionPlaybackModeLoopCurrentTrack->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackModeLoopPlaylist, &QAction::toggled, this, [this](bool checked) {
        if (checked) {
            ui->actionPlaybackModeDoNotLoopPlaylist->setChecked(false);
            ui->actionPlaybackModeLoopCurrentTrack->setChecked(false);
            m_controller->playlist()->setRepeatMode(Playlist::RepeatMode::RepeatAll);
        }
    });
    this->addAction(ui->actionPlaybackModeLoopPlaylist);
    ui->actionPlaybackModeLoopPlaylist->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackModeShuffle, &QAction::toggled, this, [this](bool checked) {
        m_controller->playlist()->setShuffled(checked);
    });
    this->addAction(ui->actionPlaybackModeShuffle);
    ui->actionPlaybackModeShuffle->setShortcutContext(Qt::WindowShortcut);

    // File Actions
    connect(ui->actionFileOpenRecentClear, &QAction::triggered, this, &VuraMainWindow::actionFileOpenRecentClear);
    this->addAction(ui->actionFileOpenRecentClear);
    ui->actionFileOpenRecentClear->setShortcutContext(Qt::WindowShortcut);

    // Playback Actions
    connect(ui->actionPlaybackNext, &QAction::triggered, m_controller, &MediaController::next);
    this->addAction(ui->actionPlaybackNext);
    ui->actionPlaybackNext->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackPrevious, &QAction::triggered, m_controller, &MediaController::previous);
    this->addAction(ui->actionPlaybackPrevious);
    ui->actionPlaybackPrevious->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackTogglePlay, &QAction::triggered, m_controller, &MediaController::togglePlayPause);
    this->addAction(ui->actionPlaybackTogglePlay);
    ui->actionPlaybackTogglePlay->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackRestartVideo, &QAction::triggered, m_controller, &MediaController::restart);
    this->addAction(ui->actionPlaybackRestartVideo);
    ui->actionPlaybackRestartVideo->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewTogglePlaylist, &QAction::triggered, this, &VuraMainWindow::actionViewTogglePlaylist);
    this->addAction(ui->actionViewTogglePlaylist);
    ui->actionViewTogglePlaylist->setShortcutContext(Qt::WindowShortcut);

    // Audio Actions
    connect(ui->actionAudioToggleMute, &QAction::toggled, m_controller, &MediaController::setMuted);
    this->addAction(ui->actionAudioToggleMute);
    ui->actionAudioToggleMute->setShortcutContext(Qt::WindowShortcut);

    //connect(ui->actionAudioVolumeDown, &QAction::triggered, m_playbackController, &PlaybackController::volumeDown);
    this->addAction(ui->actionAudioVolumeDown);
    ui->actionAudioVolumeDown->setShortcutContext(Qt::WindowShortcut);

    //connect(ui->actionAudioVolumeUp, &QAction::triggered, m_playbackController, &PlaybackController::volumeUp);
    this->addAction(ui->actionAudioVolumeUp);
    ui->actionAudioVolumeUp->setShortcutContext(Qt::WindowShortcut);

    //m_videoMarkerController = new VideoMarkerController(this);

    // Video Slider
    //m_videoSlider = new VideoSlider(m_videoMarkerController, this);
    //m_videoSliderWidget = new VideoSliderWidget(*m_videoSlider, *m_playbackController, this);
    //connect(m_videoMarkerController, &VideoMarkerController::markerAdded, m_videoSlider, &VideoSlider::updateVideoSlider);
    //connect(m_videoMarkerController, &VideoMarkerController::markersLoaded, m_videoSlider, &VideoSlider::loadVideoMarkers);
    //connect(m_videoMarkerController, &VideoMarkerController::markersUpdated, m_videoSlider, &VideoSlider::updateVideoSlider);

    //ui->verticalLayout->addWidget(m_videoSliderWidget);
    //ui->verticalLayout->setStretch(0, 1);

    //connect(m_playbackController, &PlaybackController::positionChanged, m_videoSlider, &VideoSlider::setValue);
    //connect(m_playbackController, &PlaybackController::durationChanged, m_videoSlider, &VideoSlider::setMaximum);
    //connect(m_playbackController, &PlaybackController::durationChanged, this, &VuraMainWindow::durationChanged);
    //connect(m_playbackController, &PlaybackController::sourceChanged, this, &VuraMainWindow::sourceChanged);
    //connect(m_playbackController, &PlaybackController::stateChanged, this, &VuraMainWindow::stateChanged);
    //connect(m_playbackController, &PlaybackController::jumpCompleted, this, &VuraMainWindow::resetVideoSliderVisibility);
    //connect(m_videoSlider, &VideoSlider::valueChanged, m_playbackController, &PlaybackController::setPosition);
    //connect(m_videoSlider, &VideoSlider::sliderPressed, m_playbackController, &PlaybackController::setPaused);

    //connect(m_playbackController, &PlaybackController::positionChanged, this, [this](const qint64 pos) {
    //    m_lastPosition = pos;
    //});

    //const int autoHideTimer = settings.value("sliderAutohideTime", 5).toInt() * 1000;
    //m_videoSliderHideTimer = new QTimer(this);
    //m_videoSliderHideTimer->setInterval(autoHideTimer);
    //m_videoSliderHideTimer->setSingleShot(true);
    //connect(m_videoSliderHideTimer, &QTimer::timeout, this, &VuraMainWindow::hideVideoSlider);

    connect(ui->actionFileEmergencyClose, &QAction::triggered, this, &VuraMainWindow::actionEmergencyClose);
    this->addAction(ui->actionFileEmergencyClose);
    ui->actionFileEmergencyClose->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileExit, &QAction::triggered, this, &VuraMainWindow::actionExit);
    this->addAction(ui->actionFileExit);
    ui->actionFileExit->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileOpenFile, &QAction::triggered, this, &VuraMainWindow::actionFileOpenFile);
    this->addAction(ui->actionFileOpenFile);
    ui->actionFileOpenFile->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileOpenFolder, &QAction::triggered, this, &VuraMainWindow::actionFileOpenFolder);
    this->addAction(ui->actionFileOpenFolder);
    ui->actionFileOpenFolder->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileOpenNetworkStream, &QAction::triggered, this, &VuraMainWindow::actionOpenNetworkStream);
    this->addAction(ui->actionFileOpenNetworkStream);
    ui->actionFileOpenNetworkStream->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileOpenMultipleFiles, &QAction::triggered, this, &VuraMainWindow::actionFileOpenMultipleFiles);
    this->addAction(ui->actionFileOpenMultipleFiles);
    ui->actionFileOpenMultipleFiles->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileSavePlaylist, &QAction::triggered, this, &VuraMainWindow::actionFileSavePlaylist);
    this->addAction(ui->actionFileSavePlaylist);
    ui->actionFileSavePlaylist->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileOpenPlaylist, &QAction::triggered, this, &VuraMainWindow::actionFileOpenPlaylist);
    this->addAction(ui->actionFileOpenPlaylist);
    ui->actionFileOpenPlaylist->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleStatusBar, &QAction::triggered, this, &VuraMainWindow::actionViewToggleStatusBar);
    this->addAction(ui->actionViewToggleStatusBar);
    ui->actionViewToggleStatusBar->setShortcutContext(Qt::WindowShortcut);
    if (settings.value("showStatusBarOnStart", false).toBool()) {
        ui->actionViewToggleStatusBar->setChecked(true);
        ui->statusBar->show();
    } else {
        ui->actionViewToggleStatusBar->setChecked(false);
        ui->statusBar->hide();
    }

    connect(ui->actionViewToggleVideoResolution, &QAction::triggered, this, &VuraMainWindow::actionViewToggleVideoResolution);
    this->addAction(ui->actionViewToggleVideoResolution);
    ui->actionViewToggleVideoResolution->setShortcutContext(Qt::WindowShortcut);
    if (settings.value("showVideoResolutionOnStart", false).toBool()) {
        ui->actionViewToggleVideoResolution->setChecked(true);
    } else {
        ui->actionViewToggleVideoResolution->setChecked(false);
    }

    connect(ui->actionViewMediaInformation, &QAction::triggered, this, &VuraMainWindow::actionViewMediaInformation);
    this->addAction(ui->actionViewMediaInformation);
    ui->actionViewMediaInformation->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewPreferences, &QAction::triggered, this, &VuraMainWindow::actionShowSettings);
    connect(ui->actionHelpViewCurrentLog, &QAction::triggered, this, &VuraMainWindow::actionShowLogViewer);

    connect(ui->actionToolsTestFunction, &QAction::triggered, this, &VuraMainWindow::actionTestFunction);
    this->addAction(ui->actionToolsTestFunction);
    ui->actionToolsTestFunction->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionVideoFullscreen, &QAction::triggered, this, &VuraMainWindow::actionToggleFullscreen);
    this->addAction(ui->actionVideoFullscreen);
    ui->actionVideoFullscreen->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleVideoControls, &QAction::triggered, this, &VuraMainWindow::actionToggleVideoControls);

    // Playback Jumping
    connect(ui->actionPlaybackJumpForwardExtraLarge, &QAction::triggered, this, [this]() {
        QSettings settings;
        int value = settings.value("extraLargeJump", 90).toInt();
        m_controller->seekRelative(value * 1000);
    });
    this->addAction(ui->actionPlaybackJumpForwardExtraLarge);
    ui->actionPlaybackJumpForwardExtraLarge->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpBackwardExtraLarge, &QAction::triggered, this, [this]() {
        QSettings settings;
        int value = settings.value("extraLargeJump", 90).toInt();
        m_controller->seekRelative(value * -1000);
    });
    this->addAction(ui->actionPlaybackJumpBackwardExtraLarge);
    ui->actionPlaybackJumpBackwardExtraLarge->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpForwardLarge, &QAction::triggered, this, [this]() {
        QSettings settings;
        int value = settings.value("largeJump", 30).toInt();
        m_controller->seekRelative(value * 1000);
    });
    this->addAction(ui->actionPlaybackJumpForwardLarge);
    ui->actionPlaybackJumpForwardLarge->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpBackwardLarge, &QAction::triggered, this, [this]() {
        QSettings settings;
        int value = settings.value("largeJump", 30).toInt();
        m_controller->seekRelative(value * -1000);
    });
    this->addAction(ui->actionPlaybackJumpBackwardLarge);
    ui->actionPlaybackJumpBackwardLarge->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpForwardMedium, &QAction::triggered, this, [this]() {
        QSettings settings;
        int value = settings.value("mediumJump", 15).toInt();
        m_controller->seekRelative(value * 1000);
    });
    this->addAction(ui->actionPlaybackJumpForwardMedium);
    ui->actionPlaybackJumpForwardMedium->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpBackwardMedium, &QAction::triggered, this, [this]() {
        QSettings settings;
        int value = settings.value("mediumJump", 15).toInt();
        m_controller->seekRelative(value * -1000);
    });
    this->addAction(ui->actionPlaybackJumpBackwardMedium);
    ui->actionPlaybackJumpBackwardMedium->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpForwardSmall, &QAction::triggered, this, [this]() {
        QSettings settings;
        int value = settings.value("smallJump", 5).toInt();
        m_controller->seekRelative(value * 1000);
    });
    this->addAction(ui->actionPlaybackJumpForwardSmall);
    ui->actionPlaybackJumpForwardSmall->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpBackwardSmall, &QAction::triggered, this, [this]() {
        QSettings settings;
        int value = settings.value("smallJump", 5).toInt();
        m_controller->seekRelative(value * -1000);
    });
    this->addAction(ui->actionPlaybackJumpBackwardSmall);
    ui->actionPlaybackJumpBackwardSmall->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpForwardExtraSmall, &QAction::triggered, this, [this]() {
        QSettings settings;
        int value = settings.value("extraSmallJump", 1).toInt();
        m_controller->seekRelative(value * 1000);
    });
    this->addAction(ui->actionPlaybackJumpForwardExtraSmall);
    ui->actionPlaybackJumpForwardExtraSmall->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackJumpBackwardExtraSmall, &QAction::triggered, this, [this]() {
        QSettings settings;
        int value = settings.value("extraSmallJump", 1).toInt();
        m_controller->seekRelative(value * -1000);
    });
    this->addAction(ui->actionPlaybackJumpBackwardExtraSmall);
    ui->actionPlaybackJumpBackwardExtraSmall->setShortcutContext(Qt::WindowShortcut);


    // Playback Rate
    connect(ui->actionPlaybackSpeedFaster, &QAction::triggered, this, [this]() {
        const QSettings settings;
        const double playbackRateStep = settings.value("playbackSpeedAdjustment", 0.5).toDouble();
        const double maxPlaybackRate = settings.value("playbackSpeedMax", 10.0).toDouble();

        qreal currentPlaybackRate = m_controller->rate();
        qreal newPlaybackRate = currentPlaybackRate + playbackRateStep;
        if (newPlaybackRate > maxPlaybackRate)
            newPlaybackRate = maxPlaybackRate;
        m_controller->setRate(newPlaybackRate);
    });
    this->addAction(ui->actionPlaybackSpeedFaster);
    ui->actionPlaybackSpeedFaster->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackSpeedFasterFine, &QAction::triggered, this, [this]() {
        const QSettings settings;
        const double playbackRateStep = settings.value("playbackSpeedAdjustmentFine", 0.25).toDouble();
        const double maxPlaybackRate = settings.value("playbackSpeedMax", 10.0).toDouble();

        qreal currentPlaybackRate = m_controller->rate();
        qreal newPlaybackRate = currentPlaybackRate + playbackRateStep;
        if (newPlaybackRate > maxPlaybackRate)
            newPlaybackRate = maxPlaybackRate;
        m_controller->setRate(newPlaybackRate);
    });
    this->addAction(ui->actionPlaybackSpeedFasterFine);
    ui->actionPlaybackSpeedFasterFine->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackSpeedNormal, &QAction::triggered, this, [this]() {
        m_controller->setRate(1.0);
    });
    this->addAction(ui->actionPlaybackSpeedNormal);
    ui->actionPlaybackSpeedNormal->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackSpeedSlowerFine, &QAction::triggered, this, [this]() {
        const QSettings settings;
        const double playbackRateStep = settings.value("playbackSpeedAdjustmentFine", 0.25).toDouble();

        qreal currentPlaybackRate = m_controller->rate();
        qreal newPlaybackRate = currentPlaybackRate - playbackRateStep;
        if (newPlaybackRate < 0.1)
            newPlaybackRate = 0.1;
        m_controller->setRate(newPlaybackRate);
    });
    this->addAction(ui->actionPlaybackSpeedSlowerFine);
    ui->actionPlaybackSpeedSlowerFine->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackSpeedSlower, &QAction::triggered, this, [this]() {
        const QSettings settings;
        const double playbackRateStep = settings.value("playbackSpeedAdjustment", 0.5).toDouble();

        qreal currentPlaybackRate = m_controller->rate();
        qreal newPlaybackRate = currentPlaybackRate - playbackRateStep;
        if (newPlaybackRate < 0.1)
            newPlaybackRate = 0.1;
        m_controller->setRate(newPlaybackRate);
    });
    this->addAction(ui->actionPlaybackSpeedSlower);
    ui->actionPlaybackSpeedSlower->setShortcutContext(Qt::WindowShortcut);



    connect(ui->actionMarkersClearIn, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearIn);
    this->addAction(ui->actionMarkersClearIn);
    ui->actionMarkersClearIn->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersClearInOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearInOut);
    this->addAction(ui->actionMarkersClearInOut);
    ui->actionMarkersClearInOut->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersClearMarkers, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearMarkers);
    this->addAction(ui->actionMarkersClearMarkers);
    ui->actionMarkersClearMarkers->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersClearOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearOut);
    this->addAction(ui->actionMarkersClearOut);
    ui->actionMarkersClearOut->setShortcutContext(Qt::WindowShortcut);


    connect(ui->actionMarkersEditSelectedMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersEditSelectedMarker);
    this->addAction(ui->actionMarkersEditSelectedMarker);
    ui->actionMarkersEditSelectedMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersGoToIn, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToIn);
    this->addAction(ui->actionMarkersGoToIn);
    ui->actionMarkersGoToIn->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersGoToOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToOut);
    this->addAction(ui->actionMarkersGoToOut);
    ui->actionMarkersGoToOut->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersMarkIn, &QAction::triggered, this, &VuraMainWindow::actionMarkersMarkIn);
    this->addAction(ui->actionMarkersMarkIn);
    ui->actionMarkersMarkIn->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersMarkOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersMarkOut);
    this->addAction(ui->actionMarkersMarkOut);
    ui->actionMarkersMarkOut->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileConvertSave, &QAction::triggered, this, &VuraMainWindow::actionShowConvertMedia);
    this->addAction(ui->actionFileConvertSave);
    ui->actionFileConvertSave->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionSubtitlesOpenSubtitlesFile, &QAction::triggered, this, &VuraMainWindow::actionSubtitlesOpenSubtitlesFile);
    this->addAction(ui->actionSubtitlesOpenSubtitlesFile);
    ui->actionSubtitlesOpenSubtitlesFile->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionSubtitlesToggleSubtitles, &QAction::toggled, this, &VuraMainWindow::actionSubtitlesToggleSubtitles);
    this->addAction(ui->actionSubtitlesToggleSubtitles);
    ui->actionSubtitlesToggleSubtitles->setShortcutContext(Qt::WindowShortcut);
}

void VuraMainWindow::buildPlaylistDock()
{
    QSettings settings;

    m_playlistDock = new QDockWidget(tr("Playlist"), this);
    m_playlistDock->setObjectName(QStringLiteral("playlistDock"));

    m_playlistWidget = new PlaylistWidget(this);
    m_playlistWidget->setPlaylistModel(m_controller->playlist());

    connect(m_playlistWidget, &PlaylistWidget::doubleClicked, this, [this](const QModelIndex &index) {
        m_controller->playIndex(index.row());
    });

    m_playlistDock->setWidget(m_playlistWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_playlistDock);

    if (settings.value("showPlaylistOnStart", true).toBool()) {
        m_playlistDock->show();
        ui->actionViewTogglePlaylist->setChecked(true);
    } else {
        m_playlistDock->hide();
        ui->actionViewTogglePlaylist->setChecked(false);
    }
}

void VuraMainWindow::initUI()
{
    QSettings settings;

    const int defaultWindowHeight = settings.value("defaultWindowHeight", 530).toInt();
    const int defaultWindowWidth = settings.value("defaultWindowWidth", 887).toInt();
    resize(defaultWindowWidth, defaultWindowHeight);

    if (settings.value("rememberWindowSize", false).toBool()) {
        qDebug() << "Rememmber window size setting set to true. Restoring previous window size.";
        restoreGeometry(settings.value("geometry").toByteArray());
    }

    m_videoSlider = new VideoSlider(this);
    connect(m_videoSlider, &VideoSlider::scrubbed, m_controller, [this](qint64 ms) {
        m_controller->seek(ms);
    });
    connect(m_videoSlider, &VideoSlider::scrubFinished, m_controller, [this](qint64 ms) {
        m_controller->seek(ms);
    });
    connect(m_videoSlider, &VideoSlider::markerEditRequested, this, [this](const VideoMarkerRecord &marker) {
        if (m_markerEditDialog)
            m_markerEditDialog->close();

        m_markerEditDialog = new MarkerEditDialog(marker, m_controller->duration(), this);
        m_markerEditDialog->setAttribute(Qt::WA_DeleteOnClose, true);
        m_markerEditDialog->show();

        connect(m_markerEditDialog, &MarkerEditDialog::markerEdited, this, [this](const VideoMarkerRecord &m) {
            m_videoMarkerController->addVideoMarker(m);
        });
        connect(m_markerEditDialog, &MarkerEditDialog::markerDeleted, this, [this](const VideoMarkerRecord &m) {
            m_videoMarkerController->deleteVideoMarker(m);
        });
    });
    connect(m_videoSlider, &VideoSlider::markerTypeHidden, this, [this](const QString &type) {
        static const QHash<QString, QAction*> actions = {
            {"marker",  ui->actionViewToggleMarkersMarkers},
            {"cumshot", ui->actionViewToggleMarkersCumshotMarkers},
            { "cyan", ui->actionViewToggleMarkersCyanMarkers},
            { "dialog", ui->actionViewToggleMarkersDialogMarkers},
            { "magenta", ui->actionViewToggleMarkersMagentaMarkers},
            { "orange", ui->actionViewToggleMarkersOrangeMarkers},
            { "scene", ui->actionViewToggleMarkersSceneMarkers},
            { "strip", ui->actionViewToggleMarkersStripMarkers},
        };
        if (QAction *action = actions.value(type))
            action->setChecked(false);
    });

    m_videoMarkerController = new VideoMarkerController(*m_videoSlider, this);
    connect(ui->actionViewToggleMarkersCumshotMarkers, &QAction::toggled, m_videoMarkerController, &VideoMarkerController::setCumshotMarkerVisibility);
    connect(ui->actionViewToggleMarkersCyanMarkers, &QAction::toggled, m_videoMarkerController, &VideoMarkerController::setCyanMarkerVisibility);
    connect(ui->actionViewToggleMarkersDialogMarkers, &QAction::toggled, m_videoMarkerController, &VideoMarkerController::setDialogMarkerVisibility);
    connect(ui->actionViewToggleMarkersMagentaMarkers, &QAction::toggled, m_videoMarkerController, &VideoMarkerController::setMagentaMarkerVisibility);
    connect(ui->actionViewToggleMarkersMarkers, &QAction::toggled, m_videoMarkerController, &VideoMarkerController::setMarkerVisibility);
    connect(ui->actionViewToggleMarkersOrangeMarkers, &QAction::toggled, m_videoMarkerController, &VideoMarkerController::setOrangeMarkerVisibility);
    connect(ui->actionViewToggleMarkersSceneMarkers, &QAction::toggled, m_videoMarkerController, &VideoMarkerController::setSceneMarkerVisibility);
    connect(ui->actionViewToggleMarkersStripMarkers, &QAction::toggled, m_videoMarkerController, &VideoMarkerController::setStripMarkerVisibility);
    connect(ui->actionMarkersAddCumshotMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::addCumshotMarker);
    connect(ui->actionMarkersAddCyanMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::addCyanMarker);
    connect(ui->actionMarkersAddDialogMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::addDialogMarker);
    connect(ui->actionMarkersAddMagentaMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::addMagentaMarker);
    connect(ui->actionMarkersAddMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::addMarker);
    connect(ui->actionMarkersAddOrangeMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::addOrangeMarker);
    connect(ui->actionMarkersAddSceneMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::addSceneMarker);
    connect(ui->actionMarkersAddStripMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::addStripMarker);
    connect(ui->actionMarkersClearSelectedMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::clearSelectedMarker);
    connect(ui->actionMarkersGoToNextMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::goToNextMarker);
    connect(ui->actionMarkersGoToPreviousMarker, &QAction::triggered, m_videoMarkerController, &VideoMarkerController::goToPreviousMarker);

    connect(m_videoSlider, &VideoSlider::markerDeleteRequested, m_videoMarkerController, &VideoMarkerController::deleteVideoMarker);


    m_videoSliderWidget = new VideoSliderWidget(*m_videoSlider, this);
    connect(m_controller, &MediaController::rateChanged, m_videoSliderWidget, &VideoSliderWidget::playbackRateChanged);


    ui->verticalLayout->addWidget(m_videoSliderWidget);
    ui->verticalLayout->setStretch(0, 1);

    const int autoHideTimer = settings.value("sliderAutohideTime", 5).toInt() * 1000;
    m_videoSliderHideTimer = new QTimer(this);
    m_videoSliderHideTimer->setInterval(autoHideTimer);
    m_videoSliderHideTimer->setSingleShot(true);
    connect(m_videoSliderHideTimer, &QTimer::timeout, this, &VuraMainWindow::hideVideoSlider);

    if (settings.value("showVideoControlsOnStart", false).toBool())
        actionToggleVideoControls();
}

void VuraMainWindow::connectController()
{
    //connect(m_playbackController, &PlaybackController::jumpCompleted, this, &VuraMainWindow::resetVideoSliderVisibility);
    //connect(m_videoSlider, &VideoSlider::valueChanged, m_playbackController, &PlaybackController::setPosition);
    //connect(m_videoSlider, &VideoSlider::sliderPressed, m_playbackController, &PlaybackController::setPaused);


    connect(m_controller, &MediaController::positionChanged, this, [this](media::Msec ms) {
        //m_videoSlider->setValue(ms);
        m_lastPosition = ms;
        m_videoSlider->setPositionFromEngine(ms);
        m_videoSliderWidget->positionChanged(ms);
    });

    connect(m_controller, &MediaController::durationChanged, this, [this](media::Msec ms) {
        //m_videoSlider->setMaximum(ms);
        //durationChanged(ms);
        m_videoSlider->setRange(0, int(ms));
        m_videoSliderWidget->durationChanged(ms);
    });

    connect(m_controller, &MediaController::playbackStateChanged, this, [this](media::PlaybackState state) {
        stateChanged(state);
        const bool playing = state == media::PlaybackState::Playing;
        const bool hasVideo = !m_controller->engine() || !m_controller->engine()->tracks(media::TrackType::Video).isEmpty();
        m_sleepInhibitor->setInhibited(playing && hasVideo, tr("Playing video"));
    });

    connect(m_controller, &MediaController::mediaStatusChanged, this, [this](media::MediaStatus status) {
        switch (status) {
            case media::MediaStatus::Loading:
                qCDebug(Playback) << "Media Status Changed: Loading...";
                break;

            case media::MediaStatus::Buffering:
                qCDebug(Playback) << "Media Status Changed: Buffering...";
                break;

            case media::MediaStatus::Buffered:
                qCDebug(Playback) << "Media Status Changed: Buffered";
                break;

            case media::MediaStatus::Loaded:
                qCDebug(Playback) << "Media Status Changed: Loaded";
                break;

            case media::MediaStatus::Invalid:
                qCWarning(Playback) << "This file could not be played.";
                QMessageBox::critical(this, tr("Error"), tr("This file could not be played."));
                break;

            default:
                break;
        }
    });

    connect(m_controller, &MediaController::tracksChanged, this, &VuraMainWindow::rebuildTrackMenus);
    connect(m_controller, &MediaController::audioDevicesChanged, this, &VuraMainWindow::rebuildAudioDeviceMenu);
    connect(m_controller, &MediaController::backendChanged, this, [this] { rebuildAudioDeviceMenu(); });
    rebuildAudioDeviceMenu();
    connect(m_controller, &MediaController::capabilitiesChanged, this, &VuraMainWindow::applyCapabilities);

    connect(m_controller, &MediaController::currentItemChanged, this, [this](const PlaylistItem &item) {
        sourceChanged(item.url);
        m_stage->setCaption(item.displayTitle());
        setApplicationWindowTitle();
    });

    connect(m_controller, &MediaController::metaDataChanged, this, [this](const QVariantMap &) {
        setApplicationWindowTitle();
    });

    connect(m_controller, &MediaController::errorOccurred, this, [this](media::ErrorKind kind, const QString &detail) {
        qCWarning(Playback) << "Error occurred: " << QString(detail.isEmpty() ? media::describe(kind) : detail);
    });
}

void VuraMainWindow::initMisc()
{
    m_crashReporter = new CrashReporter(this);
    connect(m_crashReporter, &CrashReporter::scanFinished, this, &VuraMainWindow::crashReportScanFinished);
    connect(m_crashReporter, &CrashReporter::uploadStarted, this, &VuraMainWindow::crashReportUploadStarted);
    connect(m_crashReporter, &CrashReporter::finished, this, &VuraMainWindow::crashReportUploadFinished);
    m_crashReporter->checkForPreviousCrashes();
}

void VuraMainWindow::applyCapabilities(const media::Capabilities &capabilities)
{
    ui->menuVideoTrack->setEnabled(capabilities.videoTrackSelection);
    ui->menuAudioTrack->setEnabled(capabilities.audioTrackSelection);
    //ui->menuSubtitleTrack->setEnabled(capabilities.subtitleTrackSelection);
    ui->menuAudioDevice->setEnabled(capabilities.audioDeviceSelection);
}

void VuraMainWindow::rebuildTrackMenus()
{
    media::Engine *engine = m_controller->engine();
    if (!engine)
        return;

    const auto populate = [this, engine](QMenu *menu, media::TrackType type, bool allowNone) {
        menu->clear();
        auto *group = new QActionGroup(menu);
        const QString active = engine->activeTrack(type);

        if (allowNone) {
            QAction *off = menu->addAction(tr("Off"));
            off->setCheckable(true);
            off->setChecked(active.isEmpty());
            group->addAction(off);
            connect(off, &QAction::triggered, this,
                    [this, type] { m_controller->selectTrack(type, {}); });
        }

        const auto tracks = engine->tracks(type);
        for (int i = 0; i < tracks.size(); ++i) {
            const media::TrackInfo &track = tracks.at(i);
            QAction *action = menu->addAction(track.displayName(i));
            action->setCheckable(true);
            action->setChecked(track.id == active);
            group->addAction(action);
            connect(action, &QAction::triggered, this,
                    [this, type, id = track.id] { m_controller->selectTrack(type, id); });
        }

        if (tracks.isEmpty() && !allowNone)
            menu->addAction(tr("No tracks"))->setEnabled(false);
    };

    populate(ui->menuVideoTrack, media::TrackType::Video, true);
    populate(ui->menuAudioTrack, media::TrackType::Audio, true);
    populate(ui->menuSubtitleTrack, media::TrackType::Subtitle, true);
}

void VuraMainWindow::rebuildAudioDeviceMenu()
{
    QMenu *menu = ui->menuAudioDevice;
    menu->clear();

    auto *group = new QActionGroup(menu);
    const QString active = m_controller->activeAudioDevice();

    QAction *systemDefault = menu->addAction(tr("System Default"));
    systemDefault->setCheckable(true);
    systemDefault->setChecked(active.isEmpty());
    group->addAction(systemDefault);
    connect(systemDefault, &QAction::triggered, this,
            [this] { m_controller->setAudioDevice({}); });

    const auto devices = m_controller->audioDevices();
    if (!devices.isEmpty())
        menu->addSeparator();

    for (const media::AudioDeviceInfo &device : devices) {
        QAction *action = menu->addAction(device.displayName());
        action->setCheckable(true);
        action->setChecked(device.id == active);
        group->addAction(action);
        connect(action, &QAction::triggered, this,
                [this, id = device.id] { m_controller->setAudioDevice(id); });
    }

    if (devices.isEmpty())
        menu->addAction(tr("No audio devices"))->setEnabled(false);

    menu->setEnabled(m_controller->capabilities().audioDeviceSelection);
}

void VuraMainWindow::updateRecentFilesList(const QString &fileName)
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    files.removeAll(fileName);
    files.prepend(fileName);

    while (files.size() > MaxRecentFiles) {
        files.removeLast();
    }

    settings.setValue("recentFileList", files);
}

void VuraMainWindow::setTrackInfo(const QString &trackInfo)
{
    m_trackInfo = trackInfo;
    //this->setWindowTitle("Vura - " + trackInfo);
}

void VuraMainWindow::setApplicationWindowTitle()
{
    const QString title = m_controller->currentTitle();
    setWindowTitle(title.isEmpty() ? QString("Vura %1").arg(VURA_VERSION_STRING) : QString("Vura %1 - %2").arg(VURA_VERSION_STRING).arg(title));
}

QString VuraMainWindow::trackName(const QMediaMetaData &metaData, const int index)
{
    QString name;
    const QString title = metaData.stringValue(QMediaMetaData::Title);
    const auto lang = metaData.value(QMediaMetaData::Language).value<QLocale::Language>();

    if (title.isEmpty()) {
        if (lang == QLocale::Language::AnyLanguage)
            name = tr("Track %1").arg(index + 1);
        else
            name = QLocale::languageToString(lang);
    } else {
        if (lang == QLocale::Language::AnyLanguage)
            name = title;
        else
            name = QStringLiteral("%1 - [%2]").arg(title).arg(QLocale::languageToString(lang));
    }
    return name;
}

void VuraMainWindow::updateMarkerMenuItems()
{
    ui->actionMarkersEditSelectedMarker->setEnabled(m_videoMarkerController->checkMarkerProximity());
}

void VuraMainWindow::onUpdateConfirmed(const QString &targetDownloadUrl, const QString &expectedHash)
{
    auto *progressDialog = new QProgressDialog(tr("Downloading Update..."), tr("Cancel"), 0, 100, this);
    progressDialog->setWindowModality(Qt::WindowModal);

    auto *updater = new Updater(this);

    connect(updater, &Updater::downloadProgress, this, [progressDialog](const qint64 received, const qint64 total) {
        if (total > 0) {
            const int percentage = static_cast<int>((received * 100) / total);
            progressDialog->setValue(percentage);
        }
    });

    connect(progressDialog, &QProgressDialog::canceled, updater, []() {
        // Handle download abortion if necessary
    });

    connect(updater, &Updater::downloadFinished, this, [progressDialog](const bool success, const QString &message) {
        progressDialog->close();
        if (!success) {
            QMessageBox::critical(nullptr, tr("Update Error"), message);
        }
    });

    updater->startDownload(targetDownloadUrl, expectedHash);
}

void VuraMainWindow::crashReportScanFinished(const bool crashFileExists)
{
    if (crashFileExists) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Vura Crash Recovery"),
            tr("Vura Video Player closed unexpectedly during your last session.\n\n"
            "Would you like to send the crash dump to the developers to help fix the issue?"),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            qDebug() << "User selected to send crash files.";
            m_crashReporter->uploadCrashFile(true);
        } else {
            qDebug() << "User selected not to send crash files.";
            m_crashReporter->uploadCrashFile(false);
        }
    }
}

void VuraMainWindow::crashReportUploadStarted() {}

void VuraMainWindow::crashReportUploadFinished(const bool success, const QString& message)
{
    if (success) {
        QMessageBox::information(this, tr("Vura Crash Recovery"), tr("Crash report uploaded successfully!"));
    } else {
        QMessageBox::warning(this, tr("Vura Crash Recovery"),
            tr("Failed to upload crash report.\n\n"
            "Error message: ") + message);
    }
}

void VuraMainWindow::configureUpdater()
{
    /*
    auto* updater = new UpdateChecker(this);

    connect(updater, &UpdateChecker::noUpdateAvailable, this, []() {
        qDebug() << "Vura is up to date";
    });

    // Show a non-blocking notification when an update is found.
    // The download of updater.exe is already running in the background
    // at this point — we just let the user know what's happening.
    connect(updater, &UpdateChecker::updateAvailable,
            this,    [this](const QString& version) {
        // Use a non-modal notification so the user can keep using the app
        // while updater.exe downloads in the background.
                ui->statusBar->show();
        ui->statusBar->showMessage(
            QString("Update %1 found — downloading updater...").arg(version),
            0 // 0 = show until cleared
        );
    });

    // Updater is downloaded and is about to launch — inform the user
    // that the app will close.
    connect(updater, &UpdateChecker::updateReadyToInstall, this, [this]() {
        QMessageBox::information(
            this,
            "Update Ready",
            "A new version of Vura is ready to install.\n\n"
            "Vura will close now and the updater will run automatically.\n"
            "Vura will relaunch when the update is complete."
        );
        // UpdateChecker calls QCoreApplication::quit() after this signal,
        // so we don't need to do anything else here.
    });

    // Show errors in the status bar — don't bother the user with a dialog
    // for a background update check failure
    connect(updater, &UpdateChecker::error, this, [this](const QString& msg) {
        ui->statusBar->showMessage(QString("Update check: %1").arg(msg), 8000);
        qWarning() << "UpdateChecker:" << msg;
    });

    // Check on startup — slightly delayed so the main window appears first
    //QTimer::singleShot(3000, updater, &UpdateChecker::check);

    connect(ui->actionHelpCheckForUpdates, &QAction::triggered, updater, &UpdateChecker::check);
*/
}

void VuraMainWindow::showResumeOverlay(const qint64 savedPosition)
{
    QSettings settings;

    if (m_continuePlaybackWidget)
        m_continuePlaybackWidget->deleteLater();

    m_continuePlaybackWidget = new ContinuePlaybackWidget(savedPosition, this);
    connect(m_continuePlaybackWidget, &ContinuePlaybackWidget::continuePlayback, this, &VuraMainWindow::continuePlaybackAccepted);
    connect(m_continuePlaybackWidget, &ContinuePlaybackWidget::closeWidget, this, &VuraMainWindow::continuePlaybackDeclined);

    // Stop timer when mouse enters the widget
    connect(m_continuePlaybackWidget, &ContinuePlaybackWidget::mouseEntered, this, [this]() {
        if (m_continuePlaybackBannerTimer && m_continuePlaybackBannerTimer->isActive()) {
            m_continuePlaybackBannerTimer->stop();
        }
    });

    // Restart timer when mouse leaves the widget
    connect(m_continuePlaybackWidget, &ContinuePlaybackWidget::mouseLeft, this, [this]() {
        if (m_continuePlaybackBannerTimer) {
            m_continuePlaybackBannerTimer->start();
        }
    });

    ui->verticalLayout->insertWidget(0, m_continuePlaybackWidget);
    ui->verticalLayout->setStretch(1, 1);

    if (m_continuePlaybackBannerTimer) {
        m_continuePlaybackBannerTimer->stop();
        delete m_continuePlaybackBannerTimer;
        m_continuePlaybackBannerTimer = nullptr;
    }

    int continuePlaybackBannerTime = settings.value("continuePlaybackBannerTime", 5).toInt();
    m_continuePlaybackBannerTimer = new QTimer(this);
    m_continuePlaybackBannerTimer->setSingleShot(true);
    m_continuePlaybackBannerTimer->setInterval(continuePlaybackBannerTime * 1000);
    connect(m_continuePlaybackBannerTimer, &QTimer::timeout, this, &VuraMainWindow::continuePlaybackDelete);
    m_continuePlaybackBannerTimer->start();
}

void VuraMainWindow::continuePlaybackDeclined()
{
    continuePlaybackDelete();
}

void VuraMainWindow::continuePlaybackAccepted(const qint64 savedPosition)
{
    m_controller->seek(savedPosition);
    continuePlaybackDelete();
}

void VuraMainWindow::continuePlaybackDelete()
{
    if (ui->verticalLayout->indexOf(m_continuePlaybackWidget) != -1)
        ui->verticalLayout->removeWidget(m_continuePlaybackWidget);

    if (m_continuePlaybackWidget) {
        m_continuePlaybackWidget->deleteLater();
        m_continuePlaybackWidget = nullptr;
    }
}

void VuraMainWindow::systemTray_Clicked()
{
    if (isHidden() || isMinimized()) {
        showNormal();
        activateWindow();
    }
}

void VuraMainWindow::systemTray_Hide(const bool hiding)
{
    if (hiding) {
        hide();
        m_controller->pause();

    } else {
        show();
        showNormal();
        raise();
        activateWindow();
    }
}
