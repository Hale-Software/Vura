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

#include "updatewindow.h"
#include "ui_updatewindow.h"


UpdateDialog::UpdateDialog(const QString& versionString, const QString& releaseDateString, const QString& downloadUrl,
                                 const QString& changelog, QWidget *parent)
    : QDialog(parent), ui(new Ui::UpdateDialog), m_versionString(versionString), m_releaseDateString(releaseDateString), m_downloadUrl(downloadUrl), m_changelog(changelog)
{
    ui->setupUi(this);

    ui->label->setText(QString("Version %1 (%2)").arg(versionString).arg(releaseDateString));
    ui->text->setText(changelog);

    connect(ui->nowButton, &QPushButton::clicked, this, &UpdateDialog::nowButton_Clicked);
    connect(ui->remindButton, &QPushButton::clicked, this, &UpdateDialog::remindButton_Clicked);
    connect(ui->skipButton, &QPushButton::clicked, this, &UpdateDialog::skipButton_Clicked);
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::nowButton_Clicked()
{

}

void UpdateDialog::remindButton_Clicked()
{

}

void UpdateDialog::skipButton_Clicked()
{

}
