#include "settingswindow.h"
#include "../forms/ui_settingswindow.h"

#include <config.h>

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    ui->settingsPageDisplay->setCurrentIndex(0);

    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &SettingsWindow::pageSelection_Changed);
    connect(ui->okButton, &QPushButton::clicked, this, &SettingsWindow::okButton_Clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &SettingsWindow::cancelButton_Clicked);
    connect(ui->applyButton, &QPushButton::clicked, this, &SettingsWindow::applyButton_Clicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &SettingsWindow::resetButton_Clicked);

    connect(ui->language, &QComboBox::currentIndexChanged, this, &SettingsWindow::language_Changed);
    connect(ui->rememberWindowSize, &QCheckBox::checkStateChanged, this, &SettingsWindow::rememberWindowSize_Checked);
    connect(ui->maxLogs, &QSpinBox::valueChanged, this, &SettingsWindow::maxLogs_Changed);
    connect(ui->logToFile, &QCheckBox::checkStateChanged, this, &SettingsWindow::logToFile_Checked);
    connect(ui->hashFile, &QCheckBox::checkStateChanged, this, &SettingsWindow::hashFile_Checked);
    connect(ui->markerFileBrowseButton, &QPushButton::clicked, this, &SettingsWindow::markerFileBrowseButton_Clicked);
    connect(ui->markerFile, &QLineEdit::textChanged, this, &SettingsWindow::markerFile_TextChanged);
    connect(ui->showStatusBarOnStart, &QCheckBox::checkStateChanged, this, &SettingsWindow::showStatusBarOnStart_Checked);
    connect(ui->showPlaylistOnStart, &QCheckBox::checkStateChanged, this, &SettingsWindow::showPlaylistOnStart_Checked);
    connect(ui->showVideoControlsOnStart, &QCheckBox::checkStateChanged, this, &SettingsWindow::showVideoControlsOnStart_Checked);
    connect(ui->maxRecentFiles, &QSpinBox::valueChanged, this, &SettingsWindow::maxRecentFiles_Changed);
    connect(ui->updateChannel, &QComboBox::currentIndexChanged, this, &SettingsWindow::updateChannel_Changed);
    connect(ui->autoUpdate, &QCheckBox::checkStateChanged, this, &SettingsWindow::automaticUpdates_Checked);
    connect(ui->playbackSpeedAdjustment, &QLineEdit::textChanged, this, &SettingsWindow::playbackSpeedAdjustment_TextChanged);
    connect(ui->playbackSpeedAdjustmentFine, &QLineEdit::textChanged, this, &SettingsWindow::playbackSpeedAdjustmentFine_TextChanged);
    connect(ui->volumeStep, &QDoubleSpinBox::valueChanged, this, &SettingsWindow::volumeStep_Changed);
    connect(ui->frameWalk, &QLineEdit::textChanged, this, &SettingsWindow::frameWalk_TextChanged);
    connect(ui->smallJump, &QLineEdit::textChanged, this, &SettingsWindow::smallJump_TextChanged);
    connect(ui->mediumJump, &QLineEdit::textChanged, this, &SettingsWindow::mediumJump_TextChanged);
    connect(ui->largeJump, &QLineEdit::textChanged, this, &SettingsWindow::largeJump_TextChanged);
    connect(ui->extraLargeJump, &QLineEdit::textChanged, this, &SettingsWindow::extraLargeJump_TextChanged);

    connect(ui->theme, &QComboBox::currentIndexChanged, this, &SettingsWindow::theme_Changed);
    connect(ui->appearanceFontScaleText, &QLineEdit::textChanged, this, &SettingsWindow::fontScaleTextBox_TextChanged);

    connect(ui->filterTextBox, &QLineEdit::textChanged, this, &SettingsWindow::filterTextBox_TextChanged);
    connect(ui->hotkeyFilterTextBox, &QKeySequenceEdit::keySequenceChanged, this, &SettingsWindow::hotkeyFilterTextBox_KeySequenceChanged);
    connect(ui->hotkeyFilterClearButton, &QPushButton::clicked, this, &SettingsWindow::hotkeyFilterClearButton_Clicked);

    loadSettings();
    loadHotkeys();
    m_settingsLoaded = true;
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::pageSelection_Changed()
{
    ui->settingsPageDisplay->setCurrentIndex(ui->listWidget->currentRow());
}

void SettingsWindow::loadSettings()
{
    QSettings settings;
    QString defaultMarkerFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/markers.json";
    if (VURA_BUILD_TYPE == "Debug") {
        defaultMarkerFile = "C:/Users/halea/vura-debug/markers.json";
    }

    // Load Settings
    m_language = settings.value("language", "en-US").toString();
    m_rememberWindowSize = settings.value("rememberWindowSize", true).toBool();
    m_showStatusBarOnStart = settings.value("showStatusBarOnStart", false).toBool();
    m_showPlaylistOnStart = settings.value("showPlaylistOnStart", false).toBool();
    m_showVideoControlsOnStart = settings.value("showVideoControlsOnStart", false).toBool();
    m_maxLogs = settings.value("maxLogFiles", 10).toInt();
    m_logToFile = settings.value("logToFile", true).toBool();
    m_hashFile = settings.value("hashFile", false).toBool();
    m_markerFile = settings.value("markerFile", defaultMarkerFile).toString();
    m_frameWalkTime = settings.value("frameWalkTime", 1000).toInt();
    m_smallJump = settings.value("jumpSmall", 5).toInt();
    m_mediumJump = settings.value("jumpMedium", 15).toInt();
    m_largeJump = settings.value("jumpLarge", 30).toInt();
    m_extraLargeJump = settings.value("jumpExtraLarge", 90).toInt();
    m_playbackSpeedAdjustment = settings.value("playbackSpeedAdjustment", 0.5).toDouble();
    m_playbackSpeedAdjustmentFine = settings.value("playbackSpeedFineAdjustment", 0.25).toDouble();
    m_volumeStep = settings.value("volumeStep", 0.10).toDouble();
    m_maxRecentFiles = settings.value("maxRecentFiles", 9).toInt();
    m_updateChannel = settings.value("updateChannel", "stable").toString();
    m_autoUpdate = settings.value("autoUpdate", true).toBool();
    m_hideCursorWhenPlaying = settings.value("hideCursorWhenPlaying", true).toBool();
    m_hideCursorTime = settings.value("hideCursorTime", 2000).toInt() * 1000;

    m_theme = settings.value("theme", "System").toString();
    m_fontSize = settings.value("fontSize", 10).toInt();

    // Populate UI Values
    if (m_language == "en-US") {
        ui->language->setCurrentIndex(0);
    }

    if (m_updateChannel == "stable") {
        ui->updateChannel->setCurrentIndex(0);
    } else {
        ui->updateChannel->setCurrentIndex(1);
    }

    ui->rememberWindowSize->setChecked(m_rememberWindowSize);
    ui->showStatusBarOnStart->setChecked(m_showStatusBarOnStart);
    ui->showPlaylistOnStart->setChecked(m_showPlaylistOnStart);
    ui->showVideoControlsOnStart->setChecked(m_showVideoControlsOnStart);
    ui->maxRecentFiles->setValue(m_maxRecentFiles);
    ui->maxLogs->setValue(m_maxLogs);
    ui->logToFile->setChecked(m_logToFile);
    ui->hashFile->setChecked(m_hashFile);
    ui->markerFile->setText(m_markerFile);
    ui->autoUpdate->setChecked(m_autoUpdate);
    ui->frameWalk->setText(QString::number(m_frameWalkTime));
    ui->smallJump->setText(QString::number(m_smallJump));
    ui->mediumJump->setText(QString::number(m_mediumJump));
    ui->largeJump->setText(QString::number(m_largeJump));
    ui->extraLargeJump->setText(QString::number(m_extraLargeJump));
    ui->playbackSpeedAdjustment->setText(QString::number(m_playbackSpeedAdjustment));
    ui->playbackSpeedAdjustmentFine->setText(QString::number(m_playbackSpeedAdjustmentFine));
    ui->volumeStep->setValue(m_volumeStep);
    ui->hideCursorWhenPlaying->setChecked(m_hideCursorWhenPlaying);
    ui->hideCursorTime->setValue(m_hideCursorTime);

    if (m_theme == "System") {
        ui->theme->setCurrentIndex(0);
    } else if (m_theme == "Light") {
        ui->theme->setCurrentIndex(1);
    } else if (m_theme == "Dark") {
        ui->theme->setCurrentIndex(2);
    }

    ui->appearanceFontScaleText->setText(QString::number(m_fontSize));
}

void SettingsWindow::loadHotkeys()
{

    QSettings settings;
    settings.beginGroup("Hotkeys");
    const QStringList childKeys = settings.childKeys();
    foreach (const QString &childKey, childKeys)
    {
        m_hotkeys[childKey] = settings.value(childKey).toString();
    }
    settings.endGroup();

    QMapIterator<QString, QString> i(m_hotkeys);
    int x = 0;
    while (i.hasNext()) {
        i.next();
        QString defaultKey = defaultHotkeys().value(i.key());
        m_hotkeyWidget[x] = new HotkeyWidget(x, i.key(), defaultKey, i.value(), this);
        connect(m_hotkeyWidget[x], &HotkeyWidget::hotkeyChanged, this, &SettingsWindow::hotkey_Changed);
        ui->verticalLayout_11->addWidget(m_hotkeyWidget[x]);

        ++x;
    }
    m_numHotkeys = x;
}

void SettingsWindow::hotkey_Changed(int id, QString action, QString oldHotkey, QString newHotkey)
{
    if (newHotkey.isEmpty()) {
        m_hotkeyWidget[id]->clearHotkey();
        settingsChanged();
        return;
    }

    for (int i = 0; i < m_numHotkeys; ++i) {
        if (i != id) {
            if (m_hotkeyWidget[i]->getHotkey() == newHotkey) {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this,
                    tr("Hotkey Exists"),
                    tr("The hotkey %1 is being used. Do you want to overwrite it?").arg(newHotkey),
                    QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::Yes) {
                    //m_hotkeyWidget[i]->clearHotkey();
                    m_hotkeyWidget[id]->setHotkey(newHotkey);
                    settingsChanged();
                    return;
                } else {
                    m_hotkeyWidget[id]->setHotkey(oldHotkey);
                    return;
                }
            }
        }
    }
}

QMap<QString, QString> SettingsWindow::defaultHotkeys()
{
    QMap<QString, QString> hotkeyMap;

    // File
    hotkeyMap["&Emergency Collapse"] = "C";
    hotkeyMap["&Exit"] = "Ctrl+Q";
    hotkeyMap["&Open File..."] = "O";
    hotkeyMap["&Open Folder..."] = "Ctrl+F";
    hotkeyMap["&Open Multiple Files..."] = "Alt+O";
    hotkeyMap["&Open Network Stream..."] = "Ctrl+N";
    hotkeyMap["&Save"] = "Ctrl+S";
    hotkeyMap["&Save As..."] = "Ctrl+Shift+S";
    hotkeyMap["&Save Playlist"] = "Alt+S";
    hotkeyMap["&Save A Copy..."] = "Ctrl+Alt+S";
    hotkeyMap["&Close"] = "Ctrl+A";
    hotkeyMap["&Close All"] = "Ctrl+Shift+A";
    hotkeyMap["&Preferences"] = "Ctrl+Shift+P";
    hotkeyMap["&Clear"] = "";


    // Playback
    hotkeyMap["&Next Video"] = "N";
    hotkeyMap["&Next Frame"] = "]";
    hotkeyMap["&Previous Video"] = "P";
    hotkeyMap["&Previous Frame"] = "[";
    hotkeyMap["&Restart Video"] = "R";
    hotkeyMap["&Jump to Specific Time..."] = "Ctrl+T";
    hotkeyMap["&Jump Back Extra Large"] = "Shift+Left";
    hotkeyMap["&Jump Back Large"] = "Q";
    hotkeyMap["&Jump Back Medium"] = "A";
    hotkeyMap["&Jump Back Small"] = "Z";
    hotkeyMap["&Jump Fwd Extra Large"] = "Shift+Right";
    hotkeyMap["&Jump Fwd Large"] = "W";
    hotkeyMap["&Jump Fwd Medium"] = "S";
    hotkeyMap["&Jump Fwd Small"] = "X";
    hotkeyMap["&Faster"] = "Num++";
    hotkeyMap["&Faster (fine)"] = "Ctrl+Num++";
    hotkeyMap["&Normal"] = "=";
    hotkeyMap["&Slower"] = "Num+-";
    hotkeyMap["&Slower (fine)"] = "Ctrl+Num+-";

    // Audio
    hotkeyMap["&Decrease Volume"] = "Down";
    hotkeyMap["&Increase Volume"] = "Up";
    hotkeyMap["&Mute"] = "Ctrl+M";

    // Video
    hotkeyMap["&Fullscreen"] = "F";
    hotkeyMap["&Take Snapshot"] = "";

    // View
    hotkeyMap["&Toggle Playlist"] = "Ctrl+P";
    hotkeyMap["&Toggle Status Bar"] = "Ctrl+B";
    hotkeyMap["&Log Viewer"] = "Ctrl+L";
    hotkeyMap["&Toggle Video Controls"] = "Ctrl+V";
    hotkeyMap["&Media Information"] = "";
    hotkeyMap["&Toggle Tags"] = "";

    // Markers
    hotkeyMap["&Add Cumshot Marker"] = "Ctrl+3";
    hotkeyMap["&Add Cyan Marker"] = "Ctrl+5";
    hotkeyMap["&Add Dialog Marker"] = "Ctrl+4";
    hotkeyMap["&Add Magenta Marker"] = "Ctrl+6";
    hotkeyMap["&Add Marker"] = "M";
    hotkeyMap["&Add Orange Marker"] = "Ctrl+7";
    hotkeyMap["&Add Scene Transition Marker"] = "Ctrl+1";
    hotkeyMap["&Add Strip Marker"] = "Ctrl+2";
    hotkeyMap["&Clear In"] = "Ctrl+Shift+,";
    hotkeyMap["&Clear In and Out"] = "Ctrl+Shift+X";
    hotkeyMap["&Clear Markers"] = "Ctrl+Shift+M";
    hotkeyMap["&Clear Out"] = "Ctrl+Shift+.";
    hotkeyMap["&Clear Selected Marker"] = "Ctrl+C";
    hotkeyMap["&Edit Selected Marker..."] = "Alt+M";
    hotkeyMap["&Go to In"] = "Shift+,";
    hotkeyMap["&Go to Next Marker"] = "B";
    hotkeyMap["&Go to Out"] = "Shift+.";
    hotkeyMap["&Go to Previous Marker"] = "V";
    hotkeyMap["&Mark In"] = ",";
    hotkeyMap["&Mark Out"] = ".";

    // Tools
    hotkeyMap["&Make Subclip"] = "Ctrl+U";
    hotkeyMap["&Stream Video from Stash..."] = "Ctrl+X";
    hotkeyMap["&Test Function"] = "";

    // Subtitle
    hotkeyMap["&Toggle Subtitles"] = "";

    // Help
    hotkeyMap["&Help"] = "F1";
    hotkeyMap["&About"] = "";

    return hotkeyMap;
}

void SettingsWindow::settingsChanged()
{
    if (m_settingsLoaded) {
        ui->applyButton->setEnabled(true);
        m_unsavedChanges = true;
    }
}

void SettingsWindow::saveSettings()
{
    QSettings settings;

    settings.setValue("language", m_language);
    settings.setValue("rememberWindowSize", m_rememberWindowSize);
    settings.setValue("showStatusBarOnStart", m_showStatusBarOnStart);
    settings.setValue("showPlaylistOnStart", m_showPlaylistOnStart);
    settings.setValue("showVideoControlsOnStart", m_showVideoControlsOnStart);
    settings.setValue("maxLogFiles", m_maxLogs);
    settings.setValue("logToFile", m_logToFile);
    settings.setValue("hashFile", m_hashFile);
    settings.setValue("markerFile", m_markerFile);
    settings.setValue("frameWalkTime", m_frameWalkTime);
    settings.setValue("jumpSmall", m_smallJump);
    settings.setValue("jumpMedium", m_mediumJump);
    settings.setValue("jumpLarge", m_largeJump);
    settings.setValue("jumpExtraLarge", m_extraLargeJump);
    settings.setValue("playbackSpeedAdjustment", m_playbackSpeedAdjustment);
    settings.setValue("playbackSpeedAdjustmentFine", m_playbackSpeedAdjustmentFine);
    settings.setValue("volumeStep", m_volumeStep);
    settings.setValue("maxRecentFiles", m_maxRecentFiles);
    settings.setValue("updateChannel", m_updateChannel);
    settings.setValue("autoUpdate", m_autoUpdate);
    settings.setValue("hideCursorWhenPlaying", m_hideCursorWhenPlaying);
    int cursorTime = m_hideCursorTime / 1000;
    settings.setValue("hideCursorTime", cursorTime);

    settings.setValue("theme", m_theme);
    settings.setValue("fontSize", m_fontSize);

    settings.beginGroup("Hotkeys");
    settings.remove("");
    QMapIterator<QString, QString> i(m_hotkeys);
    int x = 0;
    while (i.hasNext()) {
        i.next();
        m_hotkeys.insert(i.key(), m_hotkeyWidget[x]->getHotkey());

        ++x;
    }

    settings.endGroup();
    settings.sync();

    m_unsavedChanges = false;
    ui->applyButton->setEnabled(false);
    emit(updateSettings());
}

void SettingsWindow::resetButton_Clicked()
{
    QMessageBox::StandardButton confirmationBox = QMessageBox::question(
            this,
            tr("Reset Settings"),
            tr("Are you sure you want to reset settings to default?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );

    if (confirmationBox == QMessageBox::Yes) {
        QSettings settings;
        settings.clear();
        settings.sync();
        m_settingsLoaded = false;
        m_unsavedChanges = true;
        m_unsavedChanges = false;
        loadSettings();
    }
}

void SettingsWindow::applyButton_Clicked()
{
    saveSettings();
}

void SettingsWindow::cancelButton_Clicked()
{
    m_cancelChanges = true;
    this->close();
}

void SettingsWindow::okButton_Clicked()
{
    this->close();
}

void SettingsWindow::language_Changed(int index)
{
    QString selectedLanguage = ui->language->itemText(index);
    QString lang;

    if (selectedLanguage == "English") {
        lang = "en-US";
    }

    if (!lang.isEmpty() && lang != m_language) {
        m_language = lang;
        settingsChanged();
    }
}

void SettingsWindow::rememberWindowSize_Checked(int state)
{
    if (state == Qt::Checked) {
        m_rememberWindowSize = true;
    } else {
        m_rememberWindowSize = false;
    }
    settingsChanged();
}

void SettingsWindow::maxLogs_Changed(int i)
{
    m_maxLogs = i;
    settingsChanged();
}

void SettingsWindow::logToFile_Checked(int state)
{
    if (state == Qt::Checked) {
        m_logToFile = true;
    } else {
        m_logToFile = false;
    }
    settingsChanged();
}

void SettingsWindow::hashFile_Checked(int state)
{
    if (state == Qt::Checked) {
        m_hashFile = true;
    } else {
        m_hashFile = false;
    }
    settingsChanged();
}

void SettingsWindow::markerFile_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text != m_markerFile) {
            m_markerFile = text;
            settingsChanged();
        }
    }
}

void SettingsWindow::markerFileBrowseButton_Clicked()
{
    QSettings settings;

    // File filters
    QStringList fileFilters;
    fileFilters << "Video Marker Files (*.vvm)";
    fileFilters << "All Files (*.*)";

    // Create open file dialog.
    QFileDialog fileDialog(this);
    fileDialog.setNameFilters(fileFilters);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Marker File"));
    fileDialog.setDirectory(settings.value("lastFileDirectory", QStandardPaths::AppDataLocation).toString());

    if (fileDialog.exec() == QDialog::Accepted) {
        QStringList selectedFiles = fileDialog.selectedFiles();
        if (!selectedFiles.isEmpty()) {
            m_markerFile = selectedFiles.first();
            ui->markerFile->setText(m_markerFile);
            settingsChanged();

            // Set last file directory where file was opened.
            QString lastFileDirectory = selectedFiles.last();
            settings.setValue("lastFileDirectory", QFileInfo(lastFileDirectory).path());
        }
    }
}

void SettingsWindow::showStatusBarOnStart_Checked(int state)
{
    if (state == Qt::Checked) {
        m_showStatusBarOnStart = true;
    } else {
        m_showStatusBarOnStart = false;
    }
    settingsChanged();
}

void SettingsWindow::showPlaylistOnStart_Checked(int state)
{
    if (state == Qt::Checked) {
        m_showPlaylistOnStart = true;
    } else {
        m_showPlaylistOnStart = false;
    }
    settingsChanged();
}

void SettingsWindow::showVideoControlsOnStart_Checked(int state)
{
    if (state == Qt::Checked) {
        m_showVideoControlsOnStart = true;
    } else {
        m_showVideoControlsOnStart = false;
    }
    settingsChanged();
}

void SettingsWindow::maxRecentFiles_Changed(int i)
{
    m_maxRecentFiles = i;
    settingsChanged();
}

void SettingsWindow::updateChannel_Changed(int index)
{
    if (index == 0) {
        if (m_updateChannel != "stable") {
            m_updateChannel = "stable";
            settingsChanged();
        }
    } else if (index == 1) {
        if (m_updateChannel != "beta") {
            m_updateChannel = "beta";
            settingsChanged();
        }
    }
}

void SettingsWindow::automaticUpdates_Checked(int state)
{
    if (state == Qt::Checked) {
        m_autoUpdate = true;
    } else {
        m_autoUpdate = false;
    }
    settingsChanged();
}

void SettingsWindow::playbackSpeedAdjustment_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_playbackSpeedAdjustment) {
            m_playbackSpeedAdjustment = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::playbackSpeedAdjustmentFine_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_playbackSpeedAdjustmentFine) {
            m_playbackSpeedAdjustmentFine = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::volumeStep_Changed(double value)
{
    m_volumeStep = value;
    settingsChanged();
}

void SettingsWindow::frameWalk_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_frameWalkTime) {
            m_frameWalkTime = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::smallJump_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_smallJump) {
            m_smallJump = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::mediumJump_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_mediumJump) {
            m_mediumJump = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::largeJump_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_largeJump) {
            m_largeJump = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::extraLargeJump_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_extraLargeJump) {
            m_extraLargeJump = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::theme_Changed(int index)
{
    QString selectedTheme = ui->theme->itemText(index);

    if (selectedTheme == "Use System") {
        selectedTheme = "System";
    }

    if (!selectedTheme.isEmpty() && selectedTheme != m_theme) {
        m_theme = selectedTheme;
        settingsChanged();
    }
}

void SettingsWindow::hideCursorWhenPlaying_Checked(int state)
{
    if (state == Qt::Checked) {
        m_hideCursorWhenPlaying = true;
    } else {
        m_hideCursorWhenPlaying = false;
    }
    settingsChanged();
}

void SettingsWindow::hideCursorTime_Changed(double value)
{
    m_hideCursorTime = value;
    settingsChanged();
}

void SettingsWindow::fontScaleTextBox_TextChanged(const QString &text) {}

void SettingsWindow::fontScale_ValueChanged(int value) {}

void SettingsWindow::filterTextBox_TextChanged(const QString &text) {}

void SettingsWindow::hotkeyFilterTextBox_KeySequenceChanged(const QKeySequence &keySequence) {}

void SettingsWindow::hotkeyFilterClearButton_Clicked() {}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
    if (m_cancelChanges)
        event->accept();

    if (m_unsavedChanges) {
        QMessageBox::StandardButton confirmationBox = QMessageBox::question(
            this,
            tr("Unsaved Changes"),
            tr("You have unsaved changes. Save Changes?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );

        if (confirmationBox == QMessageBox::No) {
            event->accept();
        } else if (confirmationBox == QMessageBox::Cancel) {
            event->ignore();
            return;
        } else {
            saveSettings();
            m_unsavedChanges = false;
        }
    }
    event->accept();
}
