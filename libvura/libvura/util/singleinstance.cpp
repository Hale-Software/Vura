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
    connect(&m_server, &QLocalServer::newConnection, this, &SingleInstance::onNewConnection);
}

void SingleInstance::listen(const QString &name)
{
    QLocalServer::removeServer(name);
    m_server.listen(name);
}

// static
bool SingleInstance::hasPrevious(const QString &name, int argc, char *argv[])
{
    QLocalSocket socket;
    socket.connectToServer(name);

    if (!socket.waitForConnected(500))
        return false;

    // Collect path arguments: argv[1] (and optionally argv[2] for the
    // "playlist <path>" two-argument form your main.cpp already handles).
    // We send a single path string; main.cpp on the receiving end calls
    // processOpenParams, so just forward the raw arguments as a newline-
    // delimited list and let the running instance re-parse them the same
    // way it would on a fresh launch.
    QStringList args;
    for (int i = 1; i < argc; ++i)
        args << QString::fromUtf8(argv[i]);

    // Serialise with QDataStream so the reader knows exactly how many bytes
    // to expect (the quint32 size prefix is written automatically).
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << args.join('\n');   // join into one string; receiver splits on \n

    socket.write(block);
    socket.flush();
    socket.waitForBytesWritten(500);
    socket.disconnectFromServer();

    return true;
}

// private slot
void SingleInstance::onNewConnection()
{
    QLocalSocket *socket = m_server.nextPendingConnection();
    if (!socket)
        return;

    // Use a lambda that captures the socket so we can read when data arrives.
    connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_6_0);

        QString payload;
        in >> payload;          // reads the quint32-prefixed string written above

        socket->deleteLater();

        emit newInstance();     // bring the window to the front

        if (!payload.isEmpty()) {
            // Re-split and emit one signal per distinct path argument.
            // The "playlist <path>" two-arg form sends two lines; we emit
            // only the path (last token), consistent with what processOpenParams
            // already does.  Adjust here if you need finer control.
            const QStringList args = payload.split('\n', Qt::SkipEmptyParts);
            if (!args.isEmpty()) {
                // If the first arg is a verb like "playlist", the path is arg[1];
                // otherwise arg[0] is the path directly.
                const QString path = (args.size() > 1) ? args.at(1) : args.at(0);
                emit openPathRequested(path);
            }
        }
    });

    // Clean up the socket if the sender disconnects before sending data.
    connect(socket, &QLocalSocket::disconnected, socket, &QLocalSocket::deleteLater);
}
