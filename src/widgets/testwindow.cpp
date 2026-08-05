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

#include "testwindow.h"
#include "ui_TestWindow.h"


TestWindow::TestWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::TestWindow)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    m_mediaPlayer = new VuraMediaEngine(this);
    ui->verticalLayout_2->insertWidget(0, m_mediaPlayer);
    ui->verticalLayout_2->setStretch(0, 1);
    connect(ui->playButton, &QToolButton::clicked, m_mediaPlayer, &VuraMediaEngine::play);
    connect(ui->pauseButton, &QToolButton::clicked, m_mediaPlayer, &VuraMediaEngine::pause);
    connect(ui->stopButton, &QToolButton::clicked, m_mediaPlayer, &VuraMediaEngine::stop);
    connect(ui->timelineSlider, &QSlider::sliderMoved, m_mediaPlayer, &VuraMediaEngine::seek);

    connect(ui->actionOpen_File, &QAction::triggered, this, &TestWindow::actionOpenFile);
    connect(ui->previousButton, &QToolButton::clicked, this, &TestWindow::previousButtonClicked);
    connect(ui->jumpBackwardsButton, &QToolButton::clicked, this, &TestWindow::jumpBackwardsButtonClicked);
    connect(ui->jumpForwardsButton, &QToolButton::clicked, this, &TestWindow::jumpForwardButtonClicked);
    connect(ui->nextButton, &QToolButton::clicked, this, &TestWindow::nextButtonClicked);

    connect(m_mediaPlayer, &VuraMediaEngine::positionChanged, this, &TestWindow::positionChanged);
    connect(m_mediaPlayer, &VuraMediaEngine::durationChanged, this, &TestWindow::durationChanged);
    connect(m_mediaPlayer, &VuraMediaEngine::errorOccurred, this, &TestWindow::errorOccurred);
}

TestWindow::~TestWindow()
{
    delete ui;
}

void TestWindow::closeEvent(QCloseEvent *event)
{
    if (m_mediaPlayer)
        m_mediaPlayer->stop();

    QMainWindow::closeEvent(event);
}

void TestWindow::positionChanged(const qint64 positionMs)
{
    m_position = qint64ToInt(positionMs);
    ui->timelineSlider->setValue(qint64ToInt(positionMs));
    ui->positionLabel->setText(timestampString(positionMs));
}

void TestWindow::durationChanged(const qint64 durationMs)
{
    m_duration = qint64ToInt(durationMs);
    ui->timelineSlider->setMaximum(qint64ToInt(durationMs));
    ui->durationLabel->setText(timestampString(durationMs));
}

void TestWindow::errorOccurred(const QString &errorMessage)
{
    QMessageBox::critical(this, "Error", errorMessage);
}

void TestWindow::actionOpenFile()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), tr("All Files (*.*)"));

    if (!fileName.isEmpty()) {
        m_mediaPlayer->loadMedia(fileName);
    }
}

void TestWindow::previousButtonClicked()
{
    m_mediaPlayer->seek(0);
}

void TestWindow::jumpBackwardsButtonClicked()
{
    int pos = m_position - 15;
    if (pos < 0) pos = 0;
    m_mediaPlayer->seek(pos);
}

void TestWindow::jumpForwardButtonClicked()
{
    int pos = m_position + 15;
    if (pos > m_duration) pos = m_duration;
    m_mediaPlayer->seek(pos);
}

void TestWindow::nextButtonClicked()
{

}

QString TestWindow::timestampString(const qint64 timestampMs)
{
    const int pos = static_cast<int>(timestampMs);

    const QTime time((pos / 3600) % 24, (pos / 60) % 60, pos % 60);
    QString timestamp = time.toString("hh:mm:ss");

    return timestamp;
}

int TestWindow::qint64ToInt(const qint64 value)
{
    const int newValue = static_cast<int>(value);
    return newValue;
}
