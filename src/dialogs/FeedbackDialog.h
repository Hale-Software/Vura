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

#pragma once

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>


QT_BEGIN_NAMESPACE

namespace Ui {
    class FeedbackDialog;
}

QT_END_NAMESPACE

class FeedbackDialog : public QDialog {
    Q_OBJECT

public:
    explicit FeedbackDialog(QWidget *parent = nullptr);
    ~FeedbackDialog() override;

private slots:
    void cancelButton_Clicked();
    void sendFeedback_Clicked();
    void privacyPolicy_Clicked();
    void emailAddress_TextChanged();


private:
    Ui::FeedbackDialog *ui;
};
