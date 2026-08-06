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

#include "InterfaceSettings.h"
#include "ui_InterfaceSettings.h"


InterfaceSettings::InterfaceSettings(QWidget *parent) : QWidget(parent), ui(new Ui::InterfaceSettings)
{
    ui->setupUi(this);

    const QSettings settings;
    ui->currentTheme->setCurrentIndex(settings.value("theme", 0).toInt());
    ui->rememberWindowSize->setChecked(settings.value("rememberWindowSize", false).toBool());
    ui->showPlaylistOnStart->setChecked(settings.value("showPlaylistOnStart", true).toBool());
    ui->showVideoControlsOnStart->setChecked(settings.value("showVideoControlsOnStart", false).toBool());
    ui->showStatusBarOnStart->setChecked(settings.value("showStatusBarOnStart", false).toBool());
    ui->autohideSlider->setChecked(settings.value("autohideSlider", true).toBool());
    ui->unhideSliderOnHotkey->setChecked(settings.value("unhideSliderOnHotkey", true).toBool());
    ui->sliderAutohideTimer->setValue(settings.value("sliderAutohideTime", 5).toInt());
    ui->sliderHeight->setValue(settings.value("sliderHeight", 6).toInt());
    ui->videoMarkerHeight->setValue(settings.value("videoMarkerHeight", 10).toInt());
    ui->videoMarkerWidth->setValue(settings.value("videoMarkerWidth", 2).toInt());
    ui->videoMarkerSideLength->setValue(settings.value("videoMarkerSideLength", 14).toInt());
}

InterfaceSettings::~InterfaceSettings()
{
    delete ui;
}

bool InterfaceSettings::unsavedChanges()
{
    m_unsavedChanges = false;
    const QSettings settings;
    if (settings.value("theme", 0).toInt() != ui->currentTheme->currentIndex()) {
        m_unsavedChanges = true;
    } else if (settings.value("rememberWindowSize", false).toBool() != ui->rememberWindowSize->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("showPlaylistOnStart", true).toBool() != ui->showPlaylistOnStart->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("showVideoControlsOnStart", false).toBool() != ui->showVideoControlsOnStart->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("showStatusBarOnStart", false).toBool() != ui->showStatusBarOnStart->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("autohideSlider", true).toBool() != ui->autohideSlider->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("unhideSliderOnHotkey", true).toBool() != ui->unhideSliderOnHotkey->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("sliderAutohideTime", 5).toInt() != ui->sliderAutohideTimer->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("sliderHeight", 6).toInt() != ui->sliderHeight->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("videoMarkerHeight", 10).toInt() != ui->videoMarkerHeight->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("videoMarkerWidth", 2).toInt() != ui->videoMarkerWidth->value()) {
        m_unsavedChanges = true;
    } else if (settings.value("videoMarkerSideLength", 14).toInt() != ui->videoMarkerSideLength->value()) {
        m_unsavedChanges = true;
    }

    return m_unsavedChanges;
}

void InterfaceSettings::resetToDefaults()
{
    const QSettings settings;
    ui->currentTheme->setCurrentIndex(0);
    ui->rememberWindowSize->setChecked(false);
    ui->showPlaylistOnStart->setChecked(true);
    ui->showVideoControlsOnStart->setChecked(false);
    ui->showStatusBarOnStart->setChecked(false);
    ui->autohideSlider->setChecked(true);
    ui->unhideSliderOnHotkey->setChecked(true);
    ui->sliderAutohideTimer->setValue(5);
    ui->sliderHeight->setValue(6);
    ui->videoMarkerHeight->setValue(10);
    ui->videoMarkerWidth->setValue(2);
    ui->videoMarkerSideLength->setValue(14);
    m_unsavedChanges = true;
}

void InterfaceSettings::saveSettings()
{
    QSettings settings;
    settings.setValue("theme", ui->currentTheme->currentIndex());
    settings.setValue("rememberWindowSize", ui->rememberWindowSize->isChecked());
    settings.setValue("showPlaylistOnStart", ui->showPlaylistOnStart->isChecked());
    settings.setValue("showVideoControlsOnStart", ui->showVideoControlsOnStart->isChecked());
    settings.setValue("showStatusBarOnStart", ui->showStatusBarOnStart->isChecked());
    settings.setValue("autohideSlider", ui->autohideSlider->isChecked());
    settings.setValue("unhideSliderOnHotkey", ui->unhideSliderOnHotkey->isChecked());
    settings.setValue("sliderAutohideTime", ui->sliderAutohideTimer->value());
    settings.setValue("sliderHeight", ui->sliderHeight->value());
    settings.setValue("videoMarkerHeight", ui->videoMarkerHeight->value());
    settings.setValue("videoMarkerWidth", ui->videoMarkerWidth->value());
    settings.setValue("videoMarkerSideLength", ui->videoMarkerSideLength->value());
    m_unsavedChanges = false;
}
