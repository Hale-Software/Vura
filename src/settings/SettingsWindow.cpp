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

#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

#include <ui-config.h>


SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    ui->navigationList->setStyleSheet("QListWidget { background: transparent; }");
    ui->navigationList->viewport()->setAutoFillBackground(false);
    ui->navigationList->setCurrentRow(0);

    connect(ui->navigationList, &QListWidget::itemSelectionChanged, this, &SettingsWindow::pageSelection_Changed);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::pageSelection_Changed()
{
    ui->settingsViewArea->setCurrentIndex(ui->navigationList->currentRow());
}

void SettingsWindow::resetButton_Clicked() {}

void SettingsWindow::applyButton_Clicked() {}

void SettingsWindow::cancelButton_Clicked() {}

// General Settings
void SettingsWindow::language_Changed(int index) {}

void SettingsWindow::theme_Changed(int index) {}

void SettingsWindow::showPlaylistOnStart_Checked(int state) {}

void SettingsWindow::useHardwareAcceleration_Checked(int state) {}

void SettingsWindow::defaultVideoContrast_ValueChanged(int value) {}

void SettingsWindow::defaultAspectRatio_Changed(int index) {}

void SettingsWindow::checkForUpdates_Clicked() {}

void SettingsWindow::enableAutomaticUpdates_Checked(int state) {}

void SettingsWindow::updateChannel_Changed(int index) {}

// Interface Settings
void SettingsWindow::autohideSliderWhenPlaying_Checked(int state) {}

void SettingsWindow::autohideSliderTime_ValueChanged(double value) {}

void SettingsWindow::unhideSliderOnHotkey_Checked(int state) {}

// Player Settings
void SettingsWindow::allowOnlyOneInstance_Checked(int state) {}

void SettingsWindow::showMediaChangeNotification_Changed(int index) {}

void SettingsWindow::continuePlayback_Changed(int index) {}
