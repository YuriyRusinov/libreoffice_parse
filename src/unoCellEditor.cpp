/* @brief
 * Класс рeдактирования ячейки таблицы в файле libreoffice через механизм uno.
 * (C) НИИ Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */
#include <sstream>
#include <QAbstractButton>
#include <QtDebug>
#include <com/sun/star/text/XText.hpp>

#include "unoCellEditor.h"
#include "ui_uno_cell_editor.h"
using namespace com::sun::star::text;

using std::stringstream;

unoCellEditor::unoCellEditor(Reference< XCell > wCell, int iRow, int jColumn, QWidget* parent, Qt::WindowFlags flags)
    : QDialog( parent, flags ),
    _UI( new Ui::uno_cell_editor ),
    _wCell( wCell ),
    _iRow( iRow ),
    _jColumn( jColumn ) {
    _UI->setupUi(this);
    if (!wCell.is())
        return;

    Reference< XText > wText = Reference< XText >( wCell, UNO_QUERY );
    stringstream wCellStr;
    if (!wText.is())
        return;

    wCellStr << wText->getString();
    _UI->tECellEdit->setPlainText( QString::fromStdString(wCellStr.str()) );
    connect(_UI->buttonBox, &QDialogButtonBox::clicked, this, &unoCellEditor::buttonClicked);
    connect(_UI->buttonBox, &QDialogButtonBox::accepted, this, &unoCellEditor::acceptCell);
}

unoCellEditor::~unoCellEditor() {
    delete _UI;
}

void unoCellEditor::acceptCell() {
    emit updateCell(_wCell, _UI->tECellEdit->toPlainText());
    if (parentWidget())
        parentWidget()->close();
}

void unoCellEditor::buttonClicked(QAbstractButton* button) {
    if (_UI->buttonBox->buttonRole( button ) == QDialogButtonBox::ApplyRole)
        emit updateCell(_wCell, _UI->tECellEdit->toPlainText());
    else if (_UI->buttonBox->buttonRole( button ) == QDialogButtonBox::RejectRole)
        close();
}

void unoCellEditor::closeEvent( QCloseEvent* event ) {
    qDebug() << __PRETTY_FUNCTION__;
    QDialog::closeEvent(event);
}
