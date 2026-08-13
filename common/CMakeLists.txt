add_library(common STATIC
    library_loader.cpp
)

target_include_directories(common
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(common PRIVATE
    -Wall
    -Wextra
    -pedantic
)

target_link_libraries(common
    dl
)
