cmake_minimum_required(VERSION 3.10)
project(UXP1A C)

set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(BISON)
find_package(FLEX)

set(CMAKE_C_STANDARD 11)
set (CMAKE_C_FLAGS "-lfl")

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})


BISON_TARGET(MyParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
FLEX_TARGET(MyScanner scanner.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

file(GLOB SOURCES "src/*.c")

add_executable(UXP1A
        ${BISON_MyParser_OUTPUTS}
        ${FLEX_MyScanner_OUTPUTS}
        ${SOURCES}
        )


