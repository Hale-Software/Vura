#include "sleep-inhibitor.h"

#include <QCoreApplication>
#include <QDebug>

#if defined(Q_OS_WIN)
#include <windows.h>
#elif defined(Q_OS_MACOS)
#include <IOKit/pwr_mgt/IOPMLib.h>
#elif defined(Q_OS_LINUX) && defined(MEDIA_HAVE_DBUS)
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#endif

#if defined(Q_OS_MACOS)
namespace {
IOPMAssertionID g_assertionId = 0;
}
#endif


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
#if defined(Q_OS_WIN)
    Q_UNUSED(reason)
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);

#elif defined(Q_OS_MACOS)
    CFStringRef cfReason = CFStringCreateWithCString(kCFAllocatorDefault, reason.toUtf8().constData(), kCFStringEncodingUTF8);
    IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep, kIOPMAssertionLevelOn, cfReason, &g_assertionId);
    if (cfReason)
        CFRelease(cfReason);

#elif defined(Q_OS_LINUX) && defined(MEDIA_HAVE_DBUS)
    QDBusInterface screensaver(QStringLiteral("org.freedesktop.ScreenSaver"),
                               QStringLiteral("/org/freedesktop/ScreenSaver"),
                               QStringLiteral("org.freedesktop.ScreenSaver"),
                               QDBusConnection::sessionBus());
    if (!screensaver.isValid())
        return;

    const QDBusReply<quint32> reply =
            screensaver.call(QStringLiteral("Inhibit"),
                             QCoreApplication::applicationName(), reason);
    if (reply.isValid())
        m_cookie = reply.value();

#else
    Q_UNUSED(reason)
#endif
}

void SleepInhibitor::release()
{
#if defined(Q_OS_WIN)
    SetThreadExecutionState(ES_CONTINUOUS);

#elif defined(Q_OS_MACOS)
    if (g_assertionId != 0) {
        IOPMAssertionRelease(g_assertionId);
        g_assertionId = 0;
    }

#elif defined(Q_OS_LINUX) && defined(MEDIA_HAVE_DBUS)
    if (m_cookie == 0)
        return;

    QDBusInterface screensaver(QStringLiteral("org.freedesktop.ScreenSaver"),
                               QStringLiteral("/org/freedesktop/ScreenSaver"),
                               QStringLiteral("org.freedesktop.ScreenSaver"),
                               QDBusConnection::sessionBus());
    if (screensaver.isValid())
        screensaver.call(QStringLiteral("UnInhibit"), m_cookie);
    m_cookie = 0;
#endif
}
