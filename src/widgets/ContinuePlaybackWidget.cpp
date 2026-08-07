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

#include "ContinuePlaybackWidget.h"
#include "ui_ContinuePlaybackWidget.h"


ContinuePlaybackWidget::ContinuePlaybackWidget(const qint64 savedPosition, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ContinuePlaybackWidget),
      m_savedPosition(savedPosition)
{
    ui->setupUi(this);

    QTime time(0, 0, 0);
    time = time.addMSecs(static_cast<int>(m_savedPosition));
    const QString timeString = time.toString("hh:mm:ss");

    ui->questionLabel->setText(QString("Do you want to restart the playback at %1?").arg(timeString));

    connect(ui->continueButton, &QPushButton::clicked, this, &ContinuePlaybackWidget::continuePlaybackClicked);
    connect(ui->closeButton, &QToolButton::clicked, this, &ContinuePlaybackWidget::closeWidgetClicked);
}

ContinuePlaybackWidget::~ContinuePlaybackWidget()
{
    delete ui;
}

void ContinuePlaybackWidget::continuePlaybackClicked()
{
    emit continuePlayback(m_savedPosition);
}

void ContinuePlaybackWidget::closeWidgetClicked()
{
    emit closeWidget();
}
