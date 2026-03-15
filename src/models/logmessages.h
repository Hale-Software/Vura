#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

class LogMessages : public QObject
{
    Q_OBJECT
public:
    LogMessages();

    QStringList getAllMessages();

public slots:
    void append(QString message);

signals:
    void newMessage(QString newMessage);

private:
    QStringList h_messages;

};
