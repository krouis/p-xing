# Add executable
add_executable(p-xing main.c pbm.c)

# Include directories
target_include_directories(p-xing PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

