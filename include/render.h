#ifndef RENDER_H
#define RENDER_H

#include "pxing.h"

void render_init(void);
void render_cleanup(void);
void render_draw(const pxing_t *puzzle, const game_t *game);
int  render_getch(void);

#endif /* RENDER_H */
