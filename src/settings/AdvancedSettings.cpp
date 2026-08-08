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

#include "AdvancedSettings.h"
#include "ui_AdvancedSettings.h"

#include <QSettings>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDebug>


AdvancedSettings::AdvancedSettings(QWidget *parent) : QWidget(parent), ui(new Ui::AdvancedSettings)
{
    ui->setupUi(this);

    connect(ui->logToFile, &QCheckBox::stateChanged, this, &AdvancedSettings::logToFile_StateChanged);
    connect(ui->maxLogFiles, &QSpinBox::valueChanged, this, &AdvancedSettings::maxLogFiles_ValueChanged);
    connect(ui->markerProximityThreshold, &QDoubleSpinBox::valueChanged, this, &AdvancedSettings::markerProximityThreshold_ValueChanged);
    connect(ui->playbackSpeedMax, &QDoubleSpinBox::valueChanged, this, &AdvancedSettings::playbackSpeedMax_ValueChanged);
    connect(ui->defaultWindowHeight, &QSpinBox::valueChanged, this, &AdvancedSettings::defaultWindowHeight_ValueChanged);
    connect(ui->defaultWindowWidth, &QSpinBox::valueChanged, this, &AdvancedSettings::defaultWindowWidth_ValueChanged);
    connect(ui->continuePlaybackBannerTime, &QSpinBox::valueChanged, this, &AdvancedSettings::continuePlaybackBannerTime_ValueChanged);

    const QSettings settings;
    ui->logToFile->setChecked(settings.value("logToFile", true).toBool());
    ui->maxLogFiles->setValue(settings.value("maxLogFiles", 10).toInt());
    ui->markerProximityThreshold->setValue(settings.value("markerProximityThreshold", 0.005).toDouble());
    ui->playbackSpeedMax->setValue(settings.value("playbackSpeedMax", 10.0).toDouble());
    ui->defaultWindowHeight->setValue(settings.value("defaultWindowHeight", 550).toInt());
    ui->defaultWindowWidth->setValue(settings.value("defaultWindowWidth", 955).toInt());
    ui->continuePlaybackBannerTime->setValue(settings.value("continuePlaybackBannerTime", 5).toInt());
}

AdvancedSettings::~AdvancedSettings()
{
    delete ui;
}

bool AdvancedSettings::unsavedChanges()
{
    m_unsavedChanges = false;
    const QSettings settings;
    if (settings.value("logToFile", true).toBool() != ui->logToFile->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("maxLogFiles", 10).toInt() != ui->maxLogFiles->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("markerProximityThreshold", 0.005).toDouble() != ui->markerProximityThreshold->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("playbackSpeedMax", 10.0).toDouble() != ui->playbackSpeedMax->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("defaultWindowHeight", 550).toInt() != ui->defaultWindowHeight->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("defaultWindowWidth", 955).toInt() != ui->defaultWindowWidth->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("continuePlaybackBannerTime", 5).toInt() != ui->continuePlaybackBannerTime->value()) {
        m_unsavedChanges = true;
    }

    qDebug() << "Advanced Settings Unsaved Changes:" << m_unsavedChanges;
    return m_unsavedChanges;
}

void AdvancedSettings::resetToDefaults()
{
    qDebug() << "Resetting Advanced Settings to Defaults";
    ui->logToFile->setChecked(true);
    ui->maxLogFiles->setValue(10);
    ui->markerProximityThreshold->setValue(0.005);
    ui->playbackSpeedMax->setValue(10.0);
    ui->defaultWindowHeight->setValue(550);
    ui->defaultWindowWidth->setValue(955);
    ui->continuePlaybackBannerTime->setValue(5);
    m_unsavedChanges = true;
}

void AdvancedSettings::saveSettings()
{
    qDebug() << "Saving Advanced Settings";
    QSettings settings;
    settings.setValue("logToFile", ui->logToFile->isChecked());
    settings.setValue("maxLogFiles", ui->maxLogFiles->value());
    settings.setValue("markerProximityThreshold", ui->markerProximityThreshold->value());
    settings.setValue("playbackSpeedMax", ui->playbackSpeedMax->value());
    settings.setValue("defaultWindowHeight", ui->defaultWindowHeight->value());
    settings.setValue("defaultWindowWidth", ui->defaultWindowWidth->value());
    settings.setValue("continuePlaybackBannerTime", ui->continuePlaybackBannerTime->value());
    m_unsavedChanges = false;
}

void AdvancedSettings::logToFile_StateChanged(const int state)
{
    unsavedChanges();
    emit settingsChanged();
}

void AdvancedSettings::maxLogFiles_ValueChanged(const int value)
{
    unsavedChanges();
    emit settingsChanged();
}

void AdvancedSettings::markerProximityThreshold_ValueChanged(double value)
{
    unsavedChanges();
    emit settingsChanged();
}

void AdvancedSettings::playbackSpeedMax_ValueChanged(double value)
{
    unsavedChanges();
    emit settingsChanged();
}

void AdvancedSettings::defaultWindowHeight_ValueChanged(int value)
{
    unsavedChanges();
    emit settingsChanged();
}

void AdvancedSettings::defaultWindowWidth_ValueChanged(int value)
{
    unsavedChanges();
    emit settingsChanged();
}

void AdvancedSettings::continuePlaybackBannerTime_ValueChanged(int value)
{
    unsavedChanges();
    emit settingsChanged();
}
