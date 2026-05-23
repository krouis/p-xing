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

int compute_clues(const pbm_t *pix, pxing_t *puzzle);
void print_row_clues(const pxing_t *puzzle);
void print_col_clues(const pxing_t *puzzle);

#endif /* PXING_H */
