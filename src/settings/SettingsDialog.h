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

#pragma once

#include <QDialog>
#include <QSettings>
#include <QMessageBox>
#include <QStackedWidget>
#include <QCloseEvent>
#include <QDebug>

#include "AdvancedSettings.h"
#include "AssociationsSettings.h"
#include "GeneralSettings.h"
#include "HotkeysSettings.h"
#include "InterfaceSettings.h"
#include "PlaybackSettings.h"
#include "PlayerSettings.h"
#include "PlaylistSettings.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class SettingsDialog;
}
QT_END_NAMESPACE


class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void settingsChanged();
    void requiresRestart();

private slots:
    void pageSelection_Changed();
    void resetToDefaults_Clicked();
    void applyChanges_Clicked();
    void cancel_Clicked();

private:
    Ui::SettingsDialog *ui;

    AdvancedSettings *m_advancedSettings = nullptr;
    AssociationsSettings *m_associationsSettings = nullptr;
    GeneralSettings *m_generalSettings = nullptr;
    HotkeysSettings *m_hotkeysSettings = nullptr;
    InterfaceSettings *m_interfaceSettings = nullptr;
    PlaybackSettings *m_playbackSettings = nullptr;
    PlayerSettings *m_playerSettings = nullptr;
    PlaylistSettings *m_playlistSettings = nullptr;

};
