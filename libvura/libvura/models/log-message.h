#pragma once

#include <QString>


enum LogLevel
{
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

struct LogMessage
{
    QString timestamp;
    LogLevel level;
    QString component;
    QString message;
    QString fullText = "";
};
