/* @brief
 * Форма выбора координат ячейки в таблице
 *
 * (C) НИИ Рубин
 * @author:
 *   Ю.Л.Русинов
 */
#include <QtDebug>
#include "unoTableCellForm.h"
#include "ui_uno_table_cell_form.h"

UnoTableCellForm::UnoTableCellForm(int rowMax, int colMax, QWidget* parent, Qt::WindowFlags flags)
    : QDialog( parent, flags ),
    _UI(new Ui::uno_table_cell_form) {
    _UI->setupUi( this );
    _UI->spRow->setMaximum( rowMax );
    _UI->spColumn->setMaximum( colMax );
}

UnoTableCellForm::~UnoTableCellForm() {
    delete _UI;
}

int UnoTableCellForm::getRow() const {
    return _UI->spRow->value();
}

int UnoTableCellForm::getColumn() const {
    return _UI->spColumn->value();
}
