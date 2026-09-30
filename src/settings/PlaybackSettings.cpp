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

#include "PlaybackSettings.h"
#include "ui_PlaybackSettings.h"

#include <QSettings>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDebug>


PlaybackSettings::PlaybackSettings(QWidget *parent) : QWidget(parent), ui(new Ui::PlaybackSettings)
{
    ui->setupUi(this);

    const QSettings settings;
    ui->playbackSpeedAdjustment->setValue(settings.value("playbackSpeedAdjustment", 0.5).toDouble());
    ui->playbackSpeedAdjustmentFine->setValue(settings.value("playbackSpeedAdjustmentFine", 0.25).toDouble());
    ui->volumeStep->setValue(settings.value("volumeStep", 0.10).toDouble());
    ui->extraLargeJump->setValue(settings.value("extraLargeJump", 90).toInt());
    ui->largeJump->setValue(settings.value("largeJump", 30).toInt());
    ui->mediumJump->setValue(settings.value("mediumJump", 15).toInt());
    ui->smallJump->setValue(settings.value("smallJump", 5).toInt());
    ui->extraSmallJump->setValue(settings.value("extraSmallJump", 1).toInt());
    ui->jumpToEndPercentage->setValue(settings.value("jumpToEndPercentage", 90).toInt());

    connect(ui->playbackSpeedAdjustment, &QDoubleSpinBox::valueChanged, this, &PlaybackSettings::playbackSpeedAdjustment_ValueChanged);
    connect(ui->playbackSpeedAdjustmentFine, &QDoubleSpinBox::valueChanged, this, &PlaybackSettings::playbackSpeedAdjustmentFine_ValueChanged);
    connect(ui->volumeStep, &QDoubleSpinBox::valueChanged, this, &PlaybackSettings::volumeStep_ValueChanged);
    connect(ui->extraLargeJump, &QSpinBox::valueChanged, this, &PlaybackSettings::extraLargeJump_ValueChanged);
    connect(ui->largeJump, &QSpinBox::valueChanged, this, &PlaybackSettings::largeJump_ValueChanged);
    connect(ui->mediumJump, &QSpinBox::valueChanged, this, &PlaybackSettings::mediumJump_ValueChanged);
    connect(ui->smallJump, &QSpinBox::valueChanged, this, &PlaybackSettings::smallJump_ValueChanged);
    connect(ui->extraSmallJump, &QSpinBox::valueChanged, this, &PlaybackSettings::extraSmallJump_ValueChanged);
    connect(ui->jumpToEndPercentage, &QSpinBox::valueChanged, this, &PlaybackSettings::jumpToEndPercentage_ValueChanged);
}

PlaybackSettings::~PlaybackSettings()
{
    delete ui;
}

bool PlaybackSettings::unsavedChanges()
{
    m_unsavedChanges = false;
    const QSettings settings;
    if (settings.value("playbackSpeedAdjustment", 0.5).toDouble() != ui->playbackSpeedAdjustment->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("playbackSpeedAdjustmentFine", 0.25).toDouble() != ui->playbackSpeedAdjustmentFine->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("volumeStep", 0.10).toDouble() != ui->volumeStep->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("extraLargeJump", 90).toInt() != ui->extraLargeJump->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("largeJump", 30).toInt() != ui->largeJump->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("mediumJump", 15).toInt() != ui->mediumJump->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("smallJump", 5).toInt() != ui->smallJump->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("extraSmallJump", 1).toInt() != ui->extraSmallJump->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("jumpToEndPercentage", 90).toInt() != ui->jumpToEndPercentage->value()) {
        m_unsavedChanges = true;
    }
    return m_unsavedChanges;
}

void PlaybackSettings::resetToDefaults()
{
    ui->playbackSpeedAdjustment->setValue(0.5);
    ui->playbackSpeedAdjustmentFine->setValue(0.25);
    ui->volumeStep->setValue(0.10);
    ui->extraLargeJump->setValue(90);
    ui->largeJump->setValue(30);
    ui->mediumJump->setValue(15);
    ui->smallJump->setValue(5);
    ui->extraSmallJump->setValue(1);
    ui->jumpToEndPercentage->setValue(90);
    m_unsavedChanges = true;
}

void PlaybackSettings::saveSettings()
{
    QSettings settings;
    settings.setValue("playbackSpeedAdjustment", ui->playbackSpeedAdjustment->value());
    settings.setValue("playbackSpeedAdjustmentFine", ui->playbackSpeedAdjustmentFine->value());
    settings.setValue("volumeStep", ui->volumeStep->value());
    settings.setValue("extraLargeJump", ui->extraLargeJump->value());
    settings.setValue("largeJump", ui->largeJump->value());
    settings.setValue("mediumJump", ui->mediumJump->value());
    settings.setValue("smallJump", ui->smallJump->value());
    settings.setValue("extraSmallJump", ui->extraSmallJump->value());
    settings.setValue("jumpToEndPercentage", ui->jumpToEndPercentage->value());
    m_unsavedChanges = false;
}

void PlaybackSettings::playbackSpeedAdjustment_ValueChanged(double value)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlaybackSettings::playbackSpeedAdjustmentFine_ValueChanged(double value)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlaybackSettings::volumeStep_ValueChanged(double value)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlaybackSettings::extraLargeJump_ValueChanged(int value)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlaybackSettings::largeJump_ValueChanged(int value)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlaybackSettings::mediumJump_ValueChanged(int value)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlaybackSettings::smallJump_ValueChanged(int value)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlaybackSettings::extraSmallJump_ValueChanged(int value)
{
    unsavedChanges();
    emit settingsChanged();
}

void PlaybackSettings::jumpToEndPercentage_ValueChanged(int value)
{
    unsavedChanges();
    emit settingsChanged();
}
