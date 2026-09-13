#pragma once

#include <QObject>

#include "engine.h"


class MediaController : public QObject
{
    Q_OBJECT
public:
    void setEngine(std::unique_ptr<Engine> engine);

private:
    std::unique_ptr<Engine> m_engine;
    Playlist m_playlist;
    qint64 m_cachedPosition = 0;

};
