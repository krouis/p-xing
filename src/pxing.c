#include <stdio.h>
#include <string.h>
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
    game->undo_top    = 0;
}

static void game_push_undo(game_t *game) {
    int slot = game->undo_top % MAX_UNDO;
    memcpy(game->undo_grid[slot], game->grid, sizeof(game->grid));
    game->undo_cursor_row[slot] = game->cursor_row;
    game->undo_cursor_col[slot] = game->cursor_col;
    game->undo_top++;
}

void game_set_won(game_t *game) {
    game->solve_seconds = game_elapsed_seconds(game);
    game->won           = 1;
}

void game_undo(game_t *game) {
    if (game->undo_top == 0) return;
    game->undo_top--;
    int slot = game->undo_top % MAX_UNDO;
    memcpy(game->grid, game->undo_grid[slot], sizeof(game->grid));
    game->cursor_row = game->undo_cursor_row[slot];
    game->cursor_col = game->undo_cursor_col[slot];
    game->won        = 0;
}

static int clues_match(const clue_t *a, const clue_t *b) {
    if (a->count != b->count) return 0;
    for (int k = 0; k < a->count; k++)
        if (a->runs[k] != b->runs[k]) return 0;
    return 1;
}

static void game_apply_auto_cross(game_t *game, const pxing_t *puzzle) {
    int pixels[MAX_PBM_LN > MAX_PBM_CL ? MAX_PBM_LN : MAX_PBM_CL];
    clue_t actual;

    for (int r = 0; r < puzzle->height; r++) {
        for (int c = 0; c < puzzle->width; c++)
            pixels[c] = (game->grid[r * puzzle->width + c] == CELL_FILLED) ? 1 : 0;
        compute_line_clue(pixels, puzzle->width, &actual);
        if (clues_match(&actual, &puzzle->rows[r]))
            for (int c = 0; c < puzzle->width; c++)
                if (game->grid[r * puzzle->width + c] == CELL_UNKNOWN)
                    game->grid[r * puzzle->width + c] = CELL_CROSSED;
    }

    for (int col = 0; col < puzzle->width; col++) {
        for (int r = 0; r < puzzle->height; r++)
            pixels[r] = (game->grid[r * puzzle->width + col] == CELL_FILLED) ? 1 : 0;
        compute_line_clue(pixels, puzzle->height, &actual);
        if (clues_match(&actual, &puzzle->cols[col]))
            for (int r = 0; r < puzzle->height; r++)
                if (game->grid[r * puzzle->width + col] == CELL_UNKNOWN)
                    game->grid[r * puzzle->width + col] = CELL_CROSSED;
    }
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
        case PXING_KEY_UP:    if (game->cursor_row > 0)                game->cursor_row--; break;
        case PXING_KEY_DOWN:  if (game->cursor_row < puzzle->height-1) game->cursor_row++; break;
        case PXING_KEY_LEFT:  if (game->cursor_col > 0)                game->cursor_col--; break;
        case PXING_KEY_RIGHT: if (game->cursor_col < puzzle->width-1)  game->cursor_col++; break;
        case ' ':
            game_push_undo(game);
            *cell = (*cell == CELL_FILLED) ? CELL_UNKNOWN : CELL_FILLED;
            if (*cell == CELL_FILLED)
                game_apply_auto_cross(game, puzzle);
            break;
        case 'x': case 'X':
            game_push_undo(game);
            *cell = (*cell == CELL_CROSSED) ? CELL_UNKNOWN : CELL_CROSSED;
            break;
        default: break;
    }
}

static void mark_line_errors(const cell_state_t *line, int len,
                              const clue_t *expected, int *errors) {
    int clue_idx = 0;
    int run_start = -1;
    int run_len = 0;

    for (int i = 0; i <= len; i++) {
        int filled = (i < len) && (line[i] == CELL_FILLED);
        if (filled) {
            if (run_start < 0) { run_start = i; run_len = 0; }
            run_len++;
        } else if (run_start >= 0) {
            int bad = (clue_idx >= expected->count) ||
                      (run_len > expected->runs[clue_idx]);
            if (bad)
                for (int k = run_start; k < run_start + run_len; k++)
                    errors[k] = 1;
            clue_idx++;
            run_start = -1;
            run_len   = 0;
        }
    }
}

void game_compute_errors(const game_t *game, const pxing_t *puzzle,
                         int errors[MAX_PBM_LN * MAX_PBM_CL]) {
    memset(errors, 0, MAX_PBM_LN * MAX_PBM_CL * sizeof(int));

    cell_state_t line[MAX_PBM_LN > MAX_PBM_CL ? MAX_PBM_LN : MAX_PBM_CL];
    int          lerr[MAX_PBM_LN > MAX_PBM_CL ? MAX_PBM_LN : MAX_PBM_CL];

    for (int r = 0; r < puzzle->height; r++) {
        for (int c = 0; c < puzzle->width; c++)
            line[c] = game->grid[r * puzzle->width + c];
        memset(lerr, 0, puzzle->width * sizeof(int));
        mark_line_errors(line, puzzle->width, &puzzle->rows[r], lerr);
        for (int c = 0; c < puzzle->width; c++)
            if (lerr[c]) errors[r * puzzle->width + c] = 1;
    }

    for (int col = 0; col < puzzle->width; col++) {
        for (int r = 0; r < puzzle->height; r++)
            line[r] = game->grid[r * puzzle->width + col];
        memset(lerr, 0, puzzle->height * sizeof(int));
        mark_line_errors(line, puzzle->height, &puzzle->cols[col], lerr);
        for (int r = 0; r < puzzle->height; r++)
            if (lerr[r]) errors[r * puzzle->width + col] = 1;
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
