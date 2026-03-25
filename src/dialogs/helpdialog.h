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

#ifndef VURA_HELPDIALOG_H
#define VURA_HELPDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>
#include <QFile>


QT_BEGIN_NAMESPACE

namespace Ui {
    class HelpDialog;
}

QT_END_NAMESPACE

class HelpDialog : public QDialog {
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog() override;

private slots:
    void close_Clicked();

private:
    Ui::HelpDialog *ui;
};


#endif //VURA_HELPDIALOG_H