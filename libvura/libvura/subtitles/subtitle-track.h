#pragma once

#include <QObject>
#include <QList>

#include "../models/subtitle-cue.h"


class QString;
class QRegularExpressionMatch;

class SubtitleTrack : public QObject
{
    Q_OBJECT
public:
    explicit SubtitleTrack(QObject *parent = nullptr);

    bool loadSrt(const QString &filePath);
    SubtitleCue *cueAt(qint64 pos);

private:
    static qint64 parseTs(const QRegularExpressionMatch &m, int base);

    QList<SubtitleCue> m_cues;

};
