#include <QAction>
#include <QGridLayout>
#include <QTextEdit>
#include <QToolBar>
#include <QtDebug>

#include "unoFileWidget.h"

unoFileWidget::unoFileWidget(QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags),
    _tbActions(new QToolBar),
    _fileEditW(new QTextEdit) {

    QGridLayout * gridLay = new QGridLayout(this);
    gridLay->addWidget(_tbActions, 0, 0, 1, 1);
    gridLay->addWidget(_fileEditW, 1, 0, 1, 1);

    QAction* actOpen  = _tbActions->addAction(QIcon(":/libre_resources/open.png"), tr("Open ..."));
    QAction* actClose = _tbActions->addAction(QIcon(":/libre_resources/close.png"), tr("Close"));

    QObject::connect(actOpen, &QAction::triggered, this, &unoFileWidget::slotFileOpen);
    QObject::connect(actClose, &QAction::triggered, this, &unoFileWidget::slotFileClose);
}

unoFileWidget::~unoFileWidget() {
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
