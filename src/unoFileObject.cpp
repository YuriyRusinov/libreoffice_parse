/* @brief
 * Класс работы с файлами libreoffice через механизм uno.
 * (C) НИИ Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <cppuhelper/bootstrap.hxx>

#include <osl/file.hxx>

#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess2.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/io/XTextOutputStream.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/awt/FontWeight.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/beans/Property.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/table/XTable.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>

#include <QFile>
#include <QUrl>
#include <QtDebug>
#include "unoFileObject.h"
#include "unoFileWidget.h"
#include "unoSearchResultsForm.h"
#include "unoSearchResultsModel.h"

using std::cerr;
using std::endl;
using std::ofstream;
using std::stringstream;
using std::string;
using std::hex;
using std::pair;

using namespace com::sun::star::io;
using namespace com::sun::star::ucb;
using namespace com::sun::star::awt;
using namespace com::sun::star::beans;
using namespace com::sun::star::lang;

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

    QObject::connect(unoFileW, &unoFileWidget::tableActSignal, this, &unoFileObject::slotTableAction);

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

void unoFileObject::searchUnoTables(QString searchStr, vector< Reference< XTextTable > > searchTables, QStringList searchTableNames) {
    qDebug() << __PRETTY_FUNCTION__ << searchStr;
    int nTables = searchTables.size();
    unoSearchResultsForm* uResForm = new unoSearchResultsForm(searchStr, searchTables);
    Reference< XInterface  > wCellRef = _xOfficeServiceManager->createInstance( OUString::createFromAscii( "com.sun.star.text.Cell" ));
    QStringList searchResTables;
    vector< Reference< XTextTable > > searchResXTables;
    vector< pair<long, long> > tIndices;
    for (int i=0; i<nTables; i++) {
        Reference< XTextTable > sTable = searchTables[i];
        Reference< XCellRange > tableCells( sTable, UNO_QUERY );
        qDebug() << __PRETTY_FUNCTION__ << "Cell range was " << (tableCells.is() ? "initialized" : "not initialized");
        Reference< XTableRows > tabRows( sTable->getRows() );
        Reference< XTableColumns > tabCols( sTable->getColumns() );
        long nRow = tabRows->getCount();
        long nCols = tabCols->getCount();
        qDebug() << __PRETTY_FUNCTION__ << "Table row number = " << nRow << ", column number = " << nCols;
        for (long ir=0; ir<nRow; ir++) {
            for (long jc=0; jc<nCols; jc++) {
                Reference< XCell > wCell = nullptr;
                try {
                    //
                    // Первая координата -- номер столбца !
                    //
                    wCell = tableCells->getCellByPosition(jc, ir);
                }
                catch( IndexOutOfBoundsException& e ) {
                    stringstream err_mess;
                    err_mess << e.Message;
                    qDebug() << __PRETTY_FUNCTION__ << "Index out of bounds " << err_mess.str().c_str() << ' ' << ir << ' ' << jc;
                    continue;
                }
                Reference< XText > wText = Reference< XText > (wCell, UNO_QUERY);
                //qDebug() << __PRETTY_FUNCTION__ << wCell->getType() << wText.is();
                Reference< XTextCursor > xTextCursor = wText->createTextCursor();
                stringstream wCellStr;
                if (wText.is()) {
                    wCellStr << wText->getString();
                }
                QString cellStr = QString::fromStdString(wCellStr.str());
                if (cellStr.contains(searchStr, Qt::CaseInsensitive)) {
                    qDebug() << __PRETTY_FUNCTION__ << searchStr << " was found at (" << ir << ',' << jc << ")";
                    pair<long, long> matrInd (ir, jc);
                    tIndices.push_back( matrInd );
                    if (!searchResTables.contains(searchTableNames[i])) {
                        searchResTables << searchTableNames[i];
                        searchResXTables.push_back( sTable );
                    }
                }
            }
        }
    }
    unoSearchResultsModel* resModel = new unoSearchResultsModel(searchResTables, searchResXTables, tIndices);
    uResForm->setResultsModel( resModel );
    emit viewWidget( uResForm );
}

void unoFileObject::slotTableAction(QModelIndex tableIndex, Reference< XTextTable > wTable, int tableActCode, int tableCoordPar, int iPar) {
    if (wTable.get() == nullptr)
        return;

    Reference< XTableRows > tabRows( nullptr );
    Reference< XTableColumns > tabCols( nullptr );
    switch( tableCoordPar ) {
        case unoFileWidget::tableRow: {
                                          tabRows = wTable->getRows();
                                          break;
                                      }
        case unoFileWidget::tableColumn: {
                                             tabCols = wTable->getColumns();
                                             break;
                                         }
        default: return; break;
    }
    qDebug() << __PRETTY_FUNCTION__ << tabRows.is() << tabCols.is();
    switch( tableActCode ) {
        case unoFileWidget::tableAdd: {
                                          if (tabRows.is())
                                              tabRows->insertByIndex(iPar, 1);
                                          else if (tabCols.is())
                                              tabCols->insertByIndex(iPar, 1);
                                          else
                                              return;
                                          break;
                                      }
        case unoFileWidget::tableDel: {
                                          if (tabRows.is())
                                              tabRows->removeByIndex(iPar-1, 1);
                                          else if (tabCols.is())
                                              tabCols->removeByIndex(iPar-1, 1);
                                          else
                                              return;
                                          break;
                                      }
        default: return; break;
    }
    emit updateTables(tableIndex, wTable);
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

