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
    connect(ui->cancelButton, &QPushButton::clicked, this, &SettingsWindow::cancelButton_Clicked);
    connect(ui->applyButton, &QPushButton::clicked, this, &SettingsWindow::applyButton_Clicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &SettingsWindow::resetButton_Clicked);

    connect(ui->language, &QComboBox::currentIndexChanged, this, &SettingsWindow::language_Changed);
    connect(ui->systemTrayIcon, &QCheckBox::checkStateChanged, this, &SettingsWindow::systemTray_Checked);
    connect(ui->currentTheme, &QComboBox::currentIndexChanged, this, &SettingsWindow::theme_Changed);
    connect(ui->rememberWindowSize, &QCheckBox::checkStateChanged, this, &SettingsWindow::rememberWindowSize_Checked);
    connect(ui->showPlaylistOnStart, &QCheckBox::checkStateChanged, this, &SettingsWindow::showPlaylistOnStart_Checked);
    connect(ui->showVideoControlsOnStart, &QCheckBox::checkStateChanged, this, &SettingsWindow::showVideoControlsOnStart_Checked);
    connect(ui->showStatusBarOnStart, &QCheckBox::checkStateChanged, this, &SettingsWindow::showStatusBarOnStart_Checked);
    connect(ui->updateBranch, &QComboBox::currentIndexChanged, this, &SettingsWindow::updateBranch_Changed);
    connect(ui->checkForUpdates, &QPushButton::clicked, this, &SettingsWindow::checkForUpdates_Clicked);
    connect(ui->enableAutomaticUpdates, &QCheckBox::checkStateChanged, this, &SettingsWindow::enableAutomaticUpdates_Checked);

    connect(ui->allowOneInstance, &QCheckBox::checkStateChanged, this, &SettingsWindow::allowOnlyOneInstance_Checked);
    connect(ui->showMediaChangeNotification, &QComboBox::currentIndexChanged, this, &SettingsWindow::showMediaChangeNotification_Changed);
    connect(ui->continueVideoPlayback, &QComboBox::currentIndexChanged, this, &SettingsWindow::continuePlayback_Changed);
    connect(ui->pausePlaybackWhenMinimized, &QCheckBox::checkStateChanged, this, &SettingsWindow::pausePlaybackWhenMinimized_Checked);
    connect(ui->pauseOnLastVideoFrame, &QCheckBox::checkStateChanged, this, &SettingsWindow::pauseOnLastFrameOfVideo_Checked);
    connect(ui->useHardwareAcceleration, &QCheckBox::checkStateChanged, this, &SettingsWindow::useHardwareAcceleration_Checked);
    connect(ui->defaultVideoContrast, &QSlider::valueChanged, this, &SettingsWindow::defaultVideoContrast_ValueChanged);
    connect(ui->defaultAspectRatio, &QComboBox::currentIndexChanged, this, &SettingsWindow::defaultAspectRatio_Changed);

    connect(ui->playbackSpeedAdjustment, &QDoubleSpinBox::valueChanged, this, &SettingsWindow::playbackSpeedAdjustment_Changed);
    connect(ui->playbackSpeedAdjustmentFine, &QDoubleSpinBox::valueChanged, this, &SettingsWindow::playbackSpeedAdjustmentFine_Changed);
    connect(ui->volumeStep, &QSpinBox::valueChanged, this, &SettingsWindow::volumeStep_Changed);
    connect(ui->extraLargeJump, &QSpinBox::valueChanged, this, &SettingsWindow::extraLargeJump_Changed);
    connect(ui->largeJump, &QSpinBox::valueChanged, this, &SettingsWindow::largeJump_Changed);
    connect(ui->mediumJump, &QSpinBox::valueChanged, this, &SettingsWindow::mediumJump_Changed);
    connect(ui->smallJump, &QSpinBox::valueChanged, this, &SettingsWindow::smallJump_Changed);
    connect(ui->extraSmallJump, &QSpinBox::valueChanged, this, &SettingsWindow::extraSmallJump_Changed);
    connect(ui->jumpToEndPercentage, &QSpinBox::valueChanged, this, &SettingsWindow::jumpToEndPercentage_Changed);

    connect(ui->autohideSlider, &QCheckBox::checkStateChanged, this, &SettingsWindow::autohideSliderWhenPlaying_Checked);
    connect(ui->sliderAutohideTime, &QDoubleSpinBox::valueChanged, this, &SettingsWindow::autohideSliderTime_ValueChanged);
    connect(ui->unhideSliderOnHotkeyPress, &QCheckBox::checkStateChanged, this, &SettingsWindow::unhideSliderOnHotkey_Checked);
    connect(ui->sliderHeight, &QSpinBox::valueChanged, this, &SettingsWindow::sliderHeight_Changed);
    connect(ui->videoMarkerHeight, &QSpinBox::valueChanged, this, &SettingsWindow::videoMarkerHeight_Changed);
    connect(ui->videoMarkerWidth, &QSpinBox::valueChanged, this, &SettingsWindow::videoMarkerWidth_Changed);

    connect(ui->filter, &QLineEdit::textChanged, this, &SettingsWindow::filter_TextChanged);
    connect(ui->hotkeyFilter, &QKeySequenceEdit::keySequenceChanged, this, &SettingsWindow::filterHotkey_KeySequenceChanged);
    connect(ui->resetAllHotkeys, &QPushButton::clicked, this, &SettingsWindow::resetHotkeys_Clicked);

    connect(ui->logToFile, &QCheckBox::checkStateChanged, this, &SettingsWindow::logToFile_Checked);
    connect(ui->maxLogFiles, &QSpinBox::valueChanged, this, &SettingsWindow::maxLogs_ValueChanged);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{

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

void SettingsWindow::systemTray_Checked(int state)
{

}

void SettingsWindow::theme_Changed(int index) {}

void SettingsWindow::rememberWindowSize_Checked(int state)
{

}

void SettingsWindow::showPlaylistOnStart_Checked(int state) {}

void SettingsWindow::showVideoControlsOnStart_Checked(int state)
{

}

void SettingsWindow::showStatusBarOnStart_Checked(int state)
{

}

void SettingsWindow::updateBranch_Changed(int index)
{

}

void SettingsWindow::checkForUpdates_Clicked() {}

void SettingsWindow::enableAutomaticUpdates_Checked(int state) {}


// Player Settings
void SettingsWindow::allowOnlyOneInstance_Checked(int state) {}

void SettingsWindow::showMediaChangeNotification_Changed(int index) {}

void SettingsWindow::continuePlayback_Changed(int index) {}

void SettingsWindow::pausePlaybackWhenMinimized_Checked(int state)
{

}

void SettingsWindow::pauseOnLastFrameOfVideo_Checked(int state)
{

}

void SettingsWindow::useHardwareAcceleration_Checked(int state) {}

void SettingsWindow::defaultVideoContrast_ValueChanged(int value) {}

void SettingsWindow::defaultAspectRatio_Changed(int index) {}


// Playback Settings
void SettingsWindow::playbackSpeedAdjustment_Changed(double value) {}

void SettingsWindow::playbackSpeedAdjustmentFine_Changed(double value) {}

void SettingsWindow::volumeStep_Changed(double value) {}

void SettingsWindow::extraLargeJump_Changed(double value)
{

}

void SettingsWindow::largeJump_Changed(double value)
{

}

void SettingsWindow::mediumJump_Changed(double value)
{

}

void SettingsWindow::smallJump_Changed(double value)
{

}

void SettingsWindow::extraSmallJump_Changed(double value)
{

}

void SettingsWindow::jumpToEndPercentage_Changed(double value)
{

}


// Interface Settings
void SettingsWindow::autohideSliderWhenPlaying_Checked(int state) {}

void SettingsWindow::autohideSliderTime_ValueChanged(double value) {}

void SettingsWindow::unhideSliderOnHotkey_Checked(int state) {}

void SettingsWindow::sliderHeight_Changed(int value) {}

void SettingsWindow::videoMarkerHeight_Changed(int value) {}

void SettingsWindow::videoMarkerWidth_Changed(int value) {}


// Hotkey Settings
void SettingsWindow::filter_TextChanged(QString value)
{

}

void SettingsWindow::filterHotkey_KeySequenceChanged(const QKeySequence &keySequence)
{

}

void SettingsWindow::resetHotkeys_Clicked()
{

}


// Playlist Settings


// File Associations Settings


// Advanced Settings
void SettingsWindow::logToFile_Checked(int state) {}

void SettingsWindow::maxLogs_ValueChanged(int value)
{

}


void SettingsWindow::loadSettings()
{
    QSettings settings;

    ui->language->setCurrentIndex(settings.value("language", 0).toInt());
    ui->systemTrayIcon->setChecked(settings.value("systemTrayIcon", true).toBool());
    ui->currentTheme->setCurrentIndex(settings.value("currentTheme", 0).toInt());
    ui->rememberWindowSize->setChecked(settings.value("rememberWindowSize", false).toBool());
    ui->showPlaylistOnStart->setChecked(settings.value("showPlaylistOnStart", true).toBool());
    ui->showVideoControlsOnStart->setChecked(settings.value("showVideoControlsOnStart", false).toBool());
    ui->showStatusBarOnStart->setChecked(settings.value("showStatusBarOnStart", false).toBool());
    ui->updateBranch->setCurrentIndex(settings.value("updateBranch", 0).toInt());
    ui->enableAutomaticUpdates->setChecked(settings.value("enableAutomaticUpdates", true).toBool());

    ui->allowOneInstance->setChecked(settings.value("allowOnlyOneInstance", true).toBool());
    ui->showMediaChangeNotification->setCurrentIndex(settings.value("showMediaChangeNotification", 1).toInt());
    ui->continueVideoPlayback->setCurrentIndex(settings.value("continuePlayback", 1).toInt());
    ui->pausePlaybackWhenMinimized->setChecked(settings.value("pausePlaybackWhenMinimized", true).toBool());
    ui->pauseOnLastVideoFrame->setChecked(settings.value("pauseOnLastFrameOfVideo", false).toBool());
    ui->useHardwareAcceleration->setChecked(settings.value("useHardwareAcceleration", false).toBool());
    ui->defaultVideoContrast->setValue(settings.value("defaultVideoContrast", 50).toInt());
    ui->defaultAspectRatio->setCurrentIndex(settings.value("defaultAspectRatio", 0).toInt());

    ui->playbackSpeedAdjustment->setValue(settings.value("playbackSpeedAdjustment", 0.5).toDouble());
    ui->playbackSpeedAdjustmentFine->setValue(settings.value("playbackSpeedAdjustmentFine", 0.25).toDouble());
    ui->volumeStep->setValue(settings.value("volumeStep", 10).toInt());
    ui->extraLargeJump->setValue(settings.value("extraLargeJump", 90000).toInt());
    ui->largeJump->setValue(settings.value("largeJump", 60000).toInt());
    ui->mediumJump->setValue(settings.value("mediumJump", 30000).toInt());
    ui->smallJump->setValue(settings.value("smallJump", 15000).toInt());
    ui->extraSmallJump->setValue(settings.value("extraSmallJump", 5000).toInt());
    ui->jumpToEndPercentage->setValue(settings.value("jumpToEndPercentage", 0.05).toInt());

    ui->autohideSlider->setChecked(settings.value("autohideSliderWhenPlaying", true).toBool());
    ui->sliderAutohideTime->setValue(settings.value("sliderAutohideTime", 5).toDouble());
    ui->unhideSliderOnHotkeyPress->setChecked(settings.value("unhideSliderOnHotkeyPress", false).toBool());
    ui->sliderHeight->setValue(settings.value("sliderHeight", 10).toInt());
    ui->videoMarkerHeight->setValue(settings.value("videoMarkerHeight", 10).toInt());
    ui->videoMarkerWidth->setValue(settings.value("videoMarkerWidth", 10).toInt());

    ui->logToFile->setChecked(settings.value("logToFile", true).toBool());
    ui->maxLogFiles->setValue(settings.value("maxLogFiles", 10).toInt());
}
