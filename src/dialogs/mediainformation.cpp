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

#include "mediainformation.h"
#include "ui_mediainformation.h"


MediaInformation::MediaInformation(QWidget *parent) : QDialog(parent), ui(new Ui::MediaInformation)
{
    ui->setupUi(this);

    connect(ui->close, &QPushButton::clicked, this, &MediaInformation::close_Clicked);
    connect(ui->fingerprint, &QPushButton::clicked, this, &MediaInformation::fingerprint_Clicked);
}

MediaInformation::~MediaInformation()
{
    delete ui;
}

void MediaInformation::setMediaInformation(const QString &filePath, const QMediaMetaData &metaData)
{
    ui->location->setText(filePath);

    ui->author->setText(metaData.value(QMediaMetaData::Author).toString());
    ui->comments->setText(metaData.value(QMediaMetaData::Comment).toString());
    ui->copyright->setText(metaData.value(QMediaMetaData::Copyright).toString());
    ui->date->setText(metaData.value(QMediaMetaData::Date).toString());
    ui->resolution->setText(metaData.value(QMediaMetaData::Resolution).toString());
    ui->genre->setText(metaData.value(QMediaMetaData::Genre).toString());
    ui->language->setText(metaData.value(QMediaMetaData::Language).toString());
    ui->publisher->setText(metaData.value(QMediaMetaData::Publisher).toString());
    ui->trackNumber->setText(metaData.value(QMediaMetaData::TrackNumber).toString());
    ui->title->setText(metaData.value(QMediaMetaData::Title).toString());

}

void MediaInformation::close_Clicked()
{
    this->close();
}

void MediaInformation::fingerprint_Clicked() {}
