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

#include <QWidget>

class QSettings;
class QComboBox;
class QCheckBox;
class QPushButton;
class QMessageBox;
class QDebug;


QT_BEGIN_NAMESPACE
namespace Ui {
class GeneralSettings;
}
QT_END_NAMESPACE

class GeneralSettings : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralSettings(QWidget *parent = nullptr);
    ~GeneralSettings() override;

    bool unsavedChanges();

signals:
    void settingsChanged();
    void requiresRestart();

public slots:
    void resetToDefaults();
    void saveSettings();

private slots:
    void language_Changed(int index);
    void systemTrayIcon_Checked(int state);
    void updateBranch_Changed(int index);
    void checkForUpdates_Clicked();
    void enableAutomaticUpdates_Checked(int state);

private:
    Ui::GeneralSettings *ui;
    bool m_unsavedChanges = false;

};
