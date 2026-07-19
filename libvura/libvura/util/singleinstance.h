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

#pragma once

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QStringList>
#include <QDataStream>


class SingleInstance : public QObject
{
    Q_OBJECT

public:
    explicit SingleInstance(QObject *parent = nullptr);

    void listen(const QString &name);

    // Returns true if a previous instance is running.
    // If argc/argv contain a path argument, it is forwarded to the running instance.
    static bool hasPrevious(const QString &name, int argc, char *argv[]);

signals:
    void newInstance();

    // Emitted when the running instance receives a path from a second launch.
    // |path| is the file or folder path passed by the second instance.
    void openPathRequested(const QString &path);

private slots:
    void onNewConnection();

private:
    QLocalServer m_server;
};
