/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

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

#include "single-instance-controller.h"


SingleInstanceController::SingleInstanceController(const QString &uniqueKey, QObject *parent)
    : QObject(parent), m_serverName(uniqueKey) {}

bool SingleInstanceController::checkForExistingInstance(const QStringList &args)
{
    qDebug() << "Checking for existing instance...";
    QLocalSocket socket;
    socket.connectToServer(m_serverName);

    if (socket.waitForConnected(500)) {
        qDebug() << "Connected to existing instance";
        QDataStream stream(&socket);
        stream.setVersion(QDataStream::Qt_6_0);

        stream << args;
        socket.waitForBytesWritten(1000);
        return true;
    }

    qDebug() << "No existing instance found";
    QLocalServer::removeServer(m_serverName);

    qDebug() << "Starting local instance server...";
    m_localServer = new QLocalServer(this);
    connect(m_localServer, &QLocalServer::newConnection, this, &SingleInstanceController::handleNewConnection);
    m_localServer->listen(m_serverName);

    return false;
}

void SingleInstanceController::handleNewConnection()
{
    QLocalSocket *socket = m_localServer->nextPendingConnection();
    if (!socket) return;

    connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
        QDataStream stream(socket);
        stream.setVersion(QDataStream::Qt_6_0);

        QStringList args;
        stream >> args;

        if (args.size() > 1) {
            emit fileReceived(args.at(1)); // Index 0 is the executable path, Index 1 is the file
        }
        socket->disconnectFromServer();
    });

    connect(socket, &QLocalSocket::disconnected, socket, &QLocalSocket::deleteLater);
}
