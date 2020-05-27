/* @brief
 * Синглтон библиотеки, запускающий необходимое фоновое приложение 
 * e.g. soffice
 *
 * (C) НИИ Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */
#include <QtGlobal>
#include <QCoreApplication>
#include <QtDebug>

#include "unoFileObject.h"
#include "unoSingleton.h"

UnoSingleton* UnoSingleton::_instance = nullptr;

UnoSingleton::UnoSingleton(QObject* parent)
    : QObject(parent),
    _sofficeProc(new QProcess(this)),
    _unoFileObj(nullptr),
    _procId(-1)
{
    if (_instance) {
        qFatal("There should be only one UnoSingleton object");
    }
    _instance = this;

    QObject::connect(_sofficeProc, 
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            QOverload<int, QProcess::ExitStatus>::of(&UnoSingleton::slotProcEnd)
            );
}

UnoSingleton::~UnoSingleton() {
    qDebug() << __PRETTY_FUNCTION__ << _procId;
    if (_procId > 0) {
        _sofficeProc->waitForFinished(5000);
        slotChildProcEnd( _procId );
        _sofficeProc->kill();
    }
    _sofficeProc->setParent(nullptr);
    delete _sofficeProc;
    _instance = nullptr;
}

UnoSingleton* UnoSingleton::getUnoS (QObject* parent) {
    if (!_instance) {
        _instance = new UnoSingleton(parent);
    }

    return _instance;
}

void UnoSingleton::initProc(const QString& program, const QStringList& arguments) {
    _sofficeProc->setProgram(program);
    _sofficeProc->setArguments(arguments);
}

void UnoSingleton::startProc() {
    _sofficeProc->start();//Detached();
    if (_sofficeProc->state() == QProcess::NotRunning) {
        qDebug () << __PRETTY_FUNCTION__ << tr("Cannot run soffice in listening mode error code is ").arg( _sofficeProc->error() );
        return;
    }
    _procId = _sofficeProc->processId();
    qDebug () << __PRETTY_FUNCTION__ << _procId;
}

void UnoSingleton::slotProcEnd(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug () << __PRETTY_FUNCTION__ << exitCode << exitStatus;
}

QProcess* UnoSingleton::getProc() const {
    return _sofficeProc;
}

unoFileObject* UnoSingleton::getUnoFileObj(QObject* parent) const {
    if (!_unoFileObj)
        _unoFileObj = new unoFileObject(parent);
    else
        _unoFileObj->setParent(parent);

    return _unoFileObj;
}

void UnoSingleton::slotChildProcEnd(qint64 procId) {
    QProcess get_child_a;
    QStringList get_child_a_cmd;
    get_child_a_cmd << "--ppid" << QString::number(_procId) << "-o" << "pid" << "--no-heading";
    get_child_a.start("ps", get_child_a_cmd);
    get_child_a.waitForFinished(5000);
    QString child_a_str = get_child_a.readAllStandardOutput();
    int child_a = child_a_str.toInt();

    QProcess::execute("kill " + QString::number(child_a));
    qDebug () << __PRETTY_FUNCTION__ << tr("Child processes were finished");
}
