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

#ifndef VURA_LOGUPLOADDIALOG_H
#define VURA_LOGUPLOADDIALOG_H

#include <QDialog>


QT_BEGIN_NAMESPACE

namespace Ui {
    class LogUploadDialog;
}

QT_END_NAMESPACE

class LogUploadDialog : public QDialog {
    Q_OBJECT

public:
    explicit LogUploadDialog(QWidget *parent = nullptr);

    ~LogUploadDialog() override;

private:
    Ui::LogUploadDialog *ui;
};


#endif //VURA_LOGUPLOADDIALOG_H