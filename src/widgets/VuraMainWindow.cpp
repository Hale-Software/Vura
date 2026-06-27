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

#include "PlaylistEmptyStateWidget.h"
#include "ui_VuraMainWindow.h"
#include "libvura/media-io/media-functions.h"

#include <ui-config.h>
#include <qglobal.h>

static Blogger* globalRedirector = nullptr;


VuraMainWindow::VuraMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::VuraMainWindow)
{
    setAcceptDrops(true);

    ui->setupUi(this);
    qInstallMessageHandler(Blogger::messageHandler);
    globalRedirector = Blogger::instance();

    this->setStyleSheet(
        "QMainWindow { background-color: #1e1e1e; }"
        "QMenuBar { background-color: #252526; color: #ffffff; padding: 5px; font-family: 'Segoe UI'; }"
        "QMenuBar::item:selected { background-color: #3e3e40; }"
        "QMenu { background-color: #252526; color: #ffffff; border: 1px solid #3e3e40; padding: 5px; }"
        "QMenu::item:selected { background-color: #007acc; }"
    );

    ui->statusBar->hide();

    ui->splitter->setStretchFactor(0, 1);

    m_playbackController = new PlaybackController(this);
    m_playbackController->getPlayer()->setVideoOutput(ui->videoWidget);

    m_playlistController = new PlaylistController(
            ui->playlistView,
            ui->emptyPlaylistView,
            ui->playlistWidget,
            this
        );

    m_videoSlider = new VideoSlider(&m_videoMarkers, this);
    m_videoSliderWidget = new VideoSliderWidget(*m_videoSlider, *m_playbackController->getPlayer(), this);

    ui->verticalLayout->addWidget(m_videoSliderWidget);
    ui->verticalLayout->setStretch(0, 1);

    connect(this, &VuraMainWindow::updateVideoSlider, m_videoSlider, &VideoSlider::updateVideoSlider);
    connect(m_videoSlider, &VideoSlider::valueChanged, m_playbackController, &PlaybackController::seek);
    connect(m_videoSlider, &VideoSlider::sliderPressed, m_playbackController, &PlaybackController::setPaused);
    connect(m_playbackController, &PlaybackController::positionChanged, m_videoSlider, &VideoSlider::setValue);
    connect(m_playbackController, &PlaybackController::durationChanged, m_videoSlider, &VideoSlider::setMaximum);
    connect(m_playbackController, &PlaybackController::sourceChanged, this, &VuraMainWindow::sourceChanged);
    connect(m_playlistController, &PlaylistController::playTrackRequested, m_playbackController, &PlaybackController::playTrack);
    connect(ui->emptyPlaylistView, &PlaylistEmptyStateWidget::requestFileImport, m_playlistController, &PlaylistController::requestFileImport);
    //connect(m_playbackController, &PlaybackController::positionChanged, this, &VuraMainWindow::);
    //connect(m_playbackController, &PlaybackController::durationChanged, this, &VuraMainWindow::);


    connect(ui->actionAudioToggleMute, &QAction::triggered, m_playbackController, &PlaybackController::toggleMute);
    connect(ui->actionAudioVolumeDown, &QAction::triggered, this, &VuraMainWindow::actionVolumeDown);
    connect(ui->actionAudioVolumeUp, &QAction::triggered, this, &VuraMainWindow::actionVolumeUp);
    connect(ui->actionFileEmergencyClose, &QAction::triggered, this, &VuraMainWindow::actionEmergencyClose);
    connect(ui->actionFileExit, &QAction::triggered, this, &VuraMainWindow::actionExit);
    connect(ui->actionFileOpenFile, &QAction::triggered, m_playlistController, &PlaylistController::requestFileImport);
    connect(ui->actionViewPreferences, &QAction::triggered, this, &VuraMainWindow::actionShowSettings);
    connect(ui->actionHelpViewCurrentLog, &QAction::triggered, this, &VuraMainWindow::actionShowLogViewer);
    //connect(ui->actionJump_Backward, &QAction::triggered, this, &VuraMainWindow::actionJumpBackward);
    //connect(ui->actionJump_Forward, &QAction::triggered, this, &VuraMainWindow::actionJumpForward);
    connect(ui->actionPlaybackNext, &QAction::triggered, m_playlistController, &PlaylistController::nextTrack);
    connect(ui->actionPlaybackPrevious, &QAction::triggered, m_playlistController, &PlaylistController::previousTrack);
    connect(ui->actionPlaybackRestartVideo, &QAction::triggered, m_playbackController, &PlaybackController::restart);
    connect(ui->actionPlaybackTogglePlay, &QAction::triggered, m_playbackController, &PlaybackController::togglePlayPause);
    connect(ui->actionToolsTestFunction, &QAction::triggered, this, &VuraMainWindow::actionTestFunction);
    connect(ui->actionVideoFullscreen, &QAction::triggered, this, &VuraMainWindow::actionToggleFullscreen);
    connect(ui->actionViewTogglePlaylist, &QAction::triggered, this, &VuraMainWindow::actionTogglePlaylist);
    connect(ui->actionViewToggleVideoControls, &QAction::triggered, this, &VuraMainWindow::actionToggleVideoControls);

    //connect(ui->action, &QAction::triggered, this, &VuraMainWindow::);

    m_playlistController->hidePlaylist();


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




    qCDebug(Core) << "Application Initialized!";
    qCInfo(Core) << "Vura Version: " << VURA_VERSION_STRING;
}

void VuraMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qCDebug(Core) << "Drop event received in VuraMainWindow";
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();

    } else {
        event->ignore();
    }
}

void VuraMainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();

        // Iterate through all dropped files/folders
        for (const QUrl &url : urlList) {
            // Convert file protocol (file://) to local OS path string
            QString filePath = url.toLocalFile();

            if (!filePath.isEmpty()) {
                qCDebug(Core) << "Dropped File Path:" << filePath;

                // Optional: Check if item is a file or folder
                QFileInfo fileInfo(filePath);
                if (fileInfo.isDir()) {
                    //m_playbackController->loadMedia(filePath);
                    // Handle folder entry
                } else {
                    // Handle file entry
                    m_playbackController->loadMedia(filePath);
                }
            }
        }
        event->acceptProposedAction();
    }
}

void VuraMainWindow::sourceChanged(const QUrl &source)
{
    QSettings settings;

    m_videoMarkers.clear();
    m_videoMarkers = VideoMarkers::read("debug/global.json", source.toString());

    m_videoSlider->setSource(source.toLocalFile());
    qCDebug(Core) << "Source changed to: " << source.toLocalFile();
}

void VuraMainWindow::errorOccurred(const QString &errorMessage)
{
    qCCritical(Core) << "QMediaPlayer Error: " << errorMessage;
    QMessageBox::critical(this, "Media Player Error", errorMessage);
    this->close();
}

void VuraMainWindow::actionTestFunction()
{
    qCDebug(Core) << "Test function called.";
    qCInfo(Core) << "Test function called.";
    qCWarning(Core) << "Test function called.";
    qCCritical(Core) << "Test function called.";
}

void VuraMainWindow::actionVolumeUp()
{
    float oldVolume = m_playbackController->getAudioOutput()->volume();
    if (oldVolume >= 1.0) return;
    m_playbackController->getAudioOutput()->setVolume(oldVolume + 0.1);
}

void VuraMainWindow::actionVolumeDown()
{
    float oldVolume = m_playbackController->getAudioOutput()->volume();
    if (oldVolume <= 0.1) return;
    m_playbackController->getAudioOutput()->setVolume(oldVolume - 0.1);
}

void VuraMainWindow::actionJumpForward()
{
    const qint64 currentPosition = m_playbackController->getPlayer()->position();
    const qint64 duration = m_playbackController->getPlayer()->duration();
    qint64 jumpTo = currentPosition + 30000;
    if (jumpTo > duration) jumpTo = duration;
    m_playbackController->seek(jumpTo);
}

void VuraMainWindow::actionJumpBackward()
{
    const qint64 currentPosition = m_playbackController->getPlayer()->position();
    qint64 jumpTo = currentPosition - 30000;
    if (jumpTo < 0) jumpTo = 0;
    m_playbackController->seek(jumpTo);
}

void VuraMainWindow::actionEmergencyClose()
{
    m_playbackController->pause();
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
    if (ui->videoWidget->isFullScreen()) {
        ui->videoWidget->setFullScreen(false);
        ui->videoWidget->showNormal();
    } else {
        ui->videoWidget->setFullScreen(true);
    }
}

void VuraMainWindow::actionShowSettings()
{
    if (m_settingsWindow)
        m_settingsWindow->close();

    m_settingsWindow = new SettingsWindow(this);
    m_settingsWindow->show();
    m_settingsWindow->setAttribute(Qt::WA_DeleteOnClose, true);
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

void VuraMainWindow::actionTogglePlaylist() const
{
    m_playlistController->togglePlaylist();
}

void VuraMainWindow::actionToggleVideoControls()
{
    if (m_showingVideoControls) {
        ui->verticalLayout->removeWidget(m_videoControlWidget);
        delete m_videoControlWidget;
        m_showingVideoControls = false;

    } else {
        m_videoControlWidget = new VideoControlWidget(this);
        m_videoControlWidget->setMuted(m_playbackController->getAudioOutput()->isMuted());
        m_videoControlWidget->setVolume(m_playbackController->getAudioOutput()->volume());
        ui->verticalLayout->addWidget(m_videoControlWidget);
        connect(m_playbackController->getPlayer(), &QMediaPlayer::playbackStateChanged, m_videoControlWidget, &VideoControlWidget::setState);
        connect(m_videoControlWidget, &VideoControlWidget::play, m_playbackController, &PlaybackController::play);
        connect(m_videoControlWidget, &VideoControlWidget::pause, m_playbackController, &PlaybackController::pause);
        connect(m_videoControlWidget, &VideoControlWidget::stop, m_playbackController, &PlaybackController::stop);
        connect(m_videoControlWidget, &VideoControlWidget::changeVolume, m_playbackController, &PlaybackController::changeVolume);
        connect(m_videoControlWidget, &VideoControlWidget::changeMuting, m_playbackController, &PlaybackController::setMute);
        //connect(m_playbackController->getAudioOutput(), &QAudioOutput::volumeChanged, m_videoControlWidget, &VideoControlWidget::setVolume);
        //connect(m_playbackController->getAudioOutput(), &QAudioOutput::mutedChanged, m_videoControlWidget, &VideoControlWidget::setMuted);
        m_showingVideoControls = true;
    }
}

void VuraMainWindow::setTrackInfo(const QString &trackInfo)
{
    m_trackInfo = trackInfo;
    this->setWindowTitle("Vura - " + trackInfo);
}

void VuraMainWindow::setApplicationWindowTitle()
{
    QString windowTitle;

    if (!m_playbackController->getPlayer()->source().isEmpty()) {
        windowTitle = QString("%1 - Vura %2").arg(MediaFunctions::strippedFileName(m_playbackController->getPlayer()->source().toLocalFile()), VURA_VERSION_STRING);
    } else {
        windowTitle = QString("Vura %1").arg(VURA_VERSION_STRING);
    }

    this->setWindowTitle(windowTitle);
}

QString VuraMainWindow::trackName(const QMediaMetaData &metaData, int index)
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
