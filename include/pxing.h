#ifndef PXING_H
#define PXING_H

#include <time.h>
#include "pbm.h"

#define MAX_CLUES ((MAX_PBM_LN + 1) / 2)
#define MAX_UNDO  16

typedef struct {
    int runs[MAX_CLUES];
    int count;
} clue_t;

typedef struct {
    clue_t rows[MAX_PBM_LN];
    clue_t cols[MAX_PBM_CL];
    int height;
    int width;
} pxing_t;

typedef enum {
    CELL_UNKNOWN = 0,
    CELL_FILLED  = 1,
    CELL_CROSSED = 2
} cell_state_t;

typedef struct {
    cell_state_t grid[MAX_PBM_LN * MAX_PBM_CL];
    int cursor_row;
    int cursor_col;
    int won;
    time_t start_time;
    int    solve_seconds; /* frozen at win time */
    /* undo stack */
    int          undo_top;
    cell_state_t undo_grid[MAX_UNDO][MAX_PBM_LN * MAX_PBM_CL];
    int          undo_cursor_row[MAX_UNDO];
    int          undo_cursor_col[MAX_UNDO];
} game_t;

int  compute_clues(const pbm_t *pix, pxing_t *puzzle);
void game_init(game_t *game);
int  game_elapsed_seconds(const game_t *game);
void game_handle_key(game_t *game, const pxing_t *puzzle, int key);
int  game_check_win(const game_t *game, const pxing_t *puzzle);
void game_undo(game_t *game);
void game_set_won(game_t *game);
void game_compute_errors(const game_t *game, const pxing_t *puzzle,
                         int errors[MAX_PBM_LN * MAX_PBM_CL]);
void print_row_clues(const pxing_t *puzzle);
void print_col_clues(const pxing_t *puzzle);

#endif /* PXING_H */
