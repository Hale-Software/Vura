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

#include "windowskeyboardhandler.h"

WindowsKeyboardHandler *m_handler;

bool m_83key = false;
bool m_91key = false;
bool m_160key = false;

HHOOK hKeyboardHook;

// Callback function to intercept Windows hotkeys
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;

            if (pKeyStruct->vkCode == 83) {
                m_83key = true;
            } else if (pKeyStruct->vkCode == 91) {
                m_91key = true;
            } else if (pKeyStruct->vkCode == 160) {
                m_160key = true;
            } else {
                m_83key = false;
                m_91key = false;
                m_160key = false;
            }

            if (m_83key) {
                if (m_91key) {
                    if (m_160key) {
                        m_handler->hotkey("prtscr");
                        return 1;
                    }
                }
            }
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

WindowsKeyboardHandler::WindowsKeyboardHandler(QObject *parent) : QObject(parent)
{

}

WindowsKeyboardHandler::~WindowsKeyboardHandler()
{

}

void WindowsKeyboardHandler::hotkey(QString hotkey)
{
    emit hotkeyPressed(hotkey);
}
