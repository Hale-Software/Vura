/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

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
#include "ui_testwindow.h"


static Blog* globalRedirector = nullptr;

TestWindow::TestWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::TestWindow)
{
    ui->setupUi(this);

    qInstallMessageHandler(Blog::messageHandler);
    globalRedirector = Blog::instance();

    m_rangeSlider = new RangeSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles, nullptr);
    ui->horizontalLayout->insertWidget(1, m_rangeSlider);
    ui->horizontalLayout->setStretch(1, 1);

    QUrl url = QUrl::fromLocalFile(videoFileName);
    m_player = new QMediaPlayer(this);
    m_player->setVideoOutput(ui->widget);
    m_player->setSource(url);
    m_player->play();

}

TestWindow::~TestWindow()
{
    delete ui;
}
