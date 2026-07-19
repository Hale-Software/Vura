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
#include <QListWidget>
#include <QListWidgetItem>
#include <QIcon>
#include <QSize>
#include <QPushButton>
#include <QStackedWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class SettingsWindow;
}
QT_END_NAMESPACE


class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow() override;

private slots:
    void pageSelection_Changed();
    void resetButton_Clicked();
    void applyButton_Clicked();
    void cancelButton_Clicked();

    // General Settings
    void language_Changed(int index);
    void theme_Changed(int index);
    void showPlaylistOnStart_Checked(int state);
    void useHardwareAcceleration_Checked(int state);
    void defaultVideoContrast_ValueChanged(int value);
    void defaultAspectRatio_Changed(int index);
    void checkForUpdates_Clicked();
    void enableAutomaticUpdates_Checked(int state);
    void updateChannel_Changed(int index);

    // Interface Settings
    void autohideSliderWhenPlaying_Checked(int state);
    void autohideSliderTime_ValueChanged(double value);
    void unhideSliderOnHotkey_Checked(int state);

    // Player Settings
    void allowOnlyOneInstance_Checked(int state);
    void showMediaChangeNotification_Changed(int index);
    void continuePlayback_Changed(int index);

private:
    Ui::SettingsWindow *ui;

};
