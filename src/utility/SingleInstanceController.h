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

#pragma once

#include <QObject>
#include <QLocalServer>


class SingleInstanceController : public QObject
{
    Q_OBJECT

public:
    explicit SingleInstanceController(const QString &uniqueKey, QObject *parent = nullptr);

    // Checks if another instance is running. If yes, sends arguments and returns true.
    bool checkForExistingInstance(const QStringList &args);

signals:
    // Emitted by the primary instance when a secondary instance sends a new file path
    void fileReceived(const QString &filePath);

private slots:
    void handleNewConnection();

private:
    QString m_serverName;
    QLocalServer *m_localServer = nullptr;

};
