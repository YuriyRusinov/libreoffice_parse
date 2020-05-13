/* @brief
 * Модель визуального отображения результатов поиска в таблицах в odt-файле
 *
 * (C) НИИ Рубин
 * @author:
 *   Ю.Л.Русинов
 */
#pragma once

#include <QAbstractItemModel>
#include <vector>
#include <utility>
#include <uno/unoQtReferences.h>

using std::vector;
using std::pair;

using namespace com::sun::star::table;

class unoSearchResultsModel : public QAbstractItemModel {
public:
    unoSearchResultsModel(const QStringList& tableNames, const vector< Reference< XTextTable > >& xTableRef, const vector< pair<long, long> >& matrIndices, QObject* parent=nullptr);
    virtual ~unoSearchResultsModel();

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QStringList _tableNames;
    vector< Reference< XTextTable > > _xTablesRef;
    vector< pair<long, long> > _matrIndices;

private:
    Q_OBJECT
};
