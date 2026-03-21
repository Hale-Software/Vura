/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

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
#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QListWidgetItem>
#include <QMap>
#include <QKeySequence>
#include <QKeySequenceEdit>
#include <QToolButton>
#include <QMessageBox>
#include <QStandardPaths>

#include "hotkeywidget.h"
#include "../constants.h"

namespace Ui {
class SettingsWindow;
}


class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

public slots:
    void pageSelection_Changed();
    void resetButton_Clicked();
    void okButton_Clicked();
    void cancelButton_Clicked();
    void applyButton_Clicked();

    // General Settings
    void language_Changed(int index);
    void rememberWindowSize_Checked(int state);
    void maxLogs_Changed(int i);
    void logToFile_Checked(int state);
    void hashFile_Checked(int state);
    void markerFile_TextChanged(const QString &text);
    void markerFileBrowseButton_Clicked();
    void showStatusBarOnStart_Checked(int state);
    void showPlaylistOnStart_Checked(int state);
    void showVideoControlsOnStart_Checked(int state);
    void maxRecentFiles_Changed(int i);
    void hideCursorWhenPlaying_Checked(int state);
    void hideCursorTime_Changed(double value);

    void updateChannel_Changed(int index);
    void automaticUpdates_Checked(int state);

    void systemTray_Checked(int state);
    void mediaChangeNotification_Changed(int i);
    void showVideoControlsWhenFullscreen_Checked(int state);
    void startInMinimalViewMode_Checked(int state);
    void pausePlaybackWhenMinimized_Checked(int state);
    void allowOnlyOneInstance_Checked(int state);
    void oneInstanceFromFileManager_Checked(int state);
    void continuePlayback_Changed(int i);
    void pauseOnLastFrameOfVideo_Checked(int state);

    void playbackSpeedAdjustment_TextChanged(const QString &text);
    void playbackSpeedAdjustmentFine_TextChanged(const QString &text);
    void volumeStep_Changed(double value);
    void frameWalk_TextChanged(const QString &text);
    void smallJump_TextChanged(const QString &text);
    void mediumJump_TextChanged(const QString &text);
    void largeJump_TextChanged(const QString &text);
    void extraLargeJump_TextChanged(const QString &text);

    // Appearance Settings
    void theme_Changed(int index);
    void fontScaleTextBox_TextChanged(const QString &text);
    void fontScale_ValueChanged(int value);

    // Audio Settings

    // Video Settings

    // Hotkey Settings
    void filterTextBox_TextChanged(const QString &text);
    void hotkeyFilterTextBox_KeySequenceChanged(const QKeySequence &keySequence);
    void hotkeyFilterClearButton_Clicked();
    void hotkey_Changed(int id, QString action, QString oldHotkey, QString newHotkey);
    void setOverrideWindowsHotkeys_Checked(bool state);

    // Advanced Settings
    void stashServer_TextChanged(const QString &text);

signals:
    void updateSettings();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::SettingsWindow *ui;
    HotkeyWidget *m_hotkeyWidget[250];
    int m_numHotkeys = 0;
    QMap<QString,QString> m_changedHotkeys;
    bool m_settingsLoaded = false;
    bool m_unsavedChanges = false;
    bool m_cancelChanges = false;

    void loadSettings();
    void settingsChanged();
    void saveSettings();
    void loadHotkeys();

    // General Settings
    QString m_language;
    bool m_rememberWindowSize;
    bool m_showStatusBarOnStart;
    bool m_showPlaylistOnStart;
    bool m_showVideoControlsOnStart;
    int m_maxLogs;
    bool m_logToFile;
    bool m_hashFile;
    QString m_markerFile;
    int m_frameWalkTime;
    int m_smallJump;
    int m_mediumJump;
    int m_largeJump;
    int m_extraLargeJump;
    double m_playbackSpeedAdjustment;
    double m_playbackSpeedAdjustmentFine;
    double m_volumeStep;
    int m_maxRecentFiles;
    QString m_updateChannel;
    bool m_autoUpdate;
    bool m_hideCursorWhenPlaying;
    double m_hideCursorTime;
    QString m_stashServerUrl;
    bool m_systemTray;
    int m_mediaChangeNotification;
    bool m_showVideoControlsWhenFullscreen;
    bool m_startInMinimalViewMode;
    bool m_pausePlaybackWhenMinimized;
    bool m_allowOnlyOneInstance;
    bool m_oneInstanceFromFileManager;
    int m_continuePlayback;
    bool m_pauseOnLastFrameOfVideo;
    bool m_setOverrideWindowsHotkeys;

    // Appearance Settings
    QString m_theme;
    int m_fontSize;

    // Audio Settings

    // Video Settings

    // Hotkey Settings
    QMap<QString,QString> defaultHotkeys();
    QMap<QString,QString> m_hotkeys;
    //HotkeyWidget *hotkeyWidget;
    QListWidgetItem *m_hotkeyItem;

};
