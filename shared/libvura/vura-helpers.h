#pragma once

#include <QFileInfo>
#include <QString>
#include <QByteArray>


class VuraHelpers
{
public:
    static void simulateApplicationCrash();
    static bool fileExists(const QString &fileName);
    static const char *QStringToChar(const QString &text);
    static int qint64ToInt(const qint64 &number);

};

