#include <QDir>
#include <QMdiArea>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QUrl>
#include <QtDebug>

#include <sstream>
#include <iostream>

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
#include <rtl/byteseq.h>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>
#include <com/sun/star/table/XTable.hpp>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::bridge;
using namespace com::sun::star::frame;
using namespace com::sun::star::registry;
using namespace com::sun::star::table;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::rtl::OUStringToOString;
using std::stringstream;
using std::cerr;
using std::endl;

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
    int nlen = fileUrl.toString().length();
    qDebug() << __PRETTY_FUNCTION__ << fileUrl.toString().toStdString().c_str() << nlen;

    OUString sConnectionString("uno:socket,host=localhost,port=2083;urp;StarOffice.ServiceManager");

    OUStringBuffer buf;
    for (int i=0; i<nlen; i++)
        buf.append( fileUrl.toString().toStdString().at(i) );
    cerr << __PRETTY_FUNCTION__ << ' ' << buf.toString() << ' ' << fileUrl.toString().toStdString() << endl;
    osl::File osf( buf.toString() );
    osl::FileBase::RC res = osf.open (0);
    if (res != osl::FileBase::E_None)
        return;
    sal_uInt64 fileSize;
    osf.getSize( fileSize );
    void* fileContent = operator new(fileSize);

    sal_uInt64 readBytes;
    res = osf.read( fileContent, fileSize, readBytes);
    cerr << __PRETTY_FUNCTION__ << ' ' << fileSize << ' ' << readBytes << ' ' << res << ' ' << osl::FileBase::E_None << endl;

    osf.close();
    QByteArray ba = QByteArray::fromRawData( static_cast<const char*>(fileContent), readBytes );
    QFile fileTest("ttt.odt");
    if (!fileTest.open(QIODevice::WriteOnly))
        return;
    QDataStream tstStr( &fileTest );
    tstStr.writeRawData( ba.constData(), ba.size());

    Reference< XComponentContext > xComponentContext(::cppu::defaultBootstrap_InitialComponentContext());
    /* Gets the service manager instance to be used (or null). This method has
       been added for convenience, because the service manager is a often used
       object.
    */
    Reference< XMultiComponentFactory > xMultiComponentFactoryClient(
        xComponentContext->getServiceManager() );

    /* Creates an instance of a component which supports the services specified
       by the factory.
    */
    Reference< XInterface > xInterface =
        xMultiComponentFactoryClient->createInstanceWithContext(
            "com.sun.star.bridge.UnoUrlResolver",
            xComponentContext );

    Reference< XUnoUrlResolver > resolver( xInterface, UNO_QUERY );
    // Resolves the component context from the office, on the uno URL given by argv[1].
    try
    {
        xInterface = Reference< XInterface >(
            resolver->resolve( sConnectionString ), UNO_QUERY );
    }
    catch ( Exception& e )
    {
        printf("Error: cannot establish a connection using '%s':\n       %s\n",
               OUStringToOString(sConnectionString, RTL_TEXTENCODING_ASCII_US).getStr(),
               OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US).getStr());
        exit(1);
    }

    unoFileWidget* w = new unoFileWidget;
    w->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow * subW = _mdiArea->addSubWindow(w);
    w->show();
    operator delete( fileContent );
}
