#pragma once

#include <QMediaMetaData>
#include <QDateTime>
#include <QLocale>
#include <QStringList>
#include <QUrl>
#include <QMediaFormat>
#include <QImage>
#include <QSize>


struct MetaData
{
    QUrl Source = QUrl();
    QString Title = QString();
    QStringList Author = QStringList();
    QString Comment = QString();
    QString Description = QString();
    QStringList Genre = QStringList();
    QDateTime Date = QDateTime();
    QLocale::Language Language = QLocale::Language();
    QString Publisher = QString();
    QString Copyright = QString();
    QUrl Url = QUrl();
    QString MediaType = QString();
    QMediaFormat::FileFormat FileFormat = QMediaFormat::FileFormat();
    qint64 Duration = 0;
    int AudioBitRate = 0;
    QMediaFormat::AudioCodec AudioCodec = QMediaFormat::AudioCodec();
    qreal VideoFrameRate = 0.0;
    int VideoBitRate = 0;
    QMediaFormat::VideoCodec VideoCodec = QMediaFormat::VideoCodec();
    bool HasHdrContent = false;
    QString AlbumTitle = QString();
    QString AlbumArtist = QString();
    QStringList ContributingArtist = QStringList();
    int TrackNumber = 0;
    QStringList Composer = QStringList();
    QStringList LeadPerformer = QStringList();
    QImage ThumbnailImage = QImage();
    QImage CoverArtImage = QImage();
    int Orientation = 0;
    QSize Resolution = QSize();

    MetaData parseQMediaMetaData(QMediaMetaData metaData)
    {
        MetaData m;

        m.Title = metaData.value(QMediaMetaData::Title).toString();
        m.Author = metaData.value(QMediaMetaData::Author).toStringList();
        m.Comment = metaData.value(QMediaMetaData::Comment).toString();
        m.Description = metaData.value(QMediaMetaData::Description).toString();
        m.Genre = metaData.value(QMediaMetaData::Genre).toStringList();
        m.Date = metaData.value(QMediaMetaData::Date).toDateTime();
        m.Publisher = metaData.value(QMediaMetaData::Publisher).toString();
        m.Copyright = metaData.value(QMediaMetaData::Copyright).toString();
        m.Url = metaData.value(QMediaMetaData::Url).toUrl();
        m.MediaType = metaData.value(QMediaMetaData::MediaType).toString();
        m.Duration = metaData.value(QMediaMetaData::Duration).toInt();
        m.AudioBitRate = metaData.value(QMediaMetaData::AudioBitRate).toInt();
        m.VideoFrameRate = metaData.value(QMediaMetaData::VideoFrameRate).toReal();
        m.VideoBitRate = metaData.value(QMediaMetaData::VideoBitRate).toInt();
        m.HasHdrContent = metaData.value(QMediaMetaData::HasHdrContent).toBool();
        m.AlbumTitle = metaData.value(QMediaMetaData::AlbumTitle).toString();
        m.AlbumArtist = metaData.value(QMediaMetaData::AlbumArtist).toString();
        m.ContributingArtist = metaData.value(QMediaMetaData::ContributingArtist).toStringList();
        m.TrackNumber = metaData.value(QMediaMetaData::TrackNumber).toInt();
        m.Composer = metaData.value(QMediaMetaData::Composer).toStringList();
        m.LeadPerformer = metaData.value(QMediaMetaData::LeadPerformer).toStringList();
        m.Orientation = metaData.value(QMediaMetaData::Orientation).toInt();
        m.Resolution = metaData.value(QMediaMetaData::Resolution).toSize();

        QLocale lang = metaData.value(QMediaMetaData::Language).toLocale();
        m.Language = lang.language();

        QVariant fileFormat = metaData.value(QMediaMetaData::FileFormat);
        if (fileFormat.isValid())
            m.FileFormat = fileFormat.value<QMediaFormat::FileFormat>();

        QVariant audioCodec = metaData.value(QMediaMetaData::AudioCodec);
        if (audioCodec.isValid())
            m.AudioCodec = audioCodec.value<QMediaFormat::AudioCodec>();

        QVariant videoCodec = metaData.value(QMediaMetaData::VideoCodec);
        if (videoCodec.isValid())
            m.VideoCodec = videoCodec.value<QMediaFormat::VideoCodec>();

        const QVariant thumbnail = metaData.value(QMediaMetaData::ThumbnailImage);
        if (thumbnail.isValid() && thumbnail.canConvert<QImage>()) {
            m.ThumbnailImage = thumbnail.value<QImage>();
        }

        QVariant coverArtImage = metaData.value(QMediaMetaData::CoverArtImage);
        if (coverArtImage.isValid() && coverArtImage.canConvert<QImage>()) {
            m.CoverArtImage = coverArtImage.value<QImage>();
        }

        return m;
    }
};
