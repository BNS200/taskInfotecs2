add_executable(app2
    main.cpp
    socket_server.cpp
    client_handler.cpp
    message_parser.cpp
)

target_include_directories(app2
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(app2
    common          
    dl              
    pthread         
)

target_compile_options(app2 PRIVATE
    -Wall
    -Wextra
    -pedantic
)

target_link_options(app2 PRIVATE
    -pthread
)

add_custom_command(TARGET app2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/app2
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/lib.so        
        ${CMAKE_BINARY_DIR}/app2/
)