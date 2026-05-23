#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "pxing.h"

static void compute_line_clue(const int *pixels, int len, clue_t *clue) {
    clue->count = 0;
    int run = 0;
    for (int i = 0; i < len; i++) {
        if (pixels[i]) {
            run++;
        } else if (run > 0) {
            clue->runs[clue->count++] = run;
            run = 0;
        }
    }
    if (run > 0)
        clue->runs[clue->count++] = run;
    if (clue->count == 0) {
        clue->runs[0] = 0;
        clue->count = 1;
    }
}

int compute_clues(const pbm_t *pix, pxing_t *puzzle) {
    puzzle->width  = pix->width;
    puzzle->height = pix->height;

    for (int row = 0; row < pix->height; row++)
        compute_line_clue(&pix->data[row * pix->width], pix->width, &puzzle->rows[row]);

    int col_pixels[MAX_PBM_CL];
    for (int col = 0; col < pix->width; col++) {
        for (int row = 0; row < pix->height; row++)
            col_pixels[row] = pix->data[row * pix->width + col];
        compute_line_clue(col_pixels, pix->height, &puzzle->cols[col]);
    }
    return 0;
}

void game_init(game_t *game) {
    memset(game->grid, CELL_UNKNOWN, sizeof(game->grid));
    game->cursor_row  = 0;
    game->cursor_col  = 0;
    game->won         = 0;
    game->start_time  = time(NULL);
    game->solve_seconds = 0;
}

int game_elapsed_seconds(const game_t *game) {
    if (game->won)
        return game->solve_seconds;
    return (int)(time(NULL) - game->start_time);
}

int game_check_win(const game_t *game, const pxing_t *puzzle) {
    int pixels[MAX_PBM_LN];
    clue_t actual;

    for (int r = 0; r < puzzle->height; r++) {
        for (int c = 0; c < puzzle->width; c++)
            pixels[c] = (game->grid[r * puzzle->width + c] == CELL_FILLED) ? 1 : 0;
        compute_line_clue(pixels, puzzle->width, &actual);
        const clue_t *expected = &puzzle->rows[r];
        if (actual.count != expected->count) return 0;
        for (int k = 0; k < actual.count; k++)
            if (actual.runs[k] != expected->runs[k]) return 0;
    }

    for (int c = 0; c < puzzle->width; c++) {
        for (int r = 0; r < puzzle->height; r++)
            pixels[r] = (game->grid[r * puzzle->width + c] == CELL_FILLED) ? 1 : 0;
        compute_line_clue(pixels, puzzle->height, &actual);
        const clue_t *expected = &puzzle->cols[c];
        if (actual.count != expected->count) return 0;
        for (int k = 0; k < actual.count; k++)
            if (actual.runs[k] != expected->runs[k]) return 0;
    }

    return 1;
}

void game_handle_key(game_t *game, const pxing_t *puzzle, int key) {
    cell_state_t *cell = &game->grid[game->cursor_row * puzzle->width + game->cursor_col];
    switch (key) {
        case KEY_UP:    if (game->cursor_row > 0)                game->cursor_row--; break;
        case KEY_DOWN:  if (game->cursor_row < puzzle->height-1) game->cursor_row++; break;
        case KEY_LEFT:  if (game->cursor_col > 0)                game->cursor_col--; break;
        case KEY_RIGHT: if (game->cursor_col < puzzle->width-1)  game->cursor_col++; break;
        case ' ':
            *cell = (*cell == CELL_FILLED) ? CELL_UNKNOWN : CELL_FILLED;
            break;
        case 'x': case 'X':
            *cell = (*cell == CELL_CROSSED) ? CELL_UNKNOWN : CELL_CROSSED;
            break;
        default: break;
    }
}

void print_row_clues(const pxing_t *puzzle) {
    printf("Row clues:\n");
    for (int i = 0; i < puzzle->height; i++) {
        const clue_t *c = &puzzle->rows[i];
        for (int j = 0; j < c->count; j++)
            printf("%d ", c->runs[j]);
        printf("\n");
    }
}

void print_col_clues(const pxing_t *puzzle) {
    printf("Column clues:\n");
    for (int i = 0; i < puzzle->width; i++) {
        const clue_t *c = &puzzle->cols[i];
        for (int j = 0; j < c->count; j++)
            printf("%d ", c->runs[j]);
        printf("\n");
    }
}
