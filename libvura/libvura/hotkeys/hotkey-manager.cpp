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

#include "hotkey-manager.h"
#include "global-hotkey-host.h"

#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QDebug>

#include <algorithm>
#include <utility>

namespace {

// Fallback categories, longest first so "PlaybackMode" resolves before "Playback".
const QStringList kObjectNameCategories = {
    QStringLiteral("Subtitles"),
    QStringLiteral("Playback"),
    QStringLiteral("Playlist"),
    QStringLiteral("Markers"),
    QStringLiteral("Audio"),
    QStringLiteral("Video"),
    QStringLiteral("Tools"),
    QStringLiteral("File"),
    QStringLiteral("View"),
    QStringLiteral("Help")
};

bool isMediaKey(const Qt::Key key)
{
    switch (key) {
    case Qt::Key_MediaPlay:
    case Qt::Key_MediaPause:
    case Qt::Key_MediaTogglePlayPause:
    case Qt::Key_MediaStop:
    case Qt::Key_MediaNext:
    case Qt::Key_MediaPrevious:
    case Qt::Key_MediaRecord:
    case Qt::Key_VolumeUp:
    case Qt::Key_VolumeDown:
    case Qt::Key_VolumeMute:
        return true;
    default:
        return false;
    }
}

} // namespace


HotkeyManager::HotkeyManager(QObject *parent) : QObject(parent)
{
}

HotkeyManager *HotkeyManager::instance()
{
    static HotkeyManager manager;
    return &manager;
}

void HotkeyManager::registerWindow(QMainWindow *window)
{
    if (!window) {
        return;
    }

    m_window = window;

    const QList<QAction *> actions = window->findChildren<QAction *>();
    for (QAction *action : actions) {
        // Unnamed actions are created at runtime (recent files, audio devices) and
        // have no stable persistence key, so they are not rebindable.
        if (action->objectName().isEmpty() || action->isSeparator()) {
            continue;
        }
        // Submenu title actions are not commands.
        if (action->menu() != nullptr) {
            continue;
        }
        if (action->text().isEmpty()) {
            continue;
        }
        // Escape hatch: setProperty("hotkeyExclude", true) in Designer or code.
        if (action->property("hotkeyExclude").toBool()) {
            continue;
        }
        registerAction(action);
    }

    std::sort(m_bindings.begin(), m_bindings.end(), [](const HotkeyBinding &a, const HotkeyBinding &b) {
        if (a.category != b.category) {
            return a.category.localeAwareCompare(b.category) < 0;
        }
        return a.label.localeAwareCompare(b.label) < 0;
    });

    m_index.clear();
    for (int i = 0; i < m_bindings.size(); ++i) {
        m_index.insert(m_bindings.at(i).id, i);
    }

    qDebug() << "HotkeyManager: registered" << m_bindings.size() << "rebindable actions";
}

void HotkeyManager::registerAction(QAction *action, const QString &category)
{
    if (!action || action->objectName().isEmpty()) {
        return;
    }
    if (m_index.contains(action->objectName())) {
        return;
    }

    HotkeyBinding binding;
    binding.id = action->objectName();
    binding.label = labelFor(action);
    binding.category = category.isEmpty() ? categoryFor(action) : category;
    binding.defaultSequence = action->shortcut();
    binding.sequence = binding.defaultSequence;
    binding.action = action;

    m_index.insert(binding.id, m_bindings.size());
    m_bindings.append(binding);
}

void HotkeyManager::load()
{
    QSettings settings;

    settings.beginGroup(settingsGroup());
    for (HotkeyBinding &binding : m_bindings) {
        if (settings.contains(binding.id)) {
            // An empty stored value means the user deliberately cleared the binding.
            binding.sequence = QKeySequence::fromString(settings.value(binding.id).toString(),
                                                        QKeySequence::PortableText);
        } else {
            binding.sequence = binding.defaultSequence;
        }
        binding.global = settings.value(QStringLiteral("global/") + binding.id, binding.defaultGlobal).toBool();
    }
    settings.endGroup();

    applyAll();
    emit bindingsChanged();
}

void HotkeyManager::commit(const QList<HotkeyBinding> &edited)
{
    QSettings settings;
    settings.beginGroup(settingsGroup());

    for (const HotkeyBinding &incoming : edited) {
        const int row = indexOf(incoming.id);
        if (row < 0) {
            continue;
        }

        HotkeyBinding &binding = m_bindings[row];
        binding.sequence = incoming.sequence;
        binding.global = incoming.global && supportsGlobalHotkeys() && isValidGlobalSequence(incoming.sequence);

        // Only overrides are persisted, so shipping a new default in a future
        // release picks up automatically for every user who never rebound it.
        if (binding.sequence == binding.defaultSequence) {
            settings.remove(binding.id);
        } else {
            settings.setValue(binding.id, binding.sequence.toString(QKeySequence::PortableText));
        }

        const QString globalKey = QStringLiteral("global/") + binding.id;
        if (binding.global == binding.defaultGlobal) {
            settings.remove(globalKey);
        } else {
            settings.setValue(globalKey, binding.global);
        }
    }

    settings.endGroup();
    settings.sync();

    applyAll();
    emit bindingsChanged();
}

void HotkeyManager::restoreDefaults()
{
    QSettings settings;
    settings.remove(settingsGroup());
    settings.sync();

    for (HotkeyBinding &binding : m_bindings) {
        binding.sequence = binding.defaultSequence;
        binding.global = binding.defaultGlobal;
    }

    applyAll();
    emit bindingsChanged();
}

QKeySequence HotkeyManager::sequenceFor(const QString &id) const
{
    const int row = indexOf(id);
    return row < 0 ? QKeySequence() : m_bindings.at(row).sequence;
}

bool HotkeyManager::supportsGlobalHotkeys()
{
    return GlobalHotkeyHost::isSupported();
}

bool HotkeyManager::isValidGlobalSequence(const QKeySequence &sequence)
{
    if (sequence.isEmpty()) {
        return false;
    }

    const QKeyCombination combination = sequence[0];
    const Qt::KeyboardModifiers modifiers =
        combination.keyboardModifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier | Qt::MetaModifier);

    return modifiers != Qt::NoModifier || isMediaKey(combination.key());
}

bool HotkeyManager::handleNativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    return m_globalHost && m_globalHost->handleNativeEvent(eventType, message, result);
}

void HotkeyManager::globalHotkeyActivated(const QString &id)
{
    const int row = indexOf(id);
    if (row < 0) {
        return;
    }

    QAction *action = m_bindings.at(row).action;
    if (action && action->isEnabled()) {
        action->trigger();
    }
}

int HotkeyManager::indexOf(const QString &id) const
{
    return m_index.value(id, -1);
}

void HotkeyManager::applyAll()
{
    for (const HotkeyBinding &binding : std::as_const(m_bindings)) {
        applyBinding(binding);
    }
    rebuildGlobalHotkeys();
}

void HotkeyManager::applyBinding(const HotkeyBinding &binding) const
{
    QAction *action = binding.action;
    if (!action) {
        return;
    }

    action->setShortcut(binding.sequence);
    action->setShortcutContext(Qt::WindowShortcut);

    // Menu bar shortcuts stop firing once the menu bar is hidden (fullscreen),
    // so every rebindable action is also owned by the window itself.
    if (m_window && !m_window->actions().contains(action)) {
        m_window->addAction(action);
    }
}

void HotkeyManager::rebuildGlobalHotkeys()
{
    if (!supportsGlobalHotkeys() || !m_window) {
        return;
    }

    if (!m_globalHost) {
        m_globalHost = new GlobalHotkeyHost(m_window, this);
        connect(m_globalHost, &GlobalHotkeyHost::activated, this, &HotkeyManager::globalHotkeyActivated);
    }

    m_globalHost->clear();

    for (HotkeyBinding &binding : m_bindings) {
        if (!binding.global || binding.sequence.isEmpty()) {
            continue;
        }
        if (!m_globalHost->registerSequence(binding.id, binding.sequence)) {
            // Another application already owns it - drop back to a local binding
            // rather than silently pretending the hotkey is live.
            qWarning() << "HotkeyManager: could not claim global hotkey"
                       << binding.sequence.toString(QKeySequence::NativeText) << "for" << binding.id;
            binding.global = false;
        }
    }
}

QString HotkeyManager::labelFor(const QAction *action)
{
    QString label = stripMnemonics(action->text());
    while (label.endsWith(QLatin1Char('.'))) {
        label.chop(1);
    }
    return label.trimmed();
}

QString HotkeyManager::categoryFor(const QAction *action)
{
    // Climb from the owning menu up to the top level menu so that submenu entries
    // ("Log Files") are grouped under their menu bar heading ("Help").
    const QList<QObject *> associated = action->associatedObjects();
    for (QObject *object : associated) {
        const auto *menu = qobject_cast<QMenu *>(object);
        if (!menu) {
            continue;
        }

        for (int depth = 0; depth < 8; ++depth) {
            QAction *menuAction = menu->menuAction();
            const QMenu *parentMenu = nullptr;
            if (menuAction) {
                const QList<QObject *> owners = menuAction->associatedObjects();
                for (QObject *owner : owners) {
                    if (auto *candidate = qobject_cast<QMenu *>(owner)) {
                        parentMenu = candidate;
                        break;
                    }
                }
            }
            if (!parentMenu) {
                break;
            }
            menu = parentMenu;
        }

        const QString title = stripMnemonics(menu->title());
        if (!title.isEmpty()) {
            return title;
        }
    }

    return categoryFromObjectName(action->objectName());
}

QString HotkeyManager::categoryFromObjectName(const QString &objectName)
{
    QString name = objectName;
    if (name.startsWith(QLatin1String("action"))) {
        name.remove(0, 6);
    }

    for (const QString &category : kObjectNameCategories) {
        if (name.startsWith(category)) {
            return category;
        }
    }

    return QObject::tr("General");
}

QString HotkeyManager::stripMnemonics(const QString &text)
{
    QString result = text;
    result.replace(QLatin1String("&&"), QLatin1String("\x01"));
    result.remove(QLatin1Char('&'));
    result.replace(QLatin1Char('\x01'), QLatin1Char('&'));
    return result;
}
