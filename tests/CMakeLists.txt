add_executable(test_pbm test_pbm.c ../src/pbm.c)
target_sources(test_pbm PRIVATE ${UNITY_SOURCE})
target_include_directories(test_pbm PRIVATE ${UNITY_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_test(NAME test_pbm COMMAND test_pbm)
