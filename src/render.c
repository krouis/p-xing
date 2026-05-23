#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include "render.h"

#define CELL_W 2  /* chars per grid column: cell char + trailing space */

/* Color pair IDs */
#define CP_CLUE    1
#define CP_CURSOR  2
#define CP_FILLED  3
#define CP_CROSSED 4
#define CP_WIN     5

static int get_col_depth(const pxing_t *p) {
    int m = 1;
    for (int c = 0; c < p->width; c++)
        if (p->cols[c].count > m) m = p->cols[c].count;
    return m;
}

/* Returns the width (in chars) reserved for row clues + the │ separator. */
static int get_row_clue_width(const pxing_t *p) {
    int m = 0;
    for (int r = 0; r < p->height; r++) {
        int len = 0;
        for (int k = 0; k < p->rows[r].count; k++)
            len += (p->rows[r].runs[k] >= 10) ? 3 : 2;
        if (len > m) m = len;
    }
    return m + 1; /* +1 for the │ column */
}

void render_init(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(CP_CLUE,    COLOR_CYAN,   -1);
        init_pair(CP_CURSOR,  COLOR_BLACK,   COLOR_YELLOW);
        init_pair(CP_FILLED,  COLOR_BLACK,   COLOR_WHITE);
        init_pair(CP_CROSSED, COLOR_RED,    -1);
        init_pair(CP_WIN,     COLOR_BLACK,   COLOR_GREEN);
    }
}

void render_cleanup(void) {
    endwin();
}

void render_draw(const pxing_t *puzzle, const game_t *game) {
    int depth = get_col_depth(puzzle);
    int rw    = get_row_clue_width(puzzle);

    clear();

    /* --- Column clue header --- */
    for (int d = 0; d < depth; d++) {
        for (int c = 0; c < puzzle->width; c++) {
            const clue_t *cl = &puzzle->cols[c];
            int k = d - (depth - cl->count);
            if (k < 0 || k >= cl->count)
                continue;
            if (has_colors()) attron(COLOR_PAIR(CP_CLUE));
            mvprintw(d, rw + c * CELL_W, "%-2d", cl->runs[k]);
            if (has_colors()) attroff(COLOR_PAIR(CP_CLUE));
        }
    }

    /* --- Grid rows with row clues --- */
    for (int r = 0; r < puzzle->height; r++) {
        int y = depth + r;

        /* Row clue: build string then right-align against the │ separator */
        const clue_t *rc = &puzzle->rows[r];
        char buf[256];
        int  pos = 0;
        for (int k = 0; k < rc->count; k++)
            pos += snprintf(buf + pos, (int)sizeof(buf) - pos, "%d ", rc->runs[k]);
        buf[pos] = '\0';

        if (has_colors()) attron(COLOR_PAIR(CP_CLUE));
        mvprintw(y, rw - 1 - pos, "%s", buf);
        mvaddch(y, rw - 1, ACS_VLINE);
        if (has_colors()) attroff(COLOR_PAIR(CP_CLUE));

        /* Cells */
        for (int c = 0; c < puzzle->width; c++) {
            cell_state_t state =
                game ? game->grid[r * puzzle->width + c] : CELL_UNKNOWN;
            int is_cursor =
                game && game->cursor_row == r && game->cursor_col == c;
            int x = rw + c * CELL_W;

            if (is_cursor)
                attron(has_colors() ? COLOR_PAIR(CP_CURSOR) : A_REVERSE);
            else if (state == CELL_FILLED && has_colors())
                attron(COLOR_PAIR(CP_FILLED));
            else if (state == CELL_CROSSED && has_colors())
                attron(COLOR_PAIR(CP_CROSSED));

            switch (state) {
                case CELL_FILLED:  mvprintw(y, x, "##"); break;
                case CELL_CROSSED: mvprintw(y, x, "X "); break;
                default:           mvprintw(y, x, ". "); break;
            }

            if (is_cursor)
                attroff(has_colors() ? COLOR_PAIR(CP_CURSOR) : A_REVERSE);
            else if (state == CELL_FILLED && has_colors())
                attroff(COLOR_PAIR(CP_FILLED));
            else if (state == CELL_CROSSED && has_colors())
                attroff(COLOR_PAIR(CP_CROSSED));
        }
    }

    /* Status bar / win banner */
    int status_y = depth + puzzle->height + 1;
    if (game && game->won) {
        if (has_colors()) attron(COLOR_PAIR(CP_WIN));
        mvprintw(status_y, 0, " *** Solved! Press q to quit. *** ");
        if (has_colors()) attroff(COLOR_PAIR(CP_WIN));
    } else {
        mvprintw(status_y, 0, "arrows: move  space: fill  x: cross  q: quit");
    }

    refresh();
}

int render_getch(void) {
    return getch();
}
