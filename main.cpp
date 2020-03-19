#include <QApplication>
#include "unomainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    UnoMainWindow w;
    w.show();

    int ret = app.exec();
    return ret;
}
