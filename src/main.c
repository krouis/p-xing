#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <ncurses.h>
#include "pbm.h"
#include "pxing.h"
#include "render.h"

#define PUZZLE_DIR "puzzles"

typedef struct {
    char **paths;
    int count;
    int capacity;
} puzzle_list_t;

void display_usage(const char* progname) {
    printf("Usage: %s [OPTIONS] [PBM_FILE]\n", progname);
    printf("Options:\n");
    printf("  -h           Display this usage message\n");
    printf("  -v           Display version information\n");
    printf("  -a           Enable assist mode (auto-cross and wrong fills in red)\n");
    printf("If PBM_FILE is omitted, bundled puzzles from %s/ are loaded.\n", PUZZLE_DIR);
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

static int has_pbm_suffix(const char *name) {
    size_t len = strlen(name);
    return len > 4 && strcmp(name + len - 4, ".pbm") == 0;
}

static const char *basename_of(const char *path) {
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

static int difficulty_rank(const char *path) {
    const char *name = basename_of(path);

    if (strncmp(name, "easy-", 5) == 0)
        return 0;
    if (strncmp(name, "medium-", 7) == 0)
        return 1;
    if (strncmp(name, "hard-", 5) == 0)
        return 2;
    return 0;
}

static int compare_puzzle_paths(const void *a, const void *b) {
    const char *const *sa = a;
    const char *const *sb = b;
    int ra = difficulty_rank(*sa);
    int rb = difficulty_rank(*sb);

    if (ra != rb)
        return ra - rb;
    return strcmp(*sa, *sb);
}

static void puzzle_list_free(puzzle_list_t *list) {
    for (int i = 0; i < list->count; i++)
        free(list->paths[i]);
    free(list->paths);
    list->paths = NULL;
    list->count = 0;
    list->capacity = 0;
}

static int puzzle_list_reserve(puzzle_list_t *list, int capacity) {
    char **paths;

    if (capacity <= list->capacity)
        return 0;

    paths = realloc(list->paths, (size_t)capacity * sizeof(list->paths[0]));
    if (!paths) {
        perror("realloc");
        return -1;
    }

    list->paths = paths;
    list->capacity = capacity;
    return 0;
}

static int add_puzzle_path(puzzle_list_t *list, const char *dir, const char *name) {
    char path[PATH_MAX];
    int len;
    char *copy;

    len = snprintf(path, sizeof(path), "%s/%s", dir, name);
    if (len < 0 || (size_t)len >= sizeof(path)) {
        fprintf(stderr, "Puzzle path is too long: %s/%s\n", dir, name);
        return -1;
    }

    if (list->count == list->capacity) {
        int next_capacity = list->capacity ? list->capacity * 2 : 32;
        if (puzzle_list_reserve(list, next_capacity) != 0)
            return -1;
    }

    copy = malloc(strlen(path) + 1);
    if (!copy) {
        perror("malloc");
        return -1;
    }

    strcpy(copy, path);
    list->paths[list->count] = copy;
    list->count++;
    return 0;
}

static int load_bundled_puzzles(puzzle_list_t *list) {
    DIR *dir = opendir(PUZZLE_DIR);
    struct dirent *entry;

    list->paths = NULL;
    list->count = 0;
    list->capacity = 0;
    if (!dir) {
        perror(PUZZLE_DIR);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (!has_pbm_suffix(entry->d_name))
            continue;
        if (add_puzzle_path(list, PUZZLE_DIR, entry->d_name) != 0) {
            closedir(dir);
            puzzle_list_free(list);
            return -1;
        }
    }

    if (closedir(dir) != 0) {
        perror("closedir");
        puzzle_list_free(list);
        return -1;
    }

    if (list->count == 0) {
        fprintf(stderr, "No .pbm puzzles found in %s/\n", PUZZLE_DIR);
        return -1;
    }

    qsort(list->paths, (size_t)list->count, sizeof(list->paths[0]), compare_puzzle_paths);
    return 0;
}

static int load_puzzle(const char *path, pxing_t *puzzle, game_t *game) {
    pbm_t pix;

    if (read_pbm(path, &pix) != 0)
        return -1;

    compute_clues(&pix, puzzle);
    game_init(game);
    return 0;
}

int main(int argc, char* argv[]) {
    int assist  = 0;
    int pbm_idx = -1;
    puzzle_list_t playlist = {0};
    int playlist_index = 0;
    int playlist_mode = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0)       assist = 1;
        else if (strcmp(argv[i], "-v") == 0) { display_version();        return EXIT_SUCCESS; }
        else if (strcmp(argv[i], "-h") == 0) { display_usage(argv[0]);   return EXIT_SUCCESS; }
        else if (argv[i][0] == '-') {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            display_usage(argv[0]);
            return EXIT_FAILURE;
        } else if (pbm_idx >= 0) {
            fprintf(stderr, "Only one PBM file can be specified\n");
            display_usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            pbm_idx = i;
        }
    }

    pxing_t puzzle;
    game_t game;

    if (pbm_idx >= 0) {
        if (load_puzzle(argv[pbm_idx], &puzzle, &game) != 0)
            return EXIT_FAILURE;
    } else {
        playlist_mode = 1;
        if (load_bundled_puzzles(&playlist) != 0)
            return EXIT_FAILURE;
        if (load_puzzle(playlist.paths[playlist_index], &puzzle, &game) != 0) {
            puzzle_list_free(&playlist);
            return EXIT_FAILURE;
        }
    }

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
        } else if (ch == 'n') {
            if (playlist_mode && playlist.count > 1) {
                playlist_index = (playlist_index + 1) % playlist.count;
                if (load_puzzle(playlist.paths[playlist_index], &puzzle, &game) != 0)
                    ch = 'q';
            }
        } else if (ch == 'u') {
            game_undo(&game);
        } else if (ch != ERR && !game.won) {
            game_handle_key(&game, &puzzle, ch, assist);
            if (game_check_win(&game, &puzzle))
                game_set_won(&game);
        }
    } while (ch != 'q');

    render_cleanup();
    if (playlist_mode)
        puzzle_list_free(&playlist);
    return EXIT_SUCCESS;
}
