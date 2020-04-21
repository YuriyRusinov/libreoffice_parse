/*
 * Модель визуального отображения списка таблиц в odt-файле
 */
#pragma once

#include <QAbstractItemModel>
#include <vector>
#include <com/sun/star/text/XTextTable.hpp>

using std::vector;

using namespace com::sun::star::table;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;

class unoTablesModel : public QAbstractItemModel {
public:
    unoTablesModel(const QStringList& tableNames, const vector< Reference< XTextTable > >& xTableRef, QObject* parent=nullptr);
    virtual ~unoTablesModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QStringList _tableNames;
    vector< Reference< XTextTable > > _xTablesRef;
    vector< int > _tableCols;
    vector< int > _tableRows;

private:
    Q_OBJECT
};

