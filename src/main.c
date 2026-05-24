#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "pbm.h"
#include "pxing.h"
#include "render.h"

void display_usage(const char* progname) {
    printf("Usage: %s [OPTIONS] <PBM_FILE>\n", progname);
    printf("Options:\n");
    printf("  -h           Display this usage message\n");
    printf("  -v           Display version information\n");
    printf("  -a           Enable assist mode (highlight wrong cells in red)\n");
}

void display_license() {
    printf("Copyright (c) 2024, Khalifa Rouis\n");
    printf("BSD 2-Clause License <https://github.com/krouis/p-xing/blob/main/LICENSE>.\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
}

void display_version() {
    printf("%s version: %s\n", PROJECT_NAME, PROJECT_VERSION);
    printf("This build is made from commit: %s\n", GIT_COMMIT_ID);
    printf("\n");
    display_license();
}

int main(int argc, char* argv[]) {
    int assist  = 0;
    int pbm_idx = -1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0)       assist = 1;
        else if (strcmp(argv[i], "-v") == 0) { display_version();        return EXIT_SUCCESS; }
        else if (strcmp(argv[i], "-h") == 0) { display_usage(argv[0]);   return EXIT_SUCCESS; }
        else                                   pbm_idx = i;
    }

    if (pbm_idx < 0) {
        display_usage(argv[0]);
        return EXIT_FAILURE;
    }

    pbm_t pix;
    if (read_pbm(argv[pbm_idx], &pix) != 0) {
        return EXIT_FAILURE;
    }

    pxing_t puzzle;
    compute_clues(&pix, &puzzle);

    game_t game;
    game_init(&game);

    render_init();

    int errors[MAX_PBM_LN * MAX_PBM_CL];
    int ch;
    do {
        if (assist) game_compute_errors(&game, &puzzle, errors);
        render_draw(&puzzle, &game, assist ? errors : NULL);
        ch = render_getch();

        /* Translate ncurses arrow keys to backend-independent PXING_KEY_* values. */
        switch (ch) {
            case KEY_UP:    ch = PXING_KEY_UP;    break;
            case KEY_DOWN:  ch = PXING_KEY_DOWN;  break;
            case KEY_LEFT:  ch = PXING_KEY_LEFT;  break;
            case KEY_RIGHT: ch = PXING_KEY_RIGHT; break;
            default: break;
        }

        if (ch == 'r') {
            game_init(&game);
        } else if (ch == 'u') {
            game_undo(&game);
        } else if (ch != ERR && !game.won) {
            game_handle_key(&game, &puzzle, ch);
            if (game_check_win(&game, &puzzle))
                game_set_won(&game);
        }
    } while (ch != 'q');

    render_cleanup();
    return EXIT_SUCCESS;
}
