/*
 * Класс работы с файлами libreoffice через механизм uno.
 * (C) НПО Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/frame/Desktop.hpp>

#include <QUrl>
#include <QtDebug>
#include "unoFileObject.h"
#include "unoFileWidget.h"

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::rtl::OUStringToOString;

unoFileObject::unoFileObject(QObject* parent)
    : QObject(parent),
    _sConnectionString("uno:socket,host=localhost,port=2083;urp;StarOffice.ServiceManager") {
//    init();
}

unoFileObject::~unoFileObject() {
}

QWidget* unoFileObject::guiView(QWidget* parent, Qt::WindowFlags flags) {
    unoFileWidget* w = new unoFileWidget(parent, flags);

    return w;
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
    return xComponent;
}

void unoFileObject::initUnoComponents() {
    qDebug() << __PRETTY_FUNCTION__;
    init();
}
