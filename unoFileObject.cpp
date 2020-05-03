/*
 * Класс работы с файлами libreoffice через механизм uno.
 * (C) НПО Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */
#include <iostream>
#include <sstream>
#include <cppuhelper/bootstrap.hxx>

#include <osl/file.hxx>

#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess2.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/io/XTextOutputStream.hpp>

#include <QFile>
#include <QUrl>
#include <QtDebug>
#include "unoFileObject.h"
#include "unoFileWidget.h"

using std::cerr;
using std::endl;
using std::stringstream;
using namespace com::sun::star::io;
using namespace com::sun::star::ucb;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::rtl::OUStringToOString;

unoFileObject::unoFileObject(QObject* parent)
    : QObject(parent),
    _sConnectionString("uno:socket,host=localhost,port=2083;urp;StarOffice.ServiceManager") {
}

unoFileObject::~unoFileObject() {
}

QWidget* unoFileObject::guiView(const QUrl& fileUrl, QWidget* parent, Qt::WindowFlags flags) {
    unoFileWidget* unoFileW = new unoFileWidget(fileUrl, parent, flags);
    QObject::connect(unoFileW, &unoFileWidget::search, this, &unoFileObject::searchUnoTables);
    QObject::connect(unoFileW, &unoFileWidget::addRowToTable, this, &unoFileObject::addTableRow);
    QObject::connect(unoFileW, &unoFileWidget::delRowFromTable, this, &unoFileObject::delTableRow);
    QObject::connect(unoFileW, &unoFileWidget::saveWriterFile, this, &unoFileObject::saveWorkFile);
    QObject::connect(this, &unoFileObject::updateTables, unoFileW, &unoFileWidget::updateTableModel);

    return unoFileW;
}

void unoFileObject::init() {
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
    // gets the service manager from the office
    _xOfficeServiceManager = Reference< XMultiServiceFactory >(_xInterface, UNO_QUERY );
    _xMultiComponentFactoryServer = Reference< XMultiComponentFactory >(
            _xComponentContext->getServiceManager()
            );
    Reference< XInterface  > xDesktop =_xOfficeServiceManager->createInstance(
        OUString::createFromAscii( "com.sun.star.frame.Desktop" ));
    // get an instance of the remote office desktop UNO service
    // and query the XComponentLoader interface
    _xComponentLoader = Reference < XDesktop2 >( xDesktop, UNO_QUERY);//Desktop::create(_xComponentContext) );
}

Reference< XComponent > unoFileObject::loadFromURL(const QUrl& fileUrl) {
    int nlen = fileUrl.toString().length();
    qDebug() << __PRETTY_FUNCTION__ << fileUrl.toString().toStdString().c_str() << nlen;
    OUStringBuffer buf;
    for (int i=0; i<nlen; i++)
        buf.append( fileUrl.toString().toStdString().at(i) );

    Reference< XComponent > xComponent = _xComponentLoader->loadComponentFromURL(
        buf.toString(), OUString( "_blank" ), 0,
        Sequence < ::com::sun::star::beans::PropertyValue >() );
    _xSimpleFileAccessInterface = Reference< XInterface >(
                _xMultiComponentFactoryClient->createInstanceWithContext(
                    "com.sun.star.ucb.SimpleFileAccess",
                     _xComponentContext )
            );
    qDebug() << __PRETTY_FUNCTION__ << _xSimpleFileAccessInterface.is() << xComponent.is();
    Reference< XSimpleFileAccess > xSF ( _xSimpleFileAccessInterface, UNO_QUERY );
    qDebug() << __PRETTY_FUNCTION__ << xSF.is();
    return xComponent;
}

void unoFileObject::initUnoComponents() {
    qDebug() << __PRETTY_FUNCTION__;
    init();
}

void unoFileObject::searchUnoTables(QString searchStr) {
}

void unoFileObject::addTableRow(Reference< XTextTable > wTable, int iRow) {
    if (wTable.get() == nullptr)
        return;
    Reference< XTableRows > tabRows = wTable->getRows();
    tabRows->insertByIndex(iRow, 1);
    emit updateTables(wTable);
}

void unoFileObject::delTableRow(Reference< XTextTable > wTable, int iRow) {
    if (wTable.get() == nullptr)
        return;
    Reference< XTableRows > tabRows = wTable->getRows();
    tabRows->removeByIndex(iRow, 1);
    emit updateTables(wTable);
}

Reference< XInterface > unoFileObject::getInterface() const {
    return _xInterface;
}

Reference< XDesktop2 > unoFileObject::getComponentLoader() const {
    return _xComponentLoader;
}

Reference< XInterface > unoFileObject::getSimpleFileAccess() const {
    return _xSimpleFileAccessInterface;
}

void unoFileObject::saveWorkFile(QUrl saveFileUrl) {
    OUString sDocUrl;
    osl::FileBase::getFileURLFromSystemPath(
                 OUString::createFromAscii(saveFileUrl.path().toUtf8().constData()),sDocUrl);
    Reference< XSimpleFileAccess2 > xSimpleFileAcc (_xSimpleFileAccessInterface, UNO_QUERY );
    bool isFileExist = QFile::exists(saveFileUrl.path());
    qDebug () << __PRETTY_FUNCTION__ << saveFileUrl << isFileExist;
    cerr << __PRETTY_FUNCTION__ << sDocUrl << endl;
/*    Reference< XComponent > xWriterComponent = _xComponentLoader->loadComponentFromURL(
	    (isFileExist ? sDocUrl : "private:factory/swriter"),
        OUString::createFromAscii("_blank"),
        0,
        Sequence < ::com::sun::star::beans::PropertyValue >());
*/
    stringstream textStr;
    textStr << "Hello world" << endl;
    Sequence< sal_Int8 > bseq (textStr.str().size());
    for (int i=0; i<textStr.str().size(); i++)
        bseq[i] = textStr.str().at(i);
    Reference< XOutputStream > xOut = xSimpleFileAcc->openFileWrite( sDocUrl );
/*    Reference< XTextDocument > xTextDocument (xWriterComponent, UNO_QUERY);
    Reference< XText > xText = xTextDocument->getText();
    Reference< XTextRange > xTextRange = xText->getStart();
    xTextRange->setString(OUString::createFromAscii(textStr.str().c_str()));
    //xOut->writeBytes(OUString::createFromAscii(textStr.str().c_str()));
*/
    xOut->writeBytes(bseq);
}

