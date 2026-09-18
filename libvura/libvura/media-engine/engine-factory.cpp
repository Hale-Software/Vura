#include "engine-factory.h"

#include "engine.h"
#include "null-engine.h"

#include <QCoreApplication>
#include <QDebug>

#ifdef MEDIA_HAVE_QTMULTIMEDIA
#include "qt-engine.h"
#endif

#ifdef MEDIA_HAVE_MPV
#include "mpv-engine.h"
#endif

#include <stdexcept>

namespace media {
namespace {

bool hasQtMultimedia()
{
#ifdef MEDIA_HAVE_QTMULTIMEDIA
    return true;
#else
    return false;
#endif
}

bool hasMpv()
{
#ifdef MEDIA_HAVE_MPV
    return true;
#else
    return false;
#endif
}

std::unique_ptr<Engine> tryCreate(Backend backend, QString *errorOut)
{
    switch (backend) {
    case Backend::Null:
        return std::make_unique<NullEngine>();

    case Backend::QtMultimedia:
#ifdef MEDIA_HAVE_QTMULTIMEDIA
        return std::make_unique<QtEngine>();
#else
        if (errorOut)
            *errorOut = QCoreApplication::translate(
                    "media", "This build does not include the Qt Multimedia backend.");
        return nullptr;
#endif

    case Backend::Mpv:
#ifdef MEDIA_HAVE_MPV
        try {
            return std::make_unique<MpvEngine>();
        } catch (const std::exception &e) {
            if (errorOut)
                *errorOut = QString::fromUtf8(e.what());
            return nullptr;
        }
#else
        if (errorOut)
            *errorOut = QCoreApplication::translate(
                    "media", "This build does not include the mpv backend.");
        return nullptr;
#endif

    case Backend::Auto:
        break;
    }
    return nullptr;
}

} // namespace

QVector<BackendInfo> availableBackends()
{
    return {
        {Backend::Mpv, QStringLiteral("mpv"), QCoreApplication::translate("media", "mpv"),
         hasMpv()},
        {Backend::QtMultimedia, QStringLiteral("qtmultimedia"),
         QCoreApplication::translate("media", "Qt Multimedia"), hasQtMultimedia()},
        {Backend::Null, QStringLiteral("null"),
         QCoreApplication::translate("media", "None (simulated)"), true},
    };
}

Backend backendFromId(const QString &id, bool *ok)
{
    if (ok)
        *ok = true;
    if (id.compare(QLatin1String("auto"), Qt::CaseInsensitive) == 0)
        return Backend::Auto;
    for (const BackendInfo &info : availableBackends()) {
        if (info.id.compare(id, Qt::CaseInsensitive) == 0)
            return info.backend;
    }
    if (ok)
        *ok = false;
    return Backend::Auto;
}

QString idForBackend(Backend backend)
{
    if (backend == Backend::Auto)
        return QStringLiteral("auto");
    for (const BackendInfo &info : availableBackends()) {
        if (info.backend == backend)
            return info.id;
    }
    return QStringLiteral("null");
}

std::unique_ptr<Engine> createEngine(Backend backend, QString *errorOut)
{
    QString error;

    if (backend != Backend::Auto) {
        if (auto engine = tryCreate(backend, &error))
            return engine;
        qWarning() << "Backend" << idForBackend(backend) << "unavailable:" << error;
    }

    // Preference order for Auto and for fallback. mpv first because it is
    // the most capable; the null engine is the floor and always works.
    for (Backend candidate : {Backend::Mpv, Backend::QtMultimedia, Backend::Null}) {
        if (candidate == backend)
            continue;
        QString candidateError;
        if (auto engine = tryCreate(candidate, &candidateError)) {
            if (backend != Backend::Auto && errorOut) {
                *errorOut = QCoreApplication::translate(
                                    "media", "Falling back to the %1 backend: %2")
                                    .arg(engine->name(), error);
            }
            return engine;
        }
    }

    if (errorOut)
        *errorOut = error;
    return std::make_unique<NullEngine>();
}

}
