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


#include "helpdialog.h"
#include "ui_helpdialog.h"


HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent), ui(new Ui::HelpDialog)
{
    ui->setupUi(this);

    connect(ui->close, &QPushButton::clicked, this, &HelpDialog::close_Clicked);

    QFile file("data/help.html");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->textBrowser->setHtml(file.readAll());
        file.close();
    }
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::close_Clicked()
{
    this->close();
}
