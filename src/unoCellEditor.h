/* @brief
 * Класс рeдактирования ячейки таблицы в файле libreoffice через механизм uno.
 * (C) НИИ Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */
#pragma once

#include <QDialog>

#include <cppu/unotype.hxx>

#include <com/sun/star/text/XTextTable.hpp>
#include <com/sun/star/table/XCell.hpp>

using namespace com::sun::star::table;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;

class QAbstractButton;

namespace Ui {
    class uno_cell_editor;
}

class unoCellEditor : public QDialog {
public:
    unoCellEditor(Reference< XCell > wCell, int iRow, int jColumn, QWidget* parent=nullptr, Qt::WindowFlags flags=Qt::WindowFlags());
    ~unoCellEditor();

private slots:
    void buttonClicked( QAbstractButton* button );
    void acceptCell();

protected:
    void closeEvent( QCloseEvent* event ) override;

signals:
    void updateCell(Reference< XCell >, QString newText);

private:
    Ui::uno_cell_editor* _UI;
    Reference< XCell > _wCell;
    int _iRow;
    int _jColumn;

private:
    Q_OBJECT
};
