/* @brief
 * Модель визуального отображения списка таблиц в odt-файле для поиска
 *
 * (C) НИИ Рубин
 * @author:
 *   Ю.Л.Русинов
 */
#include "unoSearchTablesModel.h"

unoSearchTablesModel::unoSearchTablesModel(const QStringList& tableNames, const vector< Reference< XTextTable > >& xTableRef, QObject* parent)
    : QAbstractItemModel(parent),
    _tableNames(tableNames),
    _xTablesRef(xTableRef) {
        int nt = _xTablesRef.size();
        for (int i=0; i<nt; i++) {
            _tableCheckState.push_back (Qt::Unchecked);
        }
}

unoSearchTablesModel::~unoSearchTablesModel() {}

int unoSearchTablesModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return 1;
}

QVariant unoSearchTablesModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();
    int iRow = index.row();
    if (role == Qt::DisplayRole)
        return _tableNames[iRow];
    else if (role == Qt::CheckStateRole) {
        return _tableCheckState[iRow];
    }
    else if (role == Qt::UserRole) {
        Reference< XTextTable > xTable = _xTablesRef[iRow];
        QVariant val = QVariant::fromValue(xTable);
        return val;
    }

    return QVariant();
}

bool unoSearchTablesModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid())
        return false;

    int iRow = index.row();
    if (role == Qt::UserRole) {
        Reference< XTextTable > xTable = value.value< Reference< XTextTable > >();
        _xTablesRef[iRow] = xTable;
        QModelIndex ibegin = index.sibling(iRow, 0);
        QModelIndex iend = index.sibling(iRow, 0);
        emit dataChanged(ibegin, iend);
        return true;
    }
    else if (role == Qt::CheckStateRole) {
        _tableCheckState[iRow] = (Qt::CheckState)value.toInt();
        QModelIndex ibegin = index.sibling(iRow, 0);
        QModelIndex iend = index.sibling(iRow, 0);
        emit dataChanged(ibegin, iend);
        return true;
    }
    return false;
}

QModelIndex unoSearchTablesModel::index(int row, int column, const QModelIndex &parent) const {
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    QString tName = _tableNames[row];
    return createIndex(row, column, &tName);
}

QModelIndex unoSearchTablesModel::parent(const QModelIndex& /* index */) const {
    return QModelIndex();
}

int unoSearchTablesModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _tableNames.count();
}

QVariant unoSearchTablesModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QStringList listHeader;
    listHeader << tr("Table name") << tr("Number of rows") << tr("Number of columns");
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return listHeader[section];
    else
        return QVariant();
}

Qt::ItemFlags unoSearchTablesModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}
