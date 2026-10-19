#include "MarkerPanel.h"
#include "MarkerItemDelegate.h"

#include <libvura/models/marker-colors.h>
#include <libvura/models/marker-filter-proxy-model.h>
#include <libvura/models/marker-list-model.h>
#include <libvura/util/marker-thumbnailer.h>

#include <QAction>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QListView>
#include <QMenu>
#include <QSignalBlocker>
#include <QToolButton>
#include <QVBoxLayout>

MarkerPanel::MarkerPanel(QWidget *parent)
    : QWidget(parent),
      m_model(new MarkerListModel(this)),
      m_proxy(new MarkerFilterProxyModel(this)),
      m_delegate(new MarkerItemDelegate(this)),
      m_thumbnailer(new MarkerThumbnailer(this))
{
    m_proxy->setSourceModel(m_model);
    buildUi();

    connect(m_thumbnailer, &MarkerThumbnailer::thumbnailReady,
            m_model, &MarkerListModel::setThumbnail);
}

void MarkerPanel::buildUi()
{
    // ---- Header: search + colour chips -------------------------------------
    m_search = new QLineEdit(this);
    m_search->setPlaceholderText(tr("Search markers"));
    m_search->setClearButtonEnabled(true);
    m_search->addAction(QIcon::fromTheme(QStringLiteral("edit-find")), QLineEdit::LeadingPosition);
    connect(m_search, &QLineEdit::textChanged, m_proxy, &MarkerFilterProxyModel::setSearchText);

    auto *header = new QHBoxLayout;
    header->setSpacing(6);
    header->addWidget(m_search, 1);

    for (const MarkerColors::TypeColor &tc : MarkerColors::all()) {
        auto *chip = new QToolButton(this);
        chip->setCheckable(true);
        chip->setChecked(true);
        chip->setFixedSize(24, 24);
        chip->setToolTip(tr("Show/hide %1 markers").arg(tc.label));
        chip->setStyleSheet(QStringLiteral(
            "QToolButton { background: %1; border: 1px solid %3; border-radius: 4px; }"
            "QToolButton:!checked { background: %2; }"
            "QToolButton:hover { border: 1px solid palette(highlighted-text); }")
            .arg(tc.color.name(), tc.color.darker(300).name(), tc.color.darker(150).name()));

        const QString type = tc.type;
        connect(chip, &QToolButton::toggled, this, [this, type](bool visible) {
            m_proxy->setTypeVisible(type, visible);
            emit typeVisibilityChanged(type, visible);
        });

        m_chips.insert(type, chip);
        header->addWidget(chip);
    }

    // ---- List ---------------------------------------------------------------
    m_view = new QListView(this);
    m_view->setModel(m_proxy);
    m_view->setItemDelegate(m_delegate);
    m_view->setUniformItemSizes(true);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setMouseTracking(true);
    m_view->viewport()->setAttribute(Qt::WA_Hover);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_view, &QListView::clicked, this, [this](const QModelIndex &idx) {
        emit seekRequested(idx.data(MarkerListModel::PositionMsRole).toLongLong());
    });
    connect(m_view, &QListView::doubleClicked, this, [this](const QModelIndex &idx) {
        VideoMarkerRecord marker;
        if (recordAt(idx, &marker))
            emit editRequested(marker);
    });
    connect(m_view, &QListView::customContextMenuRequested, this, &MarkerPanel::showContextMenu);

    auto *deleteAction = new QAction(this);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(deleteAction, &QAction::triggered, this, [this] {
        VideoMarkerRecord marker;
        if (recordAt(m_view->currentIndex(), &marker))
            emit deleteRequested(marker);
    });
    m_view->addAction(deleteAction);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(8);
    layout->addLayout(header);
    layout->addWidget(m_view, 1);
}

void MarkerPanel::setSource(const QUrl &source)
{
    m_model->clearThumbnails();
    m_thumbnailer->setSource(source);
}

void MarkerPanel::setMarkers(const QList<VideoMarkerRecord> &markers)
{
    // Preserve the selection across the model reset.
    int selectedId = 0;
    VideoMarkerRecord current;
    if (recordAt(m_view->currentIndex(), &current))
        selectedId = current.id;

    m_model->setMarkers(markers);

    if (selectedId > 0) {
        for (int row = 0; row < m_proxy->rowCount(); ++row) {
            const QModelIndex idx = m_proxy->index(row, 0);
            if (idx.data(MarkerListModel::IdRole).toInt() == selectedId) {
                m_view->setCurrentIndex(idx);
                break;
            }
        }
    }

    requestMissingThumbnails();
}

void MarkerPanel::setDurationMs(qint64 ms)
{
    m_model->setDurationMs(ms);
    requestMissingThumbnails();
}

void MarkerPanel::setFrameRate(double fps)
{
    m_delegate->setFrameRate(fps);
    m_view->viewport()->update();
}

void MarkerPanel::setTypeVisible(const QString &type, bool visible)
{
    if (QToolButton *chip = m_chips.value(type)) {
        const QSignalBlocker blocker(chip);
        chip->setChecked(visible);
    }
    m_proxy->setTypeVisible(type, visible);
}

void MarkerPanel::requestMissingThumbnails()
{
    if (m_model->durationMs() <= 0)
        return;   // positions aren't known until the duration arrives

    for (const VideoMarkerRecord &m : m_model->markers()) {
        const qint64 pos = m_model->positionMs(m);
        if (!m_model->hasThumbnail(m.id, pos))
            m_thumbnailer->request(m.id, pos);
    }
}

bool MarkerPanel::recordAt(const QModelIndex &proxyIndex, VideoMarkerRecord *out) const
{
    if (!proxyIndex.isValid())
        return false;
    const int row = m_proxy->mapToSource(proxyIndex).row();
    if (row < 0 || row >= m_model->markers().size())
        return false;
    *out = m_model->markers().at(row);
    return true;
}

void MarkerPanel::showContextMenu(const QPoint &pos)
{
    const QModelIndex idx = m_view->indexAt(pos);
    VideoMarkerRecord marker;
    if (!recordAt(idx, &marker))
        return;

    QMenu menu(this);
    menu.addAction(tr("Go to Marker"), this, [this, idx] {
        emit seekRequested(idx.data(MarkerListModel::PositionMsRole).toLongLong());
    });
    menu.addAction(tr("Edit Marker…"), this, [this, marker] { emit editRequested(marker); });
    menu.addSeparator();
    menu.addAction(tr("Delete Marker"), this, [this, marker] { emit deleteRequested(marker); });
    menu.exec(m_view->viewport()->mapToGlobal(pos));
}
