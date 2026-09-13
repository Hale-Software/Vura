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

#include "Slider.h"
#include "PlaylistEmptyStateWidget.h"

#include <ui-config.h>

#include <libvura/logging/logger.h>
#include <libvura/media/media-controller.h>
#include <libvura/media/playlist.h>
#include <libvura/media/video-stage.h>
#include <libvura/media/sleep-inhibitor.h>

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

const QStringList kMediaFilters = {
    QStringLiteral("Media files (*.mp4 *.mkv *.avi *.mov *.webm *.mp3 *.flac *.m4a *.ogg *.opus "
                   "*.wav *.aac *.wmv *.mpg *.mpeg *.ts)"),
    QStringLiteral("All files (*)"),
};

} // namespace


VuraMainWindow::VuraMainWindow(MediaController *controller, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::VuraMainWindow),
      m_controller(controller),
      m_sleepInhibitor(new SleepInhibitor(this))
{
    ui->setupUi(this);
    setAcceptDrops(true);

    m_stage = new VideoStage(this);
    setCentralWidget(m_stage);

    m_controller->setVideoOutputProvider([this](media::Engine *engine) { return m_stage->outputFor(engine); });

    initSystemTray();
    buildMenus();
    buildPlaylistDock();
    connectController();
    initUI();
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
    //m_videoMarkerController->saveVideoMarkers();
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
            this->resize(1200, 700);
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

    bool replacePlaylist = false;
    if (replacePlaylist)
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
            //m_videoSliderHideTimer->start();
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
    this->setWindowState(this->windowState() | Qt::WindowMaximized);
}

void VuraMainWindow::setMainWindowVisibility(const bool state)
{
    if (state) {
        this->show();
        this->showNormal();
        this->raise();
        this->activateWindow();
    }
}

void VuraMainWindow::openFile(const QString &file)
{
    if (file.isEmpty()) {
        qDebug() << "No file specified.";
        return;
    }
    qDebug() << "Open file requested. File: " << file;

    bool replacePlaylist = false;
    if (replacePlaylist)
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

    bool replacePlaylist = false;
    if (replacePlaylist)
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

    bool replacePlaylist = false;
    if (replacePlaylist)
        openPaths({QUrl(formattedUrl)});
    else
        m_controller->enqueue({QUrl(formattedUrl)});
}

void VuraMainWindow::stateChanged(const media::PlaybackState state)
{
    //if (state == media::PlaybackState::Playing && !m_videoSliderHideTimer->isActive()) {
    //    m_videoSliderHideTimer->start();
    //} else {
    //    m_videoSliderHideTimer->stop();
    //    m_videoSliderWidget->show();
    //    this->unsetCursor();
    //}
}

void VuraMainWindow::sourceChanged(const QUrl &source)
{
    qCDebug(Core) << "Source changed to: " << source.toString();

    QSettings settings;

    m_currentSource = source;
    m_videoMarkerController->loadVideoMarkers(source);
    setApplicationWindowTitle();

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

    // Media Change Alert
    const int showMediaChangeNotification = settings.value("showMediaChangeNotification", 1).toInt();
    switch (showMediaChangeNotification) {
        // Never alert
        case 0:
            break;
        // Alert when minimized
        case 1:
            if (this->windowState() & Qt::WindowMinimized) {
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
}

void VuraMainWindow::durationChanged(const qint64 duration)
{
    m_duration = static_cast<int>(duration) / 1000;
}

void VuraMainWindow::errorOccurred(const QString &errorMessage)
{
    qCCritical(Core) << "QMediaPlayer Error: " << errorMessage;
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
            if (this->isFullScreen()) {
                m_videoSliderWidget->hide();
                this->setCursor(Qt::BlankCursor);
            }
            break;
        case 2:
            if (this->isMaximized()) {
                m_videoSliderWidget->hide();
                this->setCursor(Qt::BlankCursor);
            }
        case 3:
            m_videoSliderWidget->hide();
            this->setCursor(Qt::BlankCursor);
            break;
        default:
            break;
    }
}

void VuraMainWindow::resetVideoSliderVisibility()
{
    //m_videoSliderHideTimer->stop();
    m_videoSliderWidget->show();
    this->unsetCursor();

    //if (m_controller->playbackState() == media::PlaybackState::Playing && !m_videoSliderHideTimer->isActive())
    //    m_videoSliderHideTimer->start();
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
        bool replacePlaylist = false;
        if (replacePlaylist)
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
        bool replacePlaylist = false;
        if (replacePlaylist)
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

        bool replacePlaylist = false;
        if (replacePlaylist)
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

        bool replacePlaylist = false;
        if (replacePlaylist)
            openPaths(urls);
        else
            m_controller->enqueue(urls);
    }
}

void VuraMainWindow::actionFileOpenPlaylist()
{

}

void VuraMainWindow::actionFileSavePlaylist()
{

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
    qDebug() << "Current Height: " << QString::number(height()) << ". Current Width: " << QString::number(width());
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
        bool replacePlaylist = false;
        if (replacePlaylist)
            openPaths({QUrl(networkUrl)});
        else
            m_controller->enqueue({QUrl(networkUrl)});
    }
}

void VuraMainWindow::actionEmergencyClose()
{
    m_controller->pause();
    this->setWindowState(Qt::WindowMinimized);
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

        ui->menubar->show();
        m_videoSliderWidget->show();
        if (m_wasPlaylistShowing)
            m_playlistDock->show();

    } else {
        m_wasPlaylistShowing = m_playlistDock->isVisible();
        showFullScreen();

        ui->menubar->hide();
        m_videoSliderWidget->hide();
        m_playlistDock->hide();
    }
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
        emit restartProgram();
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
        //ui->verticalLayout->removeWidget(m_videoControlWidget);
        //delete m_videoControlWidget;
        m_showingVideoControls = false;
    } else {
        //m_videoControlWidget = new VideoControlWidget(this);
        //m_videoControlWidget->setMuted(m_controller->isMuted());
        //m_videoControlWidget->setVolume(m_controller->volume());

        //ui->verticalLayout->addWidget(m_videoControlWidget);

        //connect(m_controller, &MediaController::playbackStateChanged, m_videoControlWidget, &VideoControlWidget::setState);
        //connect(m_videoControlWidget, &VideoControlWidget::play, m_controller, &MediaController::play);
        //connect(m_videoControlWidget, &VideoControlWidget::pause, m_controller, &MediaController::pause);
        //connect(m_videoControlWidget, &VideoControlWidget::stop, m_controller, &MediaController::stop);
        //connect(m_videoControlWidget, &VideoControlWidget::changeVolume, this, [this](int value) {
        //    m_controller->setVolume(sliderToLinear(value));
        //});

        m_showingVideoControls = true;
    }
    ui->actionViewToggleVideoControls->setChecked(m_showingVideoControls);
}

void VuraMainWindow::populateAudioDevicesMenu()
{
    /*
    ui->menuAudioDevice->clear();

    auto* deviceGroup = new QActionGroup(this);
    deviceGroup->setExclusive(true);

    const QAudioDevice currentDevice = m_playbackController->getAudioOutput()->device();

    for (const QAudioDevice &device : QMediaDevices::audioOutputs()) {
        auto *action = new QAction(device.description(), this);
        action->setCheckable(true);

        if (device.id() == currentDevice.id()) {
            action->setChecked(true);
        }

        deviceGroup->addAction(action);
        ui->menuAudioDevice->addAction(action);

        connect(action, &QAction::triggered, this, [this, device]() {
            m_playbackController->getAudioOutput()->setDevice(device);
        });
    }
    */
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
    if (m_mediaInformationDialog)
        m_mediaInformationDialog->close();

    m_mediaInformationDialog = new MediaInformationDialog(this);

    //if (!m_controller->currentTitle().isEmpty())
    //    m_mediaInformationDialog->setMetaData(*m_playbackController->getMetadata());

    m_mediaInformationDialog->show();
}

void VuraMainWindow::actionViewToggleMarkersCumshotMarkers()
{
    m_cumshotMarkerVisible = !m_cumshotMarkerVisible;
    ui->actionViewToggleMarkersCumshotMarkers->setChecked(m_cumshotMarkerVisible);
    m_videoMarkerController->setCumshotMarkerVisibility(m_cumshotMarkerVisible);
    m_videoSlider->setMarkerTypeVisible("cumshot", m_cumshotMarkerVisible);
}

void VuraMainWindow::actionViewToggleMarkersCyanMarkers()
{
    m_cyanMarkerVisible = !m_cyanMarkerVisible;
    ui->actionViewToggleMarkersCyanMarkers->setChecked(m_cyanMarkerVisible);
    m_videoMarkerController->setCyanMarkerVisibility(m_cyanMarkerVisible);
    m_videoSlider->setMarkerTypeVisible("cyan", m_cyanMarkerVisible);
}

void VuraMainWindow::actionViewToggleMarkersDialogMarkers()
{
    m_dialogMarkerVisible = !m_dialogMarkerVisible;
    ui->actionViewToggleMarkersDialogMarkers->setChecked(m_dialogMarkerVisible);
    m_videoMarkerController->setDialogMarkerVisibility(m_dialogMarkerVisible);
    m_videoSlider->setMarkerTypeVisible("dialog", m_dialogMarkerVisible);
}

void VuraMainWindow::actionViewToggleMarkersMagentaMarkers()
{
    m_magentaMarkerVisible = !m_magentaMarkerVisible;
    ui->actionViewToggleMarkersMagentaMarkers->setChecked(m_magentaMarkerVisible);
    m_videoMarkerController->setMagentaMarkerVisibility(m_magentaMarkerVisible);
    m_videoSlider->setMarkerTypeVisible("magenta", m_magentaMarkerVisible);
}

void VuraMainWindow::actionViewToggleMarkersMarkers()
{
    m_markerVisible = !m_markerVisible;
    ui->actionViewToggleMarkersMarkers->setChecked(m_markerVisible);
    m_videoMarkerController->setMarkerVisibility(m_markerVisible);
    m_videoSlider->setMarkerTypeVisible("marker", m_markerVisible);
}

void VuraMainWindow::actionViewToggleMarkersOrangeMarkers()
{
    m_orangeMarkerVisible = !m_orangeMarkerVisible;
    ui->actionViewToggleMarkersOrangeMarkers->setChecked(m_orangeMarkerVisible);
    m_videoMarkerController->setOrangeMarkerVisibility(m_orangeMarkerVisible);
    m_videoSlider->setMarkerTypeVisible("orange", m_orangeMarkerVisible);
}

void VuraMainWindow::actionViewToggleMarkersSceneMarkers()
{
    m_sceneMarkerVisible = !m_sceneMarkerVisible;
    ui->actionViewToggleMarkersSceneMarkers->setChecked(m_sceneMarkerVisible);
    m_videoMarkerController->setSceneMarkerVisibility(m_sceneMarkerVisible);
    m_videoSlider->setMarkerTypeVisible("scene", m_sceneMarkerVisible);
}

void VuraMainWindow::actionViewToggleMarkersStripMarkers()
{
    m_stripMarkerVisible = !m_stripMarkerVisible;
    ui->actionViewToggleMarkersStripMarkers->setChecked(m_stripMarkerVisible);
    m_videoMarkerController->setStripMarkerVisibility(m_stripMarkerVisible);
    m_videoSlider->setMarkerTypeVisible("strip", m_stripMarkerVisible);
}

void VuraMainWindow::actionPlaybackModeDoNotLoopPlaylist()
{
    /*
    PlaylistController::PlaybackMode mode = m_playlistController->playbackMode();
    if (mode == PlaylistController::DoNotLoopPlaylist) {
        ui->actionPlaybackModeDoNotLoopPlaylist->setChecked(true);
        return;
    }

    m_playlistController->setPlaybackMode(PlaylistController::DoNotLoopPlaylist);
    ui->actionPlaybackModeDoNotLoopPlaylist->setChecked(true);
    ui->actionPlaybackModeLoopCurrentTrack->setChecked(false);
    ui->actionPlaybackModeLoopPlaylist->setChecked(false);
    ui->actionPlaybackModeShuffle->setChecked(false);
    */
}

void VuraMainWindow::actionPlaybackModeLoopCurrentTrack()
{
    /*
    PlaylistController::PlaybackMode mode = m_playlistController->playbackMode();
    if (mode == PlaylistController::LoopCurrentVideo) {
        ui->actionPlaybackModeLoopCurrentTrack->setChecked(true);
        return;
    }

    m_playlistController->setPlaybackMode(PlaylistController::LoopCurrentVideo);
    ui->actionPlaybackModeDoNotLoopPlaylist->setChecked(false);
    ui->actionPlaybackModeLoopCurrentTrack->setChecked(true);
    ui->actionPlaybackModeLoopPlaylist->setChecked(false);
    ui->actionPlaybackModeShuffle->setChecked(false);
    */
}

void VuraMainWindow::actionPlaybackModeLoopPlaylist()
{
    /*
    PlaylistController::PlaybackMode mode = m_playlistController->playbackMode();
    if (mode == PlaylistController::LoopPlaylist) {
        ui->actionPlaybackModeLoopPlaylist->setChecked(true);
        return;
    }

    m_playlistController->setPlaybackMode(PlaylistController::LoopPlaylist);
    ui->actionPlaybackModeDoNotLoopPlaylist->setChecked(false);
    ui->actionPlaybackModeLoopCurrentTrack->setChecked(false);
    ui->actionPlaybackModeLoopPlaylist->setChecked(true);
    ui->actionPlaybackModeShuffle->setChecked(false);
    */
}

void VuraMainWindow::actionPlaybackModeShuffle()
{
    /*
    PlaylistController::PlaybackMode mode = m_playlistController->playbackMode();
    if (mode == PlaylistController::Shuffle) {
        ui->actionPlaybackModeShuffle->setChecked(true);
        return;
    }

    m_playlistController->setPlaybackMode(PlaylistController::Shuffle);
    ui->actionPlaybackModeDoNotLoopPlaylist->setChecked(false);
    ui->actionPlaybackModeLoopCurrentTrack->setChecked(false);
    ui->actionPlaybackModeLoopPlaylist->setChecked(false);
    ui->actionPlaybackModeShuffle->setChecked(true);
    */
}

void VuraMainWindow::actionMarkersAddCumshotMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addCumshotMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddCyanMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addCyanMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddDialogMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addDialogMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddMagentaMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addMagentaMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddOrangeMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addOrangeMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddSceneMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addSceneMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersAddStripMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoMarkerController->addStripMarker(sliderPercent);
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

void VuraMainWindow::actionMarkersClearSelectedMarker()
{
    m_videoMarkerController->clearSelectedMarker(getSliderPercent());
}

void VuraMainWindow::actionMarkersEditSelectedMarker()
{
    qDebug() << "Current duration: " << m_duration;
    const VideoMarkerRecord marker = m_videoMarkerController->getSelectedMarker(getSliderPercent());
    if (marker.id <= 0)
        return;

    if (m_markerEditDialog)
        m_markerEditDialog->close();

    m_markerEditDialog = new MarkerEditDialog(marker, m_duration, this);
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

void VuraMainWindow::actionMarkersGoToNextMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoSlider->goToNextMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersGoToOut() {}

void VuraMainWindow::actionMarkersGoToPreviousMarker() const
{
    const double sliderPercent = getSliderPercent();
    m_videoSlider->goToPreviousMarker(sliderPercent);
}

void VuraMainWindow::actionMarkersMarkIn() {}

void VuraMainWindow::actionMarkersMarkOut() {}

void VuraMainWindow::actionSubtitlesOpenSubtitlesFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Subtitle File"),
        "/home",
        tr("All Files (*.*)")
        );

    //if (!filePath.isEmpty())
        //m_playbackController->setSubtitleFile(filePath);
}

void VuraMainWindow::actionSubtitlesToggleSubtitles(bool checked)
{
    //m_playbackController->setSubtitleEnabled(checked);
}

void VuraMainWindow::setCurrentFile(const QUrl &mediaUrl)
{
    if (mediaUrl.isLocalFile()) {
        updateRecentFilesList(mediaUrl.toLocalFile());
    } else {
        updateRecentFilesList(mediaUrl.toString());
    }
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
    
    connect(ui->actionPlaybackModeDoNotLoopPlaylist, &QAction::triggered, this, &VuraMainWindow::actionPlaybackModeDoNotLoopPlaylist);
    this->addAction(ui->actionPlaybackModeDoNotLoopPlaylist);
    ui->actionPlaybackModeDoNotLoopPlaylist->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackModeLoopCurrentTrack, &QAction::triggered, this, &VuraMainWindow::actionPlaybackModeLoopCurrentTrack);
    this->addAction(ui->actionPlaybackModeLoopCurrentTrack);
    ui->actionPlaybackModeLoopCurrentTrack->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackModeLoopPlaylist, &QAction::triggered, this, &VuraMainWindow::actionPlaybackModeLoopPlaylist);
    this->addAction(ui->actionPlaybackModeLoopPlaylist);
    ui->actionPlaybackModeLoopPlaylist->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionPlaybackModeShuffle, &QAction::triggered, this, &VuraMainWindow::actionPlaybackModeShuffle);
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


    // Marker Actions
    connect(ui->actionMarkersAddCumshotMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddCumshotMarker);
    this->addAction(ui->actionMarkersAddCumshotMarker);
    ui->actionMarkersAddCumshotMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddCyanMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddCyanMarker);
    this->addAction(ui->actionMarkersAddCyanMarker);
    ui->actionMarkersAddCyanMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddDialogMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddDialogMarker);
    this->addAction(ui->actionMarkersAddDialogMarker);
    ui->actionMarkersAddDialogMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddMagentaMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddMagentaMarker);
    this->addAction(ui->actionMarkersAddMagentaMarker);
    ui->actionMarkersAddMagentaMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddMarker);
    this->addAction(ui->actionMarkersAddMarker);
    ui->actionMarkersAddMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddOrangeMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddOrangeMarker);
    this->addAction(ui->actionMarkersAddOrangeMarker);
    ui->actionMarkersAddOrangeMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddSceneMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddSceneMarker);
    this->addAction(ui->actionMarkersAddSceneMarker);
    ui->actionMarkersAddSceneMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersAddStripMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersAddStripMarker);
    this->addAction(ui->actionMarkersAddStripMarker);
    ui->actionMarkersAddStripMarker->setShortcutContext(Qt::WindowShortcut);

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

    connect(ui->actionMarkersClearSelectedMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersClearSelectedMarker);
    this->addAction(ui->actionMarkersClearSelectedMarker);
    ui->actionMarkersClearSelectedMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersEditSelectedMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersEditSelectedMarker);
    this->addAction(ui->actionMarkersEditSelectedMarker);
    ui->actionMarkersEditSelectedMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersGoToIn, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToIn);
    this->addAction(ui->actionMarkersGoToIn);
    ui->actionMarkersGoToIn->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersGoToNextMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToNextMarker);
    this->addAction(ui->actionMarkersGoToNextMarker);
    ui->actionMarkersGoToNextMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersGoToOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToOut);
    this->addAction(ui->actionMarkersGoToOut);
    ui->actionMarkersGoToOut->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersGoToPreviousMarker, &QAction::triggered, this, &VuraMainWindow::actionMarkersGoToPreviousMarker);
    this->addAction(ui->actionMarkersGoToPreviousMarker);
    ui->actionMarkersGoToPreviousMarker->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersMarkIn, &QAction::triggered, this, &VuraMainWindow::actionMarkersMarkIn);
    this->addAction(ui->actionMarkersMarkIn);
    ui->actionMarkersMarkIn->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionMarkersMarkOut, &QAction::triggered, this, &VuraMainWindow::actionMarkersMarkOut);
    this->addAction(ui->actionMarkersMarkOut);
    ui->actionMarkersMarkOut->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionFileConvertSave, &QAction::triggered, this, &VuraMainWindow::actionShowConvertMedia);
    this->addAction(ui->actionFileConvertSave);
    ui->actionFileConvertSave->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleMarkersCumshotMarkers, &QAction::triggered, this, &VuraMainWindow::actionViewToggleMarkersCumshotMarkers);
    this->addAction(ui->actionViewToggleMarkersCumshotMarkers);
    ui->actionViewToggleMarkersCumshotMarkers->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleMarkersCyanMarkers, &QAction::triggered, this, &VuraMainWindow::actionViewToggleMarkersCyanMarkers);
    this->addAction(ui->actionViewToggleMarkersCyanMarkers);
    ui->actionViewToggleMarkersCyanMarkers->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleMarkersDialogMarkers, &QAction::triggered, this, &VuraMainWindow::actionViewToggleMarkersDialogMarkers);
    this->addAction(ui->actionViewToggleMarkersDialogMarkers);
    ui->actionViewToggleMarkersDialogMarkers->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleMarkersMagentaMarkers, &QAction::triggered, this, &VuraMainWindow::actionViewToggleMarkersMagentaMarkers);
    this->addAction(ui->actionViewToggleMarkersMagentaMarkers);
    ui->actionViewToggleMarkersMagentaMarkers->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleMarkersMarkers, &QAction::triggered, this, &VuraMainWindow::actionViewToggleMarkersMarkers);
    this->addAction(ui->actionViewToggleMarkersMarkers);
    ui->actionViewToggleMarkersMarkers->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleMarkersOrangeMarkers, &QAction::triggered, this, &VuraMainWindow::actionViewToggleMarkersOrangeMarkers);
    this->addAction(ui->actionViewToggleMarkersOrangeMarkers);
    ui->actionViewToggleMarkersOrangeMarkers->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleMarkersSceneMarkers, &QAction::triggered, this, &VuraMainWindow::actionViewToggleMarkersSceneMarkers);
    this->addAction(ui->actionViewToggleMarkersSceneMarkers);
    ui->actionViewToggleMarkersSceneMarkers->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionViewToggleMarkersStripMarkers, &QAction::triggered, this, &VuraMainWindow::actionViewToggleMarkersStripMarkers);
    this->addAction(ui->actionViewToggleMarkersStripMarkers);
    ui->actionViewToggleMarkersStripMarkers->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionSubtitlesOpenSubtitlesFile, &QAction::triggered, this, &VuraMainWindow::actionSubtitlesOpenSubtitlesFile);
    this->addAction(ui->actionSubtitlesOpenSubtitlesFile);
    ui->actionSubtitlesOpenSubtitlesFile->setShortcutContext(Qt::WindowShortcut);

    connect(ui->actionSubtitlesToggleSubtitles, &QAction::toggled, this, &VuraMainWindow::actionSubtitlesToggleSubtitles);
    this->addAction(ui->actionSubtitlesToggleSubtitles);
    ui->actionSubtitlesToggleSubtitles->setShortcutContext(Qt::WindowShortcut);

    connect(&m_mediaDevices, &QMediaDevices::audioOutputsChanged, this, &VuraMainWindow::populateAudioDevicesMenu);
    populateAudioDevicesMenu();
}

void VuraMainWindow::buildPlaylistDock()
{
    m_playlistDock = new QDockWidget(tr("Playlist"), this);
    m_playlistDock->setObjectName(QStringLiteral("playlistDock"));

    m_playlistWidget = new PlaylistWidget(this);
    m_playlistWidget->setPlaylistModel(m_controller->playlist());

    connect(m_playlistWidget, &PlaylistWidget::doubleClicked, this, [this](const QModelIndex &index) {
        m_controller->playIndex(index.row());
    });

    m_playlistDock->setWidget(m_playlistWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_playlistDock);

    ui->actionViewTogglePlaylist->setChecked(m_playlistDock->isVisible());
}

void VuraMainWindow::initUI()
{
    QSettings settings;

    //ui->playlistWidget->setStyleSheet("QStackedWidget { border: 1px solid #878787; border-right: none; border-bottom: none; }");

    const int defaultWindowHeight = settings.value("defaultWindowHeight", 550).toInt();
    const int defaultWindowWidth = settings.value("defaultWindowWidth", 955).toInt();
    resize(defaultWindowWidth, defaultWindowHeight);

    if (settings.value("rememberWindowSize", false).toBool()) {
        qDebug() << "Rememmber window size setting set to true. Restoring previous window size.";
        restoreGeometry(settings.value("geometry").toByteArray());
    }

    m_videoMarkerController = new VideoMarkerController(this);
    m_videoSlider = new VideoSlider(m_videoMarkerController, this);
    m_videoSliderWidget = new VideoSliderWidget(*m_videoSlider, this);
    connect(m_videoMarkerController, &VideoMarkerController::markerAdded, m_videoSlider, &VideoSlider::updateVideoSlider);
    connect(m_videoMarkerController, &VideoMarkerController::markersLoaded, m_videoSlider, &VideoSlider::loadVideoMarkers);
    connect(m_videoMarkerController, &VideoMarkerController::markersUpdated, m_videoSlider, &VideoSlider::updateVideoSlider);
    
    connect(m_videoSlider, &VideoSlider::scrubbed, m_controller, [this](qint64 ms) {
        m_controller->seek(ms);
    });
    connect(m_videoSlider, &VideoSlider::scrubFinished, m_controller, [this](qint64 ms) {
        m_controller->seek(ms);
    });

    m_videoSliderDock = new QDockWidget(this);
    m_videoSliderDock->setObjectName(QStringLiteral("videoSliderDock"));

    m_videoSliderDock->setWidget(m_videoSliderWidget);
    m_videoSliderDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::BottomDockWidgetArea, m_videoSliderDock);

    //ui->verticalLayout->addWidget(m_videoSliderWidget);
    //ui->verticalLayout->setStretch(0, 1);

    //m_videoControlWidget = new VideoControlWidget(this);
    //m_videoControlWidget->setMuted(m_controller->isMuted());
    //m_videoControlWidget->setVolume(m_controller->volume());

    //ui->verticalLayout->addWidget(m_videoControlWidget);

    //connect(m_controller, &MediaController::playbackStateChanged, m_videoControlWidget, &VideoControlWidget::setState);
    //connect(m_videoControlWidget, &VideoControlWidget::play, m_controller, &MediaController::play);
    //connect(m_videoControlWidget, &VideoControlWidget::pause, m_controller, &MediaController::pause);
    //connect(m_videoControlWidget, &VideoControlWidget::stop, m_controller, &MediaController::stop);
    //connect(m_videoControlWidget, &VideoControlWidget::changeVolume, this, [this](int value) {
    //    m_controller->setVolume(sliderToLinear(value));
    //});

    m_showingVideoControls = false;
    ui->actionViewToggleVideoControls->setChecked(m_showingVideoControls);

    //m_videoControlDock = new QDockWidget(tr("Video Control"), this);
    //m_videoControlDock->setObjectName(QStringLiteral("videoControlDock"));
    //m_videoControlDock->setWidget(m_videoControlWidget);
    //addDockWidget(Qt::BottomDockWidgetArea, m_videoControlDock);
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
        durationChanged(ms);
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
                qCDebug(Playback) << "Media Status Changed: Invalid";
                break;

            default:
                break;
        }
    });

    connect(m_controller, &MediaController::capabilitiesChanged, this, &VuraMainWindow::applyCapabilities);

    connect(m_controller, &MediaController::currentItemChanged, this, [this](const PlaylistItem &item) {
        sourceChanged(item.url);
        m_stage->setCaption(item.displayTitle());
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
    ui->menuSubtitleTrack->setEnabled(capabilities.subtitleTrackSelection);
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
    this->setWindowTitle("Vura - " + trackInfo);
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
    ui->actionMarkersEditSelectedMarker->setEnabled(checkMarkerProximity());
}

VideoMarkerRecord VuraMainWindow::findNearestVisibleMarker(const double sliderPercent, const double markerRange) const
{
    VideoMarkerRecord best;
    best.timestampMs = std::numeric_limits<double>::quiet_NaN();

    for (const VideoMarkerRecord &marker : m_videoMarkerController->getVideoMarkers()) {
        if (!m_videoSlider->getMarkerTypesVisible(marker.markerType)) continue;
        const double dist = std::abs(marker.timestampMs - sliderPercent);
        if (dist > markerRange) continue;
        if (std::isnan(best.timestampMs) || dist < std::abs(best.timestampMs - sliderPercent))
            best = marker;
    }
    return best;
}

double VuraMainWindow::getSliderPercent() const
{
    const double distanceFromMin = m_videoSlider->value() - m_videoSlider->minimum();
    const double sliderRange = m_videoSlider->maximum() - m_videoSlider->minimum();
    return distanceFromMin / sliderRange;
}

bool VuraMainWindow::checkMarkerProximity() const
{
    const QSettings settings;
    const double sliderPercent = getSliderPercent();
    double markerProximityThreshold = settings.value("markerProximityThreshold", 0.005).toDouble();

    const auto isVisibleMarkerNearSlider = [this, sliderPercent, markerProximityThreshold](const VideoMarkerRecord &marker) {
        if (!m_videoSlider->getMarkerTypesVisible(marker.markerType)) {
            return false;
        }

        const double distanceToSlider = std::abs(marker.timestampMs - sliderPercent);
        return distanceToSlider <= markerProximityThreshold;
    };

    return std::any_of(
        m_videoMarkerController->getVideoMarkers().cbegin(),
        m_videoMarkerController->getVideoMarkers().cend(),
        isVisibleMarkerNearSlider);
}

bool VuraMainWindow::isPreviousMarkerAvailable(const VideoMarkerRecord &videoMarker) const
{
    VideoMarkerRecord previousMarker;
    previousMarker.timestampMs = std::numeric_limits<double>::quiet_NaN();

    for (const VideoMarkerRecord &marker : m_videoMarkerController->getVideoMarkers()) {
        if (marker.timestampMs < videoMarker.timestampMs) {
            if (std::isnan(previousMarker.timestampMs)) {
                previousMarker = marker;

            } else {
                if (marker.timestampMs > previousMarker.timestampMs) {
                    previousMarker = marker;
                }
            }
        }
    }

    if (std::isnan(previousMarker.timestampMs)) {
        return false;
    }
    return true;
}

bool VuraMainWindow::isNextMarkerAvailable(const VideoMarkerRecord &videoMarker) const
{
    VideoMarkerRecord nextMarker;
    nextMarker.timestampMs = std::numeric_limits<double>::quiet_NaN();

    for (const VideoMarkerRecord &marker : m_videoMarkerController->getVideoMarkers()) {
        if (marker.timestampMs > videoMarker.timestampMs) {
            if (std::isnan(nextMarker.timestampMs)) {
                nextMarker = marker;

            } else {
                if (marker.timestampMs < nextMarker.timestampMs) {
                    nextMarker = marker;
                }
            }
        }
    }

    if (std::isnan(nextMarker.timestampMs)) {
        return false;
    }
    return true;
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
    if (m_continuePlaybackDock) {
        m_continuePlaybackDock->deleteLater();
        m_continuePlaybackDock = nullptr;
    }

    m_continuePlaybackDock = new QDockWidget(tr("Continue Playback"), this);
    m_continuePlaybackDock->setObjectName(QStringLiteral("continuePlaybackDock"));

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

    m_continuePlaybackDock->setWidget(m_continuePlaybackWidget);
    addDockWidget(Qt::TopDockWidgetArea, m_continuePlaybackDock);

    //ui->verticalLayout->insertWidget(0, m_continuePlaybackWidget);
    //ui->verticalLayout->setStretch(1, 1);

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
    if (m_continuePlaybackDock) {
        m_continuePlaybackDock->deleteLater();
        m_continuePlaybackDock = nullptr;
    }
    //if (ui->verticalLayout->indexOf(m_continuePlaybackWidget) != -1)
    //    ui->verticalLayout->removeWidget(m_continuePlaybackWidget);

    //if (m_continuePlaybackWidget) {
    //    m_continuePlaybackWidget->deleteLater();
    //    m_continuePlaybackWidget = nullptr;
    //}
}

void VuraMainWindow::systemTray_Clicked()
{
    if (this->isHidden() || this->isMinimized()) {
        this->showNormal();
        this->activateWindow();
    }
}

void VuraMainWindow::systemTray_Hide(const bool hiding)
{
    if (hiding) {
        this->hide();
        m_controller->pause();

    } else {
        this->show();
        this->showNormal();
        this->raise();
        this->activateWindow();
    }
}
