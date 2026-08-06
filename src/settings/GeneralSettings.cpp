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

#include "GeneralSettings.h"
#include "ui_GeneralSettings.h"


GeneralSettings::GeneralSettings(QWidget *parent) : QWidget(parent), ui(new Ui::GeneralSettings)
{
    ui->setupUi(this);

    connect(ui->language, &QComboBox::currentIndexChanged, this, &GeneralSettings::language_Changed);
    connect(ui->systemTray, &QCheckBox::stateChanged, this, &GeneralSettings::systemTrayIcon_Checked);
    connect(ui->updateBranch, &QComboBox::currentIndexChanged, this, &GeneralSettings::updateBranch_Changed);
    connect(ui->checkForUpdate, &QPushButton::clicked, this, &GeneralSettings::checkForUpdates_Clicked);
    connect(ui->automaticUpdates, &QCheckBox::stateChanged, this, &GeneralSettings::enableAutomaticUpdates_Checked);

    const QSettings settings;
    ui->language->setCurrentIndex(settings.value("language", 0).toInt());
    ui->systemTray->setChecked(settings.value("systemTrayIcon", true).toBool());
    ui->updateBranch->setCurrentIndex(settings.value("updateBranch", 0).toInt());
    ui->automaticUpdates->setChecked(settings.value("automaticUpdates", true).toBool());
}

GeneralSettings::~GeneralSettings()
{
    delete ui;
}

bool GeneralSettings::unsavedChanges()
{
    m_unsavedChanges = false;
    const QSettings settings;
    if (settings.value("language", 0).toInt() != ui->language->currentIndex()) {
        m_unsavedChanges = true;
    } else if (settings.value("systemTrayIcon", true).toBool() != ui->systemTray->isChecked()) {
        m_unsavedChanges = true;
    } else if (settings.value("updateBranch", 0).toInt() != ui->updateBranch->currentIndex()) {
        m_unsavedChanges = true;
    }
    return m_unsavedChanges;
}

void GeneralSettings::resetToDefaults()
{
    const QSettings settings;
    ui->language->setCurrentIndex(0);
    ui->systemTray->setChecked(true);
    ui->updateBranch->setCurrentIndex(0);
    m_unsavedChanges = true;
}

void GeneralSettings::saveSettings()
{
    QSettings settings;
    settings.setValue("language", ui->language->currentIndex());
    settings.setValue("systemTrayIcon", ui->systemTray->isChecked());
    settings.setValue("updateBranch", ui->updateBranch->currentIndex());
    m_unsavedChanges = false;
}

void GeneralSettings::language_Changed(int index)
{
    const QSettings settings;
    if (settings.value("language", 0).toInt() != ui->language->currentIndex()) {
        m_unsavedChanges = true;
        emit settingsChanged();
    }
}

void GeneralSettings::systemTrayIcon_Checked(int state)
{
    const QSettings settings;
    if (settings.value("systemTrayIcon", true).toBool() != ui->systemTray->isChecked()) {
        m_unsavedChanges = true;
        emit settingsChanged();
    }
}

void GeneralSettings::updateBranch_Changed(int index)
{
    const QSettings settings;
    if (settings.value("updateBranch", 0).toInt() != ui->updateBranch->currentIndex()) {
        m_unsavedChanges = true;
        emit settingsChanged();
    }
}

void GeneralSettings::checkForUpdates_Clicked()
{
    QMessageBox::information(this, tr("Update Check"), tr("This function is not yet implemented."));
}

void GeneralSettings::enableAutomaticUpdates_Checked(int state)
{
    const QSettings settings;
    if (settings.value("automaticUpdates", true).toBool() != ui->automaticUpdates->isChecked()) {
        m_unsavedChanges = true;
        emit settingsChanged();
    }
}
