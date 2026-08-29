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

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include <QSettings>
#include <QMessageBox>
#include <QStackedWidget>
#include <QDebug>


SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->navigationList->setStyleSheet("QListWidget { background: transparent; }");
    ui->navigationList->viewport()->setAutoFillBackground(false);
    ui->navigationList->setCurrentRow(0);

    connect(ui->navigationList, &QListWidget::itemSelectionChanged, this, &SettingsDialog::pageSelection_Changed);
    connect(ui->resetButton, &QPushButton::clicked, this, &SettingsDialog::resetToDefaults_Clicked);
    connect(ui->applyButton, &QPushButton::clicked, this, &SettingsDialog::applyChanges_Clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &SettingsDialog::cancel_Clicked);

    m_advancedSettings = new AdvancedSettings(this);
    m_associationsSettings = new AssociationsSettings(this);
    m_generalSettings = new GeneralSettings(this);
    m_hotkeysSettings = new HotkeysSettings(this);
    m_interfaceSettings = new InterfaceSettings(this);
    m_playbackSettings = new PlaybackSettings(this);
    m_playerSettings = new PlayerSettings(this);
    m_playlistSettings = new PlaylistSettings(this);

    ui->settingsViewArea->addWidget(m_generalSettings);
    ui->settingsViewArea->addWidget(m_playerSettings);
    ui->settingsViewArea->addWidget(m_playbackSettings);
    ui->settingsViewArea->addWidget(m_interfaceSettings);
    ui->settingsViewArea->addWidget(m_hotkeysSettings);
    ui->settingsViewArea->addWidget(m_playlistSettings);
    ui->settingsViewArea->addWidget(m_associationsSettings);
    ui->settingsViewArea->addWidget(m_advancedSettings);

    connect(m_advancedSettings, &AdvancedSettings::settingsChanged, this, &SettingsDialog::settingsChanged_Slot);
    connect(m_associationsSettings, &AssociationsSettings::settingsChanged, this, &SettingsDialog::settingsChanged_Slot);
    connect(m_generalSettings, &GeneralSettings::settingsChanged, this, &SettingsDialog::settingsChanged_Slot);
    connect(m_hotkeysSettings, &HotkeysSettings::settingsChanged, this, &SettingsDialog::settingsChanged_Slot);
    connect(m_interfaceSettings, &InterfaceSettings::settingsChanged, this, &SettingsDialog::settingsChanged_Slot);
    connect(m_playbackSettings, &PlaybackSettings::settingsChanged, this, &SettingsDialog::settingsChanged_Slot);
    connect(m_playerSettings, &PlayerSettings::settingsChanged, this, &SettingsDialog::settingsChanged_Slot);
    connect(m_playlistSettings, &PlaylistSettings::settingsChanged, this, &SettingsDialog::settingsChanged_Slot);

    connect(m_generalSettings, &GeneralSettings::updateRequested, this, &SettingsDialog::updateRequested);

    ui->settingsViewArea->setCurrentIndex(0);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::closeEvent(QCloseEvent *event)
{
    if (unsavedChanges()) {
        const QMessageBox::StandardButton confirmationBox = QMessageBox::question(this,
            tr("Discard Changes"),
            tr("Are you sure you want to discard changes?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (confirmationBox == QMessageBox::Yes) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

void SettingsDialog::settingsChanged_Slot() const
{
    if (unsavedChanges()) {
        ui->applyButton->setEnabled(true);
    } else {
        ui->applyButton->setEnabled(false);
    }
}

void SettingsDialog::pageSelection_Changed() const
{
    ui->settingsViewArea->setCurrentIndex(ui->navigationList->currentRow());
}

void SettingsDialog::resetToDefaults_Clicked()
{
    const QMessageBox::StandardButton confirmationBox = QMessageBox::question(this, tr("Reset Settings"), tr("Are you sure you want to reset settings to default?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (confirmationBox == QMessageBox::Yes) {
        m_advancedSettings->resetToDefaults();
        m_associationsSettings->resetToDefaults();
        m_generalSettings->resetToDefaults();
        m_hotkeysSettings->resetToDefaults();
        m_interfaceSettings->resetToDefaults();
        m_playbackSettings->resetToDefaults();
        m_playerSettings->resetToDefaults();
        m_playlistSettings->resetToDefaults();
    }
}

void SettingsDialog::applyChanges_Clicked()
{
    m_advancedSettings->saveSettings();
    m_associationsSettings->saveSettings();
    m_generalSettings->saveSettings();
    m_hotkeysSettings->saveSettings();
    m_interfaceSettings->saveSettings();
    m_playbackSettings->saveSettings();
    m_playerSettings->saveSettings();
    m_playlistSettings->saveSettings();

    ui->applyButton->setEnabled(false);
    emit settingsChanged();
}

void SettingsDialog::cancel_Clicked()
{
    this->close();
}

bool SettingsDialog::unsavedChanges() const
{
    if (m_advancedSettings->unsavedChanges()
        || m_associationsSettings->unsavedChanges()
        || m_generalSettings->unsavedChanges()
        || m_hotkeysSettings->unsavedChanges()
        || m_interfaceSettings->unsavedChanges()
        || m_playbackSettings->unsavedChanges()
        || m_playerSettings->unsavedChanges()
        || m_playlistSettings->unsavedChanges()) {
        return true;
    }

    return false;
}
