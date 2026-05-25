add_executable(test_pbm test_pbm.c ../src/pbm.c ../src/pxing.c)
target_sources(test_pbm PRIVATE ${UNITY_SOURCE})
target_include_directories(test_pbm PRIVATE ${UNITY_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../include)
# No ncurses dependency — pxing.c uses PXING_KEY_* constants, not KEY_*

add_test(NAME test_pbm COMMAND test_pbm WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

if(ENABLE_COVERAGE)
    if(NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "ENABLE_COVERAGE requires GCC or Clang-compatible gcov coverage flags")
    endif()

    find_program(GCOV_EXECUTABLE gcov REQUIRED)

    target_compile_options(test_pbm PRIVATE -O0 -g --coverage)
    target_link_libraries(test_pbm PRIVATE --coverage)

    add_custom_target(coverage
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND ${GCOV_EXECUTABLE} -b -c
                -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_pbm.dir/__/src/pbm.c.o
                ${CMAKE_SOURCE_DIR}/src/pbm.c
        COMMAND ${GCOV_EXECUTABLE} -b -c
                -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_pbm.dir/__/src/pxing.c.o
                ${CMAKE_SOURCE_DIR}/src/pxing.c
        DEPENDS test_pbm
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating gcov coverage reports"
        VERBATIM
    )
endif()
