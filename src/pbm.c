#include "pbm.h"

//skip whitespaces and comments
int skip_whitespace(int fd) {
    char buff = '\0';

    // skip whitespaces
    do {
        if (read(fd,&buff,1)<0) return -1;
    } while (isspace(buff));

    // if comments skip them
    int comment = (buff == '#') ? 1 : 0;
    while (comment>0) {
        if (read(fd, &buff,1)<0) return -1;
        if (buff == '\n') {
            if (read(fd, &buff,1)<0) return -1;
            // skip whitespaces
            while (isspace(buff))
                if(read(fd,&buff,1)<0) return -1;
            comment = (buff == '#') ? 1 : 0;
        }
    }
    // trackback 1 position before returning
    if (lseek(fd, -1, SEEK_CUR)==-1) {
        perror(NULL);
        return -1;
    }
    return 0;
}

int read_pbm_header(int fd, pbm_t* pix) {
    char buff = '\0';
    // read magic
    pix->type=-1;
    if(read(fd,&buff,1)==1){
        //check magic for P1
        if (buff=='P'){
            if(read(fd,&buff,1)==1){
                pix->type = buff - '0';
            }
        }
    }
    if (pix->type != 1){
        fprintf(stderr,"format error: not a Plain PBM file\n");
        return 0;
    }

    if (skip_whitespace(fd)<0) {
        return -1;
    }
    return 0;
}

int read_dimension(int fd) {
    char buff = '\0';
    int dim = 0; // this is a positive integer, -1 is an error
    //skip any whitespaces
    if (skip_whitespace(fd)<0) return -1;

    //read number
    if(read(fd,&buff,1)<0) return -1;

    if (isdigit(buff)) {
        while (isdigit(buff)) {
            dim = (dim * 10) + (int) buff-'0';
            if (read(fd,&buff,1)<0) {
                return -1;
            }
        }
    } else {
        fprintf(stderr,"format error: did not find dimension\n");
        return -1;
    }
    return dim;
}

int read_pbm_data(int fd, pbm_t* pix) {
    char buff = '\0';
    int i=0;
    int j=0;

    // skip whitespaces
    do {
        if(read(fd,&buff,1)<0) {
            fprintf(stderr,"format error: unexpected EOF\n");
            return -1;
        }
    }while (isspace(buff));

    // read the image and fill the data table
    while(buff!=EOF && (pix->width*j)+i<pix->width*pix->height) {
        if (buff=='0'||buff=='1') {
            pix->data[(pix->width*j)+i] = buff-'0';
            if ((pix->width*j)+i==pix->width*pix->height-1) return 0;
            i++;
            if(read(fd,&buff,1)<0) {
                fprintf(stderr,"format error: unexpected EOF\n");
                return -1;
            }
        }
        if (buff==EOF) {
            if ((i+1)*(j+1)!=pix->width*pix->height) {
                fprintf(stderr,"unexpected EOF: couldn't read PBM image of width %d and height %d\n",pix->width, pix->height);
                return -1;
            }
            return 0;
        }

        // skip whitespaces
        while (isspace(buff)) {
            if (buff=='\n') {
                j++;
                i=0;
            }
            if(read(fd,&buff,1)<0) {
                fprintf(stderr,"format error: unexpected EOF\n");
                return -1;
            }
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
        fprintf(stderr,"format error: could not read correct PBM height\n");
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

int print_pxing_lines(pbm_t* pix) {
    int cnt=-1;
    printf("print p-xing lines v1:\n");
    for (int i=0; i<pix->height;i++) {
        int j=0;
        cnt = -1;
        while (j<pix->width){
            if(pix->data[(pix->width*i)+j]==0) {
                if (cnt>0) {
                    printf("%d ",cnt);
                    cnt=0;
                }
            }else
                (cnt<0) ? cnt=1 : cnt++;
            j++;
        }
        if (cnt>0)  printf("%d",cnt);
        if (cnt<0) printf("0"); //all zeros line
        printf("\n");
    }
    return 0;
}
