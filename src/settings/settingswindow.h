#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

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

    // Advanced Settings

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

#endif // HSETTINGS_WINDOW_H
