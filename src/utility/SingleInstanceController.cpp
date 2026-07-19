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

#include "SingleInstanceController.h"
#include <QLocalSocket>
#include <QDataStream>


SingleInstanceController::SingleInstanceController(const QString &uniqueKey, QObject *parent)
    : QObject(parent), m_serverName(uniqueKey) {}

bool SingleInstanceController::checkForExistingInstance(const QStringList &args) {
    // Try to connect to an existing local server instance
    QLocalSocket socket;
    socket.connectToServer(m_serverName);

    if (socket.waitForConnected(500)) {
        // Connection succeeded! Another instance is already running.
        QDataStream stream(&socket);
        stream.setVersion(QDataStream::Qt_6_0);

        // Pass command line arguments (e.g., the video file path) to the primary instance
        stream << args;
        socket.waitForBytesWritten(1000);
        return true; // Yes, an existing instance handled it
    }

    // No existing instance found. Clean up any leftover dead server sockets from a past crash.
    QLocalServer::removeServer(m_serverName);

    // Start our own local server to listen for future secondary instances
    m_localServer = new QLocalServer(this);
    connect(m_localServer, &QLocalServer::newConnection, this, &SingleInstanceController::handleNewConnection);
    m_localServer->listen(m_serverName);

    return false; // No existing instance; this is the primary instance
}

void SingleInstanceController::handleNewConnection() {
    QLocalSocket *socket = m_localServer->nextPendingConnection();
    if (!socket) return;

    connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
        QDataStream stream(socket);
        stream.setVersion(QDataStream::Qt_6_0);

        QStringList args;
        stream >> args;

        // If arguments contain a file path, emit it to your main video widget
        if (args.size() > 1) {
            emit fileReceived(args.at(1)); // Index 0 is the executable path, Index 1 is the file
        }
        socket->disconnectFromServer();
    });

    connect(socket, &QLocalSocket::disconnected, socket, &QLocalSocket::deleteLater);
}
