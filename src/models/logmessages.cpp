#include "logmessages.h"

LogMessages::LogMessages() {}

QStringList LogMessages::getAllMessages()
{
    return h_messages;
}

void LogMessages::append(QString message) {
    h_messages.append(message);
    emit newMessage(message);
}
