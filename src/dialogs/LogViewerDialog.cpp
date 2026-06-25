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

#include "LogViewerDialog.h"
#include "ui_LogViewerDialog.h"

#include <ui-config.h>


QString getVerbosityString(int verbosity)
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


LogViewerDialog::LogViewerDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LogViewerDialog)
{
    ui->setupUi(this);

    QSettings settings;
    m_verbosity = settings.value("logViewerVerbosity", 1).toInt();
    ui->verbosityComboBox->setCurrentIndex(m_verbosity);

    connect(ui->openButton, &QPushButton::clicked, this, &LogViewerDialog::openButton_Clicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &LogViewerDialog::clearButton_Clicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &LogViewerDialog::closeButton_Clicked);
    connect(ui->verbosityComboBox, &QComboBox::currentIndexChanged, this, &LogViewerDialog::verbosityIndexChanged);
    connect(ui->simplify, &QCheckBox::checkStateChanged, this, &LogViewerDialog::simplify_Clicked);

    Blogger *blog = Blogger::instance();
    connect(blog, &Blogger::message, this, &LogViewerDialog::message);
    m_currentLogFile = blog->getLogFileName();
    m_openedLogFile = m_currentLogFile;
    refreshMessages();
}

LogViewerDialog::~LogViewerDialog()
{
    delete ui;
}

void LogViewerDialog::openFile(const QString &fileName)
{
    if (!fileName.isEmpty()) {
        m_openedLogFile = fileName;
        refreshMessages();
    }
}

void LogViewerDialog::messageFormatter(QString message)
{
    if (m_simplify) {
        int timestampIn_pos = message.indexOf("[");
        int fileIn_pos = message.indexOf("[", timestampIn_pos + 1);
        int lineIn_pos = message.indexOf("[", fileIn_pos + 1);
        int functionIn_pos = message.indexOf("[", lineIn_pos + 1);
        int fileOut_pos = message.indexOf("]", fileIn_pos);
        int lineOut_pos = message.indexOf("]", lineIn_pos);
        int functionOut_pos = message.indexOf("]", functionIn_pos);

        message.remove(functionIn_pos - 1, functionOut_pos - functionIn_pos + 2);
        message.remove(lineIn_pos - 1, lineOut_pos - lineIn_pos + 2);
        message.remove(fileIn_pos - 1, fileOut_pos - fileIn_pos + 2);
    }

    // Message Verbosity Handler
    if (m_verbosity == 0) {
        ui->textArea->append(message);
    } else if (m_verbosity == 1) {
        if (!message.contains(getVerbosityString(0))) {
            ui->textArea->append(message);
        }
    } else if (m_verbosity == 2) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))) {
            ui->textArea->append(message);
        }
    } else if (m_verbosity == 3) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))
            && !message.contains(getVerbosityString(2))) {
            ui->textArea->append(message);
            }
    } else if (m_verbosity == 4) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))
            && !message.contains(getVerbosityString(2)) && !message.contains(getVerbosityString(3))) {
            ui->textArea->append(message);
            }
    } else if (m_verbosity == 5) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))
            && !message.contains(getVerbosityString(2)) && !message.contains(getVerbosityString(3))
            && !message.contains(getVerbosityString(4))) {
            ui->textArea->append(message);
            }
    } else {
        ui->textArea->append(message);
    }
}

void LogViewerDialog::refreshMessages()
{
    ui->textArea->clear();
    QFile logFile(m_openedLogFile, this);
    if (!logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ErrorBox(this, "Error opening log file");
    } else {
        QTextStream t(&logFile);
        while (!t.atEnd()) {
            QString line = t.readLine();
            messageFormatter(line);
        }
    }
}

void LogViewerDialog::message(QString message)
{
    if (m_openedLogFile == m_currentLogFile)
        messageFormatter(message);
}

void LogViewerDialog::openButton_Clicked()
{
    // Get the log directory.
    QString logDirString = "logs";
    QString appDataDirString = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (VURA_BUILD_TYPE == "Debug") {
        appDataDirString = "debug";
    }
    QDir appDataDir(appDataDirString);
    if (appDataDir.exists()) {
        QDir appLogDir(appDataDirString + "/logs");
        if (appLogDir.exists()) {
            logDirString = appDataDirString + "/logs";
        } else {
            if (appLogDir.mkdir(".")) {
                logDirString = appDataDirString + "/logs";
            }
        }
    } else {
        if (appDataDir.mkdir(".")) {
            QDir appLogDir(appDataDirString + "/logs");
            if (appLogDir.exists()) {
                logDirString = appDataDirString + "/logs";
            } else {
                if (appLogDir.mkdir(".")) {
                    logDirString = appDataDirString + "/logs";
                }
            }
        }
    }

    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Log File"),
        logDirString,
        tr("Log Files (*.log);;All Files (*.*)"));

    if (!fileName.isEmpty()) {
        m_openedLogFile = fileName;
        refreshMessages();
    }
}

void LogViewerDialog::clearButton_Clicked()
{
    QMessageBox::StandardButton confirmationBox;
    confirmationBox = QMessageBox::question(this, "Clear Logs", "Are you sure you want to clear the logs?\n This action cannot be undone.",
                                  QMessageBox::Yes | QMessageBox::No);

    if (confirmationBox == QMessageBox::Yes) {
        if (m_openedLogFile == m_currentLogFile) {
            Blogger *blog = Blogger::instance();
            blog->clearLogFile();
        } else {
            QFile f(m_openedLogFile);
            if (f.open(QFile::WriteOnly | QFile::Truncate)) {
                f.close();
            } else {
                ErrorBox(this, "Failed to clear log file.");
            }
        }
    }
}

void LogViewerDialog::closeButton_Clicked()
{
    this->close();
}

void LogViewerDialog::verbosityIndexChanged(int index)
{
    QSettings settings;
    settings.setValue("logViewerVerbosity", index);
    m_verbosity = index;
    refreshMessages();
}

void LogViewerDialog::simplify_Clicked()
{
    m_simplify = ui->simplify->isChecked();
    refreshMessages();
}
