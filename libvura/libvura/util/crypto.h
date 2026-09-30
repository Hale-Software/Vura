#pragma once

#include <QObject>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>


class Crypto : public QObject
{
    Q_OBJECT
public:
    static QString fingerprintFile(const QString &path, qint64 sampleSize = 16384);

};
