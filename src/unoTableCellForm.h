/* @brief
 * Форма выбора координат ячейки в таблице
 *
 * (C) НИИ Рубин
 * @author:
 *   Ю.Л.Русинов
 */
#pragma once

#include <QDialog>

namespace Ui {
    class uno_table_cell_form;
}

class UnoTableCellForm : public QDialog {
public:
    UnoTableCellForm(int rowMax, int colMax, QWidget* parent=nullptr, Qt::WindowFlags flags=Qt::WindowFlags());
    ~UnoTableCellForm();

    int getRow() const;
    int getColumn() const;

private:
    Ui::uno_table_cell_form* _UI;

private:
    Q_OBJECT
};
