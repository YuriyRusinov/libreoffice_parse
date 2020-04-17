#include <QtGlobal>
#include <QtDebug>

#include "unoSingleton.h"

UnoSingleton* UnoSingleton::_instance = nullptr;

UnoSingleton::UnoSingleton(QObject* parent)
    : QObject(parent),
    _sofficeProc(new QProcess(this))
{
    if (_instance) {
        qFatal("There should be only one UnoSingleton object");
    }
    _instance = this;

    QObject::connect(_sofficeProc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            QOverload<int, QProcess::ExitStatus>::of(&UnoSingleton::slotProcEnd)
            );
}

UnoSingleton::~UnoSingleton() {
    qint64 sofficePid = _sofficeProc->processId();
    qDebug() << __PRETTY_FUNCTION__ << sofficePid;
    if (sofficePid > 0) {
        _sofficeProc->waitForFinished(5000);
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
        qDebug () << __PRETTY_FUNCTION__ << "Cannot run soffice error code is " << _sofficeProc->error();
        return;
    }
    qDebug () << __PRETTY_FUNCTION__ << _sofficeProc->processId();
}

void UnoSingleton::slotProcEnd(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug () << __PRETTY_FUNCTION__ << exitCode << exitStatus;
//    if (exitCode == 0)
//        startProc();
}

QProcess* UnoSingleton::getProc() const {
    return _sofficeProc;
}
