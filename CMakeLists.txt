cmake_minimum_required(VERSION 3.5)
project(CLPL_Compiler)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/parser)

add_executable(clplc src/main.cpp)
target_link_libraries(clplc PRIVATE clplparser)