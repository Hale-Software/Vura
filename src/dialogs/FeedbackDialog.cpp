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

// You may need to build the project (run Qt uic code generator) to get "ui_FeedbackDialog.h" resolved

#include "FeedbackDialog.h"
#include "ui_FeedbackDialog.h"


FeedbackDialog::FeedbackDialog(QWidget *parent) : QDialog(parent), ui(new Ui::FeedbackDialog)
{
    ui->setupUi(this);

    QRegularExpression rx("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");

    QRegularExpressionValidator *validator = new QRegularExpressionValidator(rx, this);
    ui->emailAddress->setValidator(validator);

    connect(ui->cancelButton, &QPushButton::clicked, this, &FeedbackDialog::cancelButton_Clicked);
    connect(ui->sendFeedback, &QPushButton::clicked, this, &FeedbackDialog::sendFeedback_Clicked);
    connect(ui->privacyPolicy, &QPushButton::clicked, this, &FeedbackDialog::privacyPolicy_Clicked);
    connect(ui->emailAddress, &QLineEdit::editingFinished, this, &FeedbackDialog::emailAddress_EditingFinished);
    connect(ui->feedbackMessage, &MonitoredTextEdit::startEditing, this, &FeedbackDialog::feedbackMessage_StartedEditing);
    connect(ui->feedbackMessage, &MonitoredTextEdit::editingFinished, this, &FeedbackDialog::feedbackMessage_EditingFinished);

}

FeedbackDialog::~FeedbackDialog()
{
    delete ui;
}

void FeedbackDialog::cancelButton_Clicked()
{
    bool feedbackMessage_empty = true;
    bool emailAddress_empty = true;
    bool feedbackType_empty = true;

    QString feedbackMessage = ui->feedbackMessage->toPlainText();
    QString emailAddress = ui->emailAddress->text();
    int feedbackType = ui->feedbackType->currentIndex();

    if (!feedbackMessage.isEmpty())
        feedbackMessage_empty = false;

    if (!emailAddress.isEmpty())
        emailAddress_empty = false;

    if (feedbackType != 0)
        feedbackType_empty = false;

    if (!feedbackMessage_empty || !emailAddress_empty || !feedbackType_empty) {
        QMessageBox::StandardButton confirmationBox = QMessageBox::question(this,
            tr("Unsaved Progress"),
            tr("You have unsaved progress. Are you sure you want to exit?"),
            QMessageBox::Yes | QMessageBox::No);

        if (confirmationBox == QMessageBox::Yes) {
            this->close();
        }

    } else {
        this->close();
    }
}

void FeedbackDialog::sendFeedback_Clicked() {}

void FeedbackDialog::privacyPolicy_Clicked()
{
    QString link = "https://github.com/Hale-Software/Vura/wiki/Privacy-Policy";
    QDesktopServices::openUrl(QUrl(link));
}

void FeedbackDialog::emailAddress_EditingFinished()
{
    if (ui->emailAddress->hasAcceptableInput()) {

    } else {
        ui->emailAddress->setStyleSheet("border: 1px solid red;");
    }
}

void FeedbackDialog::feedbackMessage_StartedEditing()
{
    //ui->feedbackMessage->setStyleSheet("border: none;");
}

void FeedbackDialog::feedbackMessage_EditingFinished()
{
    QString feedbackMessage = ui->feedbackMessage->toPlainText();
    if (feedbackMessage.isEmpty()) {
        //ui->feedbackMessage->setStyleSheet("border: 1px solid red;");
    }
}
