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
        convertFile(outFile);
    } else if (ui->tabWidget->currentIndex() == 1) {

    } else if (ui->tabWidget->currentIndex() == 2) {
        convertNetwork(outFile);
    } else if (ui->tabWidget->currentIndex() == 3) {

    }
}

void ConvertMediaDialog::convertFile(QString outFile)
{
    //m_remuxerThread = new RemuxerThread(ui->fileToConvert->text(), outFile);
    //connect(m_remuxerThread, &RemuxerThread::progressUpdated, this, &ConvertMediaDialog::progressUpdated);
    //connect(m_remuxerThread, &RemuxerThread::conversionFinished, this, &ConvertMediaDialog::conversionFinished);
    //m_remuxerThread->start();
    m_converter = new VideoConverter(this);
    m_converter->convert(ui->fileToConvert->text(), outFile, ui->comboBox_2->currentIndex());
    connect(m_converter, &VideoConverter::progressUpdated, this, &ConvertMediaDialog::progressUpdated);
}

void ConvertMediaDialog::convertNetwork(QString outFile)
{
    //m_remuxerThread = new RemuxerThread(ui->networkURL->text(), outFile);
    //connect(m_remuxerThread, &RemuxerThread::progressUpdated, this, &ConvertMediaDialog::progressUpdated);
    //connect(m_remuxerThread, &RemuxerThread::conversionFinished, this, &ConvertMediaDialog::conversionFinished);
    //m_remuxerThread->start();
    m_converter = new VideoConverter(this);
    m_converter->convert(ui->networkURL->text(), outFile, ui->comboBox_2->currentIndex());
    connect(m_converter, &VideoConverter::progressUpdated, this, &ConvertMediaDialog::progressUpdated);
}
