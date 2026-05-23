#ifndef PXING_H
#define PXING_H

#include "pbm.h"

#define MAX_CLUES ((MAX_PBM_LN + 1) / 2)

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
} game_t;

int  compute_clues(const pbm_t *pix, pxing_t *puzzle);
void game_init(game_t *game);
void game_handle_key(game_t *game, const pxing_t *puzzle, int key);
void print_row_clues(const pxing_t *puzzle);
void print_col_clues(const pxing_t *puzzle);

#endif /* PXING_H */
