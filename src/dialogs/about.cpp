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

#include "about.h"
#include "ui_about.h"

#include <config.h>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->version->setText(QString::fromStdString(VURA_VERSION_STRING));

    connect(ui->authors, &ClickableLabel::clicked, this, &AboutDialog::authors_Clicked);
    connect(ui->license, &ClickableLabel::clicked, this, &AboutDialog::license_Clicked);
    connect(ui->credits, &ClickableLabel::clicked, this, &AboutDialog::credits_Clicked);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::authors_Clicked()
{
    QFile file("data/AUTHORS");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->textBrowser->setText(file.readAll());
        file.close();
    }
}

void AboutDialog::license_Clicked()
{
    QFile file("data/license/license.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->textBrowser->setText(file.readAll());
        file.close();
    }
}

void AboutDialog::credits_Clicked()
{
    ui->textBrowser->clear();
}
