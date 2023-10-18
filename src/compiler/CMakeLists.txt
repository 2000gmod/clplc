set(sources
    compiler.cpp
)

add_library(clplcompiler ${sources})
target_include_directories(clplcompiler PUBLIC /usr/lib/llvm-15/include)
target_include_directories(clplcompiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
