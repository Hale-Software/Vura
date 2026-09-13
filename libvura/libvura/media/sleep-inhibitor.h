#pragma once

#include <QObject>
#include <QString>


class SleepInhibitor : public QObject
{
    Q_OBJECT
public:
    explicit SleepInhibitor(QObject *parent = nullptr);
    ~SleepInhibitor() override;

    void setInhibited(bool inhibited, const QString &reason = {});
    bool isInhibited() const { return m_inhibited; }

private:
    void acquire(const QString &reason);
    void release();

    bool m_inhibited = false;
    quint32 m_cookie = 0;
    
};
