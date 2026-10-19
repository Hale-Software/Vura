#include "sleep-inhibitor.h"

#include <QCoreApplication>
#include <QDebug>

#include <windows.h>


SleepInhibitor::SleepInhibitor(QObject *parent) : QObject(parent) {}

SleepInhibitor::~SleepInhibitor()
{
    if (m_inhibited)
        release();
}

void SleepInhibitor::setInhibited(bool inhibited, const QString &reason)
{
    if (m_inhibited == inhibited)
        return;

    if (inhibited)
        acquire(reason.isEmpty() ? tr("Playing media") : reason);
    else
        release();

    m_inhibited = inhibited;
}

void SleepInhibitor::acquire(const QString &reason)
{
    Q_UNUSED(reason)
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
}

void SleepInhibitor::release()
{
    SetThreadExecutionState(ES_CONTINUOUS);
}
