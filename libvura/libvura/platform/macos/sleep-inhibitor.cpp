#include "sleep-inhibitor.h"

#include <QCoreApplication>
#include <QDebug>

#include <IOKit/pwr_mgt/IOPMLib.h>


namespace {
IOPMAssertionID g_assertionId = 0;
}


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
    CFStringRef cfReason = CFStringCreateWithCString(kCFAllocatorDefault, reason.toUtf8().constData(), kCFStringEncodingUTF8);
    IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep, kIOPMAssertionLevelOn, cfReason, &g_assertionId);
    if (cfReason)
        CFRelease(cfReason);
}

void SleepInhibitor::release()
{
    if (g_assertionId != 0) {
        IOPMAssertionRelease(g_assertionId);
        g_assertionId = 0;
    }
}
