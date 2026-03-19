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
#include <QString>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>


QT_BEGIN_NAMESPACE

namespace Ui
{
    class EditMarkerDialog;
}

QT_END_NAMESPACE


class EditMarkerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditMarkerDialog(QWidget *parent = nullptr);
    ~EditMarkerDialog() override;

public slots:
    void setMarkerInfo(const QString &markerName, const double &markerTime, const QString &markerType);
    void cancelButton_Clicked();
    void okButton_Clicked();
    void deleteButton_Clicked();
    void nameTextBox_TextChanged(const QString &text);
    void timeTextBox_TextChanged(const QString &text);
    void markerTypeComboBox_CurrentIndexChanged(int index);

signals:
    void markerSaved(QString markerName, double markerTime, QString markerType);
    void markerDeleted(double markerTime);

private:
    Ui::EditMarkerDialog *ui;
    QString m_markerName;
    double m_markerTime;
    QString m_markerType;

};
