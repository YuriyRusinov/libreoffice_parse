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
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTextEdit>
#include <QToolBar>
#include <QTreeView>
#include <QtDebug>

#include "unoFileWidget.h"

unoFileWidget::unoFileWidget(QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags),
    _tbActions(new QToolBar),
    _spView(new QSplitter),
    _fileEditW(new QTextEdit),
    _tvTables(new QTreeView),
    _wSearch(new QWidget),
    _lSearch(new QLabel(tr("Search:"))),
    _leSearch(new QLineEdit) {
    QGridLayout * gridLay = new QGridLayout(this);
    gridLay->addWidget(_tbActions, 0, 0, 1, 2);
    gridLay->addWidget(_spView, 1, 0, 1, 2);
    _spView->addWidget(_fileEditW);
    _spView->addWidget(_tvTables);

    QAction* actOpen  = _tbActions->addAction(QIcon(":/libre_resources/open.png"), tr("Open ..."));
    QAction* actClose = _tbActions->addAction(QIcon(":/libre_resources/close.png"), tr("Close"));
    QAction* actSep = _tbActions->addSeparator();
    QHBoxLayout* hSLay = new QHBoxLayout(_wSearch);
    hSLay->addWidget(_lSearch);
    hSLay->addWidget(_leSearch);
    QAction* actLE = _tbActions->addWidget(_wSearch);
    QAction* actSearch = _tbActions->addAction(QIcon(":/libre_resources/search.jpg"), tr("Search ..."));

    QObject::connect(actOpen, &QAction::triggered, this, &unoFileWidget::slotFileOpen);
    QObject::connect(actSearch, &QAction::triggered, this, &unoFileWidget::slotSearch);
    QObject::connect(actClose, &QAction::triggered, this, &unoFileWidget::slotFileClose);
}

unoFileWidget::~unoFileWidget() {
    delete _leSearch;
    delete _lSearch;
    delete _wSearch;
    delete _tvTables;
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
}
