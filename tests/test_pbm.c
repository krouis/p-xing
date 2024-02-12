#include "unity.h"
#include "pbm.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_read_pbm_with_example_file(void) {
    pbm_t pix;
    int result = read_pbm("../examples/p-xing.pbm", &pix);

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(36, pix.width);
    TEST_ASSERT_EQUAL_INT(7, pix.height);
    TEST_ASSERT_EQUAL_INT(1, pix.type);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_read_pbm_with_example_file);
    return UNITY_END();
}

