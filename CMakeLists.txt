cmake_minimum_required(VERSION 3.20)

project(Solution)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup()

include(CMake/AalTest.cmake)

add_subdirectory(KDL)

add_subdirectory(Tests/AllInOne)
add_subdirectory(Tests/LexerTests)
add_subdirectory(Tests/ParserTests)
