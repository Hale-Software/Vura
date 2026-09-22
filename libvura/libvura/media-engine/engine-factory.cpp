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

#include "engine-factory.h"

#include "engine.h"
#include "null-engine.h"

#include <QCoreApplication>
#include <QDebug>

#ifdef VURA_HAVE_QTMULTIMEDIA
#include "qt-engine.h"
#endif

#ifdef VURA_HAVE_MPV
#include "mpv-engine.h"
#endif

#ifdef VURA_HAVE_OPENGL
#include "opengl-engine.h"
#endif

#include <stdexcept>

namespace media {
namespace {

bool hasQtMultimedia()
{
#ifdef VURA_HAVE_QTMULTIMEDIA
    return true;
#else
    return false;
#endif
}

bool hasMpv()
{
#ifdef VURA_HAVE_MPV
    return true;
#else
    return false;
#endif
}

bool hasOpenGL()
{
#ifdef VURA_HAVE_OPENGL
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
#ifdef VURA_HAVE_QTMULTIMEDIA
            return std::make_unique<QtEngine>();
#else
            if (errorOut)
                *errorOut = QCoreApplication::translate("media", "This build does not include the Qt Multimedia backend.");
            return nullptr;
#endif

        case Backend::Mpv:
#ifdef VURA_HAVE_MPV
            try {
                return std::make_unique<MpvEngine>();
            } catch (const std::exception &e) {
                if (errorOut)
                    *errorOut = QString::fromUtf8(e.what());
                return nullptr;
            }
#else
            if (errorOut)
                *errorOut = QCoreApplication::translate("media", "This build does not include the mpv backend.");
            return nullptr;
#endif

        case Backend::OpenGL:
#ifdef VURA_HAVE_OPENGL
            try {
                return std::make_unique<OpenGLEngine>();
            } catch (const std::exception &e) {
                if (errorOut)
                    *errorOut = QString::fromUtf8(e.what());
                return nullptr;
            }
#else
            if (errorOut)
                *errorOut = QCoreApplication::translate("media", "This build does not include the OpenGL backend.");
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
        {Backend::Mpv, QStringLiteral("mpv"), QCoreApplication::translate("media", "mpv"), hasMpv()},
        {Backend::QtMultimedia, QStringLiteral("qtmultimedia"),QCoreApplication::translate("media", "Qt Multimedia"), hasQtMultimedia()},
        {Backend::OpenGL, QStringLiteral("opengl"), QCoreApplication::translate("media", "opengl"), hasOpenGL()},
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
    for (Backend candidate : {Backend::QtMultimedia, Backend::OpenGL, Backend::Mpv, Backend::Null}) {
        if (candidate == backend)
            continue;
        QString candidateError;
        if (auto engine = tryCreate(candidate, &candidateError)) {
            if (backend != Backend::Auto && errorOut) {
                *errorOut = QCoreApplication::translate("media", "Falling back to the %1 backend: %2")
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
