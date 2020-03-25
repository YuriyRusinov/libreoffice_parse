#include <QDir>
#include <QMdiArea>
#include <QFileDialog>
#include <QUrl>
#include <QtDebug>

#include <sstream>

#include "unoFileWidget.h"
#include "unoMainWindow.h"
#include "ui_uno_main_window.h"

#include <sal/main.h>

#include <cppuhelper/bootstrap.hxx>

#include <osl/file.hxx>
#include <osl/process.h>
#include <rtl/process.h>
#include <rtl/string.h>
#include <rtl/ustrbuf.hxx>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::bridge;
using namespace com::sun::star::frame;
using namespace com::sun::star::registry;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::rtl::OUStringToOString;
using std::stringstream;

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
    QString fileName = QFileDialog::getOpenFileName (
                this,
                tr ("Open File"),
                QDir::currentPath(),
                tr ("Open Document Text Files (*.odt)"));
    if (fileName.isEmpty())
        return;

    QUrl fileUrl=QUrl::fromLocalFile(fileName);
    qDebug() << __PRETTY_FUNCTION__ << fileUrl << fileUrl.toString().length();
    OUString sConnectionString("uno:socket,host=localhost,port=2083;urp;StarOffice.ServiceManager");

    OUStringBuffer buf (fileUrl.toString().length());
    unoFileWidget* w = new unoFileWidget;
    w->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow * subW = _mdiArea->addSubWindow(w);
    w->show();
}
