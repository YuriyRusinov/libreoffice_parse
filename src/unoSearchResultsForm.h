/* @brief
 * Класс формы просмотра результатов поиска в таблицах внутри текстовых документов libreoffice.
 * (C) НИИ Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */

#pragma once

#include <vector>
#include <QWidget>
#include <cppu/unotype.hxx>
#include <com/sun/star/text/XTextTable.hpp>

namespace Ui {
    class uno_search_results_form;
}

class QAbstractItemModel;
class QModelIndex;

using std::vector;
using namespace com::sun::star::uno;
using namespace com::sun::star::text;

class unoSearchResultsForm : public QWidget {
public:
    unoSearchResultsForm(QString searchStr, const vector< Reference< XTextTable > >& sTables, QWidget* parent=nullptr, Qt::WindowFlags flags=Qt::WindowFlags());
    ~unoSearchResultsForm();

    void setResultsModel(QAbstractItemModel* resModel);

private slots:
    void tableIndexChanged(const QModelIndex& current, const QModelIndex& previous);

private:
    Ui::uno_search_results_form* _UI;
    QString _searchStr;
    vector< Reference< XTextTable > > _sTables;

private:
    Q_OBJECT
};
