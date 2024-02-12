#ifndef PBM_H
#define PBM_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_PBM_LN 70
#define MAX_PBM_CL 70

typedef struct pbm{
    int data[MAX_PBM_LN*MAX_PBM_CL];
    int height;
    int width;
    int type;
} pbm_t;

int read_pbm(const char* pbm_file, pbm_t* pix);
int print_pbm(pbm_t* pix);
int print_pxing_lines(pbm_t* pix);

#endif /* PBM_H */
