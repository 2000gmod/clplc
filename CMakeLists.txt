cmake_minimum_required(VERSION 3.5)
project(CLPL_Compiler)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_subdirectory(src/parser)
add_subdirectory(src/compiler)

add_executable(clplc src/main.cpp)
target_link_libraries(clplc PRIVATE clplparser)
target_link_libraries(clplc PRIVATE clplcompiler)
target_link_libraries(clplc PUBLIC -L/usr/lib/llvm-14/lib)
target_link_libraries(clplc PUBLIC -lLLVM-14)

install(TARGETS clplc DESTINATION bin)