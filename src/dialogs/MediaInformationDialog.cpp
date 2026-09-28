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

    //ui->location->setText(metaData.Source.toString());

    const QString title = metaData.value(QLatin1String(media::meta::Title)).toString();
    if (!title.isEmpty())
        ui->title->setText(title);

    const QString artist = metaData.value(QLatin1String(media::meta::Artist)).toString();
    if (!artist.isEmpty())
        ui->author->setText(artist);

    //ui->comments->setText(metaData.Comment);
    //ui->copyright->setText(metaData.Copyright);
    //ui->date->setText(metaData.Date.toString());

    const QSize res = metaData.value(QLatin1String(media::meta::Resolution)).toSize();
    if (res.isValid())
        ui->resolution->setText(QString("%1x%2").arg(res.width()).arg(res.height()));

    //if (!metaData.Genre.isEmpty())
    //    ui->genre->setText(metaData.Genre.join(", "));

    //ui->language->setText(QLocale::languageToString(metaData.Language));

    //ui->publisher->setText(metaData.Publisher);
    //ui->trackNumber->setText(QString::number(metaData.TrackNumber));
}

void MediaInformationDialog::close_Clicked()
{
    this->close();
}

void MediaInformationDialog::fingerprint_Clicked() {}
