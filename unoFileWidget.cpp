/*
 * Widget, выводящий необходимые параметры загруженного файла и список
 * таблиц
 *
 * (C) НПО Рубин
 * @author:
 *   Ю.Л.Русинов
 */
#include <QAbstractItemModel>
#include <QAction>
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
#include <QtDebug>

#include "unoFileWidget.h"
#include "unoTablesModel.h"

unoFileWidget::unoFileWidget(const QUrl& fileUrl, QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags),
    _fileUrl(fileUrl),
    _tbActions(new QToolBar),
    _spView(new QSplitter),
    _fileEditW(new QTextEdit),
    _wTables(new QWidget),
    _tvTables(new QTreeView),
    _tbTableActions(new QToolBar),
    _wSearch(new QWidget),
    _lSearch(new QLabel(tr("Search:"))),
    _leSearch(new QLineEdit)
{
    qDebug() << __PRETTY_FUNCTION__ << _fileUrl;
    QGridLayout * gridLay = new QGridLayout(this);
    gridLay->addWidget(_tbActions, 0, 0, 1, 2);
    gridLay->addWidget(_spView, 1, 0, 1, 2);
    _spView->addWidget(_fileEditW);
    _spView->addWidget(_wTables);

    QHBoxLayout* hTableLay = new QHBoxLayout (_wTables);
    hTableLay->addWidget(_tvTables);
    _tbTableActions->setOrientation(Qt::Vertical);
    hTableLay->addWidget(_tbTableActions);

    QAction* actOpenFile  = _tbActions->addAction(QIcon(":/libre_resources/open.png"), tr("Open ..."));
    QAction* actClose = _tbActions->addAction(QIcon(":/libre_resources/close.png"), tr("Close"));
    QAction* actSep = _tbActions->addSeparator();
    QHBoxLayout* hSLay = new QHBoxLayout(_wSearch);
    hSLay->addWidget(_lSearch);
    hSLay->addWidget(_leSearch);
    QAction* actLE = _tbActions->addWidget(_wSearch);
    QAction* actSearch = _tbActions->addAction(QIcon(":/libre_resources/search.jpg"), tr("Search ..."));
    QAction* actSaveFile = _tbActions->addAction(QIcon(":/libre_resources/save.png"), tr("Save ..."));
    actSaveFile->setToolTip(tr("Save file"));

    QAction* actTableRowAdd = _tbTableActions->addAction(QIcon(":/libre_resources/add_row.png"), tr("Add row to selected table"));
    actTableRowAdd->setToolTip(tr("Add row to selected table"));
    QAction* actTableRowDel = _tbTableActions->addAction(QIcon(":/libre_resources/del_row.png"), tr("Del row from selected table"));
    actTableRowDel->setToolTip(tr("Remove row from selected table"));

    QObject::connect(actTableRowAdd, &QAction::triggered, this, &unoFileWidget::slotAddRowToTable);
    QObject::connect(actTableRowDel, &QAction::triggered, this, &unoFileWidget::slotDelRowFromTable);

    QObject::connect(actOpenFile, &QAction::triggered, this, &unoFileWidget::slotFileOpen);
    QObject::connect(actSaveFile, &QAction::triggered, this, &unoFileWidget::slotSaveFile);
    QObject::connect(actSearch, &QAction::triggered, this, &unoFileWidget::slotSearch);
    QObject::connect(actClose, &QAction::triggered, this, &unoFileWidget::slotFileClose);
}

unoFileWidget::~unoFileWidget() {
    delete _leSearch;
    delete _lSearch;
    delete _wSearch;
    delete _tbTableActions;
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
    QString searchString = _leSearch->text();
    qDebug() << __PRETTY_FUNCTION__ << searchString;
    emit search(searchString);
}

void unoFileWidget::slotAddRowToTable() {
    QItemSelectionModel* selTableModel = _tvTables->selectionModel();
    QModelIndexList tbIndexes = selTableModel->selectedIndexes();
    if (tbIndexes.size() == 0) {
        QMessageBox::warning(this, tr("Tables"), tr("Please select table to add rows"), QMessageBox::Ok);
        return;
    }
    wTableIndex = tbIndexes[0];
    QAbstractItemModel* tModel = _tvTables->model();
    Reference< XTextTable > wTable = tModel->data(wTableIndex, Qt::UserRole).value<Reference< XTextTable > >();
    qDebug() << __PRETTY_FUNCTION__ << wTable.get();
    int iRowMin = 0;
    QModelIndex rIndex = tbIndexes[1];
    int iRowMax = tModel->data(rIndex, Qt::DisplayRole).toInt();
    bool ok;
    int nRow = QInputDialog::getInt(this, tr("Insert row"), tr("Row :"), iRowMax, 0, iRowMax, 1, &ok);
    if (ok)
        emit addRowToTable(wTable, nRow);
}

void unoFileWidget::slotDelRowFromTable() {
    QItemSelectionModel* selTableModel = _tvTables->selectionModel();
    QModelIndexList tbIndexes = selTableModel->selectedIndexes();
    if (tbIndexes.size() == 0) {
        QMessageBox::warning(this, tr("Tables"), tr("Please select table to remove rows"), QMessageBox::Ok);
        return;
    }
    wTableIndex = tbIndexes[0];
    QAbstractItemModel* tModel = _tvTables->model();
    Reference< XTextTable > wTable = tModel->data(wTableIndex, Qt::UserRole).value<Reference< XTextTable > >();
    qDebug() << __PRETTY_FUNCTION__ << wTable.get();
    int iRowMin = 0;
    QModelIndex rIndex = tbIndexes[1];
    int iRowMax = tModel->data(rIndex, Qt::DisplayRole).toInt();
    bool ok;
    int nRow = QInputDialog::getInt(this, tr("Remove row"), tr("Row :"), iRowMax, 0, iRowMax, 1, &ok);
    if (ok)
        emit delRowFromTable(wTable, nRow);
}

void unoFileWidget::updateTableModel(Reference< XTextTable > wTable) {
    QAbstractItemModel* tModel = _tvTables->model();
    QVariant val = QVariant::fromValue(wTable);
    tModel->setData(wTableIndex, val, Qt::UserRole);
}

void unoFileWidget::slotSaveFile() {
    qDebug() << __PRETTY_FUNCTION__;
}
