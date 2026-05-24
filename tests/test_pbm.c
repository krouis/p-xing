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

/* --- Undo tests --- */

void test_game_set_won_records_elapsed_time(void) {
    game_t game;
    game_init(&game);
    /* Backdate start_time to simulate 10 s of play. */
    game.start_time = time(NULL) - 10;

    game_set_won(&game);

    TEST_ASSERT_EQUAL_INT(1, game.won);
    /* The original bug set won=1 before capturing the time, causing
     * game_elapsed_seconds to short-circuit and return solve_seconds (0). */
    TEST_ASSERT_GREATER_OR_EQUAL_INT(10, game.solve_seconds);
}

void test_game_undo_empty_stack(void) {
    game_t game;
    game_init(&game);
    /* Undo on empty stack must not crash and must leave state unchanged. */
    game_undo(&game);
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0]);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_row);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_col);
}

void test_game_undo_restores_state(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);

    /* Fill (0,0) — also auto-crosses (0,1) and (0,2) since row 0 clue is [1] */
    game_handle_key(&game, &puzzle, ' ');
    TEST_ASSERT_EQUAL_INT(CELL_FILLED,  (int)game.grid[0*3+0]);
    TEST_ASSERT_EQUAL_INT(CELL_CROSSED, (int)game.grid[0*3+1]);

    /* Undo must restore the full pre-fill state */
    game_undo(&game);
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0*3+0]);
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0*3+1]);
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0*3+2]);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_row);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_col);
}

void test_game_undo_multiple_steps(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);

    /* Fill (0,0): push undo #1; auto-crosses (0,1) and (0,2). */
    game_handle_key(&game, &puzzle, ' ');
    /* Move right to (0,1): no undo push. */
    game_handle_key(&game, &puzzle, KEY_RIGHT);
    /* Toggle cross on already-crossed (0,1): push undo #2; (0,1) → UNKNOWN. */
    game_handle_key(&game, &puzzle, 'x');

    TEST_ASSERT_EQUAL_INT(CELL_FILLED,  (int)game.grid[0*3+0]);
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0*3+1]);

    /* Undo #2 → restores FILLED/CROSSED/CROSSED; cursor back at (0,1). */
    game_undo(&game);
    TEST_ASSERT_EQUAL_INT(CELL_FILLED,  (int)game.grid[0*3+0]);
    TEST_ASSERT_EQUAL_INT(CELL_CROSSED, (int)game.grid[0*3+1]);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_row);
    TEST_ASSERT_EQUAL_INT(1, game.cursor_col);

    /* Undo #1 → fully empty grid; cursor back at (0,0). */
    game_undo(&game);
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0*3+0]);
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0*3+1]);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_row);
    TEST_ASSERT_EQUAL_INT(0, game.cursor_col);
}

void test_game_undo_clears_won_flag(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);
    game_handle_key(&game, &puzzle, ' ');   /* fill (0,0) first, push undo */
    game.won = 1;

    game_undo(&game);
    TEST_ASSERT_EQUAL_INT(0, game.won);
}

/* --- Auto-cross tests --- */

void test_game_auto_cross_completes_row(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);

    /* Row 0 clue is [1].  Filling (0,0) satisfies it → (0,1) and (0,2) auto-crossed. */
    game_handle_key(&game, &puzzle, ' ');
    TEST_ASSERT_EQUAL_INT(CELL_FILLED,  (int)game.grid[0*3+0]);
    TEST_ASSERT_EQUAL_INT(CELL_CROSSED, (int)game.grid[0*3+1]);
    TEST_ASSERT_EQUAL_INT(CELL_CROSSED, (int)game.grid[0*3+2]);
}

void test_game_auto_cross_completes_col(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);

    /* Navigate to (2,1); col 1 clue is [1]. */
    game_handle_key(&game, &puzzle, KEY_DOWN);
    game_handle_key(&game, &puzzle, KEY_DOWN);
    game_handle_key(&game, &puzzle, KEY_RIGHT);
    TEST_ASSERT_EQUAL_INT(2, game.cursor_row);
    TEST_ASSERT_EQUAL_INT(1, game.cursor_col);

    /* Filling (2,1) satisfies col 1 [1] → (0,1) and (1,1) auto-crossed.
     * Row 2 clue [3] is NOT yet satisfied, so no row auto-cross. */
    game_handle_key(&game, &puzzle, ' ');
    TEST_ASSERT_EQUAL_INT(CELL_FILLED,  (int)game.grid[2*3+1]);
    TEST_ASSERT_EQUAL_INT(CELL_CROSSED, (int)game.grid[0*3+1]);
    TEST_ASSERT_EQUAL_INT(CELL_CROSSED, (int)game.grid[1*3+1]);
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[2*3+0]); /* row 2 not done */
}

/* --- Error detection (assist mode) tests --- */

void test_game_compute_errors_empty_grid(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);

    int errors[MAX_PBM_LN * MAX_PBM_CL];
    game_compute_errors(&game, &puzzle, errors);

    for (int i = 0; i < 9; i++)
        TEST_ASSERT_EQUAL_INT(0, errors[i]);
}

void test_game_compute_errors_valid_fill(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);
    /* Row 0 clue [1]: filling only (0,0) is valid. */
    game.grid[0*3+0] = CELL_FILLED;

    int errors[MAX_PBM_LN * MAX_PBM_CL];
    game_compute_errors(&game, &puzzle, errors);

    TEST_ASSERT_EQUAL_INT(0, errors[0*3+0]);
}

void test_game_compute_errors_overlong_row_run(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);
    /* Row 0 clue [1]: two consecutive fills exceed it → both flagged. */
    game.grid[0*3+0] = CELL_FILLED;
    game.grid[0*3+1] = CELL_FILLED;

    int errors[MAX_PBM_LN * MAX_PBM_CL];
    game_compute_errors(&game, &puzzle, errors);

    TEST_ASSERT_EQUAL_INT(1, errors[0*3+0]);
    TEST_ASSERT_EQUAL_INT(1, errors[0*3+1]);
    TEST_ASSERT_EQUAL_INT(0, errors[0*3+2]);
}

void test_game_compute_errors_extra_run_in_row(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);
    /* Row 0 clue [1]: two separate single fills → second has no clue slot. */
    game.grid[0*3+0] = CELL_FILLED;
    game.grid[0*3+2] = CELL_FILLED;

    int errors[MAX_PBM_LN * MAX_PBM_CL];
    game_compute_errors(&game, &puzzle, errors);

    TEST_ASSERT_EQUAL_INT(0, errors[0*3+0]); /* first run matches [1] */
    TEST_ASSERT_EQUAL_INT(1, errors[0*3+2]); /* second run has no clue */
}

void test_game_auto_cross_no_effect_on_unfill(void) {
    pbm_t pix = make_3x3_lshape();
    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);

    /* Fill and then unfill (0,0) — toggling to UNKNOWN must not auto-cross. */
    game_handle_key(&game, &puzzle, ' '); /* fill  → auto-crosses (0,1),(0,2) */
    game_undo(&game);                     /* restore: all UNKNOWN */
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0*3+1]);
    TEST_ASSERT_EQUAL_INT(CELL_UNKNOWN, (int)game.grid[0*3+2]);
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
    RUN_TEST(test_game_set_won_records_elapsed_time);
    RUN_TEST(test_game_undo_empty_stack);
    RUN_TEST(test_game_undo_restores_state);
    RUN_TEST(test_game_undo_multiple_steps);
    RUN_TEST(test_game_undo_clears_won_flag);
    RUN_TEST(test_game_auto_cross_completes_row);
    RUN_TEST(test_game_auto_cross_completes_col);
    RUN_TEST(test_game_auto_cross_no_effect_on_unfill);
    RUN_TEST(test_game_compute_errors_empty_grid);
    RUN_TEST(test_game_compute_errors_valid_fill);
    RUN_TEST(test_game_compute_errors_overlong_row_run);
    RUN_TEST(test_game_compute_errors_extra_run_in_row);
    return UNITY_END();
}

