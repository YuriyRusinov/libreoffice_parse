#include <QApplication>
#include "unoSingleton.h"
#include "unoMainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    UnoSingleton* _unoSingleton = UnoSingleton::getUnoS(&app);
    QString program = "/usr/bin/soffice";
    QStringList arguments;
    arguments << "--nologo" << "--nodefault" << "--norestore" << "--nolockcheck";
    arguments << "--accept=socket,host=localhost,port=2083;urp;StarOffice.ServiceManager";
    _unoSingleton->initProc(program, arguments);
    _unoSingleton->startProc();
    UnoMainWindow w;
    w.show();

    int ret = app.exec();
    _unoSingleton->deleteLater();
    return ret;
}
