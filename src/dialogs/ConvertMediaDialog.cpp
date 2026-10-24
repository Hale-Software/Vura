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

#include "ConvertMediaDialog.h"
#include "ui_ConvertMediaDialog.h"

#include <QThread>
#include <QProgressBar>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>


ConvertMediaDialog::ConvertMediaDialog(QWidget *parent, const QString &initialOption) : QDialog(parent), ui(new Ui::ConvertMediaDialog)
{
    ui->setupUi(this);
    m_initialOption = initialOption;
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    connect(ui->actionButton, &QPushButton::clicked, this, &ConvertMediaDialog::convert_Clicked);
    connect(ui->fileBrowseButton, &QPushButton::clicked, this, &ConvertMediaDialog::fileBrowse_Clicked);
}

ConvertMediaDialog::~ConvertMediaDialog()
{
    delete ui;
}

void ConvertMediaDialog::progressUpdated(int percentage)
{
    ui->progressBar->setValue(percentage);
}

void ConvertMediaDialog::conversionFinished(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, tr("Conversion finished"), message);
    } else {
        QMessageBox::critical(this, tr("Conversion failed"), message);
    }
}

void ConvertMediaDialog::fileBrowse_Clicked()
{
    QString inputFile = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        QDir::homePath(),
        tr("Video Files (*.mp4 *.mkv *.avi *.mov *.wmv *.flv *.webm)"));

    if (!inputFile.isEmpty()) {
        ui->fileToConvert->setText(inputFile);
    }
}

void ConvertMediaDialog::convert_Clicked()
{
    QString outFile = QFileDialog::getSaveFileName(
        this,
        tr("Save File As"),
        QDir::homePath(),
        tr("Video Files (*.mp4 *.mkv *.avi *.mov *.wmv *.flv *.webm)"));

    if (outFile.isEmpty())
        return;

    if (ui->tabWidget->currentIndex() == 0) {
        //convertFile(outFile);
    } else if (ui->tabWidget->currentIndex() == 1) {

    } else if (ui->tabWidget->currentIndex() == 2) {
        QString inFile = ui->networkURL->text();
        convertFile(inFile.trimmed(), outFile);
    } else if (ui->tabWidget->currentIndex() == 3) {

    }
}

void ConvertMediaDialog::convertFile(const QString& inFile, const QString &outFile)
{
    auto *thread = new QThread(this);
    auto *remuxer = new Remuxer;
    remuxer->moveToThread(thread);

    connect(thread, &QThread::started, remuxer, [=] {
        remuxer->remux(inFile, outFile);
    });

    connect(remuxer, &Remuxer::progress, ui->progressBar, &QProgressBar::setValue);
    connect(remuxer, &Remuxer::finished, this, [](bool ok, const QString &err) {
        if (!ok) {
            qWarning() << err;
            QMessageBox::warning(nullptr, "Error Remuxing", err);
        } else {
            QMessageBox::information(nullptr, "Remuxing Finished", "Finished Remuxing");
        }
    });
    connect(remuxer, &Remuxer::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, remuxer, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();

    connect(ui->cancel, QPushButton::clicked, remuxer, [=] {
        remuxer->cancel();
    });
}

