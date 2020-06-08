TARGET = qtuno
TEMPLATE = lib
DESTDIR = ../build
include(../qtuno.conf)

CONFIG += qt

QT += core widgets

QMAKE_CFLAGS_ISYSTEM = -I

DEFINES += SAL_UNX

INCLUDEPATH += $${OOO_SDK_HOME}/include
INCLUDEPATH += $${OOO_SDK_INCLUDE}

LIBS += -L$${OOO_SDK_LIB} -luno_cppu
LIBS += -L$${OOO_SDK_LIB} -luno_cppuhelpergcc3
LIBS += -L$${OOO_SDK_LIB} -luno_purpenvhelpergcc3
LIBS += -L$${OOO_SDK_LIB} -luno_sal
LIBS += -L$${OOO_SDK_LIB} -luno_salhelpergcc3
message("libs are $${LIBS}")

generated_files.target = $${DESTDIR}/generated_files
generated_files.command = $${OOO_SDK_PROGRAM} $${OOO_SDK_DIR}/types.rdb                $${OOO_SDK_DIR}/types/oovbaapi.rdb $${OOO_SDK_DIR}/types/offapi.rdb
generated_files.depends = genfiles
genfiles.command = @echo Building $$generated_files
message("generated_files command is $${generated_files.command}")

QMAKE_EXTRA_TARGETS += generated_files genfiles

HEADERS += unoFileWidget.h \
    unoSingleton.h \
    unoTablesModel.h \
    unoFileObject.h \
    unoSearchForm.h \
    unoSearchTablesModel.h \
    unoSearchDelegate.h \
    unoSearchResultsForm.h \
    unoSearchResultsModel.h \
    unoTableCellForm.h \
    unoCellEditor.h \
    unoMdiSubWindowCloseFilter.h

SOURCES += unoFileWidget.cpp \
    unoSingleton.cpp \
    unoTablesModel.cpp \
    unoFileObject.cpp \
    unoSearchForm.cpp \
    unoSearchTablesModel.cpp \
    unoSearchDelegate.cpp \
    unoSearchResultsForm.cpp \
    unoSearchResultsModel.cpp \
    unoTableCellForm.cpp \
    unoCellEditor.cpp \
    unoMdiSubWindowCloseFilter.cpp

FORMS += uno_search_form.ui \
    uno_search_results_form.ui \
    uno_table_cell_form.ui \
    uno_cell_editor.ui

RESOURCES += unoresource.qrc
