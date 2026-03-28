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

#include "detaildialog.h"

#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QVBoxLayout>

DetailDialog::DetailDialog(QWidget *parent) :
    QDialog(parent)
{
    resize(640, 480);
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    textBrowser = new QTextBrowser(this);
    verticalLayout->addWidget(textBrowser);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Close);

    verticalLayout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

DetailDialog::~DetailDialog()
{
}

void DetailDialog::setText(const QString &text)
{
    textBrowser->setPlainText(text);
}
