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

#include "singleinstance.h"

SingleInstance::SingleInstance(QObject *parent) : QObject(parent)
{
    connect(&m_server, &QLocalServer::newConnection, [this]() { emit newInstance(); });
}

void SingleInstance::listen(const QString &name)
{
    QLocalServer::removeServer(name);
    m_server.listen(name);
}

bool SingleInstance::hasPrevious(const QString &name, int argc, char *argv[])
{
    QLocalSocket socket;
    socket.connectToServer(name, QLocalSocket::ReadOnly);

    return socket.waitForConnected();
}

