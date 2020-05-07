/* @brief
 * Delegate, позволяющий отметить таблицы в визуальной модели
 * таблиц
 *
 * (C) НИИ Рубин
 * @author:
 *   Ю.Л.Русинов
 */
#pragma once

#include <QItemDelegate>

class unoSearchDelegate : public QItemDelegate {
public:
    unoSearchDelegate(QObject* parent = nullptr);
    ~unoSearchDelegate();

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
    Q_OBJECT
};
