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

enable_testing()

# Copy examples directory to the build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Define the path to the Unity source file
set(UNITY_SOURCE ${CMAKE_SOURCE_DIR}/unity/src/unity.c)

# Define the path to the directory containing the Unity header file
set(UNITY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/unity/src/)

# include unity for unittests
include_directories(unity/src)

include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)
