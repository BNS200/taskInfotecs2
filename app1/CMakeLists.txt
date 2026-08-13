add_executable(app1
    main.cpp
    socket_client.cpp
    input_handler.cpp
    data_processor.cpp
)

target_include_directories(app1
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(app1
    common          
    dl              
    pthread         
)

target_compile_options(app1 PRIVATE
    -Wall
    -Wextra
    -pedantic
)

target_link_options(app1 PRIVATE
    -pthread
)

add_custom_command(TARGET app1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/app1
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/lib.so        
        ${CMAKE_BINARY_DIR}/app1/
)

add_custom_command(TARGET app1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/spsc_buffer.hpp
        ${CMAKE_BINARY_DIR}/app1/
)