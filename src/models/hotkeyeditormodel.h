#ifndef HPLAYER_HOTKEYEDITORMODEL_H
#define HPLAYER_HOTKEYEDITORMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

enum class Column : uint8_t {
    Name,
    Shortcut
};

//! [0]
class HotkeyEditorModel : public QAbstractItemModel
{
    Q_OBJECT

    class HotkeyEditorModelItem
    {
    public:
        explicit HotkeyEditorModelItem(const QList<QVariant> &data,
                                         HotkeyEditorModelItem *parentItem = nullptr);
        ~HotkeyEditorModelItem();

        void appendChild(HotkeyEditorModelItem *child);

        HotkeyEditorModelItem *child(int row) const;
        int childCount() const;
        int columnCount() const;
        QVariant data(int column) const;
        int row() const;
        HotkeyEditorModelItem *parentItem() const;
        QAction *action() const;

    private:
        QList<HotkeyEditorModelItem *> m_childItems;
        QList<QVariant> m_itemData;
        HotkeyEditorModelItem *m_parentItem;
    };

public:
    explicit HotkeyEditorModel(QObject *parent = nullptr);
    ~HotkeyEditorModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    int columnCount(const QModelIndex &index = QModelIndex()) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void setActions();

private:
    void setupModelData(HotkeyEditorModelItem *parent);

    HotkeyEditorModelItem *m_rootItem;
};
//! [0]


#endif //HPLAYER_HOTKEYEDITORMODEL_H