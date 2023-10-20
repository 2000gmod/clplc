cmake_minimum_required(VERSION 3.20)
project(CLPL_Compiler)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter -Werror)

add_subdirectory(src/parser)
add_subdirectory(src/compiler)

add_executable(clplc src/main.cpp)
target_link_libraries(clplc PRIVATE clplparser)
target_link_libraries(clplc PRIVATE clplcompiler)
target_link_libraries(clplc PUBLIC -L/usr/lib/llvm-15/lib)
target_link_libraries(clplc PUBLIC -lLLVM-15)

install(TARGETS clplc DESTINATION bin)