#include <ncurses.h>
#include "unity.h"
#include "pbm.h"
#include "pxing.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_read_pbm_with_example_file(void) {
    pbm_t pix;
    int result = read_pbm("examples/p-xing.pbm", &pix);

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

void test_game_init_resets_state(void) {
    game_t game;
    /* Dirty the struct first */
    game.grid[0] = CELL_FILLED;
    game.cursor_row = 5;
    game.cursor_col = 3;
    game.won = 1;

    game_init(&game);

    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0]);
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[MAX_PBM_LN * MAX_PBM_CL - 1]);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_row);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_col);
    TEST_ASSERT_EQUAL_INT(0, game.won);
}

void test_game_init_restart_clears_progress(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);
    game.grid[0] = CELL_FILLED;
    game.grid[1] = CELL_CROSSED;
    game.cursor_row = 2;
    game.won = 1;

    game_init(&game); /* restart */

    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0]);
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[1]);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_row);
    TEST_ASSERT_EQUAL_INT(0, game.won);
}

void test_game_handle_key_fill_toggle(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);

    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0]);
    game_handle_key(&game, &puzzle, ' ');
    TEST_ASSERT_EQUAL_INT(CELL_FILLED,  (int)game.grid[0]);
    game_handle_key(&game, &puzzle, ' ');
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0]);
}

void test_game_handle_key_cross_toggle(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);

    game_handle_key(&game, &puzzle, 'x');
    TEST_ASSERT_EQUAL_INT(CELL_CROSSED, (int)game.grid[0]);
    game_handle_key(&game, &puzzle, 'x');
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0]);
}

void test_game_handle_key_cursor_bounds(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);

    /* Can't move left or up from origin */
    game_handle_key(&game, &puzzle, KEY_LEFT);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_col);
    game_handle_key(&game, &puzzle, KEY_UP);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_row);

    /* Move to bottom-right corner */
    game_handle_key(&game, &puzzle, KEY_RIGHT);
    game_handle_key(&game, &puzzle, KEY_RIGHT);
    game_handle_key(&game, &puzzle, KEY_DOWN);
    game_handle_key(&game, &puzzle, KEY_DOWN);
    TEST_ASSERT_EQUAL_INT(2, game.cursor_col);
    TEST_ASSERT_EQUAL_INT(2, game.cursor_row);

    /* Can't move beyond the grid edge */
    game_handle_key(&game, &puzzle, KEY_RIGHT);
    TEST_ASSERT_EQUAL_INT(2, game.cursor_col);
    game_handle_key(&game, &puzzle, KEY_DOWN);
    TEST_ASSERT_EQUAL_INT(2, game.cursor_row);
}

void test_game_elapsed_seconds_non_negative(void) {
    game_t game;
    game_init(&game);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, game_elapsed_seconds(&game));
}

/* --- Bundled puzzle sanity checks --- */

void test_cross_puzzle_dimensions_and_clues(void) {
    pbm_t pix;
    TEST_ASSERT_EQUAL_INT(0, read_pbm("puzzles/cross-5x5.pbm", &pix));
    TEST_ASSERT_EQUAL_INT(5, pix.width);
    TEST_ASSERT_EQUAL_INT(5, pix.height);

    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    /* Middle row / col are full — clue [5] */
    TEST_ASSERT_EQUAL_INT(1, puzzle.rows[2].count);
    TEST_ASSERT_EQUAL_INT(5, puzzle.rows[2].runs[0]);
    TEST_ASSERT_EQUAL_INT(1, puzzle.cols[2].count);
    TEST_ASSERT_EQUAL_INT(5, puzzle.cols[2].runs[0]);

    /* Corner rows / cols have a single cell — clue [1] */
    TEST_ASSERT_EQUAL_INT(1, puzzle.rows[0].count);
    TEST_ASSERT_EQUAL_INT(1, puzzle.rows[0].runs[0]);
    TEST_ASSERT_EQUAL_INT(1, puzzle.cols[0].count);
    TEST_ASSERT_EQUAL_INT(1, puzzle.cols[0].runs[0]);
}

void test_arrow_puzzle_dimensions_and_clues(void) {
    pbm_t pix;
    TEST_ASSERT_EQUAL_INT(0, read_pbm("puzzles/arrow-5x5.pbm", &pix));
    TEST_ASSERT_EQUAL_INT(5, pix.width);
    TEST_ASSERT_EQUAL_INT(5, pix.height);

    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    /* Middle row (widest part): clue [4] */
    TEST_ASSERT_EQUAL_INT(1, puzzle.rows[2].count);
    TEST_ASSERT_EQUAL_INT(4, puzzle.rows[2].runs[0]);

    /* Col 0 is empty: clue [0] */
    TEST_ASSERT_EQUAL_INT(1, puzzle.cols[0].count);
    TEST_ASSERT_EQUAL_INT(0, puzzle.cols[0].runs[0]);

    /* Col 1 is fully filled: clue [5] */
    TEST_ASSERT_EQUAL_INT(1, puzzle.cols[1].count);
    TEST_ASSERT_EQUAL_INT(5, puzzle.cols[1].runs[0]);
}

void test_house_puzzle_dimensions_and_clues(void) {
    pbm_t pix;
    TEST_ASSERT_EQUAL_INT(0, read_pbm("puzzles/house-7x7.pbm", &pix));
    TEST_ASSERT_EQUAL_INT(7, pix.width);
    TEST_ASSERT_EQUAL_INT(7, pix.height);

    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    /* Row 3 (widest, full): clue [7] */
    TEST_ASSERT_EQUAL_INT(1, puzzle.rows[3].count);
    TEST_ASSERT_EQUAL_INT(7, puzzle.rows[3].runs[0]);

    /* Rows 4 and 5 (walls with gap): clue [2, 2] */
    TEST_ASSERT_EQUAL_INT(2, puzzle.rows[4].count);
    TEST_ASSERT_EQUAL_INT(2, puzzle.rows[4].runs[0]);
    TEST_ASSERT_EQUAL_INT(2, puzzle.rows[4].runs[1]);
    TEST_ASSERT_EQUAL_INT(2, puzzle.rows[5].count);

    /* Centre column (col 3): apex + rows 1-3 + row 6 → [4, 1] */
    TEST_ASSERT_EQUAL_INT(2, puzzle.cols[3].count);
    TEST_ASSERT_EQUAL_INT(4, puzzle.cols[3].runs[0]);
    TEST_ASSERT_EQUAL_INT(1, puzzle.cols[3].runs[1]);
}

void test_game_elapsed_frozen_on_win(void) {
    game_t game;
    game_init(&game);
    game.won           = 1;
    game.solve_seconds = 42;
    TEST_ASSERT_EQUAL_INT(42, game_elapsed_seconds(&game));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_read_pbm_with_example_file);
    RUN_TEST(test_check_win_correct_solution);
    RUN_TEST(test_check_win_wrong_solution);
    RUN_TEST(test_check_win_empty_grid);
    RUN_TEST(test_game_init_resets_state);
    RUN_TEST(test_game_init_restart_clears_progress);
    RUN_TEST(test_game_handle_key_fill_toggle);
    RUN_TEST(test_game_handle_key_cross_toggle);
    RUN_TEST(test_game_handle_key_cursor_bounds);
    RUN_TEST(test_game_elapsed_seconds_non_negative);
    RUN_TEST(test_game_elapsed_frozen_on_win);
    RUN_TEST(test_cross_puzzle_dimensions_and_clues);
    RUN_TEST(test_arrow_puzzle_dimensions_and_clues);
    RUN_TEST(test_house_puzzle_dimensions_and_clues);
    return UNITY_END();
}

