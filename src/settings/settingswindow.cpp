#include "settingswindow.h"
#include "../forms/ui_settingswindow.h"

#include <config.h>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    ui->settingsPageDisplay->setCurrentIndex(0);

    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &SettingsWindow::pageSelection_Changed);
    connect(ui->okButton, &QPushButton::clicked, this, &SettingsWindow::okButton_Clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &SettingsWindow::cancelButton_Clicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &SettingsWindow::resetButton_Clicked);

    connect(ui->languageComboBox, &QComboBox::currentIndexChanged, this, &SettingsWindow::language_CurrentIndexChanged);
    connect(ui->rememberWindowSize, &QCheckBox::checkStateChanged, this, &SettingsWindow::rememberWindowSize_Clicked);
    connect(ui->maxLogs, &QSpinBox::valueChanged, this, &SettingsWindow::maxLogs_Changed);
    connect(ui->logToFile, &QCheckBox::checkStateChanged, this, &SettingsWindow::logToFile_Clicked);
    connect(ui->hashFile, &QCheckBox::checkStateChanged, this, &SettingsWindow::hashFile_Clicked);
    connect(ui->markerFileBrowseButton, &QPushButton::clicked, this, &SettingsWindow::markerFileBrowseButton_Clicked);
    connect(ui->markerFileTextBox, &QLineEdit::textChanged, this, &SettingsWindow::markerFileTextBox_TextChanged);
    connect(ui->nextFrameTextBox, &QLineEdit::textChanged, this, &SettingsWindow::nextFrameTextBox_TextChanged);
    connect(ui->previousFrameTextBox, &QLineEdit::textChanged, this, &SettingsWindow::previousFrameTextBox_TextChanged);
    connect(ui->smallJumpTextBox, &QLineEdit::textChanged, this, &SettingsWindow::smallJumpTextBox_TextChanged);
    connect(ui->mediumJumpTextBox, &QLineEdit::textChanged, this, &SettingsWindow::mediumJumpTextBox_TextChanged);
    connect(ui->largeJumpTextBox, &QLineEdit::textChanged, this, &SettingsWindow::largeJumpTextBox_TextChanged);
    connect(ui->extraLargeJumpTextBox, &QLineEdit::textChanged, this, &SettingsWindow::extraLargeJumpTextBox_TextChanged);

    connect(ui->themeComboBox, &QComboBox::currentIndexChanged, this, &SettingsWindow::theme_CurrentIndexChanged);
    connect(ui->appearanceFontScaleText, &QLineEdit::textChanged, this, &SettingsWindow::fontScaleTextBox_TextChanged);

    connect(ui->filterTextBox, &QLineEdit::textChanged, this, &SettingsWindow::filterTextBox_TextChanged);
    connect(ui->hotkeyFilterTextBox, &QKeySequenceEdit::keySequenceChanged, this, &SettingsWindow::hotkeyFilterTextBox_KeySequenceChanged);

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
    m_maxLogs = settings.value("maxLogFiles", 10).toInt();
    m_logToFile = settings.value("logToFile", true).toBool();
    m_hashFile = settings.value("hashFile", false).toBool();
    m_markerFile = settings.value("markerFile", defaultMarkerFile).toString();
    m_nextFrameTime = settings.value("nextFrameTime", 1000).toInt();
    m_previousFrameTime = settings.value("previousFrameTime", 1000).toInt();
    m_smallJump = settings.value("jumpSmall", 5).toInt();
    m_mediumJump = settings.value("jumpMedium", 15).toInt();
    m_largeJump = settings.value("jumpLarge", 45).toInt();
    m_extraLargeJump = settings.value("jumpExtraLarge", 90).toInt();

    m_theme = settings.value("theme", "System").toString();
    m_fontSize = settings.value("fontSize", 10).toInt();

    // Populate UI Values
    if (m_language == "en-US") {
        ui->languageComboBox->setCurrentIndex(0);
    }

    ui->rememberWindowSize->setChecked(m_rememberWindowSize);
    ui->maxLogs->setValue(m_maxLogs);
    ui->logToFile->setChecked(m_logToFile);
    ui->hashFile->setChecked(m_hashFile);
    ui->markerFileTextBox->setText(m_markerFile);
    ui->nextFrameTextBox->setText(QString::number(m_nextFrameTime));
    ui->previousFrameTextBox->setText(QString::number(m_previousFrameTime));
    ui->smallJumpTextBox->setText(QString::number(m_smallJump));
    ui->mediumJumpTextBox->setText(QString::number(m_mediumJump));
    ui->largeJumpTextBox->setText(QString::number(m_largeJump));
    ui->extraLargeJumpTextBox->setText(QString::number(m_extraLargeJump));

    if (m_theme == "System") {
        ui->themeComboBox->setCurrentIndex(0);
    } else if (m_theme == "Light") {
        ui->themeComboBox->setCurrentIndex(1);
    } else if (m_theme == "Dark") {
        ui->themeComboBox->setCurrentIndex(2);
    }

    ui->appearanceFontScaleText->setText(QString::number(m_fontSize));
}

void SettingsWindow::loadHotkeys()
{
    /*
    QSettings settings;
    QMap<QString,QString> hotkeys;
    settings.beginGroup("Hotkeys");
    const QStringList childKeys = settings.childKeys();
    foreach (const QString &childKey, childKeys)
    {
        hotkeys[childKey] = settings.value(childKey).toString();
    }
    settings.endGroup();

    QMapIterator<QString, QString> i(hotkeys);
    int x = 0;
    while (i.hasNext()) {
        i.next();
        auto item = new QListWidgetItem();
        QString defaultKey = defaultHotkeys().value(i.key());
        m_hotkeyWidget[x] = new HotkeyWidget(i.key(), defaultKey, i.value(), this);
        //connect(m_hotkeyWidget[x], &HotkeyWidget::hotkeyChanged, this, &SettingsWindow::hotkey_Changed);

        item->setSizeHint(m_hotkeyWidget[x]->sizeHint());

        //ui->hotkeyList->addItem(item);
        //ui->hotkeyList->setItemWidget(item, m_hotkeyWidget[x]);

        ++x;
    }
    m_numHotkeys = x;
    */
}

QMap<QString, QString> SettingsWindow::defaultHotkeys()
{
    QMap<QString, QString> hotkeyMap;

    // File
    hotkeyMap["Emergency Collapse"] = "C";
    hotkeyMap["Exit"] = "Ctrl+Q";
    hotkeyMap["Open..."] = "O";
    hotkeyMap["Open Media Folder..."] = "Ctrl+F";
    hotkeyMap["Open Multiple Media Files..."] = "Alt+O";
    hotkeyMap["Save"] = "Ctrl+S";
    hotkeyMap["Save As..."] = "Ctrl+Shift+S";
    hotkeyMap["Save Playlist"] = "Alt+S";
    hotkeyMap["Save a Copy..."] = "Ctrl+Alt+S";

    // Edit
    hotkeyMap["Clear"] = "Delete";
    hotkeyMap["Copy"] = "Ctrl+C";
    hotkeyMap["Cut"] = "Ctrl+X";
    hotkeyMap["Deselect All"] = "Ctrl+Shift+A";
    hotkeyMap["Duplicate"] = "Ctrl+Shift+/";
    //hotkeyMap["Find"] = "Ctrl+F";
    hotkeyMap["Paste"] = "Ctrl+V";
    hotkeyMap["Preferences"] = "Ctrl+Shift+P";
    hotkeyMap["Redo"] = "Ctrl+Shift+Z";
    hotkeyMap["Select All"] = "Ctrl+A";
    hotkeyMap["Undo"] = "Ctrl+Z";

    // Playback
    hotkeyMap["Next"] = "N";
    hotkeyMap["Next Frame"] = "]";
    hotkeyMap["Previous"] = "P";
    hotkeyMap["Previous Frame"] = "[";
    hotkeyMap["Jump Back Large"] = "Q";
    hotkeyMap["Jump Back Medium"] = "A";
    hotkeyMap["Jump Back Small"] = "Z";
    hotkeyMap["Jump Fwd Large"] = "W";
    hotkeyMap["Jump Fwd Medium"] = "S";
    hotkeyMap["Jump Fwd Small"] = "X";
    hotkeyMap["Faster"] = "Num++";
    hotkeyMap["Faster (fine)"] = "Ctrl+Num++";
    hotkeyMap["Normal Speed"] = "=";
    hotkeyMap["Slower"] = "Num+-";
    hotkeyMap["Slower (fine)"] = "Ctrl+Num+-";

    // Audio
    hotkeyMap["Decrease Volume"] = "Down";
    hotkeyMap["Increase Volume"] = "Up";
    hotkeyMap["Mute"] = "Ctrl+Shift+M";

    // Video
    hotkeyMap["Fullscreen"] = "F";

    // View
    hotkeyMap["Toggle Playlist"] = "Ctrl+P";

    // Markers
    hotkeyMap["Add Cumshot Marker"] = "Ctrl+3";
    hotkeyMap["Add Cyan Marker"] = "Ctrl+5";
    hotkeyMap["Add Dialog Marker"] = "Ctrl+4";
    hotkeyMap["Add Magenta Marker"] = "Ctrl+6";
    hotkeyMap["Add Marker"] = "M";
    hotkeyMap["Add Orange Marker"] = "Ctrl+7";
    hotkeyMap["Add Scene Transition Marker"] = "Ctrl+1";
    hotkeyMap["Add Strip Marker"] = "Ctrl+2";
    hotkeyMap["Clear In"] = "Ctrl+Shift+,";
    hotkeyMap["Clear In and Out"] = "Ctrl+Shift+X";
    hotkeyMap["Clear Markers"] = "Ctrl+Shift+M";
    hotkeyMap["Clear Out"] = "Ctrl+Shift+.";
    hotkeyMap["Clear Selected Marker"] = "Ctrl+N";
    hotkeyMap["Edit Marker"] = "Alt+M";
    hotkeyMap["Go to In"] = "Shift+,";
    hotkeyMap["Go to Next Marker"] = "B";
    hotkeyMap["Go to Out"] = "Shift+.";
    hotkeyMap["Go to Previous Marker"] = "V";
    hotkeyMap["Mark In"] = ",";
    hotkeyMap["Mark Out"] = ".";

    // Sequence
    hotkeyMap["Make Subclip..."] = "Ctrl+U";

    // Subtitle

    // Help
    hotkeyMap["Help"] = "F1";

    return hotkeyMap;
}

void SettingsWindow::settingsChanged()
{
    if (m_settingsLoaded) {
        ui->okButton->setEnabled(true);
        m_unsavedChanges = true;
    }
}

void SettingsWindow::saveSettings()
{
    QSettings settings;

    settings.setValue("language", m_language);
    settings.setValue("rememberWindowSize", m_rememberWindowSize);
    settings.setValue("maxLogFiles", m_maxLogs);
    settings.setValue("logToFile", m_logToFile);
    settings.setValue("hashFile", m_hashFile);
    settings.setValue("markerFile", m_markerFile);
    settings.setValue("nextFrameTime", m_nextFrameTime);
    settings.setValue("previousFrameTime", m_previousFrameTime);
    settings.setValue("jumpSmall", m_smallJump);
    settings.setValue("jumpMedium", m_mediumJump);
    settings.setValue("jumpLarge", m_largeJump);
    settings.setValue("jumpExtraLarge", m_extraLargeJump);

    settings.setValue("theme", m_theme);
    settings.setValue("fontSize", m_fontSize);

    m_unsavedChanges = false;
    ui->okButton->setEnabled(false);
    emit(updateSettings());
}

void SettingsWindow::resetButton_Clicked()
{
    QMessageBox::StandardButton confirmationBox = QMessageBox::question(
            this,
            "Reset Settings",
            "Are you sure you want to reset settings to default?",
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

void SettingsWindow::okButton_Clicked()
{
    saveSettings();
}

void SettingsWindow::cancelButton_Clicked()
{
    this->close();
}

void SettingsWindow::language_CurrentIndexChanged(int index)
{
    QString selectedLanguage = ui->languageComboBox->itemText(index);
    QString lang;

    if (selectedLanguage == "English") {
        lang = "en-US";
    }

    if (!lang.isEmpty() && lang != m_language) {
        m_language = lang;
        settingsChanged();
    }
}

void SettingsWindow::rememberWindowSize_Clicked(int state)
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

void SettingsWindow::logToFile_Clicked(int state)
{
    if (state == Qt::Checked) {
        m_logToFile = true;
    } else {
        m_logToFile = false;
    }
    settingsChanged();
}

void SettingsWindow::hashFile_Clicked(int state)
{
    if (state == Qt::Checked) {
        m_hashFile = true;
    } else {
        m_hashFile = false;
    }
    settingsChanged();
}

void SettingsWindow::markerFileTextBox_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text != m_markerFile) {
            m_markerFile = text;
            settingsChanged();

            qDebug() << "Set marker file to: " << text;
        }
    }
}

void SettingsWindow::markerFileBrowseButton_Clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Marker File"),
        "/home",
        tr("Marker Files (*.json *.hmrk);;All Files (*.*)")
        );

    if (!fileName.isEmpty()) {
        //settings.setValue("markerFile", fileName);
        if (fileName != m_markerFile) {
            m_markerFile = fileName;
            ui->markerFileTextBox->setText(m_markerFile);
            settingsChanged();

            qDebug() << "Set marker file to: " << fileName;
        }
    }
}

void SettingsWindow::nextFrameTextBox_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_nextFrameTime) {
            m_nextFrameTime = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::previousFrameTextBox_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_previousFrameTime) {
            m_previousFrameTime = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::smallJumpTextBox_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_smallJump) {
            m_smallJump = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::mediumJumpTextBox_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_mediumJump) {
            m_mediumJump = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::largeJumpTextBox_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_largeJump) {
            m_largeJump = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::extraLargeJumpTextBox_TextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (text.toInt() != m_extraLargeJump) {
            m_extraLargeJump = text.toInt();
            settingsChanged();
        }
    }
}

void SettingsWindow::theme_CurrentIndexChanged(int index)
{
    QString selectedTheme = ui->themeComboBox->itemText(index);

    if (selectedTheme == "Use System") {
        selectedTheme = "System";
    }

    if (!selectedTheme.isEmpty() && selectedTheme != m_theme) {
        m_theme = selectedTheme;
        settingsChanged();
    }
}

void SettingsWindow::fontScaleTextBox_TextChanged(const QString &text)
{

}

void SettingsWindow::fontScale_ValueChanged(int value)
{

}

void SettingsWindow::filterTextBox_TextChanged(const QString &text)
{

}

void SettingsWindow::hotkeyFilterTextBox_KeySequenceChanged(const QKeySequence &keySequence)
{

}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
    if (m_unsavedChanges) {
        QMessageBox::StandardButton confirmationBox = QMessageBox::question(
            this,
            "Unsaved Changes",
            "You have unsaved changes. Save Changes?",
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
