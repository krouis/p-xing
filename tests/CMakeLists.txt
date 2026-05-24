add_executable(test_pbm test_pbm.c ../src/pbm.c ../src/pxing.c)
target_sources(test_pbm PRIVATE ${UNITY_SOURCE})
target_include_directories(test_pbm PRIVATE ${UNITY_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../include)
# No ncurses dependency — pxing.c uses PXING_KEY_* constants, not KEY_*

add_test(NAME test_pbm COMMAND test_pbm WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
