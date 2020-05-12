/* @brief
 * Форма поиска по таблицам через механизм uno
 *  (C) НИИ Рубин
 *
 * @author
 *   Ю.Л.Русинов
 */

#include <QAbstractItemModel>
#include <QtDebug>

#include "unoSearchForm.h"
#include "unoSearchDelegate.h"
#include "ui_uno_search_form.h"

unoSearchForm::unoSearchForm(QWidget* parent, Qt::WindowFlags flags)
    : QDialog(parent, flags),
    UI(new Ui::uno_search_form) {
    UI->setupUi(this);
    QAbstractItemDelegate* deleg = new unoSearchDelegate;
    UI->tvTables->setItemDelegate(deleg);

    QObject::connect(UI->searchButtonBox, &QDialogButtonBox::accepted, this, &unoSearchForm::startSearch);
}

unoSearchForm::~unoSearchForm() {
    delete UI;
}

QAbstractItemModel* unoSearchForm::getDocTablesModel() const {
    return UI->tvTables->model();
}

void unoSearchForm::setDocTablesModel(QAbstractItemModel* docTablesModel) {
    QAbstractItemModel* oldModel = UI->tvTables->model();
    UI->tvTables->setModel(docTablesModel);
    if (oldModel && oldModel != docTablesModel)
        delete oldModel;
}

QString unoSearchForm::getSearchString() const {
    return UI->lESearch->text();
}

void unoSearchForm::startSearch() {
    qDebug() << __PRETTY_FUNCTION__ << getSearchString();
    accept();
}
