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

    // General Settings
    void language_CurrentIndexChanged(int index);
    void rememberWindowSize_Clicked(int state);
    void maxLogs_Changed(int i);
    void logToFile_Clicked(int state);
    void hashFile_Clicked(int state);
    void markerFileTextBox_TextChanged(const QString &text);
    void markerFileBrowseButton_Clicked();
    void nextFrameTextBox_TextChanged(const QString &text);
    void previousFrameTextBox_TextChanged(const QString &text);
    void smallJumpTextBox_TextChanged(const QString &text);
    void mediumJumpTextBox_TextChanged(const QString &text);
    void largeJumpTextBox_TextChanged(const QString &text);
    void extraLargeJumpTextBox_TextChanged(const QString &text);

    // Appearance Settings
    void theme_CurrentIndexChanged(int index);
    void fontScaleTextBox_TextChanged(const QString &text);
    void fontScale_ValueChanged(int value);

    // Audio Settings

    // Video Settings

    // Hotkey Settings
    void filterTextBox_TextChanged(const QString &text);
    void hotkeyFilterTextBox_KeySequenceChanged(const QKeySequence &keySequence);

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

    void loadSettings();
    void settingsChanged();
    void saveSettings();
    void loadHotkeys();

    // General Settings
    QString m_language;
    bool m_rememberWindowSize;
    int m_maxLogs;
    bool m_logToFile;
    bool m_hashFile;
    QString m_markerFile;
    int m_nextFrameTime;
    int m_previousFrameTime;
    int m_smallJump;
    int m_mediumJump;
    int m_largeJump;
    int m_extraLargeJump;

    // Appearance Settings
    QString m_theme;
    int m_fontSize;

    // Audio Settings

    // Video Settings

    // Hotkey Settings
    QMap<QString,QString> defaultHotkeys();
    HotkeyWidget *hotkeyWidget;
    QListWidgetItem *m_hotkeyItem;

};

#endif // HSETTINGS_WINDOW_H
