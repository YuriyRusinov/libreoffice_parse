/* @brief
 * Модель визуального отображения результатов поиска в таблицах в odt-файле
 *
 * (C) НИИ Рубин
 * @author:
 *   Ю.Л.Русинов
 */
#include <QtGlobal>
#include "unoSearchResultsModel.h"

unoSearchResultsModel::unoSearchResultsModel(const QStringList& tableNames, const vector< Reference< XTextTable > >& xTableRef, const vector< pair<long, long> >& matrIndices, QObject* parent) :
    QAbstractItemModel( parent ),
    _tableNames( tableNames ),
    _xTablesRef( xTableRef ),
    _matrIndices( matrIndices ){
}

unoSearchResultsModel::~unoSearchResultsModel() {}

int unoSearchResultsModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant unoSearchResultsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    int iRow = index.row();
    int iColumn = index.column();
    if (role == Qt::UserRole) {
        Reference< XTextTable > xTable = _xTablesRef[iRow];
        QVariant val = QVariant::fromValue(xTable);
        return val;
    }
    else if (role == Qt::DisplayRole) {
        switch( iColumn ) {
            case 0: return _tableNames[iRow]; break;
            case 1: return (qlonglong)_matrIndices[iRow].first; break;
            case 2: return (qlonglong)_matrIndices[iRow].second; break;
            default: return QVariant(); break;
        }
    }

    return QVariant();
}

QModelIndex unoSearchResultsModel::index(int row, int column, const QModelIndex& parent) const {
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    QString tName = _tableNames[row];
    return createIndex(row, column, &tName);
}

QModelIndex unoSearchResultsModel::parent(const QModelIndex& /* index */) const {
    return QModelIndex();
}

int unoSearchResultsModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return _tableNames.count();
}

QVariant unoSearchResultsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QStringList listHeader;
    listHeader << tr("Table name") << tr("Row") << tr("Column");
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return listHeader[section];
    else
        return QVariant();
}
