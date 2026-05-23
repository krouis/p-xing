#include <stdio.h>
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
