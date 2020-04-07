/*
 * Заголовочный файл главного окна тестового приложения
 */

#pragma once

#include <QMainWindow>
#include <QProcess>

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
#include <com/sun/star/text/XTextDocument.hpp>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::bridge;
using namespace com::sun::star::frame;
using namespace com::sun::star::registry;
using namespace com::sun::star::table;
using namespace com::sun::star::text;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::rtl::OUStringToOString;

class QMdiArea;

class UnoSingleton;

namespace Ui{
    class uno_main_window;
};

class UnoMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit UnoMainWindow(QWidget *parent = nullptr);
    virtual ~UnoMainWindow();

signals:

public slots:
private slots:
    void slotOpen();

private:
    Ui::uno_main_window* UI;
    QMdiArea* _mdiArea;
//    QProcess* _sofficeProc;

    OUString _sConnectionString;
    Reference< XComponentContext > _xComponentContext;
    Reference< XMultiComponentFactory > _xMultiComponentFactoryClient;
    Reference< XInterface > _xInterface;
    Reference< XUnoUrlResolver > _resolver;
    Reference< XPropertySet > _xPropSet;
    Reference< XMultiComponentFactory > _xMultiComponentFactoryServer;
    Reference < XDesktop2 > _xComponentLoader;
};
