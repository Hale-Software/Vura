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

#include "detaildialog.h"

#include <QByteArray>
#include <QNetworkReply>
#include <QPointer>
#include <QWidget>

namespace Ui { class MainWidget; }

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void setProgressbarMaximum(int maximum);
    void updateProgressBar(qint64 progressCount, qint64 fullCount);

signals:
    void restartCrashedApplication();
    void sendDump();
    void restartCrashedApplicationAndSendDump();
    void emailAdressChanged(const QString &email);
    void commentChanged(const QString &comment);

protected:
    void changeEvent(QEvent *e);

private:
    void restartApplication();
    void quitApplication();
    void showError(QNetworkReply::NetworkError error);
    void showDetails();
    void commentIsProvided();

private:
    Ui::MainWidget *ui;

    QPointer<DetailDialog> m_detailDialog;
    bool m_commentIsProvided = false;
};