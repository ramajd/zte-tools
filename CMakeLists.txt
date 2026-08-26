cmake_minimum_required(VERSION 3.21)
project(zte-tools VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

option(ZTE_BUILD_TESTS "Build tests" ON)
option(ZTE_BUILD_APP "Build GUI app (requires Qt6 Quick)" ON)

# Install prefix default
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install prefix" FORCE)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Network)
if(ZTE_BUILD_APP)
    find_package(Qt6 REQUIRED COMPONENTS Quick Qml)
endif()
qt_standard_project_setup(REQUIRES 6.5)
qt_policy(SET QTP0004 NEW)

# AGL framework was removed in macOS SDK 14+ but Qt 6 still links it —
# allow the undefined symbol since the app never calls AGL.
if(APPLE)
    add_link_options(-Wl,-U,AGL)
endif()

add_subdirectory(core)

if(ZTE_BUILD_APP)
    add_subdirectory(app)
endif()

if(ZTE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# CPack configuration for distributable packages
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ZTE Tools - Modem management utility")
set(CPACK_PACKAGE_VENDOR "ZTE Tools Contributors")
set(CPACK_GENERATOR "ZIP")
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
endif()
include(CPack)
