cmake_minimum_required(VERSION 3.7)

project(qtuno VERSION 1.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions(-D SAL_UNX)
if (NOT DEFINED _UNO_DEBUG_)
    set(_UNO_DEBUG_ 1)
endif()
if (NOT DEFINED _UNO_HEADERS_)
    set(_UNO_HEADERS_ OFF)
endif()
add_definitions(-D_UNO_DEBUG_=${_UNO_DEBUG_})

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${Qt5Widgets_INCLUDE_DIRS}
                    ${Qt5Core_INCLUDE_DIRS}
                    ${Qt5Gui_INCLUDE_DIRS})

set(HEADERS
    unoFileWidget.h
    unoSingleton.h
    unoTablesModel.h
    unoFileObject.h
    unoSearchForm.h
    unoSearchTablesModel.h
    unoSearchDelegate.h
    unoSearchResultsForm.h
    unoSearchResultsModel.h
    unoTableCellForm.h
    unoCellEditor.h
    unoMdiSubWindowCloseFilter.h
)

set(FORMS
    uno_search_form.ui
    uno_search_results_form.ui
    uno_table_cell_form.ui
    uno_cell_editor.ui
)

set(SOURCES
    unoFileWidget.cpp
    unoSingleton.cpp
    unoTablesModel.cpp
    unoFileObject.cpp
    unoSearchForm.cpp
    unoSearchTablesModel.cpp
    unoSearchDelegate.cpp
    unoSearchResultsForm.cpp
    unoSearchResultsModel.cpp
    unoTableCellForm.cpp
    unoCellEditor.cpp
    unoMdiSubWindowCloseFilter.cpp
)

set(RESOURCES
    unoresource.qrc
)

qt5_wrap_ui(UIS_HDRS ${FORMS} )

#
# Генерация необходимых для интеграции с libreoffice C++ headers: ${OOOSDK_DIR}/sdk/bin/cppumaker -O generated_files ${OOOSDK_DIR}/program/types.rdb ${OOOSDK_DIR}/program/types/oovbaapi.rdb ${OOOSDK_DIR}/program/types/offapi.rgb
#
find_path(OOOSDK_DIR
    NAMES sdk/bin/cppumaker program/types.rdb program/types/oovbaapi.rdb program/types/offapi.rgb
    PATHS /opt/libreoffice.org/basis3.2
          /usr/lib/libreoffice/
          /usr/lib/libreoffice/presets
          /usr/lib/libreoffice/program
          /usr/lib/libreoffice/share
          /usr/lib64/libreoffice/
          /usr/lib64/libreoffice/presets
          /usr/lib64/libreoffice/program
          /usr/lib64/libreoffice/share
)
if(OOOSDK_DIR)
    message(STATUS "cppumaker was found")
    if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/generated_files)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated_files)
    endif(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/generated_files)
    set(_OOOINCLUDE ${CMAKE_CURRENT_BINARY_DIR}/generated_files)

    include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated_files)
    add_custom_command(
            OUTPUT ${_OOOINCLUDE}/com/sun/star/uno/Exception.hpp
            COMMAND ${OOOSDK_DIR}/sdk/bin/cppumaker
                 ${OOOSDK_DIR}/program/types.rdb
                 ${OOOSDK_DIR}/program/types/oovbaapi.rdb
                 ${OOOSDK_DIR}/program/types/offapi.rdb
            WORKING_DIRECTORY ${_OOOINCLUDE}
    )
    add_custom_target(_oooheaders ALL
        DEPENDS ${_OOOINCLUDE}/com/sun/star/uno/Exception.hpp)

    add_custom_command(TARGET _oooheaders PRE_BUILD COMMAND ${OOOSDK_DIR}/sdk/bin/cppumaker -O ${CMAKE_CURRENT_BINARY_DIR}/generated_files ${OOOSDK_DIR}/program/types.rdb ${OOOSDK_DIR}/program/types/oovbaapi.rdb ${OOOSDK_DIR}/program/types/offapi.rdb)

    find_path(CPPUHELPER_INCLUDE_DIR
            NAMES cppuhelper/bootstrap.hxx
            PATHS ${OOOSDK_DIR}/sdk/include
                  /usr/include/libreoffice
    )
    if (NOT CPPUHELPER_INCLUDE_DIR)
            set(OOOSDK_ERROR 
                    "Could not find cppuhelper/bootstrap.hxx for OOoSDK.")
    endif (NOT CPPUHELPER_INCLUDE_DIR)

    find_library(OOOSDK_LIBRARIES1
            NAMES uno_cppuhelpergcc3
            PATHS ${OOOSDK_DIR}/sdk/lib
    )
    find_library(OOOSDK_LIBRARIES2
            NAMES uno_sal
            PATHS ${OOOSDK_DIR}/sdk/lib
    )
    find_library(OOOSDK_LIBRARIES3
            NAMES uno_salhelpergcc3
            PATHS ${OOOSDK_DIR}/sdk/lib
    )
    find_library(OOOSDK_LIBRARIES4
            NAMES uno_cppu
            PATHS ${OOOSDK_DIR}/sdk/lib
    )
    find_library(OOOSDK_LIBRARIES5
            NAMES uno_purpenvhelpergcc3
            PATHS ${OOOSDK_DIR}/sdk/lib
    )
    set(OOOSDK_LIBRARIES ${OOOSDK_LIBRARIES1} ${OOOSDK_LIBRARIES2} ${OOOSDK_LIBRARIES3} ${OOOSDK_LIBRARIES4})
    if (NOT OOOSDK_LIBRARIES)
       set(OOOSDK_ERROR "Could not find uno_cppuhelpergcc3 for OOoSDK.")
    else (NOT OOOSDK_LIBRARIES)
        message(STATUS "libraries are ${OOOSDK_LIBRARIES}")
        link_directories(/usr/lib/libreoffice /usr/lib/libreoffice/program /usr/lib64/libreoffice /usr/lib64/libreoffice/program)
    endif (NOT OOOSDK_LIBRARIES)
    set(OOOSDK_FOUND true)
    set(OOOSDK_INCLUDE_DIRS ${CPPUHELPER_INCLUDE_DIR} ${_OOOINCLUDE})
else(OOOSDK_DIR)
    set(OOOSDK_ERROR "Could not find sdk/bin/cppumaker
            or program/offapi.rdb for OOoSDK.")
endif(OOOSDK_DIR)

if(NOT OOoSDK_FOUND)
    if(OOoSDK_FIND_REQUIRED)
        message(FATAL_ERROR ${OOOSDK_ERROR})
    else(OOoSDK_FIND_REQUIRED)
        message(STATUS ${OOOSDK_ERROR})
    endif(OOoSDK_FIND_REQUIRED)
else(NOT OOoSDK_FOUND)
    message(STATUS "Found libreoffice SDK.")
endif(NOT OOoSDK_FOUND)

include_directories(${OOOSDK_INCLUDE_DIRS})
message(STATUS "Generated headers are ${OOOSDK_INCLUDE_DIRS}")

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} ${UIS_HDRS} ${RESOURCES} ${OOOSDK_LIBRARIES1} ${OOOSDK_LIBRARIES2} ${OOOSDK_LIBRARIES3} ${OOOSDK_LIBRARIES4} ${OOOSDK_LIBRARIES5})
add_dependencies(${PROJECT_NAME} _oooheaders)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets ${OOOSDK_LIBRARIES1} ${OOOSDK_LIBRARIES2} ${OOOSDK_LIBRARIES3} ${OOOSDK_LIBRARIES4} ${OOOSDK_LIBRARIES5})

set(${PROJECT_NAME}_RUNTIME_LIBRARY_DIRS ${CMAKE_INSTALL_LIBDIR} ${CMAKE_INSTALL_LIBDIR}:/libreoffice/program)

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION lib
)
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
if(_UNO_HEADERS_)
    install(DIRECTORY ${OOOSDK_INCLUDE_DIRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} )
else()
    install(DIRECTORY ${CPPUHELPER_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
endif()

set(CPACK_PACKAGE_EXECUTABLES "qtuno" "qtuno")
include(CPack)
