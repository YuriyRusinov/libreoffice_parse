/* @brief
 * Класс работы с файлами libreoffice через механизм uno.
 * (C) НИИ Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */

#pragma once

#include <QModelIndex>
#include <QObject>
#include <QUrl>

#include <vector>

#include <sal/main.h>

#include <rtl/string.h>
#include <rtl/ustrbuf.hxx>

#include <cppu/unotype.hxx>
#include <cppuhelper/cppuhelperdllapi.h>

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XDesktop2.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/text/XTextTable.hpp>
#include <com/sun/star/util/XCloseable.hpp>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::bridge;
using namespace com::sun::star::beans;
using namespace com::sun::star::frame;
using namespace com::sun::star::text;
using namespace com::sun::star::table;
using namespace com::sun::star::util;

using ::rtl::OUString;
using std::vector;

class QAbstractItemModel;

class CPPUHELPER_DLLPUBLIC unoFileObject : public QObject {
private:
    unoFileObject(QObject* parent=nullptr);
    virtual ~unoFileObject();

public:
    QWidget* guiView(const QUrl& fileUrl, QWidget* parent=nullptr, Qt::WindowFlags flags=Qt::WindowFlags());
    QAbstractItemModel* getTablesModel(QObject *parent = nullptr);

    friend class UnoSingleton;

    Reference< XComponent > loadFromURL(const QUrl& fileUrl);
    Reference< XInterface > getInterface() const;
    Reference< XDesktop2 > getComponentLoader() const;
    Reference< XInterface > getSimpleFileAccess() const;

public slots:
    void initUnoComponents();

signals:
    void updateTables(QModelIndex wTableIndex, Reference< XTextTable > wTable);
    void viewWidget(QWidget* w);

private slots:
    void searchUnoTables(QString searchStr, vector< Reference< XTextTable > > searchTables, QStringList searchTableNames);
    void slotTableAction(QModelIndex tableIndex, Reference< XTextTable > wTable, int tableActCode, int tableCoordPar, int iPar);
    void saveWorkFile(QUrl saveFileUrl);
    void slotTableEditCell(Reference< XTextTable > wTable, int iRow, int jColumn);
    void slotUpdateCell(Reference< XCell > wCell, QString newText);
    void slotCloseFile(QUrl fileUrl);

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
    Reference< XDesktop2 > _xComponentLoader;
    Reference< XComponent > _xComponent;
    Reference< XInterface > _xSimpleFileAccessInterface;
    Reference< XMultiServiceFactory > _xOfficeServiceManager;
    Reference< XStorable > _xStorable;
    Reference< XCloseable> _xCloseable;

private:
    Q_OBJECT
};
