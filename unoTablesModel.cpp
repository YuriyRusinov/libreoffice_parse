/*
 * Модель визуального отображения списка таблиц в odt-файле
 */
#include <com/sun/star/table/XTableColumns.hpp>
#include <com/sun/star/table/XTableRows.hpp>
#include "unoTablesModel.h"

unoTablesModel::unoTablesModel(const QStringList& tableNames, const vector< Reference< XTextTable > >& xTableRef, QObject* parent) :
    _tableNames(tableNames),
    _xTablesRef(xTableRef),
    QAbstractItemModel(parent) {
    int nt = _xTablesRef.size();
    for (int i=0; i<nt; i++) {
        Reference< XTextTable > xTable = _xTablesRef[i];
        Reference< XTableColumns > tabCols = xTable->getColumns();
        Reference< XTableRows > tabRows = xTable->getRows();
        _tableCols.push_back(tabCols->getCount());
        _tableRows.push_back(tabRows->getCount());
    }
}

unoTablesModel::~unoTablesModel() {}

int unoTablesModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant unoTablesModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    int iRow = index.row();
    int iColumn = index.column();
    if (role == Qt::DisplayRole) {
        switch (iColumn) {
            case 0: return _tableNames[iRow]; break;
            case 1: {
                        return _tableRows[iRow];
                        break;
                    }
            case 2: {
                        return _tableCols[iRow];
                        break;
                    }
            default: return QVariant(); break;
        }
    }
    else if (role == Qt::UserRole) {
        Reference< XTextTable > xTable = _xTablesRef[iRow];
        QVariant val = QVariant::fromValue(xTable);
        return val;
    }

    return QVariant();
}

bool unoTablesModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid())
        return false;

    int iRow = index.row();
    if (role == Qt::UserRole) {
        Reference< XTextTable > xTable = value.value< Reference< XTextTable > >();
        _xTablesRef[iRow] = xTable;
        Reference< XTableColumns > tabCols = xTable->getColumns();
        Reference< XTableRows > tabRows = xTable->getRows();
        _tableCols[iRow] = tabCols->getCount();
        _tableRows[iRow] = tabRows->getCount();
        QModelIndex ibegin = index.sibling(iRow, 0);
        QModelIndex iend = index.sibling(iRow, 2);
        emit dataChanged(ibegin, iend);
        return true;
    }
    else if (role == Qt::EditRole || role == Qt::DisplayRole) {
        int iCol = index.column();
        switch (iCol) {
            case 0: _tableNames[iRow] = value.toString(); break;
            case 1: _tableRows[iRow] = value.toInt(); break;
            case 2: _tableCols[iRow] = value.toInt(); break;
            default: return false; break;
        }
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

QModelIndex unoTablesModel::index(int row, int column, const QModelIndex& parent) const {
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    QString tName = _tableNames[row];
    return createIndex(row, column, &tName);
}

QModelIndex unoTablesModel::parent(const QModelIndex& /*index*/) const {
    return QModelIndex();
}

int unoTablesModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return _tableNames.count();
}

QVariant unoTablesModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QStringList listHeader;
    listHeader << tr("Table name") << tr("Number of rows") << tr("Number of columns");
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return listHeader[section];
    else
        return QVariant();
}
