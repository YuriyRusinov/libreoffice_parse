#include <QMdiArea>
#include <QFileDialog>
#include <QtDebug>

#include "unoFileWidget.h"
#include "unoMainWindow.h"
#include "ui_uno_main_window.h"
/*
#include <sal/main.h>
#include <cppuhelper/bootstrap.hxx>

#include <osl/file.hxx>
#include <osl/process.h>
#include <rtl/process.h>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>
*/

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
    unoFileWidget* w = new unoFileWidget;
    w->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow * subW = _mdiArea->addSubWindow(w);
    w->show();
}
