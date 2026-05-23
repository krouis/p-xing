add_executable(test_pbm test_pbm.c ../src/pbm.c ../src/pxing.c)
target_sources(test_pbm PRIVATE ${UNITY_SOURCE})
target_include_directories(test_pbm PRIVATE ${UNITY_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(test_pbm ${CURSES_LIBRARIES})

add_test(NAME test_pbm COMMAND test_pbm)
