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

#include "global-hotkey-host.h"

#include <QWidget>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif


#ifdef Q_OS_WIN
namespace {

bool toNativeModifiers(const Qt::KeyboardModifiers modifiers, UINT *nativeModifiers)
{
    UINT result = 0;
    if (modifiers & Qt::ControlModifier) result |= MOD_CONTROL;
    if (modifiers & Qt::AltModifier)     result |= MOD_ALT;
    if (modifiers & Qt::ShiftModifier)   result |= MOD_SHIFT;
    if (modifiers & Qt::MetaModifier)    result |= MOD_WIN;

    *nativeModifiers = result | MOD_NOREPEAT;
    return true;
}

bool toNativeKey(const Qt::Key key, UINT *virtualKey)
{
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        *virtualKey = static_cast<UINT>('A' + (key - Qt::Key_A));
        return true;
    }
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        *virtualKey = static_cast<UINT>('0' + (key - Qt::Key_0));
        return true;
    }
    if (key >= Qt::Key_F1 && key <= Qt::Key_F24) {
        *virtualKey = static_cast<UINT>(VK_F1 + (key - Qt::Key_F1));
        return true;
    }

    switch (key) {
    case Qt::Key_Space:                 *virtualKey = VK_SPACE;                 return true;
    case Qt::Key_Left:                  *virtualKey = VK_LEFT;                  return true;
    case Qt::Key_Right:                 *virtualKey = VK_RIGHT;                 return true;
    case Qt::Key_Up:                    *virtualKey = VK_UP;                    return true;
    case Qt::Key_Down:                  *virtualKey = VK_DOWN;                  return true;
    case Qt::Key_Home:                  *virtualKey = VK_HOME;                  return true;
    case Qt::Key_End:                   *virtualKey = VK_END;                   return true;
    case Qt::Key_PageUp:                *virtualKey = VK_PRIOR;                 return true;
    case Qt::Key_PageDown:              *virtualKey = VK_NEXT;                  return true;
    case Qt::Key_Insert:                *virtualKey = VK_INSERT;                return true;
    case Qt::Key_Delete:                *virtualKey = VK_DELETE;                return true;
    case Qt::Key_Return:
    case Qt::Key_Enter:                 *virtualKey = VK_RETURN;                return true;
    case Qt::Key_Tab:                   *virtualKey = VK_TAB;                   return true;
    case Qt::Key_Escape:                *virtualKey = VK_ESCAPE;                return true;
    case Qt::Key_Backspace:             *virtualKey = VK_BACK;                  return true;
    case Qt::Key_Pause:                 *virtualKey = VK_PAUSE;                 return true;
    case Qt::Key_Print:                 *virtualKey = VK_SNAPSHOT;              return true;
    case Qt::Key_MediaPlay:
    case Qt::Key_MediaPause:
    case Qt::Key_MediaTogglePlayPause:  *virtualKey = VK_MEDIA_PLAY_PAUSE;      return true;
    case Qt::Key_MediaStop:             *virtualKey = VK_MEDIA_STOP;            return true;
    case Qt::Key_MediaNext:             *virtualKey = VK_MEDIA_NEXT_TRACK;      return true;
    case Qt::Key_MediaPrevious:         *virtualKey = VK_MEDIA_PREV_TRACK;      return true;
    case Qt::Key_VolumeUp:              *virtualKey = VK_VOLUME_UP;             return true;
    case Qt::Key_VolumeDown:            *virtualKey = VK_VOLUME_DOWN;           return true;
    case Qt::Key_VolumeMute:            *virtualKey = VK_VOLUME_MUTE;           return true;
    default:
        break;
    }

    // Punctuation and anything else printable, resolved against the active layout.
    if (key > 0 && key < 0xFFFF) {
        const SHORT scan = VkKeyScanW(static_cast<WCHAR>(key));
        if (scan != -1) {
            *virtualKey = static_cast<UINT>(LOBYTE(scan));
            return true;
        }
    }

    return false;
}

} // namespace
#endif


GlobalHotkeyHost::GlobalHotkeyHost(QWidget *window, QObject *parent)
    : QObject(parent), m_window(window)
{
}

GlobalHotkeyHost::~GlobalHotkeyHost()
{
    clear();
}

bool GlobalHotkeyHost::isSupported()
{
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

bool GlobalHotkeyHost::registerSequence(const QString &id, const QKeySequence &sequence)
{
#ifdef Q_OS_WIN
    if (!m_window || sequence.isEmpty()) {
        return false;
    }

    const QKeyCombination combination = sequence[0];

    UINT nativeModifiers = 0;
    UINT virtualKey = 0;
    if (!toNativeModifiers(combination.keyboardModifiers(), &nativeModifiers)) {
        return false;
    }
    if (!toNativeKey(combination.key(), &virtualKey)) {
        return false;
    }

    const int nativeId = m_nextNativeId++;
    const auto handle = reinterpret_cast<HWND>(m_window->winId());

    if (!RegisterHotKey(handle, nativeId, nativeModifiers, virtualKey)) {
        return false;
    }

    m_registered.insert(nativeId, id);
    return true;
#else
    Q_UNUSED(id)
    Q_UNUSED(sequence)
    return false;
#endif
}

void GlobalHotkeyHost::clear()
{
#ifdef Q_OS_WIN
    if (m_window) {
        const auto handle = reinterpret_cast<HWND>(m_window->winId());
        for (auto it = m_registered.constBegin(); it != m_registered.constEnd(); ++it) {
            UnregisterHotKey(handle, it.key());
        }
    }
#endif
    m_registered.clear();
}

bool GlobalHotkeyHost::handleNativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(result)

#ifdef Q_OS_WIN
    if (eventType != "windows_generic_MSG" && eventType != "windows_dispatcher_MSG") {
        return false;
    }

    const auto *msg = static_cast<MSG *>(message);
    if (msg->message != WM_HOTKEY) {
        return false;
    }

    const int nativeId = static_cast<int>(msg->wParam);
    const QString id = m_registered.value(nativeId);
    if (id.isEmpty()) {
        return false;
    }

    emit activated(id);
    return true;
#else
    Q_UNUSED(eventType)
    Q_UNUSED(message)
    return false;
#endif
}
