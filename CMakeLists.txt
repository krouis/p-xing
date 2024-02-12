cmake_minimum_required(VERSION 3.10)
project(p-xing)

set(CMAKE_C_STANDARD 11)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_ID
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions("-DGIT_COMMIT_ID=\"${GIT_COMMIT_ID}\"")
add_definitions("-DPROJECT_NAME=\"${PROJECT_NAME}\"")
add_definitions("-DPROJECT_VERSION=\"${PROJECT_VERSION}\"")

add_compile_options(
    -Wall          # Enable all warnings
    -Werror        # Treat warnings as errors
    -ggdb          # Generate debugging information for gdb
    -fstack-usage  # Generate stack usage information
)

include_directories(include)
add_subdirectory(src)

