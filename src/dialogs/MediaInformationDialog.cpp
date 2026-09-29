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

#include "MediaInformationDialog.h"
#include "ui_MediaInformationDialog.h"

#include <QSize>


MediaInformationDialog::MediaInformationDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::MediaInformationDialog)
{
    ui->setupUi(this);

    connect(ui->close, &QPushButton::clicked, this, &MediaInformationDialog::close_Clicked);
    connect(ui->fingerprint, &QPushButton::clicked, this, &MediaInformationDialog::fingerprint_Clicked);
}

MediaInformationDialog::~MediaInformationDialog()
{
    delete ui;
}

void MediaInformationDialog::setMetaData(const QVariantMap &metaData)
{
    m_metaData = metaData;

    const QString location = metaData.value(QLatin1String(media::meta::Url)).toString();
    if (!location.isEmpty())
        ui->location->setText(location);

    const QString title = metaData.value(QLatin1String(media::meta::Title)).toString();
    if (!title.isEmpty())
        ui->title->setText(title);

    const QString author = metaData.value(QLatin1String(media::meta::Author)).toString();
    if (!author.isEmpty())
        ui->author->setText(author);

    const QString comments = metaData.value(QLatin1String(media::meta::Comment)).toString();
    if (!comments.isEmpty())
        ui->comments->setText(comments);

    const QString copyright = metaData.value(QLatin1String(media::meta::Copyright)).toString();
    if (!copyright.isEmpty())
        ui->copyright->setText(copyright);

    const QString date = metaData.value(QLatin1String(media::meta::Date)).toString();
    if (!date.isEmpty())
        ui->date->setText(date);

    const QSize res = metaData.value(QLatin1String(media::meta::Resolution)).toSize();
    if (res.isValid())
        ui->resolution->setText(QString("%1x%2").arg(res.width()).arg(res.height()));

    const QString genre = metaData.value(QLatin1String(media::meta::Genre)).toString();
    if (!genre.isEmpty())
        ui->genre->setText(genre);

    const QString language = metaData.value(QLatin1String(media::meta::Language)).toString();
    if (!language.isEmpty())
        ui->language->setText(language);

    //ui->publisher->setText(metaData.Publisher);
    //ui->trackNumber->setText(QString::number(metaData.TrackNumber));
}

void MediaInformationDialog::close_Clicked()
{
    this->close();
}

void MediaInformationDialog::fingerprint_Clicked() {}
