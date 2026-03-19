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

#include "ContinuePlaybackRibbon.h"
#include "ui_ContinuePlaybackRibbon.h"


ContinuePlaybackRibbon::ContinuePlaybackRibbon(QWidget *parent) : QWidget(parent), ui(new Ui::ContinuePlaybackRibbon)
{
    ui->setupUi(this);

    connect(ui->continueButton, &QPushButton::clicked, this, &ContinuePlaybackRibbon::continue_Clicked);
    connect(ui->closeButton, &QToolButton::clicked, this, &ContinuePlaybackRibbon::close_Clicked);
    QTimer::singleShot(2500, this, &ContinuePlaybackRibbon::timeToClose);
}

ContinuePlaybackRibbon::~ContinuePlaybackRibbon()
{
    delete ui;
}

void ContinuePlaybackRibbon::continue_Clicked()
{
    emit continuePlayback(true);
}

void ContinuePlaybackRibbon::close_Clicked()
{
    emit continuePlayback(false);
}

void ContinuePlaybackRibbon::timeToClose()
{
    emit continuePlayback(false);
}
