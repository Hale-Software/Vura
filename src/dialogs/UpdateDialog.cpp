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

#include "UpdateDialog.h"
#include "ui_UpdateDialog.h"


UpdateDialog::UpdateDialog(const QString& versionString, const QString& releaseDateString, const QString& downloadUrl,
                                 const QString& changelogUrl, const QString &expectedHash, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateDialog),
    m_versionString(versionString),
    m_releaseDateString(releaseDateString),
    m_downloadUrl(downloadUrl),
    m_changelogUrl(changelogUrl),
    m_expectedHash(expectedHash)
{
    ui->setupUi(this);

    ui->label->setText(QString("Version %1 (%2)").arg(versionString).arg(releaseDateString));
    fetchChangelog(m_changelogUrl);

    connect(ui->nowButton, &QPushButton::clicked, this, &UpdateDialog::nowButton_Clicked);
    connect(ui->remindButton, &QPushButton::clicked, this, &UpdateDialog::remindButton_Clicked);
    connect(ui->skipButton, &QPushButton::clicked, this, &UpdateDialog::skipButton_Clicked);
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::fetchChangelog(const QString &changelogUrl)
{
    auto *manager = new QNetworkAccessManager(this);
    QNetworkRequest request((QUrl(changelogUrl)));
    QNetworkReply *reply = manager->get(request);

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QString htmlContent = reply->readAll();
            ui->text->setHtml(htmlContent);

        }
        reply->deleteLater();
    });
}

void UpdateDialog::nowButton_Clicked()
{
    qDebug() << "Update now requested by user";
    emit updateNow(m_downloadUrl, m_expectedHash);
}

void UpdateDialog::remindButton_Clicked()
{
    qDebug() << "Update remind me later requested by user";
    this->close();
}

void UpdateDialog::skipButton_Clicked()
{
    qDebug() << "Update version [" << m_versionString << "] skipped by user";
    QSettings settings;
    settings.setValue("lastCheckedVersion", m_versionString);
    settings.sync();
    this->close();
}
