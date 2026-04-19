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

// You may need to build the project (run Qt uic code generator) to get "ui_MarkerEditDialog.h" resolved

#include "MarkerEditDialog.h"
#include "ui_MarkerEditDialog.h"


MarkerEditDialog::MarkerEditDialog(const VuraVideoMarker &videoMarker, int videoDuration, QWidget *parent)
    : QDialog(parent), ui(new Ui::MarkerEditDialog), m_videoMarker(videoMarker), m_videoDuration(videoDuration)
{
    ui->setupUi(this);

    ui->timestamp->setInputMask("99:99:99;00:00:00");
    populateUI();

    connect(ui->markerName, &QLineEdit::textChanged, this, &MarkerEditDialog::name_TextChanged);
    connect(ui->timestamp, &QLineEdit::textChanged, this, &MarkerEditDialog::timestamp_TextChanged);
    connect(ui->markerType, &QComboBox::currentIndexChanged, this, &MarkerEditDialog::type_IndexChanged);
    connect(ui->comments, &QTextEdit::textChanged, this, &MarkerEditDialog::comments_TextChanged);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MarkerEditDialog::deleteButton_Clicked);
    connect(ui->previousButton, &QPushButton::clicked, this, &MarkerEditDialog::prevButton_Clicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &MarkerEditDialog::nextButton_Clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &MarkerEditDialog::cancelButton_Clicked);
    connect(ui->okButton, &QPushButton::clicked, this, &MarkerEditDialog::okButton_Clicked);
}

MarkerEditDialog::~MarkerEditDialog()
{
    delete ui;
}

void MarkerEditDialog::saveMarker()
{
    m_videoMarker.markerName = ui->markerName->text();

    QString timestampString = ui->timestamp->text();

    QStringList timestringList = timestampString.split(":");
    if (timestringList.count() != 3) {
        QMessageBox::warning(this, tr("Error Saving"), tr("Failed to convert entered timestring to data file format."));
        return;
    }

    int hours = timestringList.at(0).toInt();
    int minutes = timestringList.at(1).toInt();
    int seconds = timestringList.at(2).toInt();

    int hoursToMinutes = hours * 60;
    minutes = minutes + hoursToMinutes;
    int minutesToSeconds = minutes * 60;
    seconds = seconds + minutesToSeconds;

    if (seconds > m_videoDuration)
        seconds = m_videoDuration;

    const double distanceFromMin = (seconds - 0);
    const double sliderRange = (m_videoDuration - 0);
    const double sliderPercent = (distanceFromMin / sliderRange);
    m_videoMarker.timestamp = sliderPercent;

    m_videoMarker.markerType = ui->markerType->currentText().toLower();
    m_videoMarker.comments = ui->comments->toPlainText();

    emit markerEdited(m_videoMarker);
    m_unsavedChanges = false;
}

void MarkerEditDialog::closeEvent(QCloseEvent *event)
{
    if (m_forceClose)
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
            saveMarker();
            m_unsavedChanges = false;
        }
    }
    event->accept();
}

void MarkerEditDialog::forceClose()
{
    m_forceClose = true;
    this->close();
}

void MarkerEditDialog::loadVideoMarker(const VuraVideoMarker &videoMarker)
{
    if (m_unsavedChanges) {
        QMessageBox::StandardButton confirmationBox = QMessageBox::question(
        this,
        tr("Unsaved Changes"),
        tr("Do you want to save your changes?"),
        QMessageBox::Yes | QMessageBox::No
        );

        if (confirmationBox == QMessageBox::Yes) {
            saveMarker();
        }
    }

    m_unsavedChanges = false;
    m_videoMarker = videoMarker;
    populateUI();
}

void MarkerEditDialog::setPrevButton_Enabled(bool enabled)
{
    ui->previousButton->setEnabled(enabled);
}

void MarkerEditDialog::setNextButton_Enabled(bool enabled)
{
    ui->nextButton->setEnabled(enabled);
}

void MarkerEditDialog::name_TextChanged(const QString &text)
{
    if (text != m_videoMarker.markerName) {
        m_unsavedChanges = true;
    }
}

void MarkerEditDialog::timestamp_TextChanged(const QString &text)
{
    if (text != durationToTimestampString()) {
        m_unsavedChanges = true;
        QString timestampString = ui->timestamp->text();
        QString windowTitle = "Marker @ " + timestampString;
        this->setWindowTitle(windowTitle);
    }
}

void MarkerEditDialog::type_IndexChanged(int index)
{
    if (ui->markerType->currentText().toLower() != m_videoMarker.markerType) {
        m_unsavedChanges = true;
    }
}

void MarkerEditDialog::comments_TextChanged()
{
    if (ui->comments->toPlainText() != m_videoMarker.comments) {
        m_unsavedChanges = true;
    }
}

void MarkerEditDialog::deleteButton_Clicked()
{
    QMessageBox::StandardButton confirmationBox = QMessageBox::question(
        this,
        tr("Delete Marker"),
        tr("Are you sure you want to delete this marker? This cannot be undone."),
        QMessageBox::Yes | QMessageBox::No
        );

    if (confirmationBox == QMessageBox::Yes) {
        emit markerDeleted(m_videoMarker);
        m_unsavedChanges = false;
    }
}

void MarkerEditDialog::prevButton_Clicked()
{
    emit getPrevMarker(m_videoMarker);
}

void MarkerEditDialog::nextButton_Clicked()
{
    emit getNextMarker(m_videoMarker);
}

void MarkerEditDialog::cancelButton_Clicked()
{
    this->close();
}

void MarkerEditDialog::okButton_Clicked()
{
    if (m_unsavedChanges)
        saveMarker();

    this->close();
}

QString MarkerEditDialog::durationToTimestampString() const
{
    const QString format = "hh:mm:ss";

    const int currentPosition = m_videoMarker.timestamp * m_videoDuration;

    const QTime currentTime(
            (currentPosition / 3600) % 60,
            (currentPosition / 60) % 60,
            currentPosition % 60,
            (currentPosition * 1000) % 1000);

    return currentTime.toString(format);
}

void MarkerEditDialog::populateUI()
{
    QString timestampString = durationToTimestampString();
    QString windowTitle = "Marker @ " + timestampString;
    this->setWindowTitle(windowTitle);

    ui->markerName->setText(m_videoMarker.markerName);
    ui->timestamp->setText(timestampString);
    ui->comments->setText(m_videoMarker.comments);

    if (m_videoMarker.markerType == "marker") {
        ui->markerType->setCurrentIndex(0);

    } else if (m_videoMarker.markerType == "scene") {
        ui->markerType->setCurrentIndex(1);

    } else if (m_videoMarker.markerType == "strip") {
        ui->markerType->setCurrentIndex(2);

    } else if (m_videoMarker.markerType == "cumshot") {
        ui->markerType->setCurrentIndex(3);

    } else if (m_videoMarker.markerType == "dialog") {
        ui->markerType->setCurrentIndex(4);

    } else if (m_videoMarker.markerType == "cyan") {
        ui->markerType->setCurrentIndex(5);

    } else if (m_videoMarker.markerType == "magenta") {
        ui->markerType->setCurrentIndex(6);

    } else if (m_videoMarker.markerType == "orange") {
        ui->markerType->setCurrentIndex(7);

    }
}
