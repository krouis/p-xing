#include "unity.h"
#include "pbm.h"
#include "pxing.h"

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

/* 3x3 L-shape puzzle (unique solution):
 *  # . .    row clues: [1] [1] [3]
 *  # . .    col clues: [3] [1] [1]
 *  # # #
 */
static pbm_t make_3x3_lshape(void) {
    pbm_t pix;
    pix.width = 3; pix.height = 3; pix.type = 1;
    int d[9] = { 1,0,0,
                 1,0,0,
                 1,1,1 };
    for (int i = 0; i < 9; i++) pix.data[i] = d[i];
    return pix;
}

void test_check_win_correct_solution(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);
    game.grid[0*3+0] = CELL_FILLED;
    game.grid[1*3+0] = CELL_FILLED;
    game.grid[2*3+0] = CELL_FILLED;
    game.grid[2*3+1] = CELL_FILLED;
    game.grid[2*3+2] = CELL_FILLED;

    TEST_ASSERT_EQUAL_INT(1, game_check_win(&game, &puzzle));
}

void test_check_win_wrong_solution(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);
    /* main diagonal — row 2 clue [3] is not satisfied */
    game.grid[0*3+0] = CELL_FILLED;
    game.grid[1*3+1] = CELL_FILLED;
    game.grid[2*3+2] = CELL_FILLED;

    TEST_ASSERT_EQUAL_INT(0, game_check_win(&game, &puzzle));
}

void test_check_win_empty_grid(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);

    TEST_ASSERT_EQUAL_INT(0, game_check_win(&game, &puzzle));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_read_pbm_with_example_file);
    RUN_TEST(test_check_win_correct_solution);
    RUN_TEST(test_check_win_wrong_solution);
    RUN_TEST(test_check_win_empty_grid);
    return UNITY_END();
}

