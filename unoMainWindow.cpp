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

using std::stringstream;
using std::cerr;
using std::endl;

UnoMainWindow::UnoMainWindow(QWidget *parent)
    : QMainWindow(parent),
    UI(new Ui::uno_main_window),
    _mdiArea(new QMdiArea),
    _sConnectionString("uno:socket,host=localhost,port=2083;urp;StarOffice.ServiceManager"),
    _xComponentContext( ::cppu::bootstrap() ),
    /* Gets the service manager instance to be used (or null). This method has
       been added for convenience, because the service manager is a often used
       object.
    */
    _xMultiComponentFactoryClient( _xComponentContext->getServiceManager() ),
    /* Creates an instance of a component which supports the services specified
       by the factory.
    */
    _xInterface (
                _xMultiComponentFactoryClient->createInstanceWithContext(
                    "com.sun.star.bridge.UnoUrlResolver",
                     _xComponentContext )
            ),
    _resolver( _xInterface, UNO_QUERY ),
    // gets the server component context as property of the office component factory
    _xPropSet( _xInterface, UNO_QUERY ),
    // gets the service manager from the office
    _xMultiComponentFactoryServer( _xComponentContext->getServiceManager() ),
    // get an instance of the remote office desktop UNO service
    // and query the XComponentLoader interface
    _xComponentLoader ( Desktop::create(_xComponentContext) )
{
    UI->setupUi(this);
    setCentralWidget(_mdiArea);

    // Resolves the component context from the office, on the uno URL given by argv[1].
    try
    {
        _xInterface = Reference< XInterface >(
            _resolver->resolve( _sConnectionString ), UNO_QUERY );
    }
    catch ( Exception& e )
    {
        qDebug("Error: cannot establish a connection using '%s':\n       %s\n",
               OUStringToOString(_sConnectionString, RTL_TEXTENCODING_ASCII_US).getStr(),
               OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US).getStr());
        return;
    }

    //xPropSet->getPropertyValue("DefaultContext") >>= xComponentContext;

    QObject::connect(UI->actOpen, &QAction::triggered, this, &UnoMainWindow::slotOpen);
    QObject::connect(UI->actQuit, &QAction::triggered, this, &QMainWindow::close);
}

UnoMainWindow::~UnoMainWindow() {
    //Reference< XComponent >::query( _xMultiComponentFactoryClient )->dispose();
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

/*
 *  For tests
 */
#ifdef _UNO_TEST_
    QByteArray ba = QByteArray::fromRawData( static_cast<const char*>(fileContent), readBytes );
    QFile fileTest("ttt.odt");
    if (!fileTest.open(QIODevice::WriteOnly))
        return;
    QDataStream tstStr( &fileTest );
    tstStr.writeRawData( ba.constData(), ba.size());
#endif
    cerr << __PRETTY_FUNCTION__ << "Name of " << _xComponentLoader->getName().toAsciiLowerCase () << endl;
    // getStr();
//
//  From wiki.openoffice.org for tests
//
//    Reference < XComponentLoader > xComponentLoader(
//            xMultiComponentFactoryClient->createInstanceWithContext( OUString(
//            RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ) ),
//            xComponentContext ), UNO_QUERY_THROW );

    Reference< XComponent > xComponent = _xComponentLoader->loadComponentFromURL(
        buf.toString(), OUString( "_blank" ), 0,
        Sequence < ::com::sun::star::beans::PropertyValue >() );
//
//  TODO: отладить загрузку текстового документа
//    com::sun::star::uno::Type t (14);
//    OUString typeName = "textDocument";
//    Reference< XTextDocument > xTextDoc = _xInterface->queryInterface( t, typeName);
//    //com::sun::star::uno::Type(14) );
//    //"com.sun.star.text.XTextDocument" );

    unoFileWidget* w = new unoFileWidget;
    w->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow * subW = _mdiArea->addSubWindow(w);
    w->show();
    operator delete( fileContent );
}

