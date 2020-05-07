/* @brief
 * Delegate, позволяющий отметить таблицы в визуальной модели
 * таблиц
 *
 * (C) НИИ Рубин
 * @author:
 *   Ю.Л.Русинов
 */
#include <QCheckBox>
#include <QtDebug>

#include "unoSearchDelegate.h"

unoSearchDelegate::unoSearchDelegate(QObject* parent)
    : QItemDelegate(parent) {
    qDebug() << __PRETTY_FUNCTION__;
}

unoSearchDelegate::~unoSearchDelegate() {}

QWidget* unoSearchDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    qDebug() << __PRETTY_FUNCTION__;
    QCheckBox* wcb = new QCheckBox(parent);
    wcb->resize(option.rect.width(), option.rect.height());
    qDebug() << __PRETTY_FUNCTION__ << option.rect;
    return wcb;
}

void unoSearchDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    qDebug() << __PRETTY_FUNCTION__;
    QCheckBox* cb = qobject_cast<QCheckBox *>(editor);
    Qt::CheckState chs = (Qt::CheckState)index.data(Qt::CheckStateRole).toInt();
    if (cb)
        cb->setCheckState(chs);
}

void unoSearchDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    qDebug() << __PRETTY_FUNCTION__;
    QCheckBox* cb = qobject_cast<QCheckBox *>(editor);
    if (cb) {
        Qt::CheckState chs = cb->checkState();
        model->setData(index, chs, Qt::CheckStateRole);
    }
}
