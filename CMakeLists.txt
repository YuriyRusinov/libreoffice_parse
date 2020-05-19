cmake_minimum_required(VERSION 3.7)

project(qtuno VERSION 1.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:/usr/lib/libreoffice/program")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(src)
add_subdirectory(app)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Yury Rusinov") # required
if(_UNO_HEADERS_)
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}_ooo")
else()
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
endif()
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "My uno and QT project")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
set (CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a (>=5.11), libqt5gui5 (>=5.11), libqt5widgets5 (>=5.11), libreoffice-writer (>=6.4)")
include(CPack)
