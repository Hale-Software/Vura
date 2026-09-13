#pragma once

#include <QString>
#include <QVector>

#include <memory>


namespace media {

class Engine;

enum class Backend
{
    Auto,
    Null,
    QtMultimedia,
    Mpv
};

struct BackendInfo
{
    Backend backend;
    QString id;
    QString displayName;
    bool available;
};

QVector<BackendInfo> availableBackends();

Backend backendFromId(const QString &id, bool *ok = nullptr);
QString idForBackend(Backend backend);

std::unique_ptr<Engine> createEngine(Backend backend, QString *errorOut = nullptr);

}
