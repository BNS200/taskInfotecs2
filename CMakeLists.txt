cmake_minimum_required(VERSION 3.10)
project(taskInfotecs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING 
        "Build type: Debug, Release, RelWithDebInfo, MinSizeRel" FORCE)
endif()

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -D_GLIBCXX_DEBUG -fno-omit-frame-pointer")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")


if(ENABLE_ASAN AND CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if(ENABLE_UBSAN AND CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()

if(ENABLE_TSAN AND CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(common)
add_subdirectory(lib)
add_subdirectory(app1)
add_subdirectory(app2)

add_custom_target(debug-info
    COMMAND ${CMAKE_COMMAND} -E echo "Build type: ${CMAKE_BUILD_TYPE}"
    COMMAND ${CMAKE_COMMAND} -E echo "C++ flags: ${CMAKE_CXX_FLAGS}"
    COMMAND ${CMAKE_COMMAND} -E echo "ASAN: ${ENABLE_ASAN}"
    COMMAND ${CMAKE_COMMAND} -E echo "UBSAN: ${ENABLE_UBSAN}"
)

add_custom_target(run-prog1-gdb
    COMMAND ${CMAKE_COMMAND} -E echo "Running Programme 1 with GDB..."
    COMMAND gdb --args ${CMAKE_BINARY_DIR}/app1/app1
    DEPENDS app1
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/app1
)

add_custom_target(run-prog2-gdb
    COMMAND ${CMAKE_COMMAND} -E echo "Running Programme 2 with GDB..."
    COMMAND gdb --args ${CMAKE_BINARY_DIR}/app2/app2
    DEPENDS app2
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/app2
)

add_custom_target(valgrind-prog1
    COMMAND ${CMAKE_COMMAND} -E echo "Running Programme 1 with Valgrind..."
    COMMAND valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ${CMAKE_BINARY_DIR}/app1/app1
    DEPENDS app1
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/app1
)

add_custom_target(valgrind-prog2
    COMMAND ${CMAKE_COMMAND} -E echo "Running Programme 2 with Valgrind..."
    COMMAND valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ${CMAKE_BINARY_DIR}/app2/app2
    DEPENDS app2
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/app2
)