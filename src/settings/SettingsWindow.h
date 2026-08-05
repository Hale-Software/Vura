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
#include <QListWidget>
#include <QListWidgetItem>
#include <QIcon>
#include <QSize>
#include <QPushButton>
#include <QStackedWidget>
#include <QLineEdit>
#include <QTableWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QKeySequenceEdit>
#include <QKeySequence>
#include <QMessageBox>
#include <QHeaderView>
#include <QAction>
#include <QMap>
#include <QDebug>

#include "HotkeyDelegate.h"


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

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void pageSelection_Changed();
    void resetButton_Clicked();
    void applyButton_Clicked();
    void cancelButton_Clicked();

    // General Settings
    void language_Changed(int index);
    void systemTray_Checked(int state);
    void theme_Changed(int index);
    void rememberWindowSize_Checked(int state);
    void showPlaylistOnStart_Checked(int state);
    void showVideoControlsOnStart_Checked(int state);
    void showStatusBarOnStart_Checked(int state);
    void updateBranch_Changed(int index);
    void checkForUpdates_Clicked();
    void enableAutomaticUpdates_Checked(int state);

    // Player Settings
    void allowOnlyOneInstance_Checked(int state);
    void showMediaChangeNotification_Changed(int index);
    void continuePlayback_Changed(int index);
    void pausePlaybackWhenMinimized_Checked(int state);
    void pauseOnLastFrameOfVideo_Checked(int state);
    void useHardwareAcceleration_Checked(int state);
    void defaultVideoContrast_ValueChanged(int value);
    void defaultAspectRatio_Changed(int index);

    // Playback Settings
    void playbackSpeedAdjustment_Changed(double value);
    void playbackSpeedAdjustmentFine_Changed(double value);
    void volumeStep_Changed(double value);
    void extraLargeJump_Changed(double value);
    void largeJump_Changed(double value);
    void mediumJump_Changed(double value);
    void smallJump_Changed(double value);
    void extraSmallJump_Changed(double value);
    void jumpToEndPercentage_Changed(double value);

    // Interface Settings
    void autohideSliderWhenPlaying_Checked(int state);
    void unhideSliderOnHotkey_Checked(int state);
    void autohideSliderTime_ValueChanged(double value);
    void sliderHeight_Changed(int value);
    void videoMarkerHeight_Changed(int value);
    void videoMarkerWidth_Changed(int value);

    // Hotkey Settings
    void filter_TextChanged(QString value);
    void filterHotkey_KeySequenceChanged(const QKeySequence &keySequence);
    void resetHotkeys_Clicked();

    // Playlist Settings

    // File Associations Settings

    // Advanced Settings
    void logToFile_Checked(int state);
    void maxLogs_ValueChanged(int value);

private:
    Ui::SettingsWindow *ui;

    QMap<QString, QAction*> m_hotkeys;
    bool m_unsavedChanges = false;
    bool m_cancelChanges = false;

    void loadSettings();

};
