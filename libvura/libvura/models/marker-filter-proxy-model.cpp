#include "marker-filter-proxy-model.h"
#include "marker-list-model.h"

MarkerFilterProxyModel::MarkerFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    sort(0);
}

void MarkerFilterProxyModel::setSearchText(const QString &text)
{
    const QString trimmed = text.trimmed();
    if (trimmed == m_search)
        return;
    m_search = trimmed;
    invalidateFilter();
}

void MarkerFilterProxyModel::setTypeVisible(const QString &type, bool visible)
{
    if (visible == isTypeVisible(type))
        return;
    if (visible)
        m_hiddenTypes.remove(type);
    else
        m_hiddenTypes.insert(type);
    invalidateFilter();
}

bool MarkerFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);

    if (m_hiddenTypes.contains(idx.data(MarkerListModel::TypeRole).toString()))
        return false;

    if (m_search.isEmpty())
        return true;

    return idx.data(MarkerListModel::NameRole).toString().contains(m_search, Qt::CaseInsensitive)
        || idx.data(MarkerListModel::CommentsRole).toString().contains(m_search, Qt::CaseInsensitive)
        || idx.data(MarkerListModel::TypeRole).toString().contains(m_search, Qt::CaseInsensitive);
}

bool MarkerFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return left.data(MarkerListModel::FractionRole).toDouble()
         < right.data(MarkerListModel::FractionRole).toDouble();
}
