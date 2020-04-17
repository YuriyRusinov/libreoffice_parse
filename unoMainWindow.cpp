#include <QAbstractItemModel>
#include <QDir>
#include <QMdiArea>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QUrl>
#include <QtDebug>

#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include "unoFileWidget.h"
#include "unoMainWindow.h"
#include "unoTablesModel.h"
#include "ui_uno_main_window.h"

using std::stringstream;
using std::cerr;
using std::endl;
using std::vector;
using std::string;

UnoMainWindow::UnoMainWindow(QWidget *parent)
    : QMainWindow(parent),
    _UI(new Ui::uno_main_window),
    _mdiArea(new QMdiArea),
    _isInit(false),
    _sConnectionString("uno:socket,host=localhost,port=2083;urp;StarOffice.ServiceManager")
{
    _UI->setupUi(this);
    setCentralWidget(_mdiArea);
    QObject::connect(_UI->actOpen, &QAction::triggered, this, &UnoMainWindow::slotOpen);
    QObject::connect(_UI->actQuit, &QAction::triggered, this, &QMainWindow::close);
}

UnoMainWindow::~UnoMainWindow() {
    //Reference< XComponent >::query( _xMultiComponentFactoryClient )->dispose();
    delete _mdiArea;
    delete _UI;
}

void UnoMainWindow::slotOpen() {
    qDebug () << __PRETTY_FUNCTION__;
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
    if (!_isInit)
        initUno();
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
//
//  From wiki.openoffice.org for tests
//
//    Reference < XComponentLoader > xComponentLoader(
//            xMultiComponentFactoryClient->createInstanceWithContext( OUString(
//            RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ) ),
//            xComponentContext ), UNO_QUERY_THROW );
//
    Reference< XComponent > xComponent = _xComponentLoader->loadComponentFromURL(
        buf.toString(), OUString( "_blank" ), 0,
        Sequence < ::com::sun::star::beans::PropertyValue >() );
    Reference< XTextDocument > xTextDoc (xComponent, UNO_QUERY );
    qDebug() << __PRETTY_FUNCTION__ << xTextDoc.get();
    Reference< XText > xText = xTextDoc->getText();
//
//  For debug purposes
//
//    cerr << __PRETTY_FUNCTION__ << " " << xText->getString() << endl;
    stringstream textStr;
    textStr << xText->getString();
    Reference< XMultiServiceFactory > xMultiServ (xTextDoc, UNO_QUERY );
    qDebug() << __PRETTY_FUNCTION__ << "Document multiservice factory is " << xMultiServ.get();
    Reference< XTextTablesSupplier > xTextTablesSuppl (xTextDoc, UNO_QUERY );
    qDebug() << __PRETTY_FUNCTION__ << "Tables supplier is " << xTextTablesSuppl.get();
    Reference< XNameAccess > xNamedTables = xTextTablesSuppl->getTextTables();
    qDebug() << __PRETTY_FUNCTION__ << "Named Tables list is " << xNamedTables.get();
    Sequence < ::rtl::OUString > tSeq = xNamedTables->getElementNames();
    qDebug() << __PRETTY_FUNCTION__ << "Number of tables is " << tSeq.getLength();
    QStringList tableNames;
    vector< Reference< XTextTable > > xTablesVec;
    for (::rtl::OUString* ptab = tSeq.begin();
            ptab != tSeq.end();
            ptab++) {
        stringstream tableStr;
        tableStr << *ptab;
        QString tableName = QString::fromStdString(tableStr.str());
        tableNames.append(tableName);
        Any any = xNamedTables->getByName(*ptab);
        Reference< XTextTable > xTable(any, UNO_QUERY);
        xTablesVec.push_back( xTable );
        qDebug() << __PRETTY_FUNCTION__ << tableName << xTable.get();
    }

    QAbstractItemModel* tModel = new unoTablesModel(tableNames, xTablesVec);

    //Reference< XTextTable > xTextTables ( XTextTable(
    //        xMultiServ->createInstance(
    //            "com.sun.star.text.TextTable" ));
    //        );
    //qDebug() << __PRETTY_FUNCTION__ << xTextTables.get();

    unoFileWidget* w = new unoFileWidget;
    w->setText(QString::fromStdString(textStr.str()));
    w->setTablesModel(tModel);
    w->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow * subW = _mdiArea->addSubWindow(w);
    w->show();
    operator delete( fileContent );
}

void UnoMainWindow::initUno() {
    _xComponentContext = Reference< XComponentContext >( ::cppu::bootstrap() );
    /* Gets the service manager instance to be used (or null). This method has
       been added for convenience, because the service manager is a often used
       object.
    */
    qDebug() << __PRETTY_FUNCTION__ << "XComponentContext " << _xComponentContext.get();
    _xMultiComponentFactoryClient = Reference< XMultiComponentFactory >( _xComponentContext->getServiceManager() );
    /* Creates an instance of a component which supports the services specified
       by the factory.
    */
    _xInterface = Reference< XInterface >(
                _xMultiComponentFactoryClient->createInstanceWithContext(
                    "com.sun.star.bridge.UnoUrlResolver",
                     _xComponentContext )
            );
    qDebug() << __PRETTY_FUNCTION__ << "XInterface " << _xInterface.get();
    _resolver = Reference< XUnoUrlResolver >( _xInterface, UNO_QUERY );
    qDebug() << __PRETTY_FUNCTION__ << "XUnoUrlResolver " << _resolver.get();
    // gets the server component context as property of the office component factory
    _xPropSet = Reference< XPropertySet >( _xInterface, UNO_QUERY );
    qDebug() << __PRETTY_FUNCTION__ << "XPropertySet " << _xPropSet.get() << "UNO_QUERY is " << UNO_QUERY;
    try {
        //_xPropSet->getPropertyValue("DefaultContext") >>= _xComponentContext;
    }
    catch( Exception& e) {
        qDebug () << __PRETTY_FUNCTION__ << OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US).getStr();
    }
    catch( ... ) {
        qDebug () << __PRETTY_FUNCTION__;
    }
    // gets the service manager from the office
    _xMultiComponentFactoryServer = Reference< XMultiComponentFactory >(
            _xComponentContext->getServiceManager()
            );
    // get an instance of the remote office desktop UNO service
    // and query the XComponentLoader interface
    _xComponentLoader = Reference < XDesktop2 >( Desktop::create(_xComponentContext) );
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
    _isInit = true;
}
