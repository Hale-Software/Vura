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


AdvancedSettings::AdvancedSettings(QWidget *parent) : QWidget(parent), ui(new Ui::AdvancedSettings)
{
    ui->setupUi(this);

    connect(ui->logToFile, &QCheckBox::stateChanged, this, &AdvancedSettings::logToFile_Checked);
    connect(ui->maxLogFiles, &QSpinBox::valueChanged, this, &AdvancedSettings::maxLogFiles_ValueChanged);

    const QSettings settings;
    ui->logToFile->setChecked(settings.value("logToFile", true).toBool());
    ui->maxLogFiles->setValue(settings.value("maxLogFiles", 10).toInt());
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
    }

    qDebug() << "Advanced Settings Unsaved Changes:" << m_unsavedChanges;
    return m_unsavedChanges;
}

void AdvancedSettings::resetToDefaults()
{
    qDebug() << "Resetting Advanced Settings to Defaults";
    ui->logToFile->setChecked(true);
    ui->maxLogFiles->setValue(10);
    m_unsavedChanges = true;
}

void AdvancedSettings::saveSettings()
{
    qDebug() << "Saving Advanced Settings";
    QSettings settings;
    settings.setValue("logToFile", ui->logToFile->isChecked());
    settings.setValue("maxLogFiles", ui->maxLogFiles->value());
    m_unsavedChanges = false;
}

void AdvancedSettings::logToFile_Checked(const int state)
{
    unsavedChanges();
    emit settingsChanged();
}

void AdvancedSettings::maxLogFiles_ValueChanged(const int value)
{
    unsavedChanges();
    emit settingsChanged();
}
