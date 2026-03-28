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
#include <QString>
#include <QDebug>
#include <QTime>
#include <QChar>

#include <iostream>
#include <windows.h>
#pragma comment(lib, "user32.lib")

class WindowsKeyboardHandler : public QObject
{
    Q_OBJECT
public:
    explicit WindowsKeyboardHandler(QObject *parent = nullptr);
    ~WindowsKeyboardHandler();

    void hotkey(QString hotkey);

signals:
    void hotkeyPressed(QString hotkey);

};

