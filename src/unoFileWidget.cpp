/* @brief
 * Widget, выводящий необходимые параметры загруженного файла и список
 * таблиц
 *
 * (C) НИИ Рубин
 * @author:
 *   Ю.Л.Русинов
 */
#include <QAbstractItemModel>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTextEdit>
#include <QToolBar>
#include <QTreeView>
#include <QUrl>
#include <QtDebug>

#include "unoFileWidget.h"
#include "unoTablesModel.h"
#include "unoSearchForm.h"
#include "unoSearchTablesModel.h"
#include "unoTableCellForm.h"

unoFileWidget::unoFileWidget(const QUrl& fileUrl, QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags),
    _fileUrl(fileUrl),
    _tbActions(new QToolBar),
    _spView(new QSplitter),
    _fileEditW(new QTextEdit),
    _wTables(new QWidget),
    _tvTables(new QTreeView)
{
    qDebug() << __PRETTY_FUNCTION__ << _fileUrl;
    QGridLayout * gridLay = new QGridLayout(this);
    gridLay->addWidget(_tbActions, 0, 0, 1, 2);
    gridLay->addWidget(_spView, 1, 0, 1, 2);
    _spView->addWidget(_fileEditW);
    _spView->addWidget(_wTables);

    QHBoxLayout* hTableLay = new QHBoxLayout (_wTables);
    hTableLay->addWidget(_tvTables);
    _tvTables->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _tvTables->setSelectionBehavior(QAbstractItemView::SelectRows);

    this->initActions();
}

unoFileWidget::~unoFileWidget() {
    delete _tvTables;
    delete _wTables;
    delete _fileEditW;
    delete _spView;
    delete _tbActions;
}

void unoFileWidget::slotFileOpen() {
    qDebug () << __PRETTY_FUNCTION__;
}

void unoFileWidget::slotFileClose() {
    qDebug () << __PRETTY_FUNCTION__;
    if (parentWidget())
        parentWidget()->close();
    close();
}

void unoFileWidget::setText(const QString& text) {
    _fileEditW->setPlainText(text);
}

void unoFileWidget::setTablesModel(QAbstractItemModel* tableListModel) {
    QAbstractItemModel * oldModel =_tvTables->model();
    _tvTables->setModel(tableListModel);
    if (oldModel && oldModel != tableListModel)
        delete oldModel;
}

void unoFileWidget::slotSearch() {
    unoSearchForm* uSearchForm = new unoSearchForm;
    if (!uSearchForm)
        return;
    QAbstractItemModel* tModel = _tvTables->model();
    unoTablesModel* unoTabMod = qobject_cast<unoTablesModel *>(tModel);
    if (!unoTabMod)
        return;
    QStringList tableNames;
    vector< Reference< XTextTable > > refTab;
    int nt = tModel->rowCount();
    for (int i=0; i<nt; i++) {
        QModelIndex wInd = tModel->index(i, 0);
        QString tableName = tModel->data(wInd, Qt::DisplayRole).toString();
        Reference< XTextTable > table = tModel->data(wInd, Qt::UserRole).value< Reference< XTextTable > >();
        tableNames.append( tableName );
        refTab.push_back( table );
    }
    unoSearchTablesModel* stModel = new unoSearchTablesModel( tableNames, refTab );
    uSearchForm->setDocTablesModel( stModel );
    QString searchString;
    if (uSearchForm->exec() == QDialog::Accepted) {
        searchString = uSearchForm->getSearchString();
    }
    else {
        delete uSearchForm;
        return;
    }
    QAbstractItemModel* stModelRet = uSearchForm->getDocTablesModel();
    int nTables = stModelRet->rowCount();
    vector< Reference< XTextTable > > selTables;
    QStringList selTableNames;
    for (int i=0; i<nTables; i++) {
        QModelIndex tIndex = stModelRet->index(i, 0);
        Qt::CheckState isTableChecked = stModelRet->data(tIndex, Qt::CheckStateRole).value<Qt::CheckState>();
        if (isTableChecked == Qt::Checked) {
            Reference< XTextTable > table = stModelRet->data(tIndex, Qt::UserRole).value< Reference< XTextTable > >();
            QString tableName = stModelRet->data(tIndex, Qt::DisplayRole).toString();
            selTables.push_back( table );
            selTableNames.append( tableName );
        }
    }
    qDebug() << __PRETTY_FUNCTION__ << searchString;
    delete uSearchForm;
    if (!selTables.empty())
        emit search(searchString, selTables, selTableNames);
}

void unoFileWidget::slotAddRowToTable() {
    QModelIndexList tbIndexes = getTableIndexes();
    Reference< XTextTable > wTable = getTable();
    qDebug() << __PRETTY_FUNCTION__ << wTable.get();
    if (!wTable.is())
        return;
    int nRow = getTableParameter(tbIndexes, tableAdd, tableRow);
    if (nRow >= 0)
        emit tableActSignal(tbIndexes[0], wTable, tableAdd, tableRow, nRow);
}

void unoFileWidget::slotDelRowFromTable() {
    QModelIndexList tbIndexes = getTableIndexes();
    Reference< XTextTable > wTable = getTable();
    qDebug() << __PRETTY_FUNCTION__ << wTable.get();
    if (!wTable.is())
        return;
    int nRow = getTableParameter(tbIndexes, tableDel, tableRow);
    if (nRow >= 0)
        emit tableActSignal(tbIndexes[0], wTable, tableDel, tableRow, nRow);
}

void unoFileWidget::slotAddColumnToTable() {
    QModelIndexList tbIndexes = getTableIndexes();
    Reference< XTextTable > wTable = getTable();
    qDebug() << __PRETTY_FUNCTION__ << wTable.get();
    if (!wTable.is())
        return;
    int nCol = getTableParameter(tbIndexes, tableAdd, tableColumn);
    if (nCol >= 0)
        emit tableActSignal(tbIndexes[0], wTable, tableAdd, tableColumn, nCol);
}

void unoFileWidget::slotDelColumnFromTable() {
    QModelIndexList tbIndexes = getTableIndexes();
    Reference< XTextTable > wTable = getTable();
    qDebug() << __PRETTY_FUNCTION__ << wTable.get();
    if (!wTable.is())
        return;
    int nCol = getTableParameter(tbIndexes, tableDel, tableColumn);
    if (nCol >= 0)
        emit tableActSignal(tbIndexes[0], wTable, tableDel, tableColumn, nCol);
}

void unoFileWidget::updateTableModel(QModelIndex wTableIndex, Reference< XTextTable > wTable) {
    QAbstractItemModel* tModel = _tvTables->model();
    QVariant val = QVariant::fromValue(wTable);
    tModel->setData(wTableIndex, val, Qt::UserRole);
}

void unoFileWidget::slotSaveFile() {
    qDebug() << __PRETTY_FUNCTION__;
    QUrl saveUrl = QFileDialog::getSaveFileUrl(this, tr("Save file"), QUrl(), tr ("Open Document Text Files (*.odt);;All files (*)"));
    emit saveWriterFile(saveUrl);
}

QModelIndexList unoFileWidget::getTableIndexes() const {
    QItemSelectionModel* selTableModel = _tvTables->selectionModel();
    QModelIndexList tbIndexes = selTableModel->selectedIndexes();
    return tbIndexes;
}

Reference< XTextTable > unoFileWidget::getTable() {
    QModelIndexList tbIndexes = getTableIndexes();
    if (tbIndexes.size() == 0) {
        QMessageBox::warning(this, tr("Tables"), tr("Please select table"), QMessageBox::Ok);
        return nullptr;
    }
    QAbstractItemModel* tModel = _tvTables->model();
    QModelIndex tableIndex = tbIndexes[0];
    Reference< XTextTable > wTable = tModel->data(tableIndex, Qt::UserRole).value<Reference< XTextTable > >();
    return wTable;
}

int unoFileWidget::getTableParameter(const QModelIndexList& selIndexes, unoFileWidget::tableActions tabActCode, unoFileWidget::tableCellParams tabParam) {
    if (selIndexes.size() < 3)
        return -1;
    int iRowMin = 0;
    QModelIndex rIndex = (tabParam == tableRow ? selIndexes[1] : selIndexes[2]);
    int valMax = rIndex.data(Qt::DisplayRole).toInt();
    bool ok;
    QString tablePar = QString("%1").arg(tabParam == tableRow ? tr("Row") : tr("Column"));
    QString title = QString("%1 %2").arg(tabActCode==tableAdd ? tr("Insert") : tr("Remove"))
                                    .arg(tablePar);
    int nVal = QInputDialog::getInt(this, title, tr("%1 :").arg(tablePar), valMax, 0, valMax, 1, &ok);
    if (ok)
        return nVal;
    return -1;
}

void unoFileWidget::slotEditCellInTable() {
    Reference< XTextTable > wTable = getTable();
    if( !wTable.is() )
        return;
    Reference< XTableRows > tabRows( wTable->getRows() );
    Reference< XTableColumns > tabCols( wTable->getColumns() );
    int nRow = (int)tabRows->getCount()-1;
    int nCols = (int)tabCols->getCount()-1;
    UnoTableCellForm* tabCellForm = new UnoTableCellForm( nRow, nCols );
    if( !tabCellForm || tabCellForm->exec() != QDialog::Accepted) {
        if (tabCellForm)
            delete tabCellForm;
        return;
    }

    int ir = tabCellForm->getRow();
    int jc = tabCellForm->getColumn();
    emit editTableCell( wTable, ir, jc);
}

void unoFileWidget::initActions() {
    QAction* actOpenFile  = _tbActions->addAction(QIcon(":/libre_resources/open.png"), tr("Open ..."));
    QAction* actClose = _tbActions->addAction(QIcon(":/libre_resources/close.png"), tr("Close"));
    QAction* actSep = _tbActions->addSeparator();
    QAction* actTableCellEdit = _tbActions->addAction( QIcon(":/libre_resources/edit.png"), tr("Edit cell in table") );
    actTableCellEdit->setToolTip(tr("Edit cell content in selected table"));

    QAction* actSearch = _tbActions->addAction(QIcon(":/libre_resources/search.jpg"), tr("Search ..."));
    actSearch->setToolTip(tr("Search text in table cells"));
    QAction* actSep1 = _tbActions->addSeparator();
    QAction* actTableRowAdd = _tbActions->addAction(QIcon(":/libre_resources/add_row.png"), tr("Add row to selected table"));
    actTableRowAdd->setToolTip(tr("Add row to selected table"));
    QAction* actTableRowDel = _tbActions->addAction(QIcon(":/libre_resources/del_row.png"), tr("Delete row from selected table"));
    actTableRowDel->setToolTip(tr("Remove row from selected table"));
    QAction* actTableColumnAdd = _tbActions->addAction(QIcon(":/libre_resources/add_column.png"), tr("Add column to selected table"));
    actTableColumnAdd->setToolTip(tr("Add column to selected table"));
    QAction* actTableColumnDel = _tbActions->addAction(QIcon(":/libre_resources/del_column.png"), tr("Delete column from selected table"));
    actTableColumnDel->setToolTip(tr("Remove column from selected table"));

    QAction* actSep2 = _tbActions->addSeparator();
    QAction* actSaveFile = _tbActions->addAction(QIcon(":/libre_resources/save.png"), tr("Save file as ..."));
    actSaveFile->setToolTip(tr("Save file as ..."));

    QObject::connect(actTableRowAdd, &QAction::triggered, this, &unoFileWidget::slotAddRowToTable);
    QObject::connect(actTableRowDel, &QAction::triggered, this, &unoFileWidget::slotDelRowFromTable);
    QObject::connect(actTableColumnAdd, &QAction::triggered, this, &unoFileWidget::slotAddColumnToTable);
    QObject::connect(actTableColumnDel, &QAction::triggered, this, &unoFileWidget::slotDelColumnFromTable);
    QObject::connect(actTableCellEdit, &QAction::triggered, this, &unoFileWidget::slotEditCellInTable);

    QObject::connect(actOpenFile, &QAction::triggered, this, &unoFileWidget::slotFileOpen);
    QObject::connect(actSaveFile, &QAction::triggered, this, &unoFileWidget::slotSaveFile);
    QObject::connect(actSearch, &QAction::triggered, this, &unoFileWidget::slotSearch);
    QObject::connect(actClose, &QAction::triggered, this, &unoFileWidget::slotFileClose);
}
