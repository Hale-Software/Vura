/*******************************************************************************
     Copyright (c) 2026. by Andrew Hale <halea2196@gmail.com>

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

#include "log-viewer-model.h"


LogViewerModel::LogViewerModel(QObject *parent)
    : QAbstractTableModel(parent) {}

LogViewerModel::LogViewerModel(const QList<LogMessage> &logMessages, QObject *parent)
    : QAbstractTableModel(parent), m_logMessages(logMessages) {}

int LogViewerModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_logMessages.size();
}

int LogViewerModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 3;
}

QVariant LogViewerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_logMessages.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        const auto &logMessage = m_logMessages.at(index.row());

        switch (index.column()) {
            case 0:
                return logMessage.timestamp;
            case 1:
                return logMessage.verbosity;
            case 2:
                return logMessage.message;
            default:
                break;
        }
    }
    return QVariant();
}

QVariant LogViewerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Timestamp");
            case 1:
                return tr("Verbosity");
            case 2:
                return tr("Message");
            default:
                break;
        }
    }
    return QVariant();
}

bool LogViewerModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        m_logMessages.insert(position, { QString(), QString(), QString() });

    endInsertRows();
    return true;
}

bool LogViewerModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        m_logMessages.removeAt(position);

    endRemoveRows();
    return true;
}

bool LogViewerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        const int row = index.row();
        auto logMessage = m_logMessages.value(row);

        switch (index.column()) {
            case 0:
                logMessage.timestamp = value.toString();
                break;
            case 1:
                logMessage.verbosity = value.toString();
                break;
            case 2:
                logMessage.message = value.toString();
                break;
            default:
                return false;
        }
        m_logMessages.replace(row, logMessage);
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

        return true;
    }

    return false;
}

Qt::ItemFlags LogViewerModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

const QList<LogMessage> &LogViewerModel::getLogMessages() const
{
    return m_logMessages;
}
