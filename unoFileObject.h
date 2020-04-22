/*
 * Класс работы с файлами libreoffice через механизм uno.
 * (C) НПО Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */

#pragma once

#include <QObject>

#include <sal/main.h>

#include <rtl/string.h>
#include <rtl/ustrbuf.hxx>

#include <cppu/unotype.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XDesktop2.hpp>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::bridge;
using namespace com::sun::star::beans;
using namespace com::sun::star::frame;

using ::rtl::OUString;

class unoFileObject : public QObject {
private:
    unoFileObject(QObject* parent=nullptr);
    virtual ~unoFileObject();

public:
    QWidget* guiView(QWidget* parent=nullptr, Qt::WindowFlags flags=Qt::WindowFlags());

    friend class UnoSingleton;

    Reference< XComponent > loadFromURL(const QUrl& fileUrl);

public slots:
    void initUnoComponents();

private:
    //
    // Functions
    //
    void init();
private:
    //
    // Variables
    //
    OUString _sConnectionString;
    Reference< XComponentContext > _xComponentContext;
    Reference< XMultiComponentFactory > _xMultiComponentFactoryClient;
    Reference< XInterface > _xInterface;
    Reference< XUnoUrlResolver > _resolver;
    Reference< XPropertySet > _xPropSet;
    Reference< XMultiComponentFactory > _xMultiComponentFactoryServer;
    Reference < XDesktop2 > _xComponentLoader;

private:
    Q_OBJECT
};
