set(sources
    parser.cpp
    parserutils.cpp
    scanner.cpp
    token.cpp
    type.cpp
)
add_library(clplparser ${sources})
target_include_directories(clplparser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})