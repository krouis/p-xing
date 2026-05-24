#ifndef RENDER_H
#define RENDER_H

#include "pxing.h"

void render_init(void);
void render_cleanup(void);
/* errors: array of MAX_PBM_LN*MAX_PBM_CL ints, non-zero means error; pass NULL to disable. */
void render_draw(const pxing_t *puzzle, const game_t *game, const int *errors);
int  render_getch(void);

#endif /* RENDER_H */
