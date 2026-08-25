cmake_minimum_required(VERSION 3.21)
project(zte-tools VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

option(ZTE_BUILD_TESTS "Build tests" ON)
option(ZTE_BUILD_APP "Build GUI app (requires Qt6 Quick)" ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)
if(ZTE_BUILD_APP)
    find_package(Qt6 REQUIRED COMPONENTS Quick Qml)
endif()
qt_standard_project_setup(REQUIRES 6.5)
qt_policy(SET QTP0004 NEW)

add_subdirectory(core)

if(ZTE_BUILD_APP)
    add_subdirectory(app)
endif()

if(ZTE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
