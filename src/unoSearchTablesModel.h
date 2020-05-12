/* @brief
 * Модель визуального отображения списка таблиц в odt-файле для поиска
 *
 * (C) НИИ Рубин
 * @author:
 *   Ю.Л.Русинов
 */
#pragma once

#include <QAbstractItemModel>
#include <vector>
#include <uno/unoQtReferences.h>

using std::vector;

using namespace com::sun::star::table;

class unoSearchTablesModel : public QAbstractItemModel {
public:
    unoSearchTablesModel(const QStringList& tableNames, const vector< Reference< XTextTable > >& xTableRef, QObject* parent=nullptr);
    virtual ~unoSearchTablesModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QStringList _tableNames;
    vector< Reference< XTextTable > > _xTablesRef;
    vector< int > _tableCheckState;

private:
    Q_OBJECT
};
