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

#include "ErrorDialog.h"
#include "ui_ErrorDialog.h"


ErrorDialog::ErrorDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ErrorDialog)
{
    ui->setupUi(this);

    connect(&ErrorService::instance(), &ErrorService::errorOccurred, this, &ErrorDialog::showError);
    connect(ui->closeButton, &QPushButton::clicked, this, &ErrorDialog::closeButton_Clicked);
    connect(ui->errorActionButton, &QPushButton::clicked, this, &ErrorDialog::errorActionButton_Clicked);

    this->hide();
}

ErrorDialog::~ErrorDialog()
{
    delete ui;
}

void ErrorDialog::showError(const PlayerError &error)
{
    ui->errorActionButton->show();

    m_error = error;
    this->setWindowTitle(error.title);
    ui->errorLabel->setText(error.message);

    if (error.action == ErrorAction::None) {
        ui->errorActionButton->hide();

    } else if (error.action == ErrorAction::RetryPlayback) {
        ui->errorActionButton->setText("Retry");

    } else if (error.action == ErrorAction::ReloadLibrary) {
        ui->errorActionButton->setText("Reload Library");

    } else if (error.action == ErrorAction::ReconnectStream) {
        ui->errorActionButton->setText("Reconnect Stream");

    }

    this->show();
}

void ErrorDialog::closeButton_Clicked()
{
    this->hide();
}

void ErrorDialog::errorActionButton_Clicked()
{

}
