#include "hvideowidget.h"
#include "../utility/logger.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

// Global pointer to Logger for use in messageHandler
static Logger* globalRedirector = nullptr;

using namespace Qt::Literals;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QSettings settings;
    //settings.beginGroup("Hotkeys");
    //settings.remove("");
    //settings.endGroup();
    //settings.sync();

    // Set the global redirector and install the custom message handler
    qInstallMessageHandler(Logger::messageHandler);
    globalRedirector = Logger::instance();
    qInfo() << "Starting application...";

    Startup startup;
    startup.Initialize();

    hData = new HData;

    loadSettings();
    configureApplication();
    configureStatusBar();
    configureVideoWidget();
    configureVideoWidgetConnections();
    configureUI();
    configureMenuItems();
    configureHotkeys();
    createRecentFileActions();
    updateRecentFileActions();
    createAudioOutputActions();
    createAudioTrackActions();
    createVideoTrackActions();
    createSubtitleTrackActions();
    updateAudioOutputActions();

    QObject::connect(&m_mediaDevices, &QMediaDevices::audioOutputsChanged, this, [this] {
        updateAudioOutputActions();
    });

    if (!isPlayerAvailable()) {
        qWarning() << "The QMediaPlayer object does not have a valid service. Please check the media service plugins are installed.";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openedWithFile(QString file)
{
    const int previousMediaCount = m_playlist->mediaCount();
    qDebug() << "File " << file << " opened with application.";
    if (!file.isEmpty()) {
        QUrl url = QUrl::fromLocalFile(file);
        if (!isPlaylist(url)) {
            m_playlist->addMedia(url);
            if (m_playlist->mediaCount() > previousMediaCount) {
                auto index = m_playlistModel->index(previousMediaCount, 0);
                ui->playlistView->setCurrentIndex(index);
                jump(index);
            }
        } else {
            m_playlist->loadPlaylist(file);
            if (m_playlist->mediaCount() > previousMediaCount) {
                auto index = m_playlistModel->index(previousMediaCount, 0);
                ui->playlistView->setCurrentIndex(index);
                jump(index);
            }
        }
    }
}

void MainWindow::loadSettings()
{
    QSettings settings;

    m_rememberWindowSize = settings.value("rememberWindowSize", true).toBool();
    m_hashFile = settings.value("hashFile", false).toBool();
    m_jumpSmall = settings.value("jumpSmall", 5).toInt();
    m_jumpMedium = settings.value("jumpMedium", 15).toInt();
    m_jumpLarge = settings.value("jumpLarge", 45).toInt();
    m_jumpExtraLarge = settings.value("jumpExtraLarge", 45).toInt();

    emit refreshSettings();
}

void MainWindow::testFunction()
{
    //MediaFile mediaFile;
    //int fileDuration = mediaFile.file_duration("C:\\Users\\halea\\Videos\\Extra\\Veronica-R-My-Dirty-Maid");
    //int fileDuration = mediaFile.fe_decode_open("C:\\Users\\halea\\Videos\\Extra\\Veronica-R-My-Dirty-Maid");
    //qDebug() << "Media file duration: " << QString::number(fileDuration);
}

#pragma region MEDIA PLAYER

bool MainWindow::isPlayerAvailable() const
{
    return m_player->isAvailable();
}

void MainWindow::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    m_videoSlider->setMaximum(static_cast<int>(duration));
}

void MainWindow::positionChanged(qint64 progress)
{
    if (!m_videoSlider->isSliderDown())
        m_videoSlider->setValue(static_cast<int>(progress));

    updateDurationInfo(progress / 1000);
}

// TODO: Implement
void MainWindow::metaDataChanged()
{
    /*
    auto metaData = m_player->metaData();
    setTrackInfo(QStringLiteral("%1 - %2")
                         .arg(metaData.value(QMediaMetaData::AlbumArtist).toString())
                         .arg(metaData.value(QMediaMetaData::Title).toString()));

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    for (int i = 0; i < QMediaMetaData::NumMetaData; i++) {
        if (QLineEdit *field = qobject_cast<QLineEdit *>(m_metaDataFields[i]))
            field->clear();
        else if (QLabel *label = qobject_cast<QLabel *>(m_metaDataFields[i]))
            label->clear();
        m_metaDataFields[i]->setDisabled(true);
        m_metaDataLabels[i]->setDisabled(true);
    }

    for (auto &&[key, value] : metaData.asKeyValueRange()) {
        int i = int(key);
        if (key == QMediaMetaData::CoverArtImage) {
            if (QLabel *cover = qobject_cast<QLabel *>(m_metaDataFields[key])) {
                QImage coverImage = value.value<QImage>();
                cover->setPixmap(QPixmap::fromImage(coverImage));
            }
        } else if (key == QMediaMetaData::ThumbnailImage) {
            if (QLabel *thumbnail = qobject_cast<QLabel *>(m_metaDataFields[key])) {
                QImage thumbnailImage = value.value<QImage>();
                thumbnail->setPixmap(QPixmap::fromImage(thumbnailImage));
            }
        } else if (QLineEdit *field = qobject_cast<QLineEdit *>(m_metaDataFields[key])) {
            QString stringValue = metaData.stringValue(key);
            field->setText(stringValue);
        }
        m_metaDataFields[i]->setDisabled(false);
        m_metaDataLabels[i]->setDisabled(false);
    }

    const QList<QMediaMetaData> tracks = m_player->videoTracks();
    const int currentVideoTrack = m_player->activeVideoTrack();
    if (currentVideoTrack >= 0 && currentVideoTrack < tracks.size()) {
        const QMediaMetaData track = tracks.value(currentVideoTrack);
        for (const QMediaMetaData::Key &key : track.keys()) {
            if (QLineEdit *field = qobject_cast<QLineEdit *>(m_metaDataFields[key])) {
                QString stringValue = track.stringValue(key);
                field->setText(stringValue);
            }
            m_metaDataFields[key]->setDisabled(true);
            m_metaDataLabels[key]->setDisabled(true);
        }
    }
#endif
*/
}

QString MainWindow::trackName(const QMediaMetaData &metaData, int index)
{
    QString name;
    QString title = metaData.stringValue(QMediaMetaData::Title);
    QLocale::Language lang = metaData.value(QMediaMetaData::Language).value<QLocale::Language>();

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

void MainWindow::tracksChanged()
{
    updateAudioTrackActions();
    updateVideoTrackActions();
    updateSubtitleTrackActions();
}

void MainWindow::playlistPositionChanged(int currentItem)
{
    ui->playlistView->setCurrentIndex(m_playlistModel->index(currentItem, 0));
    m_player->setSource(m_playlist->currentMedia());
}

void MainWindow::sourceChanged(const QUrl &media)
{
    qDebug() << "Media source changed.";
    if (m_hashFile) {
        if (!currentFileHash.isEmpty()) {
            hData->saveMarkers(currentFileHash, markerMap);
        }
        currentFileHash = QString::number(fileHash(media.toLocalFile()));
        markerMap = hData->getMarkers(currentFileHash);
    } else {
        if (!currentFile.isEmpty())
            hData->saveMarkers(currentFile, markerMap);
        markerMap = hData->getMarkers(media.toString());
    }
    currentFile = media.toString();
    qInfo() << "New media source loaded: " << currentFile;
    m_videoSlider->setMarkers(markerMap);
    setWindowTitle(currentFile);
    QByteArray byteArray = currentFile.toUtf8();
    if (h_playing)
    {
        m_player->play();
    }
    else
    {
        m_player->pause();
    }
    h_inMarker = 0;
    h_outMarker = 0;
    //m_videoSlider->setVideoLoaded(true);
}

void MainWindow::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);

    // handle status message
    switch (status) {
        case QMediaPlayer::NoMedia:
        case QMediaPlayer::LoadedMedia:
            setStatusInfo(QString());
            //m_videoSlider->videoLoaded = true;
            break;
        case QMediaPlayer::LoadingMedia:
            setStatusInfo(tr("Loading..."));
            break;
        case QMediaPlayer::BufferingMedia:
        case QMediaPlayer::BufferedMedia:
            setStatusInfo(tr("Buffering %1%").arg(qRound(m_player->bufferProgress() * 100.)));
            break;
        case QMediaPlayer::StalledMedia:
            setStatusInfo(tr("Stalled %1%").arg(qRound(m_player->bufferProgress() * 100.)));
            break;
        case QMediaPlayer::EndOfMedia:
            QApplication::alert(this);
            m_playlist->next();
            break;
        case QMediaPlayer::InvalidMedia:
            displayErrorMessage();
            break;
    }
}

void MainWindow::bufferingProgress(float progress)
{
    if (m_player->mediaStatus() == QMediaPlayer::StalledMedia)
        setStatusInfo(tr("Stalled %1%").arg(qRound(progress * 100.)));
    else
        setStatusInfo(tr("Buffering %1%").arg(qRound(progress * 100.)));
}

#pragma endregion


#pragma region VIDEO CONTROLS

void MainWindow::seek(int mseconds)
{
    m_player->setPosition(mseconds);
}

// TODO: Implement
void MainWindow::handleCursor(QMediaPlayer::MediaStatus status)
{
    //#ifndef QT_NO_CURSOR
    //    if (status == QMediaPlayer::LoadingMedia || status == QMediaPlayer::BufferingMedia
    //        || status == QMediaPlayer::StalledMedia)
    //        setCursor(QCursor(Qt::BusyCursor));
    //    else
    //        unsetCursor();
    //#endif
}

void MainWindow::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        m_playlist->setCurrentIndex(index.row());
    }
}

void MainWindow::jumpTo(int mseconds)
{
    qDebug() << "Jump To command called.";
    qint64 zeroNum = 0;
    if (m_player->isAvailable()) {
        qint64 hduration = m_player->duration();
        qint64 newPosition = m_player->position() + mseconds;

        if (newPosition < hduration && newPosition > zeroNum) {
            m_player->setPosition(newPosition);

        } else if (newPosition >= hduration) {
            m_player->setPosition(hduration);

        } else if (newPosition <= zeroNum) {
            m_player->setPosition(zeroNum);
        }
    }
}

#pragma endregion


#pragma region VIDEO EDITING FUNCTIONS

void MainWindow::showErrorMessage(const QString &message)
{
    QMessageBox::warning(this, tr("vura"), message);
}

void MainWindow::extractSubclipFromVideo()
{
    if (h_inMarker <= 0 || h_outMarker <= 0) {
        showErrorMessage("Missing In Marker or Out Marker");
        return;
    }

    QString startTimestamp = createTimestampString(h_inMarker);
    QString endTimestamp = createTimestampString(h_outMarker);

    // Get the user's documents location as a QString
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString applicationFolder = "vura";
    QFileInfo fileInfo(currentFile);
    QString projectFolder = fileInfo.baseName();
    QString fileName = "subclip";
    QString dirPath = QDir::cleanPath(documentsPath + QDir::separator() + applicationFolder + QDir::separator() + projectFolder);

    // Check if project folder exists.
    QDir dir;

    if (dir.mkpath(dirPath)) {
        qDebug() << "Successfully created project directory (and parent directories if needed):" << dirPath;
    } else {
        qDebug() << "Failed to create project directory:" << dirPath;
    }

    QString outputFile = generateSubclipFilenameWithIncrement(dirPath, fileName, "mp4");

    qDebug() << "Input file: " << currentFile;
    qDebug() << "Output file: " << outputFile;
    qDebug() << "Start timestamp: " << startTimestamp;
    qDebug() << "End timestamp: " << endTimestamp;

    QProcess *ffmpegProc = new QProcess(this);
    QStringList arguments;
    arguments << "-ss" << startTimestamp << "-to" << endTimestamp << "-i" << currentFile << "-c" << "copy" << outputFile;

    // Optional: Connect signals to handle output/errors
    connect(ffmpegProc, &QProcess::readyReadStandardError, [=]() {
        qWarning() << ffmpegProc->readAllStandardError();
    });

    ffmpegProc->start("ffmpeg", arguments);
    // Use ffmpegProc->waitForFinished(); if synchronous behavior is needed
}

QString MainWindow::createTimestampString(qint64 pos)
{
    // Convert to seconds
    qint64 totalSeconds = pos / 1000;

    // Format as mm:ss or hh:mm:ss
    QTime time((totalSeconds / 3600) % 24, (totalSeconds / 60) % 60, totalSeconds % 60);
    QString format = (totalSeconds >= 3600) ? "hh:mm:ss" : "mm:ss";
    QString timestamp = time.toString(format);

    // 'timestamp' is now a QString (e.g., "01:30" or "01:05:10")
    return timestamp;
}

QString MainWindow::generateSubclipFilenameWithIncrement(const QString &directoryPath, const QString &baseFileName, const QString &extension)
{
    QString fullPath;
    int index = 0;

    // Extract base name without extension for numbering
    QString nameWithoutExt = QFileInfo(baseFileName).baseName();

    do {
        if (index == 0) {
            fullPath = QDir(directoryPath).absoluteFilePath(baseFileName + "." + extension);
        } else {
            fullPath = QDir(directoryPath).absoluteFilePath(nameWithoutExt + QString::number(index) + "." + extension);
        }
        index++;
    } while (QFile::exists(fullPath)); // Check if file exists

    return fullPath;
}

#pragma endregion


#pragma region CORE APPLICATION FUNCTIONS

void MainWindow::setTrackInfo(const QString &info)
{
    m_trackInfo = info;

    if (m_statusBar) {
        m_statusBar->showMessage(m_trackInfo);
        m_statusLabel->setText(m_statusInfo);
    } else {
        if (!m_statusInfo.isEmpty())
            setWindowTitle(QStringLiteral("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
        else
            setWindowTitle(m_trackInfo);
    }
}

void MainWindow::setStatusInfo(const QString &info)
{
    m_statusInfo = info;
    ui->statusBar->showMessage(m_trackInfo);
    m_statusLabel->setText(m_statusInfo);
}

void MainWindow::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60, currentInfo % 60,
                          (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60, m_duration % 60,
                        (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    ui->m_labelDuration->setText(tStr);
    ui->m_playbackRate->setText("x" + QString::number(h_playbackSpeed));
    if (currentInfo > 0) {
        m_videoSlider->setVideoLoaded(true);
    }
}

void MainWindow::displayErrorMessage()
{
    if (m_player->error() == QMediaPlayer::NoError)
        return;
    setStatusInfo(m_player->errorString());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_hashFile) {
        if (!currentFileHash.isEmpty())
            hData->saveMarkers(currentFileHash, markerMap);
    } else {
        if (!currentFile.isEmpty())
            hData->saveMarkers(currentFile, markerMap);
    }

    event->accept();
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_NCLBUTTONDBLCLK)
        {
            qDebug() << "WM_NCLBUTTONDBLCLK";
            this->resize(1200, 700);
            return true;
        }
    }
#endif

    // For other events, or if not handled, call the base implementation
    return QWidget::nativeEvent(eventType, message, result);
}

#pragma endregion


#pragma region STARTUP FUNCTIONS

void MainWindow::configureApplication()
{
    QSettings settings;
    if (m_rememberWindowSize) {
        restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
        restoreState(settings.value("mainWindowState").toByteArray());
    }
}

void MainWindow::configureStatusBar()
{
    m_statusLabel = new QLabel;
    ui->statusBar->addPermanentWidget(m_statusLabel);
    ui->statusBar->setSizeGripEnabled(false);
    ui->statusBar->setVisible(false);
    h_showingStatusBar = false;
}

void MainWindow::configureVideoWidget()
{
    // Video Player
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_player->setVideoOutput(ui->videoWidget);
}

void MainWindow::configureVideoWidgetConnections()
{
    // Media Player
    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::statusChanged);
    connect(m_player, &QMediaPlayer::bufferProgressChanged, this, &MainWindow::bufferingProgress);
    connect(m_player, &QMediaPlayer::errorChanged, this, &MainWindow::displayErrorMessage);
    connect(m_player, &QMediaPlayer::sourceChanged, this, &MainWindow::sourceChanged);
    connect(m_player, &QMediaPlayer::tracksChanged, this, &MainWindow::tracksChanged);
}

void MainWindow::configureUI()
{
    m_videoSlider = new VideoSlider(this);
    ui->horizontalLayout_3->insertWidget(0, m_videoSlider);
    ui->horizontalLayout_3->setStretch(0, 2);

    connect(m_videoSlider, &VideoSlider::sliderMoved, this, &MainWindow::seek);
    connect(m_videoSlider, &VideoSlider::sliderClicked, this, &MainWindow::seek);
    connect(m_videoSlider, &VideoSlider::markerSelected, this, &MainWindow::seek);

    //m_videoSlider->hide();

    m_playlistModel = new PlaylistModel(this);
    m_playlist = m_playlistModel->playlist();
    connect(m_playlist, &Playlist::currentIndexChanged, this, &MainWindow::playlistPositionChanged);

    // display
    ui->playlistView->setModel(m_playlistModel);
    ui->playlistView->setCurrentIndex(m_playlistModel->index(m_playlist->currentIndex(), 0));
    connect(ui->playlistView, &QListView::activated, this, &MainWindow::jump);
    connect(ui->playlistView, &QListView::customContextMenuRequested, this, &MainWindow::showPlaylistContextMenu);
    ui->playlistView->hide();
    h_showingPlaylist = false;
}

void MainWindow::configureMenuItems()
{
    // File
    connect(ui->actionEmergency_Collapse, &QAction::triggered, this, &MainWindow::emergencyCollapse);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::exit);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionOpen_Multiple_Media_Files, &QAction::triggered, this, &MainWindow::openMultipleFiles);
    connect(ui->actionOpen_Media_Folder, &QAction::triggered, this, &MainWindow::openFolder);
    connect(ui->actionOpen_Network_Stream, &QAction::triggered, this, &MainWindow::openNetworkStream);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui->actionSave_Playlist, &QAction::triggered, this, &MainWindow::savePlaylist);
    connect(ui->actionSave_a_Copy, &QAction::triggered, this, &MainWindow::saveACopy);
    connect(ui->actionCaptions, &QAction::triggered, this, &MainWindow::exportCaptions);
    connect(ui->actionClips, &QAction::triggered, this, &MainWindow::exportClips);
    connect(ui->actionMarkers, &QAction::triggered, this, &MainWindow::exportMarkers);
    connect(ui->actionMedia, &QAction::triggered, this, &MainWindow::exportMedia);
    connect(ui->actionTemp, &QAction::triggered, this, &MainWindow::openRecent);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::showPreferences);

    // View
    connect(ui->actionShow_Log_Files, &QAction::triggered, this, &MainWindow::showLogFileViewer);
    connect(ui->actionToggle_Status_Bar, &QAction::triggered, this, &MainWindow::toggleStatusBar);
    connect(ui->actionToggle_Playlist, &QAction::triggered, this, &MainWindow::togglePlaylist);
    connect(ui->actionToggle_Video_Controls, &QAction::triggered, this, &MainWindow::toggleVideoControls);
    connect(ui->actionShow_Cumshot_Markers, &QAction::triggered, this, &MainWindow::toggleCumshotMarkers);
    connect(ui->actionShow_Cyan_Markers, &QAction::triggered, this, &MainWindow::toggleCyanMarkers);
    connect(ui->actionShow_Dialog_Markers, &QAction::triggered, this, &MainWindow::toggleDialogMarkers);
    connect(ui->actionShow_Magenta_Markers, &QAction::triggered, this, &MainWindow::toggleMagentaMarkers);
    connect(ui->actionShow_Markers, &QAction::triggered, this, &MainWindow::toggleMarkers);
    connect(ui->actionShow_Orange_Markers, &QAction::triggered, this, &MainWindow::toggleOrangeMarkers);
    connect(ui->actionShow_Scene_Transition_Markers, &QAction::triggered, this, &MainWindow::toggleSceneTransitionMarkers);
    connect(ui->actionShow_Strip_Markers, &QAction::triggered, this, &MainWindow::toggleStripMarkers);

    // Playback
    connect(ui->actionFullscreen, &QAction::triggered, this, &MainWindow::toggleFullscreen);
    connect(ui->actionPlay_Pause_2, &QAction::triggered, this, &MainWindow::togglePlayPause);
    connect(ui->actionNext, &QAction::triggered, m_playlist, &Playlist::next);
    connect(ui->actionPrevious, &QAction::triggered, this, &MainWindow::previousVideo);
    connect(ui->actionNext_Frame_2, &QAction::triggered, this, &MainWindow::nextFrame);
    connect(ui->actionPrevious_Frame_2, &QAction::triggered, this, &MainWindow::previousFrame);
    connect(ui->actionFaster, &QAction::triggered, this, &MainWindow::fasterSpeed);
    connect(ui->actionFaster_fine, &QAction::triggered, this, &MainWindow::fasterFineSpeed);
    connect(ui->actionNormal_Speed, &QAction::triggered, this, &MainWindow::normalSpeed);
    connect(ui->actionSlower_fine, &QAction::triggered, this, &MainWindow::slowerFineSpeed);
    connect(ui->actionSlower, &QAction::triggered, this, &MainWindow::slowerSpeed);
    connect(ui->actionJumpBackwardSmall, &QAction::triggered, this, &MainWindow::jumpBackwardSmall);
    connect(ui->actionJumpBackwardMedium, &QAction::triggered, this, &MainWindow::jumpBackwardMedium);
    connect(ui->actionJumpBackwardLarge, &QAction::triggered, this, &MainWindow::jumpBackwardLarge);
    connect(ui->actionJumpBackwardExtraLarge, &QAction::triggered, this, &MainWindow::jumpBackwardExtraLarge);
    connect(ui->actionJumpForwardSmall, &QAction::triggered, this, &MainWindow::jumpForwardSmall);
    connect(ui->actionJumpForwardMedium, &QAction::triggered, this, &MainWindow::jumpForwardMedium);
    connect(ui->actionJumpForwardLarge, &QAction::triggered, this, &MainWindow::jumpForwardLarge);
    connect(ui->actionJumpForwardExtraLarge, &QAction::triggered, this, &MainWindow::jumpForwardExtraLarge);
    connect(ui->actionJump_to_Specific_Time, &QAction::triggered, this, &MainWindow::jumpToTime);
    connect(ui->actionRestart_Video, &QAction::triggered, this, &MainWindow::restartVideo);

    // Sequence
    connect(ui->actionMake_Subclip, &QAction::triggered, this, &MainWindow::createSubclip);

    // Audio
    connect(ui->actionIncrease_Volume_2, &QAction::triggered, this, &MainWindow::increaseVolume);
    connect(ui->actionDecrease_Volume_2, &QAction::triggered, this, &MainWindow::decreaseVolume);
    connect(ui->actionMute, &QAction::triggered, this, &MainWindow::toggleMute);

    // Video
    connect(ui->actionTake_Snapshot, &QAction::triggered, this, &MainWindow::takeSnapshot);

    // Marker
    connect(ui->actionAdd_Marker, &QAction::triggered, this, &MainWindow::addMarker);
    connect(ui->actionGo_to_Next_Marker, &QAction::triggered, this, &MainWindow::goToNextMarker);
    connect(ui->actionGo_to_Previous_Marker, &QAction::triggered, this, &MainWindow::goToPreviousMarker);
    connect(ui->actionClear_Selected_Marker, &QAction::triggered, this, &MainWindow::clearSelectedMarker);
    connect(ui->actionClear_Markers, &QAction::triggered, this, &MainWindow::clearAllMarkers);
    connect(ui->actionEdit_Marker, &QAction::triggered, this, &MainWindow::editMarker);
    connect(ui->actionAdd_Cumshot_Marker, &QAction::triggered, this, &MainWindow::addCumshotMarker);
    connect(ui->actionAdd_Strip_Marker, &QAction::triggered, this, &MainWindow::addStripMarker);
    connect(ui->actionAdd_Scene_Transition_Marker, &QAction::triggered, this, &MainWindow::addSceneTransitionMarker);
    connect(ui->actionAdd_Dialog_Marker, &QAction::triggered, this, &MainWindow::addDialogMarker);
    connect(ui->actionAdd_Cyan_Marker, &QAction::triggered, this, &MainWindow::addCyanMarker);
    connect(ui->actionAdd_Magenta_Marker, &QAction::triggered, this, &MainWindow::addMagentaMarker);
    connect(ui->actionAdd_Orange_Marker, &QAction::triggered, this, &MainWindow::addOrangeMarker);
    connect(ui->actionMark_In, &QAction::triggered, this, &MainWindow::addInMarker);
    connect(ui->actionMark_Out, &QAction::triggered, this, &MainWindow::addOutMarker);
    connect(ui->actionClear_In, &QAction::triggered, this, &MainWindow::clearInMarker);
    connect(ui->actionClear_Out, &QAction::triggered, this, &MainWindow::clearOutMarker);
    connect(ui->actionClear_In_and_Out, &QAction::triggered, this, &MainWindow::clearInAndOutMarker);
    connect(ui->actionGo_to_In, &QAction::triggered, this, &MainWindow::goToInMarker);
    connect(ui->actionGo_to_Out, &QAction::triggered, this, &MainWindow::goToOutMarker);

    // Tools
    connect(ui->actionStream_Stash_Video, &QAction::triggered, this, &MainWindow::streamVideoFromStash);
    connect(ui->actionTestFunction, &QAction::triggered, this, &MainWindow::testFunction);

    // Subtitle
    connect(ui->actionOpen_Subtitle_File, &QAction::triggered, this, &MainWindow::openSubtitleFile);
    connect(ui->actionToggle_Subtitles, &QAction::triggered, this, &MainWindow::toggleSubtitles);

    // Help
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
    connect(ui->actionHelp, &QAction::triggered, this, &MainWindow::showHelp);
    connect(ui->actionUpdates, &QAction::triggered, this, &MainWindow::showUpdates);
}

void MainWindow::createRecentFileActions()
{
    QSettings settings;

    for (int i = 0; i < settings.value("maxRecentFiles", 9).toInt(); ++i) {
        h_recentFileActions[i] = new QAction(this);
        h_recentFileActions[i]->setVisible(false);
        connect(h_recentFileActions[i], &QAction::triggered, this, &MainWindow::openRecentFile);
        ui->menuOpen_Recent->addAction(h_recentFileActions[i]);
    }
    h_recentFilesSeparator = new QAction(this);
    h_recentFilesSeparator->setSeparator(true);
    h_recentFilesSeparator->setVisible(false);
    ui->menuOpen_Recent->addAction(h_recentFilesSeparator);
    h_clearRecentFilesAction = new QAction(this);
    h_clearRecentFilesAction->setText("Clear");
    h_clearRecentFilesAction->setVisible(true);
    connect(h_clearRecentFilesAction, &QAction::triggered, this, &MainWindow::clearRecentFiles);
    ui->menuOpen_Recent->addAction(h_clearRecentFilesAction);
}

void MainWindow::createAudioOutputActions()
{
    for (int i = 0; i < 15; ++i) {
        h_audioOutputActions[i] = new QAction(this);
        h_audioOutputActions[i]->setCheckable(true);
        h_audioOutputActions[i]->setChecked(false);
        h_audioOutputActions[i]->setVisible(false);
        connect(h_audioOutputActions[i], &QAction::triggered, this, &MainWindow::selectAudioOutput);
        ui->menuAudio_Device->addAction(h_audioOutputActions[i]);
    }
}

void MainWindow::createAudioTrackActions()
{
    for (int i = 0; i < 15; ++i) {
        h_audioTrackActions[i] = new QAction(this);
        h_audioTrackActions[i]->setCheckable(true);
        h_audioTrackActions[i]->setChecked(false);
        h_audioTrackActions[i]->setVisible(false);
        connect(h_audioTrackActions[i], &QAction::triggered, this, &MainWindow::selectAudioTrack);
        ui->menuAudio_Track->addAction(h_audioTrackActions[i]);
    }
    ui->menuAudio_Track->setEnabled(false);
}

void MainWindow::createVideoTrackActions()
{
    for (int i = 0; i < 15; ++i) {
        h_videoTrackActions[i] = new QAction(this);
        h_videoTrackActions[i]->setCheckable(true);
        h_videoTrackActions[i]->setChecked(false);
        h_videoTrackActions[i]->setVisible(false);
        connect(h_videoTrackActions[i], &QAction::triggered, this, &MainWindow::selectVideoTrack);
        ui->menuVideo_Track->addAction(h_videoTrackActions[i]);
    }
    ui->menuVideo_Track->setEnabled(false);
}

void MainWindow::createSubtitleTrackActions()
{
    for (int i = 0; i < 15; ++i) {
        h_subtitleTrackActions[i] = new QAction(this);
        h_subtitleTrackActions[i]->setCheckable(true);
        h_subtitleTrackActions[i]->setChecked(false);
        h_subtitleTrackActions[i]->setVisible(false);
        connect(h_subtitleTrackActions[i], &QAction::triggered, this, &MainWindow::selectSubtitleTrack);
        ui->menuSubtitle_Track->addAction(h_subtitleTrackActions[i]);
    }
    ui->menuSubtitle_Track->setEnabled(false);
}

#pragma endregion


#pragma region HOTKEYS

void MainWindow::configureHotkeys()
{
    qDebug() << "Configuring hotkeys...";
    QSettings settings;
    QMap<QString,QString> hotkeys;
    settings.beginGroup("Hotkeys");
    const QStringList childKeys = settings.childKeys();
    foreach (const QString &childKey, childKeys)
    {
        hotkeys[childKey] = settings.value(childKey).toString();
    }
    settings.endGroup();

    if (hotkeys.isEmpty()) {
        qDebug() << "QSettings hotkey map is empty.";
        configureDefaultHotkeys();
    }

    foreach (QAction *action, findChildren<QAction *>())
    {
        if (!action->text().isEmpty()) {
            if (action->text() == "Play / Pause") {
                //qDebug() << "Setting hotkey Space for action " << action->text();
                action->setShortcut(tr("Space"));
                action->setShortcutContext(Qt::ApplicationShortcut);
            } else if (hotkeys.contains(action->text())) {
                //qDebug() << "Setting hotkey " << hotkeys[action->text()] << " for action " << action->text();
                action->setShortcut(hotkeys.value(action->text()));
                action->setShortcutContext(Qt::ApplicationShortcut);
            } else {
                //qDebug() << "No hotkey found for action " << action->text();
            }
        }
    }
}

void MainWindow::configureDefaultHotkeys()
{
    qDebug() << "Setting default hotkeys in QSettings.";
    // Load saved hotkeys from QSettings.
    QSettings settings;
    QMap<QString, QString> hotkeyMap;

    // File
    hotkeyMap["Emergency Collapse"] = "C";
    hotkeyMap["Exit"] = "Ctrl+Q";
    hotkeyMap["Open..."] = "O";
    hotkeyMap["Open Media Folder..."] = "Ctrl+F";
    hotkeyMap["Open Multiple Media Files..."] = "Alt+O";
    hotkeyMap["Open Network Stream..."] = "Ctrl+N";
    hotkeyMap["Save"] = "Ctrl+S";
    hotkeyMap["Save As..."] = "Ctrl+Shift+S";
    hotkeyMap["Save Playlist"] = "Alt+S";
    hotkeyMap["Save a Copy..."] = "Ctrl+Alt+S";
    hotkeyMap["Close"] = "Ctrl+A";
    hotkeyMap["Close All"] = "Ctrl+Shift+A";

    // Edit
    //hotkeyMap["Clear"] = "Delete";
    //hotkeyMap["Copy"] = "Ctrl+C";
    //hotkeyMap["Cut"] = "Ctrl+X";
    //hotkeyMap["Deselect All"] = "Ctrl+Shift+A";
    //hotkeyMap["Duplicate"] = "Ctrl+Shift+/";
    //hotkeyMap["Find"] = "Ctrl+F";
    //hotkeyMap["Paste"] = "Ctrl+V";
    hotkeyMap["Preferences"] = "Ctrl+Shift+P";
    //hotkeyMap["Redo"] = "Ctrl+Shift+Z";
    //hotkeyMap["Select All"] = "Ctrl+A";
    //hotkeyMap["Undo"] = "Ctrl+Z";

    // Playback
    hotkeyMap["Next"] = "N";
    hotkeyMap["Next Frame"] = "]";
    hotkeyMap["Previous"] = "P";
    hotkeyMap["Previous Frame"] = "[";
    hotkeyMap["Restart Video"] = "R";
    hotkeyMap["Jump to Specific Time"] = "Ctrl+T";
    hotkeyMap["Jump Back Large"] = "Q";
    hotkeyMap["Jump Back Medium"] = "A";
    hotkeyMap["Jump Back Small"] = "Z";
    hotkeyMap["Jump Fwd Large"] = "W";
    hotkeyMap["Jump Fwd Medium"] = "S";
    hotkeyMap["Jump Fwd Small"] = "X";
    hotkeyMap["Faster"] = "Num++";
    hotkeyMap["Faster (fine)"] = "Ctrl+Num++";
    hotkeyMap["Normal Speed"] = "=";
    hotkeyMap["Slower"] = "Num+-";
    hotkeyMap["Slower (fine)"] = "Ctrl+Num+-";

    // Audio
    hotkeyMap["Decrease Volume"] = "Down";
    hotkeyMap["Increase Volume"] = "Up";
    hotkeyMap["Mute"] = "Ctrl+M";

    // Video
    hotkeyMap["Fullscreen"] = "F";

    // View
    hotkeyMap["Toggle Playlist"] = "Ctrl+P";
    hotkeyMap["Toggle Status Bar"] = "Ctrl+B";
    hotkeyMap["Show Log Viewer"] = "Ctrl+L";
    hotkeyMap["Toggle Video Controls"] = "Ctrl+V";

    // Markers
    hotkeyMap["Add Cumshot Marker"] = "Ctrl+3";
    hotkeyMap["Add Cyan Marker"] = "Ctrl+5";
    hotkeyMap["Add Dialog Marker"] = "Ctrl+4";
    hotkeyMap["Add Magenta Marker"] = "Ctrl+6";
    hotkeyMap["Add Marker"] = "M";
    hotkeyMap["Add Orange Marker"] = "Ctrl+7";
    hotkeyMap["Add Scene Transition Marker"] = "Ctrl+1";
    hotkeyMap["Add Strip Marker"] = "Ctrl+2";
    hotkeyMap["Clear In"] = "Ctrl+Shift+,";
    hotkeyMap["Clear In and Out"] = "Ctrl+Shift+X";
    hotkeyMap["Clear Markers"] = "Ctrl+Shift+M";
    hotkeyMap["Clear Out"] = "Ctrl+Shift+.";
    hotkeyMap["Clear Selected Marker"] = "Ctrl+C";
    hotkeyMap["Edit Marker"] = "Alt+M";
    hotkeyMap["Go to In"] = "Shift+,";
    hotkeyMap["Go to Next Marker"] = "B";
    hotkeyMap["Go to Out"] = "Shift+.";
    hotkeyMap["Go to Previous Marker"] = "V";
    hotkeyMap["Mark In"] = ",";
    hotkeyMap["Mark Out"] = ".";

    // Tools
    hotkeyMap["Make Subclip..."] = "Ctrl+U";
    hotkeyMap["Stream Stash Video"] = "Ctrl+X";

    // Subtitle

    // Help
    hotkeyMap["Help"] = "F1";


    settings.beginGroup("Hotkeys");
    QMapIterator<QString, QString> i(hotkeyMap);
    while (i.hasNext()) {
        i.next();
        settings.setValue(i.key(), i.value());
    }

    settings.endGroup();
    settings.sync();

    // hotkeyMap["action"] = "";
    qDebug() << "Default hotkeys saved in QSettings.";
}

#pragma endregion


#pragma region FUNCTIONS

bool MainWindow::isPlaylist(const QUrl &url)
{
    qDebug() << "Checking if file is playlist: " << url.toString();
    QFileInfo fileInfo(url.toString());
    QString fileExtension = fileInfo.suffix();

    qDebug() << "fileExtension: " << fileExtension;
    if (fileExtension == "hlist") {
        return true;
    } else {
        return false;
    }
}

bool MainWindow::loadPlaylist(const QUrl &url)
{
    qDebug() << "Load playlist function called.";
    if (h_playlistLoaded) {
        qDebug() << "Playlist already loaded. Asking for user confirmation to load selected one.";
        QMessageBox::StandardButton confirmationBox;
        confirmationBox = QMessageBox::question(this, "Close Playlist", "Are you sure you want to close the current playlist and load the selected one?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (confirmationBox == QMessageBox::Yes) {
            qDebug() << "User confirmed close current playlist and load selected one.";
            m_playlist->load(url);
            return true;
        } else {
            qDebug() << "User declined to close current playlist and load selected one.";
        }
    } else {
        qDebug() << "No playlist loaded. Loading selected playlist";
        m_playlist->load(url);
        h_playlistLoaded = true;
        return true;
    }
    return false;
}

QString MainWindow::getMarker(const double &markerTime)
{
    QSettings settings;

    double markerMatchTolerance = settings.value("markerMatchTolerance", 0.001).toDouble();
    QString markerKey;
    double m_time;
    double markerDif;

    for (QMap<QString, QList<double>>::iterator i = markerMap.begin(); i != markerMap.end(); ++i)
    {
        QString m_key = i.key();
        foreach (const double marker, i.value())
        {
            if (qAbs(marker - markerTime) <= markerMatchTolerance)
            {
                if (!markerKey.isEmpty())
                {
                    if (qAbs(marker - markerTime) <= markerDif)
                    {
                        markerKey = m_key;
                        m_time = marker;
                        markerDif = qAbs(marker - markerTime);
                    }
                }
                else
                {
                    markerKey = m_key;
                    m_time = marker;
                    markerDif = qAbs(marker - markerTime);
                }
            }
        }
    }

    return markerKey;
}

void MainWindow::markersChanged(const QString &markerName, const double &markerTime, const QString &markerType)
{
    if (markerType == m_markerValue)
    {
        QList<double> markers = markerMap.value(m_markerValue).toList();

        if (!markers.isEmpty())
        {
            markers.replace(markerIndex, markerTime);
            markerMap.insert(m_markerValue, markers);
        }
    }
    else
    {
        QList<double> markers = markerMap.value(m_markerValue).toList();

        if (!markers.isEmpty())
        {
            markers.removeAt(markerIndex);
            QList<double> marker = markerMap.value(markerType).toList();
            marker.append(markerTime);
            markerMap.insert(markerType, marker);
        }
    }
    m_videoSlider->setMarkers(markerMap);
}

void MainWindow::markerDeleted(const double &markerTime)
{
    QSettings settings;

    double markerMatchTolerance = settings.value("markerMatchTolerance", 0.001).toDouble();
    QString markerKey;
    double m_time;
    double markerDif;

    for (QMap<QString, QList<double>>::iterator i = markerMap.begin(); i != markerMap.end(); ++i)
    {
        QString m_key = i.key();
        foreach (const double marker, i.value())
        {
            if (qAbs(marker - markerTime) <= markerMatchTolerance)
            {
                if (!markerKey.isEmpty())
                {
                    if (qAbs(marker - markerTime) <= markerDif)
                    {
                        markerKey = m_key;
                        m_time = marker;
                        markerDif = qAbs(marker - markerTime);
                    }
                }
                else
                {
                    markerKey = m_key;
                    m_time = marker;
                    markerDif = qAbs(marker - markerTime);
                }
            }
        }
    }

    if (!markerKey.isEmpty())
    {
        if (markerMap.contains(markerKey))
        {
            QList<double>& m_list = markerMap[markerKey];
            bool removed = m_list.removeAll(m_time);

            if (removed)
            {
                qDebug() << tr("Removed marker %1").arg(markerKey);
            }
            else
            {
                qDebug() << tr("Failed to removed marker %1").arg(markerKey);
            }
        }
    }

    m_videoSlider->setMarkers(markerMap);
}

void MainWindow::saveMediaFilterList(const QStringList& filterList)
{
    QSettings settings;
    settings.setValue("mediaFileFilter", QVariant::fromValue(filterList));
    settings.sync();
}

QStringList MainWindow::loadMediaFilterList()
{
    QSettings settings;

    QStringList defaultList;
    QVariant value = settings.value("mediaFileFilter", QVariant::fromValue(defaultList));
    QStringList fileFilters = value.value<QStringList>();

    if (fileFilters.isEmpty()) {
        fileFilters << "*.mp4" << "*.wmv";
    }

    return fileFilters;
}

void MainWindow::showNotImplemented_Message()
{
    QMessageBox::information(this, "Not Implemented", "Sorry this function has not been implemented yet.");
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), settings.value("maxRecentFiles", 9).toInt());

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1: %2").arg(i + 1).arg(files[i]);
        h_recentFileActions[i]->setText(text);
        h_recentFileActions[i]->setData(files[i]);
        h_recentFileActions[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < settings.value("maxRecentFiles", 9).toInt(); ++j)
        h_recentFileActions[j]->setVisible(false);

    h_recentFilesSeparator->setVisible(numRecentFiles > 0);
    ui->menuOpen_Recent->setEnabled(numRecentFiles > 0);
}

void MainWindow::loadFile(const QString &fileName)
{
    const int previousMediaCount = m_playlist->mediaCount();
    m_playlist->addMedia(fileName);

    if (m_playlist->mediaCount() > previousMediaCount) {
        auto index = m_playlistModel->index(previousMediaCount, 0);
        ui->playlistView->setCurrentIndex(index);
        jump(index);
    }
}

QString MainWindow::strippedFileName(const QString &fileName)
{
    return QFileInfo(fileName).fileName();
}

QString MainWindow::timestampString(qint64 position)
{
    QString tStr;
    if (position || m_duration) {
        QTime currentTime((position / 3600) % 60, (position / 60) % 60, position % 60,
                          (position * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60, m_duration % 60,
                        (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    return tStr;
}

void MainWindow::updateAudioOutputActions()
{
    for (int j = 0; j < 15; ++j)
        h_audioOutputActions[j]->setVisible(false);

    h_audioOutputActions[0]->setText("Default");
    h_audioOutputActions[0]->setData(QVariant::fromValue(QAudioDevice()));
    h_audioOutputActions[0]->setVisible(true);
    int i = 1;
    for (auto &deviceInfo : QMediaDevices::audioOutputs()) {
        if (i <= 15) {
            qDebug() << "Audio output " << i << ": " << deviceInfo.description();
            h_audioOutputActions[i]->setText(deviceInfo.description());
            h_audioOutputActions[i]->setData(QVariant::fromValue(deviceInfo));
            h_audioOutputActions[i]->setVisible(true);
            if (m_audioOutput->device() == deviceInfo) {
                h_audioOutputActions[i]->setChecked(true);
            }
            ++i;
        }
    }

    for (int j = i; j < 15; ++j)
        h_audioOutputActions[j]->setVisible(false);

    ui->menuAudio_Device->setEnabled(i > 1);
}

void MainWindow::updateAudioTrackActions()
{
    for (int j = 0; j < 15; ++j)
        h_audioTrackActions[j]->setVisible(false);

    const auto audioTracks = m_player->audioTracks();
    h_audioTrackActions[0]->setText("No audio");
    h_audioTrackActions[0]->setData(-1);
    h_audioTrackActions[0]->setVisible(true);

    for (int i = 0; i < audioTracks.size(); ++i) {
        if (i <= 14) {
            qDebug() << "Audio track " << i << ": " << trackName(audioTracks.at(i), i);
            h_audioTrackActions[i+1]->setText(trackName(audioTracks.at(i), i));
            h_audioTrackActions[i+1]->setData(i);
            h_audioTrackActions[i+1]->setVisible(true);
            if (m_player->activeAudioTrack() == i) {
                h_audioTrackActions[i+1]->setChecked(true);
            }
        }
    }

    for (int j = audioTracks.size(); j < 14; ++j)
        h_audioTrackActions[j+1]->setVisible(false);

    ui->menuAudio_Track->setEnabled(audioTracks.size() > 0);
}

void MainWindow::updateVideoTrackActions()
{
    for (int j = 0; j < 15; ++j)
        h_videoTrackActions[j]->setVisible(false);

    const auto videoTracks = m_player->videoTracks();
    h_videoTrackActions[0]->setText("No video");
    h_videoTrackActions[0]->setData(-1);
    h_videoTrackActions[0]->setVisible(true);

    for (int i = 0; i < videoTracks.size(); ++i) {
        if (i <= 14) {
            qDebug() << "Video track " << i << ": " << trackName(videoTracks.at(i), i);
            h_videoTrackActions[i+1]->setText(trackName(videoTracks.at(i), i));
            h_videoTrackActions[i+1]->setData(i);
            h_videoTrackActions[i+1]->setVisible(true);
            if (m_player->activeVideoTrack() == i) {
                h_videoTrackActions[i+1]->setChecked(true);
            }
        }
    }

    for (int j = videoTracks.size(); j < 14; ++j)
        h_videoTrackActions[j+1]->setVisible(false);

    ui->menuVideo_Track->setEnabled(videoTracks.size() > 0);
}

void MainWindow::updateSubtitleTrackActions()
{
    for (int j = 0; j < 15; ++j)
        h_subtitleTrackActions[j]->setVisible(false);

    const auto subtitleTracks = m_player->subtitleTracks();
    h_subtitleTrackActions[0]->setText("No subtitles");
    h_subtitleTrackActions[0]->setData(-1);
    h_subtitleTrackActions[0]->setVisible(true);

    for (int i = 0; i < subtitleTracks.size(); ++i) {
        if (i <= 14) {
            qDebug() << "Subtitle track " << i << ": " << trackName(subtitleTracks.at(i), i);
            h_subtitleTrackActions[i+1]->setText(trackName(subtitleTracks.at(i), i));
            h_subtitleTrackActions[i+1]->setData(i);
            h_subtitleTrackActions[i+1]->setVisible(true);
            if (m_player->activeSubtitleTrack() == i) {
                h_subtitleTrackActions[i+1]->setChecked(true);
            }
        }
    }

    for (int j = subtitleTracks.size(); j < 14; ++j)
        h_subtitleTrackActions[j+1]->setVisible(false);

    ui->menuSubtitle_Track->setEnabled(subtitleTracks.size() > 0);
}

qint64 MainWindow::fileHash(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return 0;

    XXH64_state_t* state = XXH64_createState();
    XXH64_reset(state, 0); // Seed 0

    char buffer[8192];
    qint64 bytesRead;
    while ((bytesRead = file.read(buffer, sizeof(buffer))) > 0) {
        XXH64_update(state, buffer, bytesRead);
    }

    XXH64_hash_t hash = XXH64_digest(state);
    XXH64_freeState(state);
    file.close();
    return hash;
}

#pragma endregion


#pragma region MENU ITEM SLOTS

// File Menu
void MainWindow::closeFile()
{
    showNotImplemented_Message();
}

void MainWindow::closeAll()
{
    showNotImplemented_Message();
}

void MainWindow::emergencyCollapse()
{
    qDebug() << "Emergency Collapse command called.";
    if (m_player->isPlaying()) {
        m_player->pause();
    }

    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::exit()
{
    qDebug() << "Application Quit command called.";
    QMessageBox::StandardButton confirmationBox;
    confirmationBox = QMessageBox::question(this, "Exit Application", "Are you sure you want to exit?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (confirmationBox == QMessageBox::Yes) {
        this->close();
    }
}

void MainWindow::openFile()
{
    qDebug() << "Open command called.";
    // Load file filters.
    QSettings settings;
    QString playlistFileExtension = settings.value("playlistFileExtension", "hlist").toString();
    //QStringList fileFilters = loadMediaFilterList();
    //fileFilters << "*." << playlistFileExtension;
    QStringList fileFilters;
    fileFilters << "*.*";

    // Create open file dialog.
    QFileDialog fileDialog(this);
    fileDialog.setNameFilters(fileFilters);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open File"));
    fileDialog.setDirectory(settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString());

    if (fileDialog.exec() == QDialog::Accepted) {
        QStringList selectedFiles = fileDialog.selectedFiles();
        if (!selectedFiles.isEmpty()) {
            const int previousMediaCount = m_playlist->mediaCount();
            bool loadedNewPlaylist = false;
            for (auto &url : selectedFiles) {
                if (!isPlaylist(url)) {
                    m_playlist->addMedia(url);
                    QStringList files = settings.value("recentFileList").toStringList();
                    files.removeAll(url);
                    files.prepend(url);
                    while (files.size() > settings.value("maxRecentFiles", 10).toInt()) {
                        files.removeLast();
                    }
                    settings.setValue("recentFileList", files);
                    updateRecentFileActions();
                } else {
                    //loadedNewPlaylist = loadPlaylist(url);
                    m_playlist->loadPlaylist(url);
                    loadedNewPlaylist = true;
                    break;
                }
            }
            if (!loadedNewPlaylist) {
                if (m_playlist->mediaCount() > previousMediaCount) {
                    auto index = m_playlistModel->index(previousMediaCount, 0);
                    ui->playlistView->setCurrentIndex(index);
                    //if (m_playlistView)
                    //    m_playlistView->setCurrentIndex(index);
                    jump(index);
                }
            }

            // Set last file directory where file was opened.
            QString lastFileDirectory = selectedFiles.last();
            settings.setValue("lastFileDirectory", QFileInfo(lastFileDirectory).path());
        } else {
            qDebug() << "No file selected.";
        }
    }
}

void MainWindow::openMultipleFiles()
{
    qDebug() << "Open Multiple command called.";
    QSettings settings;

    // Create file filters list.
    QStringList fileFilters = loadMediaFilterList();

    // Create open multiple files dialog.
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                           tr("Open Multiple Files"),
                                                           settings.value("lastFileDirectory", QStandardPaths::MoviesLocation).toString(),
                                                      tr("All Files (*.*)"));

    if (!fileNames.isEmpty()) {
        const int previousMediaCount = m_playlist->mediaCount();
        bool loadedNewPlaylist = false;
        foreach (const QString &fileName, fileNames) {
            QUrl url = QUrl::fromLocalFile(fileName);
            if (!isPlaylist(url)) {
                m_playlist->addMedia(url);
            } else {
                loadedNewPlaylist = loadPlaylist(url);
            }
        }
        if (!loadedNewPlaylist) {
            if (m_playlist->mediaCount() > previousMediaCount) {
                auto index = m_playlistModel->index(previousMediaCount, 0);
                ui->playlistView->setCurrentIndex(index);
                //if (m_playlistView)
                //    m_playlistView->setCurrentIndex(index);
                jump(index);
            }
        }

        // Set last file directory where file was opened.
        QString lastFileDirectory = fileNames.last();
        settings.setValue("lastFileDirectory", QFileInfo(lastFileDirectory).path());
    } else {
        qDebug() << "No files selected.";
    }
}

void MainWindow::openFolder()
{
    qDebug() << "Open Folder command called.";
    QSettings settings;
    QList<QUrl> fileList;

    // Create file filters list.
    QStringList fileFilters = loadMediaFilterList();

    // Create open folder dialog.
    QString folderName = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Directory"),
                                                    settings.value("lastFolderDirectory", QStandardPaths::MoviesLocation).toString(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    if (!folderName.isEmpty()) {
        QDirIterator it(folderName, QDir::Files | QDir::NoDotAndDotDot);
        while (it.hasNext()) {
            it.next();
            fileList.append(QUrl::fromLocalFile(it.filePath()));
        }

        const int previousMediaCount = m_playlist->mediaCount();
        bool loadedNewPlaylist = false;
        for (auto &url : fileList) {
            if (!isPlaylist(url)) {
                m_playlist->addMedia(url);
            } else {
                loadedNewPlaylist = loadPlaylist(url);
            }
        }
        if (!loadedNewPlaylist) {
            if (m_playlist->mediaCount() > previousMediaCount) {
                auto index = m_playlistModel->index(previousMediaCount, 0);
                ui->playlistView->setCurrentIndex(index);
                //if (m_playlistView)
                //    m_playlistView->setCurrentIndex(index);
                jump(index);
            }
        }

        // Set last file directory where file was opened.
        settings.setValue("lastFolderDirectory", folderName);
    } else {
        qDebug() << "No folder selected.";
    }
}

void MainWindow::savePlaylist()
{
    qDebug() << "Save Playlist command called.";
    QSettings settings;

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Playlist"),
                                                    "/Users/drew/untitled.hlist",
                                                    tr("HPlayer Playlist (*.hlist);;All Files (*)"));

    if (!fileName.isEmpty()) {
        QUrl url(fileName);

        m_playlist->savePlaylist(fileName);

        //settings.setValue("lastPlaylistFileDirectory", QFileInfo(fileName).path());
    }
}

void MainWindow::showPreferences()
{
    qDebug() << "Open Preferences command called.";
    if (h_settingsWindow)
        h_settingsWindow->close();

    h_settingsWindow = new SettingsWindow(this);
    h_settingsWindow->show();
    h_settingsWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(h_settingsWindow, &SettingsWindow::updateSettings, this, &MainWindow::loadSettings);
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}

void MainWindow::clearRecentFiles()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    while (!files.isEmpty()) {
        files.removeLast();
    }
    settings.setValue("recentFileList", files);
    updateRecentFileActions();
}

void MainWindow::save() {}

void MainWindow::saveAll()
{
    showNotImplemented_Message();
}

void MainWindow::saveAs()
{
    showNotImplemented_Message();
}

void MainWindow::saveACopy()
{
    showNotImplemented_Message();
}

void MainWindow::exportCaptions()
{
    showNotImplemented_Message();
}

void MainWindow::exportClips()
{
    showNotImplemented_Message();
}

void MainWindow::exportMarkers()
{
    showNotImplemented_Message();
}

void MainWindow::exportMedia()
{
    showNotImplemented_Message();
}

void MainWindow::openRecent()
{
    showNotImplemented_Message();
}

void MainWindow::openNetworkStream()
{
    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("Open Network Stream"),    // Dialog title
                                         tr("Network Stream URL:"),         // Input label
                                         QLineEdit::Normal,        // Input mode (e.g., Normal, NoEcho, Password)
                                         QString(),                // Default text
                                         &ok);                     // Pointer to a boolean that is true if OK was pressed

    if (ok && !text.isEmpty()) {
        const int previousMediaCount = m_playlist->mediaCount();
        m_playlist->addMedia(text);
        if (m_playlist->mediaCount() > previousMediaCount) {
            auto index = m_playlistModel->index(previousMediaCount, 0);
            ui->playlistView->setCurrentIndex(index);
            //if (m_playlistView)
            //    m_playlistView->setCurrentIndex(index);
            jump(index);
        }
    }
}


// View Menu
void MainWindow::togglePlaylist()
{
    qDebug() << "Toggle Playlist View command called.";
    if (h_showingPlaylist) {
        qDebug() << "Hiding playlist view.";
        ui->playlistView->hide();
        h_showingPlaylist = false;
    } else {
        qDebug() << "Showing playlist view.";
        ui->playlistView->show();
        h_showingPlaylist = true;
    }
}

void MainWindow::toggleStatusBar()
{
    if (h_showingStatusBar) {
        ui->statusBar->setVisible(false);
        h_showingStatusBar = false;
    } else {
        ui->statusBar->setVisible(true);
        h_showingStatusBar = true;
    }
}

void MainWindow::toggleVideoControls()
{
    if (h_showingVideoControls) {
        ui->verticalLayout->removeWidget(h_videoControlWidget);
        delete h_videoControlWidget;
        h_showingVideoControls = false;
    } else {
        h_videoControlWidget = new VideoControlWidget(this);
        h_videoControlWidget->setMuted(h_isMuted);
        h_videoControlWidget->setVolume(h_volume);
        ui->verticalLayout->addWidget(h_videoControlWidget);
        connect(m_player, &QMediaPlayer::playbackStateChanged, h_videoControlWidget, &VideoControlWidget::setState);
        connect(h_videoControlWidget, &VideoControlWidget::play, m_player, &QMediaPlayer::play);
        connect(h_videoControlWidget, &VideoControlWidget::pause, m_player, &QMediaPlayer::pause);
        connect(h_videoControlWidget, &VideoControlWidget::stop, m_player, &QMediaPlayer::stop);
        connect(h_videoControlWidget, &VideoControlWidget::next, m_playlist, &Playlist::next);
        connect(h_videoControlWidget, &VideoControlWidget::previous, this, &MainWindow::previousVideo);
        connect(h_videoControlWidget, &VideoControlWidget::fullScreen, this, &MainWindow::toggleFullscreen);
        connect(h_videoControlWidget, &VideoControlWidget::togglePlaylist, this, &MainWindow::togglePlaylist);
        connect(h_videoControlWidget, &VideoControlWidget::loopAll, this, &MainWindow::videoControl_LoopAll);
        connect(h_videoControlWidget, &VideoControlWidget::loopOne, this, &MainWindow::videoControl_LoopOne);
        connect(h_videoControlWidget, &VideoControlWidget::loopNone, this, &MainWindow::videoControl_LoopNone);
        connect(h_videoControlWidget, &VideoControlWidget::shuffle, this, &MainWindow::videoControl_Shuffle);
        connect(h_videoControlWidget, &VideoControlWidget::changeVolume, m_audioOutput, &QAudioOutput::setVolume);
        connect(h_videoControlWidget, &VideoControlWidget::changeMuting, m_audioOutput, &QAudioOutput::setMuted);
        connect(m_audioOutput, &QAudioOutput::volumeChanged, h_videoControlWidget, &VideoControlWidget::setVolume);
        connect(m_audioOutput, &QAudioOutput::mutedChanged, h_videoControlWidget, &VideoControlWidget::setMuted);
        connect(this, &MainWindow::refreshSettings, h_videoControlWidget, &VideoControlWidget::refreshUI);
        h_showingVideoControls = true;
    }
    ui->actionToggle_Video_Controls->setChecked(h_showingVideoControls);
}

void MainWindow::toggleMarkers()
{
    if (m_videoSlider->showMarkers) {
        ui->actionShow_Markers->setChecked(false);
        m_videoSlider->showMarkers = false;
    } else {
        ui->actionShow_Markers->setChecked(true);
        m_videoSlider->showMarkers = true;
    }
}

void MainWindow::toggleCumshotMarkers()
{
    if (m_videoSlider->showCumshotMarkers) {
        ui->actionShow_Cumshot_Markers->setChecked(false);
        m_videoSlider->showCumshotMarkers = false;
    } else {
        ui->actionShow_Cumshot_Markers->setChecked(true);
        m_videoSlider->showCumshotMarkers = true;
    }
}

void MainWindow::toggleCyanMarkers()
{
    if (m_videoSlider->showCyanMarkers) {
        ui->actionShow_Cyan_Markers->setChecked(false);
        m_videoSlider->showCyanMarkers = false;
    } else {
        ui->actionShow_Cyan_Markers->setChecked(true);
        m_videoSlider->showCyanMarkers = true;
    }
}

void MainWindow::toggleDialogMarkers()
{
    if (m_videoSlider->showDialogMarkers) {
        ui->actionShow_Dialog_Markers->setChecked(false);
        m_videoSlider->showDialogMarkers = false;
    } else {
        ui->actionShow_Dialog_Markers->setChecked(true);
        m_videoSlider->showDialogMarkers = true;
    }
}

void MainWindow::toggleMagentaMarkers()
{
    if (m_videoSlider->showMagentaMarkers) {
        ui->actionShow_Magenta_Markers->setChecked(false);
        m_videoSlider->showMagentaMarkers = false;
    } else {
        ui->actionShow_Magenta_Markers->setChecked(true);
        m_videoSlider->showMagentaMarkers = true;
    }
}

void MainWindow::toggleOrangeMarkers()
{
    if (m_videoSlider->showOrangeMarkers) {
        ui->actionShow_Orange_Markers->setChecked(false);
        m_videoSlider->showOrangeMarkers = false;
    } else {
        ui->actionShow_Orange_Markers->setChecked(true);
        m_videoSlider->showOrangeMarkers = true;
    }
}

void MainWindow::toggleSceneTransitionMarkers()
{
    if (m_videoSlider->showSceneMarkers) {
        ui->actionShow_Scene_Transition_Markers->setChecked(false);
        m_videoSlider->showSceneMarkers = false;
    } else {
        ui->actionShow_Scene_Transition_Markers->setChecked(true);
        m_videoSlider->showSceneMarkers = true;
    }
}

void MainWindow::toggleStripMarkers()
{
    if (m_videoSlider->showStripMarkers) {
        ui->actionShow_Strip_Markers->setChecked(false);
        m_videoSlider->showStripMarkers = false;
    } else {
        ui->actionShow_Strip_Markers->setChecked(true);
        m_videoSlider->showStripMarkers = true;
    }
}

void MainWindow::showLogFileViewer()
{
    //qInfo() << "Show Log Viewer Window";
    if (h_logviewer)
        h_logviewer->close();

    h_logviewer = new LogViewer(this);
    h_logviewer->show();
    h_logviewer->setAttribute(Qt::WA_DeleteOnClose, true);
}


// Playback Menu
void MainWindow::togglePlayPause()
{
    qDebug() << "Toggle Play/Pause command called.";
    if (m_player->isAvailable()) {
        if (m_player->isPlaying()) {
            qDebug() << "Pausing media player.";
            m_player->pause();
            h_playing = false;
        } else {
            qDebug() << "Playing media player.";
            m_player->play();
            h_playing = true;
        }
    } else {
        qDebug() << "Media Player is unavailable.";
    }
}

void MainWindow::nextVideo()
{
    qDebug() << "Next Video command called.";
    m_playlist->next();
}

void MainWindow::nextFrame()
{
    qDebug() << "Next Frame command called.";
    QSettings settings;
    int mseconds = settings.value("nextFrameTime", 1000).toInt();

    qint64 zeroNum = 0;
    if (m_player->isAvailable()) {
        qint64 hduration = m_player->duration();
        qint64 newPosition = m_player->position() + mseconds;

        if (newPosition < hduration && newPosition > zeroNum) {
            m_player->setPosition(newPosition);

        } else if (newPosition >= hduration) {
            m_player->setPosition(hduration);

        } else if (newPosition <= zeroNum) {
            m_player->setPosition(zeroNum);
        }
    }
}

void MainWindow::previousVideo()
{
    qDebug() << "Previous Video command called.";
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if (m_player->position() <= 5000) {
        m_playlist->previous();
    } else {
        m_player->setPosition(0);
    }
}

void MainWindow::previousFrame()
{
    qDebug() << "Previous Frame command called.";
    QSettings settings;
    int mseconds = settings.value("previousFrameTime", 1300).toInt();

    qint64 zeroNum = 0;
    if (m_player->isAvailable()) {
        qint64 hduration = m_player->duration();
        qint64 newPosition = m_player->position() + (mseconds*-1);

        if (newPosition < hduration && newPosition > zeroNum) {
            m_player->setPosition(newPosition);

        } else if (newPosition >= hduration) {
            m_player->setPosition(hduration);

        } else if (newPosition <= zeroNum) {
            m_player->setPosition(zeroNum);
        }
    }
}

void MainWindow::fasterSpeed()
{
    qDebug() << "Change Playback command called.";
    double mrate = 0.5;
    float newSpeed = h_playbackSpeed + mrate;
    m_player->setPlaybackRate(newSpeed);
    h_playbackSpeed = m_player->playbackRate();
    ui->m_playbackRate->setText("x" + QString::number(h_playbackSpeed));
}

void MainWindow::fasterFineSpeed()
{
    qDebug() << "Change Playback command called.";
    double mrate = 0.25;
    float newSpeed = h_playbackSpeed + mrate;
    m_player->setPlaybackRate(newSpeed);
    h_playbackSpeed = m_player->playbackRate();
    ui->m_playbackRate->setText("x" + QString::number(h_playbackSpeed));
}

void MainWindow::normalSpeed()
{
    qDebug() << "Playback Normal command called.";
    m_player->setPlaybackRate(1.0);
    h_playbackSpeed = m_player->playbackRate();
    ui->m_playbackRate->setText("x" + QString::number(h_playbackSpeed));
}

void MainWindow::slowerSpeed()
{
    qDebug() << "Change Playback command called.";
    double mrate = -0.5;
    float newSpeed = h_playbackSpeed + mrate;
    m_player->setPlaybackRate(newSpeed);
    h_playbackSpeed = m_player->playbackRate();
    ui->m_playbackRate->setText("x" + QString::number(h_playbackSpeed));
}

void MainWindow::slowerFineSpeed()
{
    qDebug() << "Change Playback command called.";
    double mrate = -0.25;
    float newSpeed = h_playbackSpeed + mrate;
    m_player->setPlaybackRate(newSpeed);
    h_playbackSpeed = m_player->playbackRate();
    ui->m_playbackRate->setText("x" + QString::number(h_playbackSpeed));
}

void MainWindow::jumpBackwardSmall()
{
    jumpTo(m_jumpSmall*-1000);
}

void MainWindow::jumpBackwardMedium()
{
    jumpTo(m_jumpMedium*-1000);
}

void MainWindow::jumpBackwardLarge()
{
    jumpTo(m_jumpLarge*-1000);
}

void MainWindow::jumpBackwardExtraLarge()
{
    jumpTo(m_jumpExtraLarge*-1000);
}

void MainWindow::jumpForwardSmall()
{
    jumpTo(m_jumpSmall*1000);
}

void MainWindow::jumpForwardMedium()
{
    jumpTo(m_jumpMedium*1000);
}

void MainWindow::jumpForwardLarge()
{
    jumpTo(m_jumpLarge*1000);
}

void MainWindow::jumpForwardExtraLarge()
{
    jumpTo(m_jumpExtraLarge*1000);
}

void MainWindow::restartVideo()
{
    m_player->setPosition(0);
}

void MainWindow::jumpToTime()
{
    QDialog timeDialog;
    timeDialog.setWindowTitle("Go to time");

    QLabel *label = new QLabel("Go to time:", &timeDialog);
    QTimeEdit *timeEdit = new QTimeEdit(&timeDialog);
    timeEdit->setDisplayFormat("HH:mm:ss");

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &timeDialog);

    connect(buttonBox, &QDialogButtonBox::accepted, &timeDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &timeDialog, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(&timeDialog);
    layout->addWidget(label);
    layout->addWidget(timeEdit);
    layout->addWidget(buttonBox);
    timeDialog.setLayout(layout);

    int result = timeDialog.exec();
    if (result == QDialog::Accepted) {
        QTime selectedTime = timeEdit->time();
        qint64 mseconds = selectedTime.msecsSinceStartOfDay();
        qDebug() << "Jump to time [" << selectedTime.toString() << "] requested";

        if (mseconds <= m_player->duration()) {
            m_player->setPosition(mseconds);
        }
    }
}


// Audio Menu
void MainWindow::selectAudioOutput()
{
    for (int i = 0; i < 15; ++i) {
        h_audioOutputActions[i]->setChecked(false);
    }

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        auto device = action->data().value<QAudioDevice>();
        m_player->audioOutput()->setDevice(device);
        action->setChecked(true);
    }
}

void MainWindow::selectAudioTrack()
{
    for (int i = 0; i < 15; ++i) {
        h_audioTrackActions[i]->setChecked(false);
    }

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        m_player->setActiveAudioTrack(action->data().toInt());
        action->setChecked(true);
    }
}

void MainWindow::increaseVolume()
{
    float n_volume = m_audioOutput->volume() + 0.15;
    if (n_volume > 1.0) {
        n_volume = 1.0;
    }
    m_audioOutput->setVolume(n_volume);
    h_volume = m_audioOutput->volume();
}

void MainWindow::decreaseVolume()
{
    float n_volume = m_audioOutput->volume() - 0.15;
    if (n_volume < 0.0) {
        n_volume = 0.0;
    }
    m_audioOutput->setVolume(n_volume);
    h_volume = m_audioOutput->volume();
}

void MainWindow::toggleMute()
{
    if (m_audioOutput->isMuted()) {
        m_audioOutput->setMuted(false);
    } else {
        m_audioOutput->setMuted(true);
    }
    h_isMuted = m_audioOutput->isMuted();
}


// Video Menu
void MainWindow::toggleFullscreen()
{
    qDebug() << "Toggle Fullscreen command called.";
    if (ui->videoWidget->isFullScreen()) {
        ui->videoWidget->setFullScreen(false);
    } else {
        ui->videoWidget->setFullScreen(true);
    }
}

void MainWindow::takeSnapshot()
{
    showNotImplemented_Message();
}

void MainWindow::selectVideoTrack()
{
    for (int i = 0; i < 15; ++i) {
        h_videoTrackActions[i]->setChecked(false);
    }

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        m_player->setActiveVideoTrack(action->data().toInt());
        action->setChecked(true);
    }
}


// Markers Menu
void MainWindow::editMarker()
{
    showNotImplemented_Message();
    /*
    qDebug() << "Edit Selected Marker command called.";
    if (hMarkerEditDialog)
        hMarkerEditDialog->close();
    hMarkerEditDialog = new HMarkerEditDialog(this);
    //hMarkerEditDialog->setMarkerInfo()
    connect(hMarkerEditDialog, &HMarkerEditDialog::markerDeleted, this, &MainWindow::markerDeleted);
    connect(hMarkerEditDialog, &HMarkerEditDialog::markerSaved, this, &MainWindow::markersChanged);
    hMarkerEditDialog->show();
    hMarkerEditDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    */
}

void MainWindow::clearInMarker()
{
    h_inMarker = 0;
}

void MainWindow::clearOutMarker()
{
    h_outMarker = 0;
}

void MainWindow::clearInAndOutMarker()
{
    h_inMarker = 0;
    h_outMarker = 0;
}

void MainWindow::goToInMarker()
{
    if (h_inMarker > 0) {
        m_player->setPosition(h_inMarker);
    }
}

void MainWindow::goToOutMarker()
{
    if (h_outMarker > 0) {
        m_player->setPosition(h_outMarker);
    }
}

void MainWindow::addMarker()
{
    qDebug() << "Add Marker command called.";
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);

    QList<double> marker = markerMap.value("marker");
    marker.append(sliderPercent);
    markerMap.insert("marker", marker);

    m_videoSlider->setMarkers(markerMap);
}

void MainWindow::goToNextMarker()
{
    qDebug() << "Go to Next Marker command called.";
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);
    m_videoSlider->jumpToNextMarker(sliderPercent);
}

void MainWindow::goToPreviousMarker()
{
    qDebug() << "Go to Previous Marker command called.";
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);
    m_videoSlider->jumpToPreviousMarker(sliderPercent);
}

void MainWindow::clearSelectedMarker()
{
    qDebug() << "Clear Selected Marker command called.";


    QList<double> markerList;
    QList<double> markers = markerMap.value("marker").toList();
    QList<double> sceneMarkers = markerMap.value("scene").toList();
    QList<double> cumshotMarkers = markerMap.value("cumshot").toList();
    QList<double> stripMarkers = markerMap.value("strip").toList();
    QList<double> dialogMarkers = markerMap.value("dialog").toList();
    QList<double> cyanMarkers = markerMap.value("cyan").toList();
    QList<double> magentaMarkers = markerMap.value("magenta").toList();
    QList<double> orangeMarkers = markerMap.value("orange").toList();

    foreach(const double marker, markers)
    {
        markerList.append(marker);
    }

    foreach(const double marker, sceneMarkers)
    {
        markerList.append(marker);
    }

    foreach(const double marker, cumshotMarkers)
    {
        markerList.append(marker);
    }

    foreach(const double marker, stripMarkers)
    {
        markerList.append(marker);
    }

    foreach(const double marker, dialogMarkers)
    {
        markerList.append(marker);
    }

    foreach(const double marker, cyanMarkers)
    {
        markerList.append(marker);
    }

    foreach(const double marker, magentaMarkers)
    {
        markerList.append(marker);
    }

    foreach(const double marker, orangeMarkers)
    {
        markerList.append(marker);
    }

    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);

    for (auto it = markerList.begin(); it != markerList.end();) {
        if (qAbs(*it - sliderPercent) <= 0.001) {
            qDebug() << "Selected marker had a distance of: " << qAbs(*it - sliderPercent);
            it = markerList.erase(it);
        } else {
            ++it;
        }
    }
    m_videoSlider->setMarkers(markerMap);
}

void MainWindow::clearAllMarkers()
{
    qDebug() << "Clear Markers command called.";

    QMessageBox::StandardButton confirmationBox;
    confirmationBox = QMessageBox::question(this, "Clear Markers", "Are you sure you want to clear all the markers?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (confirmationBox == QMessageBox::Yes) {
        qDebug() << "User confirmed clear the markers";
        markerMap.clear();
        m_videoSlider->setMarkers(markerMap);
    } else {
        qDebug() << "User declined to clear the markers";
    }
}

void MainWindow::addSceneTransitionMarker()
{
    qDebug() << "Add Scene Marker command called.";
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);

    QList<double> marker = markerMap.value("scene");
    marker.append(sliderPercent);
    markerMap.insert("scene", marker);

    m_videoSlider->setMarkers(markerMap);
}

void MainWindow::addCumshotMarker()
{
    qDebug() << "Add Cumshot Marker command called.";
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);

    QList<double> marker = markerMap.value("cumshot");
    marker.append(sliderPercent);
    markerMap.insert("cumshot", marker);

    m_videoSlider->setMarkers(markerMap);
}

void MainWindow::addStripMarker()
{
    qDebug() << "Add Strip Marker command called.";
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);

    QList<double> marker = markerMap.value("strip");
    marker.append(sliderPercent);
    markerMap.insert("strip", marker);

    m_videoSlider->setMarkers(markerMap);
}

void MainWindow::addDialogMarker()
{
    qDebug() << "Add Dialog Marker command called.";
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);

    QList<double> marker = markerMap.value("dialog");
    marker.append(sliderPercent);
    markerMap.insert("dialog", marker);

    m_videoSlider->setMarkers(markerMap);
}

void MainWindow::addCyanMarker()
{
    qDebug() << "Add Cyan Marker command called.";
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);

    QList<double> marker = markerMap.value("cyan");
    marker.append(sliderPercent);
    markerMap.insert("cyan", marker);

    m_videoSlider->setMarkers(markerMap);
}

void MainWindow::addMagentaMarker()
{
    qDebug() << "Add Magenta Marker command called.";
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);

    QList<double> marker = markerMap.value("magenta");
    marker.append(sliderPercent);
    markerMap.insert("magenta", marker);

    m_videoSlider->setMarkers(markerMap);
}

void MainWindow::addOrangeMarker()
{
    qDebug() << "Add Orange Marker command called.";
    double distanceFromMin = (m_videoSlider->value() - m_videoSlider->minimum());
    double sliderRange = (m_videoSlider->maximum() - m_videoSlider->minimum());
    double sliderPercent = (distanceFromMin / sliderRange);

    QList<double> marker = markerMap.value("orange");
    marker.append(sliderPercent);
    markerMap.insert("orange", marker);

    m_videoSlider->setMarkers(markerMap);
}

void MainWindow::addInMarker()
{
    h_inMarker = m_player->position();
    qDebug() << "Mark In Set to: " << h_inMarker;
}

void MainWindow::addOutMarker()
{
    h_outMarker = m_player->position();
    qDebug() << "Mark Out Set to: " << h_outMarker;
}


// Tools Menu
void MainWindow::streamVideoFromStash()
{
    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("Stream Stash Video"),    // Dialog title
                                         tr("Stash Video ID:"),         // Input label
                                         QLineEdit::Normal,        // Input mode (e.g., Normal, NoEcho, Password)
                                         QString(),                // Default text
                                         &ok);                     // Pointer to a boolean that is true if OK was pressed

    if (ok && !text.isEmpty()) {
        QString videoUrl = "http://192.168.8.235:8793/scene/" + text + "/stream";
        const int previousMediaCount = m_playlist->mediaCount();
        m_playlist->addMedia(videoUrl);
        if (m_playlist->mediaCount() > previousMediaCount) {
            auto index = m_playlistModel->index(previousMediaCount, 0);
            ui->playlistView->setCurrentIndex(index);
            //if (m_playlistView)
            //    m_playlistView->setCurrentIndex(index);
            jump(index);
        }
    }
}

void MainWindow::createSubclip()
{
    extractSubclipFromVideo();
}


// Subtitle Menu
void MainWindow::selectSubtitleTrack()
{
    for (int i = 0; i < 15; ++i) {
        h_subtitleTrackActions[i]->setChecked(false);
    }

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        m_player->setActiveSubtitleTrack(action->data().toInt());
        action->setChecked(true);
    }
}

void MainWindow::openSubtitleFile()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Subtitle"));

    if (fileDialog.exec() == QDialog::Accepted) {
        QUrl subUrl = fileDialog.selectedFiles().first();

    }
}

void MainWindow::toggleSubtitles()
{
    showNotImplemented_Message();
}


// Help Menu
void MainWindow::showHelp()
{
    showNotImplemented_Message();
}

void MainWindow::showUpdates()
{
    if (h_updateDialog)
        h_updateDialog->close();

    h_updateDialog = new UpdateDialog(this);
    h_updateDialog->show();
    h_updateDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

void MainWindow::showAbout()
{
    if (h_aboutDialog)
        h_aboutDialog->close();

    h_aboutDialog = new AboutDialog(this);
    h_aboutDialog->show();
    h_aboutDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

#pragma endregion




void MainWindow::videoControl_Fullscreen()
{
    ui->videoWidget->setFullScreen(true);
}

void MainWindow::videoControl_TogglePlaylist()
{
    togglePlaylist();
}

void MainWindow::videoControl_LoopAll()
{
    m_playlistLoopAll = true;
    m_playlistLoopOne = false;
    m_playlistLoopNone = false;
    m_playlist->setPlaybackMode(Playlist::Loop);
}

void MainWindow::videoControl_LoopOne()
{
    m_playlistLoopAll = false;
    m_playlistLoopOne = true;
    m_playlistLoopNone = false;
    m_playlist->setPlaybackMode(Playlist::CurrentItemInLoop);
}

void MainWindow::videoControl_LoopNone()
{
    m_playlistLoopAll = false;
    m_playlistLoopOne = false;
    m_playlistLoopNone = true;
    m_playlist->setPlaybackMode(Playlist::Sequential);
}

void MainWindow::videoControl_Shuffle()
{
    m_playlist->shuffle();
}

void MainWindow::showPlaylistContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->playlistView->mapToGlobal(pos); // Map to global position
    m_pos = pos;

    if (m_playlist->mediaCount() > 0) {
        QModelIndex index = ui->playlistView->indexAt(pos);
        if (index.isValid()) {
            QMenu videoMenu;
            QAction *playAction = videoMenu.addAction("Play");
            QAction *streamAction = videoMenu.addAction("Stream");
            QAction *saveVideoAction = videoMenu.addAction("Save");
            QAction *informationAction = videoMenu.addAction("Information");
            videoMenu.addSeparator();
            QAction *showFolderAction = videoMenu.addAction("Show Containing Folder");
            videoMenu.addSeparator();
            QAction *addFileAction = videoMenu.addAction("Add File");
            QAction *addFolderAction = videoMenu.addAction("Add Folder");
            QAction *advancedOpenAction = videoMenu.addAction("Advanced Open");
            videoMenu.addSeparator();
            QAction *saveAction = videoMenu.addAction("Save Playlist to File");
            videoMenu.addSeparator();
            QAction *removeSelectedAction = videoMenu.addAction("Remove Selected");
            QAction *clearAction = videoMenu.addAction("Clear the playlist");
            videoMenu.addSeparator();
            QAction *shuffleAction = videoMenu.addAction("Shuffle");

            connect(playAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_PlayVideoAction);
            connect(streamAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_StreamVideoAction);
            connect(saveVideoAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_SaveVideoAction);
            connect(informationAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_InformationVideoAction);
            connect(showFolderAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_ShowFolderVideoAction);
            connect(removeSelectedAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_RemoveSelectedVideoAction);
            connect(addFileAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFileAction);
            connect(addFolderAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFolderAction);
            connect(advancedOpenAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AdvancedOpenAction);
            connect(saveAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_SaveAction);
            connect(clearAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_ClearAction);
            connect(shuffleAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_ShuffleAction);

            videoMenu.exec(globalPos);

        } else {
            QMenu videoMenu;
            QAction *addFileAction = videoMenu.addAction("Add File");
            QAction *addFolderAction = videoMenu.addAction("Add Folder");
            QAction *advancedOpenAction = videoMenu.addAction("Advanced Open");
            videoMenu.addSeparator();
            QAction *saveAction = videoMenu.addAction("Save Playlist to File");
            videoMenu.addSeparator();
            QAction *clearAction = videoMenu.addAction("Clear the playlist");
            videoMenu.addSeparator();
            QAction *shuffleAction = videoMenu.addAction("Shuffle");

            connect(addFileAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFileAction);
            connect(addFolderAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFolderAction);
            connect(advancedOpenAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AdvancedOpenAction);
            connect(saveAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_SaveAction);
            connect(clearAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_ClearAction);
            connect(shuffleAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_ShuffleAction);

            videoMenu.exec(globalPos);
        }
    } else {
        QMenu emptyMenu;
        QAction *addFileAction = emptyMenu.addAction("Add File");
        QAction *addFolderAction = emptyMenu.addAction("Add Folder");
        QAction *advancedOpenAction = emptyMenu.addAction("Advanced Open");

        connect(addFileAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFileAction);
        connect(addFolderAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AddFolderAction);
        connect(advancedOpenAction, &QAction::triggered, this, &MainWindow::playlistContextMenu_AdvancedOpenAction);

        emptyMenu.exec(globalPos);
    }

}

void MainWindow::playlistContextMenu_AddFileAction()
{
    openFile();
}

void MainWindow::playlistContextMenu_AddFolderAction()
{
    openFolder();
}

void MainWindow::playlistContextMenu_AdvancedOpenAction() {}

void MainWindow::playlistContextMenu_SaveAction()
{
    savePlaylist();
}

void MainWindow::playlistContextMenu_ClearAction()
{
    m_playlist->clear();
}

void MainWindow::playlistContextMenu_ShuffleAction()
{
    m_playlist->shuffle();
}

void MainWindow::playlistContextMenu_PlayVideoAction()
{
    QModelIndex index = ui->playlistView->indexAt(m_pos);
    m_playlist->setCurrentIndex(index.row());
}

void MainWindow::playlistContextMenu_StreamVideoAction() {}

void MainWindow::playlistContextMenu_SaveVideoAction() {}

void MainWindow::playlistContextMenu_InformationVideoAction() {}

void MainWindow::playlistContextMenu_ShowFolderVideoAction()
{
    QModelIndex index = ui->playlistView->indexAt(m_pos);
    QVariant data = index.data(); // Get the data
    QString text = data.toString();
    QFileInfo info(text);
    QString directoryPath = info.absoluteDir().absolutePath();
    QUrl folderUrl = QUrl::fromLocalFile(directoryPath);
    if (!QDesktopServices::openUrl(folderUrl)) {
        // Handle error if the folder couldn't be opened
        qWarning() << "Could not open folder: " << directoryPath.toStdString();
    }
}

void MainWindow::playlistContextMenu_RemoveSelectedVideoAction()
{
    QModelIndex index = ui->playlistView->indexAt(m_pos);
    if (index.row() == m_playlist->currentIndex()) {
        m_playlist->next();
    }
    m_playlist->removeMedia(index.row());
}
