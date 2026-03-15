#include "videocontrolwidget.h"
#include "../forms/ui_VideoControlWidget.h"


VideoControlWidget::VideoControlWidget(QWidget *parent) : QWidget(parent), ui(new Ui::VideoControlWidget)
{
    ui->setupUi(this);

    connect(ui->playButton, &QToolButton::clicked, this, &VideoControlWidget::playClicked);
    connect(ui->stopButton, &QToolButton::clicked, this, &VideoControlWidget::stop);
    connect(ui->nextButton, &QToolButton::clicked, this, &VideoControlWidget::next);
    connect(ui->previousButton, &QToolButton::clicked, this, &VideoControlWidget::previous);
    connect(ui->fullscreenButton, &QToolButton::clicked, this, &VideoControlWidget::fullScreenClicked);
    connect(ui->playlistButton, &QToolButton::clicked, this, &VideoControlWidget::playlistClicked);
    connect(ui->loopButton, &QToolButton::clicked, this, &VideoControlWidget::loopClicked);
    connect(ui->shuffleButton, &QToolButton::clicked, this, &VideoControlWidget::shuffleClicked);
    connect(ui->muteButton, &QToolButton::clicked, this, &VideoControlWidget::muteClicked);

    connect(ui->volumeSlider, &QSlider::valueChanged, this, &VideoControlWidget::onVolumeSliderValueChanged);

    setState(QMediaPlayer::StoppedState);

    refreshUI();
}

VideoControlWidget::~VideoControlWidget()
{
    delete ui;
}

void VideoControlWidget::refreshUI()
{
    double volumeDouble = ui->volumeSlider->value();
    int val = qRound(volumeDouble);
    QString volumeString = QString::number(val) + "%";
    ui->volumeLabel->setText(volumeString);
    m_volumeLevel = val;

    QSettings settings;
    QString theme = settings.value("theme", "System").toString();

    if (m_playerState == QMediaPlayer::StoppedState || m_playerState == QMediaPlayer::PausedState) {
        ui->playButton->setIcon(setButtonIcon("play", theme));
    } else {
        ui->playButton->setIcon(setButtonIcon("pause", theme));
    }

    if (m_loopOption == LoopOption::LoopOne) {
        ui->loopButton->setIcon(setButtonIcon("loop-one", theme));
    } else {
        ui->loopButton->setIcon(setButtonIcon("loop", theme));
    }

    if (m_playerMuted) {
        ui->muteButton->setIcon(setButtonIcon("mute", theme));
    } else {
        if (m_volumeLevel <= 33) {
            ui->muteButton->setIcon(setButtonIcon("volume-low", theme));
        } else if (m_volumeLevel <= 66 && m_volumeLevel > 33) {
            ui->muteButton->setIcon(setButtonIcon("volume-medium", theme));
        } else {
            ui->muteButton->setIcon(setButtonIcon("volume-high", theme));
        }
    }

    ui->previousButton->setIcon(setButtonIcon("back", theme));
    ui->stopButton->setIcon(setButtonIcon("stop", theme));
    ui->nextButton->setIcon(setButtonIcon("next", theme));
    ui->fullscreenButton->setIcon(setButtonIcon("fullscreen", theme));
    ui->playlistButton->setIcon(setButtonIcon("playlist", theme));
    ui->shuffleButton->setIcon(setButtonIcon("shuffle", theme));

    if (m_loopOption == LoopOption::LoopNone) {
        ui->loopButton->setStyleSheet("QToolButton { border: none; }");
    } else {
        ui->loopButton->setStyleSheet("QToolButton { border-width: 1px; border-style: solid; border-color: rgb(0, 140, 255); }");
    }

    if (m_isShuffle) {
        ui->shuffleButton->setStyleSheet("QToolButton { border-width: 1px; border-style: solid; border-color: rgb(0, 140, 255); }");
    } else {
        ui->shuffleButton->setStyleSheet("QToolButton { border: none; }");
    }
}

QMediaPlayer::PlaybackState VideoControlWidget::state() const
{
    return m_playerState;
}

void VideoControlWidget::setState(QMediaPlayer::PlaybackState state)
{
    m_playerState = state;
    refreshUI();
}

float VideoControlWidget::volume() const
{
    qreal linearVolume = QAudio::convertVolume(ui->volumeSlider->value() / qreal(100), QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);

    return linearVolume;
}

void VideoControlWidget::setVolume(float volume)
{
    qreal logarithmicVolume = QAudio::convertVolume(volume, QAudio::LinearVolumeScale, QAudio::LogarithmicVolumeScale);
    ui->volumeSlider->setValue(qRound(logarithmicVolume * 100));

    refreshUI();
}

bool VideoControlWidget::isMuted() const
{
    return m_playerMuted;
}

void VideoControlWidget::setMuted(bool muted)
{
    m_playerMuted = muted;
    refreshUI();
}

void VideoControlWidget::playClicked()
{
    if (m_playerState == QMediaPlayer::PlayingState) {
        emit pause();
    } else if (m_playerState == QMediaPlayer::PausedState) {
        emit play();
    } else {
        emit play();
    }
}

void VideoControlWidget::fullScreenClicked()
{
    emit fullScreen();
}

void VideoControlWidget::playlistClicked()
{
    emit togglePlaylist();
}

void VideoControlWidget::loopClicked()
{
    if (m_loopOption == LoopOption::LoopAll) {
        emit loopOne();
        m_loopOption = LoopOption::LoopOne;
    } else if (m_loopOption == LoopOption::LoopOne) {
        emit loopNone();
        m_loopOption = LoopOption::LoopNone;
    } else {
        emit loopAll();
        m_loopOption = LoopOption::LoopAll;
    }

    refreshUI();
}

void VideoControlWidget::shuffleClicked()
{
    if (m_isShuffle) {
        m_isShuffle = false;
    } else {
        m_isShuffle = true;
    }
    emit shuffle();
    refreshUI();
}

void VideoControlWidget::muteClicked()
{
    emit changeMuting(!m_playerMuted);
}

void VideoControlWidget::onVolumeSliderValueChanged()
{
    emit changeVolume(volume());
    refreshUI();
}

QIcon VideoControlWidget::setButtonIcon(const QString &buttonName, const QString &theme)
{
    qDebug() << "Setting button icon...";

    QPixmap pixmap;
    QColor color;

    if (buttonName == "play") {
        pixmap = QPixmap(":/img/images/play.svg");
    } else if (buttonName == "pause") {
        pixmap = QPixmap(":/img/images/pause.svg");
    } else if (buttonName == "back") {
        pixmap = QPixmap(":/img/images/back.svg");
    } else if (buttonName == "stop") {
        pixmap = QPixmap(":/img/images/stop.svg");
    } else if (buttonName == "next") {
        pixmap = QPixmap(":/img/images/next.svg");
    } else if (buttonName == "fullscreen") {
        pixmap = QPixmap(":/img/images/fullscreen.svg");
    } else if (buttonName == "fullscreen-exit") {
        pixmap = QPixmap(":/img/images/fullscreen-exit.svg");
    } else if (buttonName == "playlist") {
        pixmap = QPixmap(":/img/images/playlist.svg");
    } else if (buttonName == "loop") {
        pixmap = QPixmap(":/img/images/loop.svg");
    } else if (buttonName == "loop-one") {
        pixmap = QPixmap(":/img/images/loop-one.svg");
    } else if (buttonName == "shuffle") {
        pixmap = QPixmap(":/img/images/shuffle.svg");
    } else if (buttonName == "mute") {
        pixmap = QPixmap(":/img/images/mute.svg");
    } else if (buttonName == "volume-low") {
        pixmap = QPixmap(":/img/images/volume-low.svg");
    } else if (buttonName == "volume-medium") {
        pixmap = QPixmap(":/img/images/volume-medium.svg");
    } else if (buttonName == "volume-high") {
        pixmap = QPixmap(":/img/images/volume-high.svg");
    } else {
        qCritical() << "Button name doesnt have a file assigned to it.";
        return QIcon();
    }

    if (theme == "System") {
        qDebug() << "Using system theme for video control buttons.";
        if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Light) {
            qDebug() << "Windows is in Light Mode.";
            color = QColor(Qt::black);
        } else if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
            qDebug() << "Windows is in Dark Mode.";
            color = QColor(Qt::white);
        } else {
            qDebug() << "Could not determine Windows Theme. Using Dark Mode.";
            color = QColor(Qt::black);
        }
    } else if (theme == "Light") {
        qDebug() << "Using Light Theme.";
        color = QColor(Qt::black);
    } else if (theme == "Dark") {
        qDebug() << "Using Dark Theme.";
        color = QColor(Qt::white);
    } else {
        qCritical() << "Could not determine Color Theme.";
        return QIcon();
    }

    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn); // This mode colors only the non-transparent parts.
    painter.fillRect(pixmap.rect(), color);
    painter.end();

    return QIcon(pixmap);
}
