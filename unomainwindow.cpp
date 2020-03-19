#include <QMdiArea>
#include <QtDebug>

#include "unomainwindow.h"
#include "ui_uno_main_window.h"

UnoMainWindow::UnoMainWindow(QWidget *parent)
    : QMainWindow(parent),
    UI(new Ui::uno_main_window),
    _mdiArea(new QMdiArea)
{
    UI->setupUi(this);
    setCentralWidget(_mdiArea);

    QObject::connect(UI->actOpen, &QAction::triggered, this, &UnoMainWindow::slotOpen);
    QObject::connect(UI->actQuit, &QAction::triggered, this, &QMainWindow::close);
}

UnoMainWindow::~UnoMainWindow() {
    delete _mdiArea;
    delete UI;
}

void UnoMainWindow::slotOpen() {
    qDebug() << __PRETTY_FUNCTION__;
}
