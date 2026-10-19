#pragma once

#include <QSet>
#include <QSortFilterProxyModel>

/// Filters by search text (name + comments) and hidden marker types,
/// and keeps rows sorted by position in the video.
class MarkerFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MarkerFilterProxyModel(QObject *parent = nullptr);

    void setSearchText(const QString &text);
    void setTypeVisible(const QString &type, bool visible);
    bool isTypeVisible(const QString &type) const { return !m_hiddenTypes.contains(type); }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString m_search;
    QSet<QString> m_hiddenTypes;
};
