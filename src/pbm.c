#include "pbm.h"
#include <string.h>

static int read_token(int fd, char *token, size_t token_len) {
    unsigned char buff = '\0';
    size_t pos = 0;

    while (1) {
        if (read(fd, &buff, 1) <= 0)
            return -1;
        if (isspace(buff))
            continue;
        if (buff == '#') {
            do {
                if (read(fd, &buff, 1) <= 0)
                    return -1;
            } while (buff != '\n');
            continue;
        }
        break;
    }

    do {
        if (buff == '#') {
            do {
                if (read(fd, &buff, 1) <= 0)
                    break;
            } while (buff != '\n');
            break;
        }
        if (isspace(buff))
            break;
        if (pos + 1 >= token_len) {
            fprintf(stderr, "format error: PBM token too long\n");
            return -1;
        }
        token[pos++] = (char)buff;
    } while (read(fd, &buff, 1) == 1);

    token[pos] = '\0';
    return (pos > 0) ? 0 : -1;
}

static int parse_positive_int(const char *token) {
    int value = 0;

    for (int i = 0; token[i] != '\0'; i++) {
        if (!isdigit((unsigned char)token[i]))
            return -1;
        value = (value * 10) + (token[i] - '0');
    }
    return value;
}

int read_pbm_header(int fd, pbm_t* pix) {
    char token[32];

    pix->type = -1;
    if (read_token(fd, token, sizeof(token)) != 0 || strcmp(token, "P1") != 0) {
        fprintf(stderr,"format error: not a Plain PBM file\n");
        return -1;
    }

    pix->type = 1;
    return 0;
}

int read_dimension(int fd) {
    char token[32];
    if (read_token(fd, token, sizeof(token)) != 0) {
        fprintf(stderr,"format error: did not find dimension\n");
        return -1;
    }
    return parse_positive_int(token);
}

int read_pbm_data(int fd, pbm_t* pix) {
    char token[32];
    int pixels_read = 0;
    int pixels_expected = pix->width * pix->height;

    while (pixels_read < pixels_expected) {
        if (read_token(fd, token, sizeof(token)) != 0) {
            fprintf(stderr, "unexpected EOF: couldn't read PBM image of width %d and height %d\n",
                    pix->width, pix->height);
            return -1;
        }
        if (token[0] != '\0' && token[1] == '\0' &&
            (token[0] == '0' || token[0] == '1')) {
            pix->data[pixels_read++] = token[0] - '0';
        } else {
            fprintf(stderr, "format error: invalid PBM data token '%s'\n", token);
            return -1;
        }
    }
    return 0;
}

int read_pbm(const char* pbm_file, pbm_t* pix) {
    int fd=-1;
    int r=0;

    fd = open(pbm_file, O_RDONLY);
    if (fd<0) {
        perror(NULL);
        return -1;
    }

    if (read_pbm_header(fd, pix)!=0) {
        r=-1;
        goto ret;
    }

    pix->width = read_dimension(fd);
    if (pix->width<=0 || pix->width > MAX_PBM_LN) {
        fprintf(stderr,"format error: could not read correct PBM width\n");
        r=-1;
        goto ret;
    }

    pix->height = read_dimension(fd);
    if (pix->height<=0 || pix->height > MAX_PBM_CL) {
        fprintf(stderr,"format error: could not read correct PBM height\n");
        r=-1;
        goto ret;
    }

    if (read_pbm_data(fd, pix) != 0) {
        fprintf(stderr,"format error: could not read correct PBM data\n");
        r=-1;
        goto ret;
    }

ret:
    if (close(fd)<0) {
        perror("error closing pbm file");
        r=-1;
    }
    return r;
}

int print_pbm(pbm_t* pix) {
    int i = 0;
    int j = 0;
    for (int w=0; w<pix->width;w++)
        printf("--");
    printf("\n");
    while(j<pix->height) {
        fprintf(stdout, " %d",pix->data[(pix->width*j)+i]);
        i++;
        if (i==pix->width) {
            i=0;
            j++;
            printf("\n");
        }
    }
    for (int w=0; w<pix->width;w++)
        printf("--");
    printf("\n");
    return 0;
}
