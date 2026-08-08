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

#include "PlayerSettings.h"
#include "ui_PlayerSettings.h"

#include <QSettings>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QDebug>


PlayerSettings::PlayerSettings(QWidget *parent) : QWidget(parent), ui(new Ui::PlayerSettings)
{
    ui->setupUi(this);

    const QSettings settings;
    ui->allowOnlyOneInstance->setChecked(settings.value("allowOnlyOneInstance", true).toBool());
    ui->showMediaChangeNotification->setCurrentIndex(settings.value("showMediaChangeNotification", 1).toInt());
    ui->continueVideoPlayback->setCurrentIndex(settings.value("continuePlayback", 1).toInt());
    ui->pausePlaybackWhenMinimized->setChecked(settings.value("pausePlaybackWhenMinimized", true).toBool());
    ui->pauseOnLastVideoFrame->setChecked(settings.value("pauseOnLastFrameOfVideo", false).toBool());
    ui->useHardwareAcceleration->setChecked(settings.value("useHardwareAcceleration", false).toBool());
    ui->defaultVideoContrast->setValue(settings.value("defaultVideoContrast", 50).toInt());
    ui->defaultAspectRatio->setCurrentIndex(settings.value("defaultAspectRatio", 0).toInt());
    ui->defaultVideoContrast->setEnabled(settings.value("useHardwareAcceleration", false).toBool());
    ui->defaultAspectRatio->setEnabled(settings.value("useHardwareAcceleration", false).toBool());

    connect(ui->allowOnlyOneInstance, &QCheckBox::stateChanged, this, &PlayerSettings::allowOnlyOneInstance_Checked);
    connect(ui->showMediaChangeNotification, &QComboBox::currentIndexChanged, this, &PlayerSettings::showMediaChangeNotification_Changed);
    connect(ui->continueVideoPlayback, &QComboBox::currentIndexChanged, this, &PlayerSettings::continueVideoPlayback_Changed);
    connect(ui->pausePlaybackWhenMinimized, &QCheckBox::stateChanged, this, &PlayerSettings::pausePlaybackWhenMinimized_Checked);
    connect(ui->pauseOnLastVideoFrame, &QCheckBox::stateChanged, this, &PlayerSettings::pauseOnLastFrameOfVideo_Checked);
    connect(ui->useHardwareAcceleration, &QCheckBox::stateChanged, this, &PlayerSettings::useHardwareAcceleration_Checked);
    connect(ui->defaultVideoContrast, &QSlider::valueChanged, this, &PlayerSettings::defaultVideoContrast_ValueChanged);
    connect(ui->defaultAspectRatio, &QComboBox::currentIndexChanged, this, &PlayerSettings::defaultAspectRatio_Changed);
}

PlayerSettings::~PlayerSettings()
{
    delete ui;
}

bool PlayerSettings::unsavedChanges()
{
    m_unsavedChanges = false;
    const QSettings settings;
    if (settings.value("allowOnlyOneInstance", true).toBool() != ui->allowOnlyOneInstance->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("showMediaChangeNotification", 1).toInt() != ui->showMediaChangeNotification->currentIndex()) {
        m_unsavedChanges = true;
    } else if (settings.value("continuePlayback", 1).toInt() != ui->continueVideoPlayback->currentIndex()) {
        m_unsavedChanges = true;
    } else if (settings.value("pausePlaybackWhenMinimized", true).toBool() != ui->pausePlaybackWhenMinimized->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("pauseOnLastFrameOfVideo", false).toBool() != ui->pauseOnLastVideoFrame->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("useHardwareAcceleration", false).toBool() != ui->useHardwareAcceleration->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("defaultVideoContrast", 50).toInt() != ui->defaultVideoContrast->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("defaultAspectRatio", 0).toInt() != ui->defaultAspectRatio->currentIndex()) {
        m_unsavedChanges = true;
    }
    return m_unsavedChanges;
}

void PlayerSettings::resetToDefaults()
{
    const QSettings settings;
    ui->allowOnlyOneInstance->setChecked(true);
    ui->showMediaChangeNotification->setCurrentIndex(1);
    ui->continueVideoPlayback->setCurrentIndex(1);
    ui->pausePlaybackWhenMinimized->setChecked(true);
    ui->pauseOnLastVideoFrame->setChecked(false);
    ui->useHardwareAcceleration->setChecked(false);
    ui->defaultVideoContrast->setValue(50);
    ui->defaultAspectRatio->setCurrentIndex(0);
    ui->defaultVideoContrast->setEnabled(false);
    ui->defaultAspectRatio->setEnabled(false);
    m_unsavedChanges = true;
}

void PlayerSettings::saveSettings()
{
    QSettings settings;
    settings.setValue("allowOnlyOneInstance", ui->allowOnlyOneInstance->isChecked());
    settings.setValue("showMediaChangeNotification", ui->showMediaChangeNotification->currentIndex());
    settings.setValue("continuePlayback", ui->continueVideoPlayback->currentIndex());
    settings.setValue("pausePlaybackWhenMinimized", ui->pausePlaybackWhenMinimized->isChecked());
    settings.setValue("pauseOnLastFrameOfVideo", ui->pauseOnLastVideoFrame->isChecked());
    settings.setValue("useHardwareAcceleration", ui->useHardwareAcceleration->isChecked());
    settings.setValue("defaultVideoContrast", ui->defaultVideoContrast->value());
    settings.setValue("defaultAspectRatio", ui->defaultAspectRatio->currentIndex());
    m_unsavedChanges = false;
}

void PlayerSettings::allowOnlyOneInstance_Checked(int state)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlayerSettings::showMediaChangeNotification_Changed(int state)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlayerSettings::continueVideoPlayback_Changed(int state)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlayerSettings::pausePlaybackWhenMinimized_Checked(int state)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlayerSettings::pauseOnLastFrameOfVideo_Checked(int state)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlayerSettings::useHardwareAcceleration_Checked(const int state)
{
    if (state == Qt::Checked) {
        ui->defaultVideoContrast->setEnabled(true);
        ui->defaultAspectRatio->setEnabled(true);
    } else {
        ui->defaultVideoContrast->setEnabled(false);
        ui->defaultAspectRatio->setEnabled(false);
    }
    unsavedChanges();
    emit settingsChanged();
}

void PlayerSettings::defaultVideoContrast_ValueChanged(int value)
{
    ui->defaultVideoContrastPercentageLabel->setText(QString("%1%").arg(QString::number(value, 'f', 0)));
    unsavedChanges();
    emit settingsChanged();
}

void PlayerSettings::defaultAspectRatio_Changed(int index)
{
    unsavedChanges();
    emit settingsChanged();
}
