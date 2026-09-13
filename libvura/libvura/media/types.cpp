#include "types.h"

#include <QCoreApplication>
#include <QTime>

namespace media {

bool isRecoverable(ErrorKind kind)
{
    switch (kind) {
    case ErrorKind::NetworkFailure:
    case ErrorKind::DecodeFailure:
        return true;
    default:
        return false;
    }
}

QString describe(ErrorKind kind)
{
    switch (kind) {
    case ErrorKind::None:
        return {};
    case ErrorKind::NotFound:
        return QCoreApplication::translate("media", "The file could not be found.");
    case ErrorKind::AccessDenied:
        return QCoreApplication::translate("media", "You do not have permission to read this file.");
    case ErrorKind::UnsupportedFormat:
        return QCoreApplication::translate("media", "This format is not supported.");
    case ErrorKind::MissingCodec:
        return QCoreApplication::translate("media", "A codec needed to play this file is missing.");
    case ErrorKind::NetworkFailure:
        return QCoreApplication::translate("media", "The stream stopped responding.");
    case ErrorKind::DecodeFailure:
        return QCoreApplication::translate("media", "Decoding failed partway through the file.");
    case ErrorKind::Internal:
        return QCoreApplication::translate("media", "The playback engine reported an internal error.");
    }
    return {};
}

QString TrackInfo::displayName(int fallbackIndex) const
{
    QStringList parts;
    if (!title.isEmpty())
        parts << title;
    if (!language.isEmpty())
        parts << QStringLiteral("[%1]").arg(language);
    if (parts.isEmpty())
        parts << QCoreApplication::translate("media", "Track %1").arg(fallbackIndex + 1);
    if (!codec.isEmpty())
        parts << QStringLiteral("(%1)").arg(codec);
    return parts.join(QLatin1Char(' '));
}

QString formatTime(Msec ms)
{
    if (ms < 0)
        return QStringLiteral("--:--");
    const qint64 totalSeconds = ms / 1000;
    const QTime t(int(totalSeconds / 3600), int((totalSeconds / 60) % 60), int(totalSeconds % 60));
    return totalSeconds >= 3600 ? t.toString(QStringLiteral("h:mm:ss"))
                                : t.toString(QStringLiteral("m:ss"));
}

} // namespace media
