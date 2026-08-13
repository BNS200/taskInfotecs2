add_library(lib SHARED
    lib.cpp
)

target_compile_definitions(lib PRIVATE LIB_EXPORTS)

target_include_directories(lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(lib PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    VERSION 1.0.0
    SOVERSION 1
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}   
)

target_compile_options(lib PRIVATE
    -Wall
    -Wextra
    -pedantic
    -fPIC
)