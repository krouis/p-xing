#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pbm.h"

void display_usage(const char* progname) {
    printf("Usage: %s [OPTIONS] <PBM_FILE>\n", progname);
    printf("Options:\n");
    printf("  -h           Display this usage message\n");
    printf("  -v           Display version information\n");
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
    if (argc < 2 ) {
        display_usage(argv[0]);
	return EXIT_FAILURE;
    }
    if (argc == 2) {
        if (strcmp(argv[1], "-v") == 0) {
            display_version();
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[1], "-h") == 0) {
            display_usage(argv[0]);
            return EXIT_SUCCESS;
        }
    }
    pbm_t pix;
    read_pbm(argv[1], &pix);
    print_pbm(&pix);
    print_pxing_lines(&pix);
    return EXIT_SUCCESS;
}
