#include "sleep-inhibitor.h"

#include <QCoreApplication>
#include <QDebug>

#if defined(MEDIA_HAVE_DBUS)
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
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
#if defined(MEDIA_HAVE_DBUS)
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
#if defined(MEDIA_HAVE_DBUS)
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
