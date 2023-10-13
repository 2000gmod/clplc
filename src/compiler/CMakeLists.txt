set(sources
    compiler.cpp
)
add_library(clplcompiler ${sources})
target_include_directories(clplcompiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})