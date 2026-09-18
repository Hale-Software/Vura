#include <QString>
#include <QRegularExpressionMatch>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

#include "subtitle-track.h"


SubtitleTrack::SubtitleTrack(QObject *parent) : QObject(parent) {}

bool SubtitleTrack::loadSrt(const QString &filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);

    static const QRegularExpression timeRe(
        R"((\d+):(\d\d):(\d\d)[,.](\d{1,3})\s*-->\s*(\d+):(\d\d):(\d\d)[,.](\d{1,3}))");

    m_cues.clear();
    SubtitleCue cur;
    bool inCue = false;

    while (!in.atEnd()) {
        const QString line = in.readLine();
        const auto m = timeRe.match(line);

        if (m.hasMatch()) {
            if (inCue && !cur.lines.isEmpty())
                m_cues.append(cur);
            cur = SubtitleCue{ parseTs(m, 1), parseTs(m, 5), {} };
            inCue = true;
        } else if (inCue) {
            if (line.trimmed().isEmpty()) {
                if (!cur.lines.isEmpty())
                    m_cues.append(cur);
                cur = {};
                inCue = false;
            } else {
                cur.lines.append(line.trimmed());
            }
        }
    }
    if (inCue && !cur.lines.isEmpty())
        m_cues.append(cur);

    std::stable_sort(m_cues.begin(), m_cues.end(),
                     [](const SubtitleCue &a, const SubtitleCue &b) { return a.start < b.start; });
    return !m_cues.isEmpty();
}

SubtitleCue *SubtitleTrack::cueAt(qint64 pos)
{
    auto it = std::upper_bound(m_cues.begin(), m_cues.end(), pos,
        [](qint64 p, const SubtitleCue &c) { return p < c.start; });

    for (int back = 0; back < 4 && it != m_cues.begin(); ++back) {
        --it;
        if (pos >= it->start && pos < it->end)
            return &*it;
        if (it->end < pos - 30000)
            break;
    }
    return nullptr;
}

qint64 SubtitleTrack::parseTs(const QRegularExpressionMatch &m, int base)
{
    return m.captured(base).toLongLong() * 3600000
         + m.captured(base + 1).toLongLong() * 60000
         + m.captured(base + 2).toLongLong() * 1000
         + m.captured(base + 3).leftJustified(3, '0').left(3).toLongLong();
}
