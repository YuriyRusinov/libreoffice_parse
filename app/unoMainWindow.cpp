/* @brief
 * Главное окно тестового приложения
 * (C) НИИ Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */

#include <QDir>
#include <QMdiArea>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QUrl>
#include <QtDebug>

#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include <osl/file.hxx>
#include <rtl/string.h>
#include <rtl/ustrbuf.hxx>
#include <rtl/byteseq.h>

#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/text/XTextTablesSupplier.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#include <com/sun/star/io/XOutputStream.hpp>

#include "unoSingleton.h"
#include "unoFileObject.h"
#include "unoFileWidget.h"
#include "unoMainWindow.h"
#include "unoTablesModel.h"
#include "unoMdiSubWindowCloseFilter.h"
#include "ui_uno_main_window.h"

using std::stringstream;
using std::cerr;
using std::endl;
using std::vector;
using std::string;

using ::rtl::OUStringBuffer;
using ::rtl::OUStringToOString;

using namespace com::sun::star::text;
using namespace com::sun::star::lang;
using namespace com::sun::star::container;
using namespace com::sun::star::io;
using namespace com::sun::star::ucb;

UnoMainWindow::UnoMainWindow(QWidget *parent)
    : QMainWindow(parent),
    _UI(new Ui::uno_main_window),
    _mdiArea(new QMdiArea),
    _isInit(false) {
    _UI->setupUi(this);
    setCentralWidget(_mdiArea);
    UnoSingleton* _unoSingleton = UnoSingleton::getUnoS();
    _unoFObj = _unoSingleton->getUnoFileObj();
    QObject::connect(_UI->actOpen, &QAction::triggered, this, &UnoMainWindow::slotOpen);
    QObject::connect(_UI->actQuit, &QAction::triggered, this, &QMainWindow::close);
    QObject::connect(this, &UnoMainWindow::initUnoComponents, _unoFObj, &unoFileObject::initUnoComponents);
    QObject::connect(_unoFObj, &unoFileObject::viewWidget, this, &UnoMainWindow::slotAddWindow);
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
    if (!_isInit) {
        _isInit = true;
        emit initUnoComponents();
    }
    OUStringBuffer buf;
    buf.append( fileUrl.toString().utf16() );
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

#if _UNO_DEBUG_==1
/*
 *  For debug
 */
    QByteArray ba = QByteArray::fromRawData( static_cast<const char*>(fileContent), readBytes );
    QFile fileTest("ttt.odt");
    if (!fileTest.open(QIODevice::WriteOnly))
        return;
    QDataStream tstStr( &fileTest );
    tstStr.writeRawData( ba.constData(), ba.size());
#endif
    Reference< XComponent > xComponent = _unoFObj->loadFromURL(fileUrl);
    qDebug() << __PRETTY_FUNCTION__ << tr("xcomponent is %1 loaded").arg( xComponent.is() ? QString() : tr("not"));
    if(!xComponent.is()) {
        QMessageBox::warning(this, tr("Load file"), tr("Cannot load file %1").arg(fileName), QMessageBox::Ok);
        return;
    }
    QAbstractItemModel* tModel = _unoFObj->getTablesModel();
    //new unoTablesModel(tableNames, xTablesVec);

    //Reference< XTextTable > xTextTables ( XTextTable(
    //        xMultiServ->createInstance(
    //            "com.sun.star.text.TextTable" ));
    //        );
    //qDebug() << __PRETTY_FUNCTION__ << xTextTables.get();

    unoFileWidget* w = qobject_cast<unoFileWidget*>(_unoFObj->guiView(fileUrl));
    //w->setText(QString::fromStdString(textStr.str()));
    w->setTablesModel(tModel);
    slotAddWindow(w);
    operator delete( fileContent );
}

void UnoMainWindow::slotAddWindow(QWidget* w) {
    if (!w)
        return;

    w->setAttribute(Qt::WA_DeleteOnClose);
    QMdiSubWindow * subW = _mdiArea->addSubWindow(w);
    unoMdiSubWindowCloseFilter* subwFilter = new unoMdiSubWindowCloseFilter;
    w->installEventFilter( subwFilter );
    subW->setAttribute(Qt::WA_DeleteOnClose, true);

    w->show();
}
