/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "system-exception-handler.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>

#if defined(Q_OS_LINUX)
#  include "client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN)
#  include "client/windows/handler/exception_handler.h"
#elif defined(Q_OS_MACOS)
#  include "client/mac/handler/exception_handler.h"
#endif

#if defined(Q_OS_LINUX)
static bool exceptionHandlerCallback(const google_breakpad::MinidumpDescriptor& descriptor,
                                     void* /*context*/,
                                     bool succeeded)
{
    if (!succeeded)
        return succeeded;

    const QStringList argumentList = {
        QString::fromLocal8Bit(descriptor.path()),
        QString::number(SystemExceptionHandler::startTime().toTime_t()),
        QCoreApplication::applicationName(),
        QCoreApplication::applicationVersion(),
        SystemExceptionHandler::plugins(),
        SystemExceptionHandler::buildVersion(),
        QCoreApplication::applicationFilePath()
    };

	qWarning() << "[execute crash handler]" << SystemExceptionHandler::crashHandlerPath();
	qWarning() << argumentList;
    return !QProcess::execute(SystemExceptionHandler::crashHandlerPath(), argumentList);
}
#elif defined(Q_OS_MACOS)
static bool exceptionHandlerCallback(const char *dump_dir,
                                     const char *minidump_id,
                                     void *context,
                                     bool succeeded)
{
    Q_UNUSED(context);

    if (!succeeded)
        return succeeded;

    const QString path = QString::fromLocal8Bit(dump_dir) + '/'
            + QString::fromLocal8Bit(minidump_id) + ".dmp";
    const QStringList argumentList = {
        path,
        QString::number(SystemExceptionHandler::startTime().toTime_t()),
        QCoreApplication::applicationName(),
        QCoreApplication::applicationVersion(),
        SystemExceptionHandler::plugins(),
        SystemExceptionHandler::buildVersion(),
        QCoreApplication::applicationFilePath()
    };

	qWarning() << "[execute crash handler]" << SystemExceptionHandler::crashHandlerPath();
	qWarning() << argumentList;
    return !QProcess::execute(SystemExceptionHandler::crashHandlerPath(), argumentList);
}
#elif defined(Q_OS_WIN)
static bool exceptionHandlerCallback(const wchar_t* dump_path,
                                     const wchar_t* minidump_id,
                                     void* context,
                                     EXCEPTION_POINTERS* exinfo,
                                     MDRawAssertionInfo* assertion,
                                     bool succeeded)
{
    Q_UNUSED(assertion);
    Q_UNUSED(exinfo);
    Q_UNUSED(context);

    if (!succeeded)
        return succeeded;

    const QString path = QString::fromWCharArray(dump_path, int(wcslen(dump_path))) + '/'
            + QString::fromWCharArray(minidump_id, int(wcslen(minidump_id))) + ".dmp";

    const QStringList argumentList = {
        path,
        QString::number(SystemExceptionHandler::startTime().toTime_t()),
        QCoreApplication::applicationName(),
        QCoreApplication::applicationVersion(),
        SystemExceptionHandler::plugins(),
        SystemExceptionHandler::buildVersion(),
        QCoreApplication::applicationFilePath()
    };

	qWarning() << "[execute crash handler]" << SystemExceptionHandler::crashHandlerPath();
	qWarning() << argumentList;
    return !QProcess::execute(SystemExceptionHandler::crashHandlerPath(), argumentList);
}
#endif

static QDir getAppTempDir()
{
	QDir temp_dir = QDir::tempPath();
	QString org_name = QCoreApplication::organizationName();
	QString app_name = QCoreApplication::applicationName();

	if (!org_name.isEmpty() && !app_name.isEmpty())
		temp_dir = temp_dir.filePath(org_name + "." + app_name);
	else if (!org_name.isEmpty())
		temp_dir = temp_dir.filePath(org_name);
	else if (!app_name.isEmpty())
		temp_dir = temp_dir.filePath(app_name);

	temp_dir.mkdir(".");
	return temp_dir;
}

static QDateTime s_startTime;
static QString s_plugins;
static QString s_buildVersion;
static QString s_crashHandlerPath;

#if defined(Q_OS_LINUX)
SystemExceptionHandler::SystemExceptionHandler(const QString &libexecPath)
    : exceptionHandler(new google_breakpad::ExceptionHandler(
                           google_breakpad::MinidumpDescriptor(QDir::tempPath().toStdString()),
                           NULL,
                           exceptionHandlerCallback,
                           NULL,
                           true,
                           -1))
{
    init(libexecPath);
}
#elif defined(Q_OS_MACOS)
SystemExceptionHandler::SystemExceptionHandler(const QString &libexecPath)
    : exceptionHandler(new google_breakpad::ExceptionHandler(
                           QDir::tempPath().toStdString(),
                           NULL,
                           exceptionHandlerCallback,
                           NULL,
                           true,
                           NULL))
{
    init(libexecPath);
}
#elif defined(Q_OS_WIN)
SystemExceptionHandler::SystemExceptionHandler(const QString &libexecPath)
    : exceptionHandler(new google_breakpad::ExceptionHandler(
                           getAppTempDir().absolutePath().toStdWString(),
                           NULL,
                           exceptionHandlerCallback,
                           NULL,
                           google_breakpad::ExceptionHandler::HANDLER_ALL))
{
    init(libexecPath);
}
#else
SystemExceptionHandler::SystemExceptionHandler(const QString & /*libexecPath*/)
    : exceptionHandler(0)
{

}
#endif

void SystemExceptionHandler::init(const QString &libexecPath)
{
    s_startTime = QDateTime::currentDateTime();

	s_crashHandlerPath = libexecPath + "/CrashHandler";

#ifdef Q_OS_WIN
    s_crashHandlerPath = libexecPath + "/CrashHandler.exe";
#endif

#ifdef Q_OS_MACOS
    s_crashHandlerPath = libexecPath + "/CrashHandler";
#endif
}

SystemExceptionHandler::~SystemExceptionHandler()
{
#ifdef ENABLE_QT_BREAKPAD
    delete exceptionHandler;
#endif
}

void SystemExceptionHandler::setPlugins(const QStringList &pluginNameList)
{
    s_plugins = QString("{%1}").arg(pluginNameList.join(","));
}

void SystemExceptionHandler::setBuildVersion(const QString &version)
{
    s_buildVersion = version;
}

QString SystemExceptionHandler::buildVersion()
{
    return s_buildVersion;
}

QString SystemExceptionHandler::plugins()
{
    return s_plugins;
}

void SystemExceptionHandler::setCrashHandlerPath(const QString &crashHandlerPath)
{
    s_crashHandlerPath = crashHandlerPath;
}

QString SystemExceptionHandler::crashHandlerPath()
{
    return s_crashHandlerPath;
}

void SystemExceptionHandler::crash()
{
  int *a = (int*)0x42;

  fprintf(stdout, "Going to crash...\n");
  fprintf(stdout, "A = %d", *a);
}

QDateTime SystemExceptionHandler::startTime()
{
    return s_startTime;
}
