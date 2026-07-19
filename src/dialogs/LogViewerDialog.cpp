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

#include "LogViewerDialog.h"
#include "ui_LogViewerDialog.h"


QString getVerbosityString(const int verbosity)
{
    switch (verbosity) {
        case 0:
            return "[DEBUG]";
        case 1:
            return "[INFO]";
        case 2:
            return "[WARNING]";
        case 3:
            return "[ERROR]";
        case 4:
            return "[CRITICAL]";
        case 5:
            return "[FATAL]";
        default:
            return "";
    }
}

QString getLogColor(const int level) {
    if (level == 0)                 return "#888888"; // Gray
    if (level == 1)                 return "#FFFFFF"; // White
    if (level == 2)                 return "#FFFF00"; // Yellow
    if (level == 3)                 return "#FF0000"; // Red
    if (level == 4)                 return "#FF00FF"; // Magenta
    return "#FFFFFF"; // Default
}

LogViewerDialog::LogViewerDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LogViewerDialog)
{
    ui->setupUi(this);

    readSettings();

    QFont monoFont("Consolas");
    monoFont.setStyleHint(QFont::Monospace);
    monoFont.setPointSize(10);
    ui->logTextArea->setFont(monoFont);

    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &LogViewerDialog::onSearchTextChanged);
    connect(ui->chkInfo, &QCheckBox::toggled, this, &LogViewerDialog::onFilterToggled);
    connect(ui->chkWarn, &QCheckBox::toggled, this, &LogViewerDialog::onFilterToggled);
    connect(ui->chkError, &QCheckBox::toggled, this, &LogViewerDialog::onFilterToggled);
    connect(ui->chkDebug, &QCheckBox::toggled, this, &LogViewerDialog::onFilterToggled);
    connect(ui->chkAutoScroll, &QCheckBox::toggled, this, &LogViewerDialog::onAutoScrollToggled);
    connect(ui->chkAlwaysOnTop, &QCheckBox::toggled, this, &LogViewerDialog::onAlwaysOnTopToggled);
    connect(ui->chkStyleMessages, &QCheckBox::toggled, this, &LogViewerDialog::onStyleMessagesToggled);
    connect(ui->btnClear, &QPushButton::clicked, this, &LogViewerDialog::clearButton_Clicked);
    connect(ui->btnExport, &QPushButton::clicked, this, &LogViewerDialog::exportButton_Clicked);

    const Blogger *blogger = Blogger::instance();
    connect(blogger, &Blogger::newLogEntry, this, &LogViewerDialog::appendLogMessage);

    QList<LogMessage> previousMessages = blogger->getLogMessages();
    for (const auto &[timestamp, type, component, message] : previousMessages) {
        LogEntry entry;
        entry.timestamp = timestamp;
        entry.level = type;
        entry.component = component;
        entry.message = message;
        entry.fullText = QString("[%1] %2  \t%3")
                            .arg(entry.timestamp, getVerbosityString(type), message);
        m_logBuffer.append(entry);
    }
    refreshLogView();
}

LogViewerDialog::~LogViewerDialog()
{
    delete ui;
}

void LogViewerDialog::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void LogViewerDialog::appendLogMessage(LogMessage message)
{
    LogEntry entry;
    entry.timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    entry.level = message.type;
    entry.component = message.component;
    entry.message = message.message;

    QString levelStr = getVerbosityString(message.type);
    entry.fullText = QString("[%1] %2  \t%3")
                        .arg(entry.timestamp, levelStr, message.message);

    m_logBuffer.append(entry);

    if (ui->searchLineEdit->text().isEmpty() && true) {
        appendToView(entry);
    }
}

void LogViewerDialog::appendToView(const LogEntry &entry) const
{
    if (m_styleMessages) {
        const QString color = getLogColor(entry.level);

        const QString styledMessage = QString("<font color=\"%1\">%2</font>")
                                .arg(color)
                                .arg(entry.fullText.toHtmlEscaped());

        ui->logTextArea->appendHtml(styledMessage);
    } else {
        ui->logTextArea->appendPlainText(entry.fullText);
    }

    if (m_autoScroll) {
        QScrollBar *bar = ui->logTextArea->verticalScrollBar();
        bar->setValue(bar->maximum());
    }
}

void LogViewerDialog::onSearchTextChanged(const QString &text)
{
    refreshLogView();
}

void LogViewerDialog::onFilterToggled()
{
    refreshLogView();
}

void LogViewerDialog::refreshLogView()
{
    ui->logTextArea->clear();
    const QString query = ui->searchLineEdit->text().toLower();

    const bool showDebug = ui->chkDebug->isChecked();
    const bool showInfo = ui->chkInfo->isChecked();
    const bool showWarn = ui->chkWarn->isChecked();
    const bool showError = ui->chkError->isChecked();

    for (const LogEntry &entry : m_logBuffer) {
        if (entry.level == 0 && !showDebug) continue;
        if (entry.level == 1 && !showInfo) continue;
        if (entry.level == 2 && !showWarn) continue;
        if (entry.level == 3 && !showError) continue;

        if (!query.isEmpty() && !entry.fullText.toLower().contains(query)) {
            continue;
        }

        appendToView(entry);
    }
}

void LogViewerDialog::onAlwaysOnTopToggled(const bool checked)
{
    const Qt::WindowFlags flags = this->windowFlags();
    if (checked) {
        this->setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    } else {
        this->setWindowFlags(flags & ~Qt::WindowStaysOnTopHint);
    }
    this->show();
    this->activateWindow();
}

void LogViewerDialog::onAutoScrollToggled(const bool checked)
{
    m_autoScroll = checked;
}

void LogViewerDialog::onStyleMessagesToggled(const bool checked)
{
    m_styleMessages = checked;
    refreshLogView();
}

void LogViewerDialog::clearButton_Clicked()
{
    m_logBuffer.clear();
    ui->logTextArea->clear();

    Blogger::instance()->clearLogFile();
}

void LogViewerDialog::exportButton_Clicked()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this, tr("Export Logs"), "", tr("Log Files (*.log);;All Files (*.*)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->logTextArea->toPlainText();
            file.close();
            QMessageBox::information(this, "Success", "Logs exported successfully.");
        }
    }
}

void LogViewerDialog::readSettings()
{
    QSettings settings;
    settings.beginGroup("LogViewerDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    m_styleMessages = settings.value("styleMessages", true).toBool();
    m_autoScroll = settings.value("autoScroll", true).toBool();
    ui->chkAlwaysOnTop->setChecked(settings.value("alwaysOnTop", true).toBool());
    ui->chkDebug->setChecked(settings.value("showDebug", false).toBool());
    ui->chkInfo->setChecked(settings.value("showInfo", true).toBool());
    ui->chkWarn->setChecked(settings.value("showWarn", true).toBool());
    ui->chkError->setChecked(settings.value("showError", true).toBool());
    settings.endGroup();
}

void LogViewerDialog::writeSettings() const
{
    QSettings settings;
    settings.beginGroup("LogViewerDialog");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("styleMessages", m_styleMessages);
    settings.setValue("autoScroll", m_autoScroll);
    settings.setValue("alwaysOnTop", ui->chkAlwaysOnTop->isChecked());
    settings.setValue("showDebug", ui->chkDebug->isChecked());
    settings.setValue("showInfo", ui->chkInfo->isChecked());
    settings.setValue("showWarn", ui->chkWarn->isChecked());
    settings.setValue("showError", ui->chkError->isChecked());
    settings.endGroup();
    settings.sync();
}
