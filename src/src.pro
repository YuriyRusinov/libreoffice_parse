TARGET = qtuno
TEMPLATE = lib
DESTDIR = ../build
include(../qtuno.conf)

CONFIG += ordered
CONFIG += qt

QT += core widgets

DEFINES += SAL_UNX
unix:  INCLUDEPATH += /usr/include

INCLUDEPATH += $${OOO_SDK_HOME}/include
INCLUDEPATH += $${OOO_SDK_INCLUDE}

include (code_generator.pri)

LIBS += -L$${OOO_SDK_LIB} -luno_cppu
LIBS += -L$${OOO_SDK_LIB} -luno_cppuhelpergcc3
LIBS += -L$${OOO_SDK_LIB} -luno_purpenvhelpergcc3
LIBS += -L$${OOO_SDK_LIB} -luno_sal
LIBS += -L$${OOO_SDK_LIB} -luno_salhelpergcc3
QMAKE_LFLAGS_RPATH += $${OOO_SDK_DIR}
message("libs are $${LIBS}")

INCLUDEPATH += $$DESTDIR/generated_files
DEPENDPATH += $$DESTDIR/generated_files
message("include path is $${INCLUDEPATH}")
QMAKE_CFLAGS_ISYSTEM = -I
#DEFINES += USING_PCH

PRECOMPILED_HEADER = $$DESTDIR/generated_files/com/sun/star/uno/Exception.hpp
#HEADERS += $$DESTDIR/generated_files/com/sun/star/uno/Exception.hpp
SOURCES = $$DESTDIR/generated_files/exception.cpp 

include(src.pri)
