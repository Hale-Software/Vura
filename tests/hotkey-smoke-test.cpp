#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QDebug>

#include <libvura/hotkeys/hotkey-manager.h>
#include <libvura/models/hotkey-model.h>

static int failures = 0;
#define CHECK(cond, msg) do { if (!(cond)) { qCritical() << "FAIL:" << msg; ++failures; } else { qInfo() << "ok  :" << msg; } } while (0)

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("HaleSoftwareTest");
    QCoreApplication::setApplicationName("VuraHotkeyTest");
    QSettings().clear();

    QMainWindow window;
    auto *fileMenu = window.menuBar()->addMenu("&File");
    auto *recentMenu = fileMenu->addMenu("Open &Recent");
    auto *playbackMenu = window.menuBar()->addMenu("&Playback");

    auto *openFile = new QAction("Open File...", &window);
    openFile->setObjectName("actionFileOpenFile");
    openFile->setShortcut(QKeySequence("O"));
    fileMenu->addAction(openFile);

    auto *clearRecent = new QAction("Clear", &window);
    clearRecent->setObjectName("actionFileOpenRecentClear");
    recentMenu->addAction(clearRecent);

    auto *togglePlay = new QAction("Play/Pause", &window);
    togglePlay->setObjectName("actionPlaybackTogglePlay");
    togglePlay->setShortcut(QKeySequence("Space"));
    playbackMenu->addAction(togglePlay);

    auto *unnamed = new QAction("Recent file 1", &window);   // no objectName
    recentMenu->addAction(unnamed);

    auto *excluded = new QAction("Internal", &window);
    excluded->setObjectName("actionToolsInternal");
    excluded->setProperty("hotkeyExclude", true);

    auto *manager = HotkeyManager::instance();
    manager->registerWindow(&window);
    manager->load();

    CHECK(manager->bindings().size() == 3, "only named, non-excluded, non-submenu actions registered");
    CHECK(!manager->contains("actionToolsInternal"), "hotkeyExclude honoured");
    CHECK(manager->sequenceFor("actionFileOpenFile") == QKeySequence("O"), "designer shortcut becomes default");

    // Submenu entry is categorised under its top level menu.
    QString recentCategory;
    for (const HotkeyBinding &b : manager->bindings()) {
        if (b.id == "actionFileOpenRecentClear") recentCategory = b.category;
    }
    CHECK(recentCategory == "File", QString("submenu climbs to top level menu (got '%1')").arg(recentCategory));

    CHECK(window.actions().contains(openFile), "action added to window so it survives a hidden menu bar");

    // --- model editing ---
    HotkeyModel model;
    bool resolverCalled = false;
    model.setConflictResolver([&](const QString &, const QString &, const QKeySequence &) {
        resolverCalled = true;
        return true;
    });
    CHECK(model.rowCount() == 3, "model mirrors the manager");
    CHECK(!model.isModified(), "fresh model is clean");

    int openRow = -1, playRow = -1;
    for (int r = 0; r < model.rowCount(); ++r) {
        const QString label = model.index(r, HotkeyModel::ActionColumn).data().toString();
        if (label == "Open File") openRow = r;
        if (label == "Play/Pause") playRow = r;
    }
    CHECK(openRow >= 0 && playRow >= 0, "labels stripped of trailing dots and mnemonics");

    // Rebind Play/Pause to Ctrl+Shift+P
    CHECK(model.setData(model.index(playRow, HotkeyModel::ShortcutColumn),
                        QVariant::fromValue(QKeySequence("Ctrl+Shift+P")), Qt::EditRole),
          "accepts a free sequence");
    CHECK(model.isModified(), "model reports unsaved changes");
    CHECK(togglePlay->shortcut() == QKeySequence("Space"), "live action untouched before commit");

    // Steal "O" from Open File
    CHECK(model.setData(model.index(playRow, HotkeyModel::ShortcutColumn),
                        QVariant::fromValue(QKeySequence("O")), Qt::EditRole),
          "conflicting sequence accepted after resolution");
    CHECK(resolverCalled, "conflict resolver consulted");
    CHECK(model.index(openRow, HotkeyModel::ShortcutColumn).data().toString().isEmpty(),
          "previous owner left unbound");

    // Global flag rules
    CHECK(!model.setData(model.index(playRow, HotkeyModel::GlobalColumn), Qt::Checked, Qt::CheckStateRole),
          "bare key rejected as a global hotkey");

    model.commit();
    CHECK(togglePlay->shortcut() == QKeySequence("O"), "commit applies to the live action");
    CHECK(openFile->shortcut().isEmpty(), "commit clears the displaced action");
    CHECK(!model.isModified(), "clean after commit");

    {
        QSettings settings;
        settings.beginGroup(HotkeyManager::settingsGroup());
        CHECK(settings.value("actionPlaybackTogglePlay").toString() == "O", "override persisted in portable text");
        CHECK(settings.contains("actionFileOpenFile"), "cleared binding persisted as an explicit empty override");
        CHECK(settings.value("actionFileOpenFile").toString().isEmpty(), "cleared binding is empty");
        settings.endGroup();
    }

    manager->restoreDefaults();
    CHECK(togglePlay->shortcut() == QKeySequence("Space"), "restoreDefaults reverts the action");
    CHECK(openFile->shortcut() == QKeySequence("O"), "restoreDefaults reverts the displaced action");
    {
        QSettings settings;
        CHECK(settings.allKeys().filter("hotkeys/").isEmpty(), "restoreDefaults wipes the settings group");
    }

    QSettings().clear();
    qInfo() << (failures == 0 ? "ALL PASSED" : "FAILURES:") << failures;
    return failures;
}
