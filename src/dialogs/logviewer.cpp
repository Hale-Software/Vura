/*******************************************************************************
     Copyright (c) 2026. by Andrew Hale <halea2196@gmail.com>

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

#include "logviewer.h"
#include "ui_logviewer.h"

#include <config.h>


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


LogViewer::LogViewer(QWidget *parent) : QDialog(parent), ui(new Ui::LogViewer)
{
    ui->setupUi(this);

    m_logViewerModel = new LogViewerModel(this);
    ui->logTable->setModel(m_logViewerModel);
    ui->logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->logTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->logTable->setSelectionMode(QAbstractItemView::SingleSelection);

    m_verbosity = 1;
    ui->verbosityComboBox->setCurrentIndex(m_verbosity);

    connect(ui->openButton, &QPushButton::clicked, this, &LogViewer::openButton_Clicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &LogViewer::clearButton_Clicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &LogViewer::closeButton_Clicked);
    connect(ui->verbosityComboBox, &QComboBox::currentIndexChanged, this, &LogViewer::verbosityIndexChanged);

    Blogger *blog = Blogger::instance();
    connect(blog, &Blogger::message, this, &LogViewer::message);
    connect(blog, &Blogger::newLogMessage, this, &LogViewer::newLogMessage);
    m_currentLogFile = blog->getLogFileName();
    m_openedLogFile = m_currentLogFile;

    QList<BloggerMessage> bloggerMessages = blog->getMessages();
    for (const BloggerMessage &bloggerMessage : bloggerMessages) {
        newLogMessage(bloggerMessage.timestamp, bloggerMessage.verbosity, bloggerMessage.message);
    }

    refreshMessages();
}

LogViewer::~LogViewer()
{
    delete ui;
}

void LogViewer::openFile(const QString &fileName)
{
    if (!fileName.isEmpty()) {
        m_openedLogFile = fileName;
        refreshMessages();
    }
}

void LogViewer::messageFormatter(const QString &message) const
{
    if (m_verbosity == 0) {
        //ui->textArea->append(message);

    } else if (m_verbosity == 1) {
        if (!message.contains(getVerbosityString(0))) {
            //ui->textArea->append(message);
        }

    } else if (m_verbosity == 2) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))) {
            //ui->textArea->append(message);
        }

    } else if (m_verbosity == 3) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))
            && !message.contains(getVerbosityString(2))) {
            //ui->textArea->append(message);
            }

    } else if (m_verbosity == 4) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))
            && !message.contains(getVerbosityString(2)) && !message.contains(getVerbosityString(3))) {
            //ui->textArea->append(message);
            }

    } else if (m_verbosity == 5) {
        if (!message.contains(getVerbosityString(0)) && !message.contains(getVerbosityString(1))
            && !message.contains(getVerbosityString(2)) && !message.contains(getVerbosityString(3))
            && !message.contains(getVerbosityString(4))) {
            //ui->textArea->append(message);
            }

    } else {
        //ui->textArea->append(message);
    }
}

void LogViewer::refreshMessages()
{
    //ui->textArea->clear();

    QFile logFile(m_openedLogFile, this);
    if (!logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        VMessageBox::critical(this,
            tr("Vura Error"),
            tr("Failed to open log file."),
            false);

        this->close();

    } else {
        QTextStream t(&logFile);

        while (!t.atEnd()) {
            QString line = t.readLine();
            messageFormatter(line);
        }
    }
}

void LogViewer::message(const QString &message) const
{
    if (m_openedLogFile == m_currentLogFile)
        messageFormatter(message);
}

void LogViewer::newLogMessage(const QString &timestamp, const QString &verbosity, const QString &msg)
{
    if (m_openedLogFile == m_currentLogFile) {
        m_logViewerModel->insertRows(0, 1, QModelIndex());

        QModelIndex index = m_logViewerModel->index(0, 0, QModelIndex());
        m_logViewerModel->setData(index, timestamp, Qt::EditRole);
        index = m_logViewerModel->index(0, 1, QModelIndex());
        m_logViewerModel->setData(index, verbosity, Qt::EditRole);
        index = m_logViewerModel->index(0, 2, QModelIndex());
        m_logViewerModel->setData(index, msg, Qt::EditRole);
    }
}

void LogViewer::openButton_Clicked()
{
    // Get the log directory.
    QString logDirString = "logs";
    QString appDataDirString = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (QStringLiteral(VURA_BUILD_TYPE) == "Debug") {
        appDataDirString = constants::ApplicationDebugFolder;
    }

    const QDir appDataDir(appDataDirString);
    if (appDataDir.exists()) {
        const QDir appLogDir(appDataDirString + "/logs");
        if (appLogDir.exists()) {
            logDirString = appDataDirString + "/logs";

        } else {
            if (appLogDir.mkdir(".")) {
                logDirString = appDataDirString + "/logs";
            }
        }
    } else {
        if (appDataDir.mkdir(".")) {
            const QDir appLogDir(appDataDirString + "/logs");
            if (appLogDir.exists()) {
                logDirString = appDataDirString + "/logs";

            } else {
                if (appLogDir.mkdir(".")) {
                    logDirString = appDataDirString + "/logs";
                }
            }
        }
    }

    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Log File"),
        logDirString,
        tr("Log Files (*.log);;All Files (*.*)"));

    if (!fileName.isEmpty()) {
        m_openedLogFile = fileName;
        refreshMessages();
    }
}

void LogViewer::clearButton_Clicked()
{
    const QMessageBox::StandardButton confirmationBox = QMessageBox::question(
        this,
        tr("Clear Logs"),
        tr("Are you sure you want to clear the logs?\n This action cannot be undone."),
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
                VMessageBox::critical(
                    this,
                    tr("Vura Error"),
                    tr("Failed to clear log file."),
                    false);
            }
        }
    }
}

void LogViewer::closeButton_Clicked()
{
    this->close();
}

void LogViewer::verbosityIndexChanged(const int index)
{
    m_verbosity = index;
    refreshMessages();
}
