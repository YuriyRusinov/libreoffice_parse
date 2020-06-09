TEMPLATE = app
TARGET = app_uno
include(../qtuno.conf)
CONFIG += qt

QT += core widgets
DESTDIR = ../build

DEFINES += SAL_UNX
INCLUDEPATH += $${OOO_SDK_HOME}/include
INCLUDEPATH += $${OOO_SDK_INCLUDE}

INCLUDEPATH += $$DESTDIR/generated_files
DEPENDPATH += $$DESTDIR/generated_files

INCLUDEPATH += ../src
DEPENDPATH += ../src

QMAKE_CFLAGS_ISYSTEM = -I

LIBS += -L$${OOO_SDK_LIB} -luno_cppu
LIBS += -L$${OOO_SDK_LIB} -luno_cppuhelpergcc3
LIBS += -L$${OOO_SDK_LIB} -luno_purpenvhelpergcc3
LIBS += -L$${OOO_SDK_LIB} -luno_sal
LIBS += -L$${OOO_SDK_LIB} -luno_salhelpergcc3
LIBS += -L$$DESTDIR -lqtuno

QMAKE_RPATHDIR += $${OOO_SDK_DIR} $${OOO_SDK_LIB} $${DESTDIR}

HEADERS += unoMainWindow.h

SOURCES += unoMainWindow.cpp \
           main.cpp

FORMS += uno_main_window.ui
