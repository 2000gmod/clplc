cmake_minimum_required(VERSION 3.5)
project(CLPL_Compiler)

set(sources
    src/main.cpp
    src/parser.cpp
    src/scanner.cpp
    src/token.cpp
    src/type.cpp
)

add_executable(clplc ${sources})