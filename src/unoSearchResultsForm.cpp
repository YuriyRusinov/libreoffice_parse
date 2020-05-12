/* @brief
 *  Класс формы просмотра результатов поиска в таблицах внутри текстовых документов libreoffice.
 * (C) НИИ Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QtDebug>

#include "unoSearchResultsForm.h"
#include "ui_uno_search_results_form.h"

unoSearchResultsForm::unoSearchResultsForm(QString searchStr, const vector< Reference< XTextTable > >& sTables, QWidget* parent, Qt::WindowFlags flags)
    : QWidget( parent, flags ),
    _UI( new Ui::uno_search_results_form ),
    _searchStr( searchStr ),
    _sTables( sTables ) {
    _UI->setupUi(this);
    _UI->lESearch->setText(_searchStr);
    QItemSelectionModel* selModel = _UI->tvSearchResults->selectionModel();

    QObject::connect(selModel, &QItemSelectionModel::currentChanged, this, &unoSearchResultsForm::tableIndexChanged);
}

unoSearchResultsForm::~unoSearchResultsForm() {
    delete _UI;
}

void unoSearchResultsForm::setResultsModel(QAbstractItemModel* resModel) {
    QAbstractItemModel* oldModel = _UI->tvSearchResults->model();
    _UI->tvSearchResults->setModel( resModel );
    if (oldModel && oldModel != resModel)
        delete oldModel;
}

void unoSearchResultsForm::tableIndexChanged(const QModelIndex& current, const QModelIndex& previous) {
    qDebug() << __PRETTY_FUNCTION__ << current << previous;
}
