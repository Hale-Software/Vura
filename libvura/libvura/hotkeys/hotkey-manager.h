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
#include <QKeySequence>
#include <QPointer>
#include <QHash>
#include <QList>
#include <QString>
#include <QStringList>

class QAction;
class QMainWindow;
class GlobalHotkeyHost;


/**
 * A single user-rebindable command.
 *
 * `id` is the QAction's objectName. It is the persistence key, so renaming an
 * action in Designer orphans the user's binding for that command - treat action
 * object names as part of the on-disk format.
 */
struct HotkeyBinding
{
    QString id;
    QString label;
    QString category;
    QKeySequence defaultSequence;
    QKeySequence sequence;
    bool defaultGlobal = false;
    bool global = false;
    QPointer<QAction> action;

    bool isDefault() const { return sequence == defaultSequence && global == defaultGlobal; }
};


/**
 * Owns the authoritative hotkey table for the application.
 *
 * Lifecycle:
 *   1. registerWindow() walks the main window's QActions and snapshots the
 *      shortcuts set in Designer as the factory defaults.
 *   2. load() reads user overrides from QSettings and applies them.
 *   3. The settings page edits a *copy* of the table and calls commit() on Apply.
 *
 * Only registerWindow()/load() touch the QActions outside of commit(), so the
 * editor can be opened, edited and cancelled without side effects.
 */
class HotkeyManager : public QObject
{
    Q_OBJECT

public:
    static HotkeyManager *instance();

    /** Discovers every eligible QAction owned by `window`. Call once, after the UI is built. */
    void registerWindow(QMainWindow *window);

    /** Registers a single action (for actions created outside the .ui file). */
    void registerAction(QAction *action, const QString &category = QString());

    /** Reads user overrides from QSettings and applies them to the registered actions. */
    void load();

    /** Persists and applies an edited copy of the table. */
    void commit(const QList<HotkeyBinding> &edited);

    /** Clears every user override and re-applies the Designer defaults. */
    void restoreDefaults();

    const QList<HotkeyBinding> &bindings() const { return m_bindings; }
    QKeySequence sequenceFor(const QString &id) const;
    bool contains(const QString &id) const { return m_index.contains(id); }

    /** True when system-wide hotkeys can be registered on this platform. */
    static bool supportsGlobalHotkeys();

    /**
     * A sequence is only acceptable as a system-wide hotkey when it carries a
     * modifier or is a dedicated media key - otherwise it would swallow the key
     * for every other application on the desktop.
     */
    static bool isValidGlobalSequence(const QKeySequence &sequence);

    /** Forward from VuraMainWindow::nativeEvent(). Returns true when consumed. */
    bool handleNativeEvent(const QByteArray &eventType, void *message, qintptr *result);

    static QString settingsGroup() { return QStringLiteral("hotkeys"); }

signals:
    void bindingsChanged();

private slots:
    void globalHotkeyActivated(const QString &id);

private:
    explicit HotkeyManager(QObject *parent = nullptr);

    int indexOf(const QString &id) const;
    void applyAll();
    void applyBinding(const HotkeyBinding &binding) const;
    void rebuildGlobalHotkeys();

    static QString labelFor(const QAction *action);
    static QString categoryFor(const QAction *action);
    static QString categoryFromObjectName(const QString &objectName);
    static QString stripMnemonics(const QString &text);

    QList<HotkeyBinding> m_bindings;
    QHash<QString, int> m_index;
    QPointer<QMainWindow> m_window;
    GlobalHotkeyHost *m_globalHost = nullptr;

};
