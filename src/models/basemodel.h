#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <QAbstractListModel>
#include <QVector>

// 所有模型继承此基类，统一实现begin/endInsertRows
class BaseModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit BaseModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

protected:
    // 安全插入单条数据
    void safeInsert(int row, const QVariant &item) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, item);
        endInsertRows();
    }

    // 安全追加数据
    void safeAppend(const QVariant &item) {
        int row = m_items.size();
        beginInsertRows(QModelIndex(), row, row);
        m_items.append(item);
        endInsertRows();
    }

    QVector<QVariant> m_items;
};

#endif // BASEMODEL_H
