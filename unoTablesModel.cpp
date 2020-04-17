#include "unoTablesModel.h"

unoTablesModel::unoTablesModel(const QStringList& tableNames, const vector< Reference< XTextTable > >& xTableRef, QObject* parent) :
    tableNames_(tableNames),
    xTablesRef_(xTableRef),
    QAbstractItemModel(parent) {
}

unoTablesModel::~unoTablesModel() {}

int unoTablesModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return 1;
}

QVariant unoTablesModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    int iRow = index.row();
    if (role == Qt::DisplayRole)
        return tableNames_[iRow];
    else if (role == Qt::UserRole)
        return iRow;

    return QVariant();
}

QModelIndex unoTablesModel::index(int row, int column, const QModelIndex& parent) const {
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    QString tName = tableNames_[row];
    return createIndex(row, column, &tName);
}

QModelIndex unoTablesModel::parent(const QModelIndex& /*index*/) const {
    return QModelIndex();
}

int unoTablesModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return tableNames_.count();
}

QVariant unoTablesModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QStringList listHeader;
    listHeader << tr("Table name");
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return listHeader[section];
    else
        return QVariant();
}
