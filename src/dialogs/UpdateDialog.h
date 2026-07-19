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

#pragma once

#include <QDialog>
#include <QPushButton>
#include <QSettings>
#include <QTextBrowser>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>


QT_BEGIN_NAMESPACE

namespace Ui {
    class UpdateDialog;
}

QT_END_NAMESPACE


class UpdateDialog : public QDialog {
    Q_OBJECT

public:
    explicit UpdateDialog(const QString& versionString, const QString& releaseDateString, const QString& downloadUrl,
                                 const QString& changelogUrl, const QString &expectedHash, QWidget *parent = nullptr);
    ~UpdateDialog() override;

    void fetchChangelog(const QString& changelogUrl);

signals:
    void updateNow(const QString &targetDownloadUrl, const QString &expectedHash);

private slots:
    void nowButton_Clicked();
    void remindButton_Clicked();
    void skipButton_Clicked();

private:
    Ui::UpdateDialog *ui;
    QString m_versionString;
    QString m_releaseDateString;
    QString m_downloadUrl;
    QString m_changelogUrl;
    QString m_expectedHash;

};
