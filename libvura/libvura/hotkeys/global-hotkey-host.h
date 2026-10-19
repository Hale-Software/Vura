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
#include <QHash>
#include <QKeySequence>
#include <QPointer>
#include <QString>

class QWidget;


/**
 * Registers system-wide hotkeys with the OS.
 *
 * Windows implementation uses RegisterHotKey() bound to the main window's HWND;
 * WM_HOTKEY messages must be forwarded in from the window's nativeEvent().
 * On other platforms every call is a no-op and isSupported() returns false.
 */
class GlobalHotkeyHost : public QObject
{
    Q_OBJECT

public:
    explicit GlobalHotkeyHost(QWidget *window, QObject *parent = nullptr);
    ~GlobalHotkeyHost() override;

    static bool isSupported();

    bool registerSequence(const QString &id, const QKeySequence &sequence);
    void clear();

    bool handleNativeEvent(const QByteArray &eventType, void *message, qintptr *result);

    signals:
        void activated(const QString &id);

private:
    QPointer<QWidget> m_window;
    QHash<int, QString> m_registered;   // native hotkey id -> binding id
    int m_nextNativeId = 0xB000;
};
