#include "pbm.h"

int read_pbm(const char* pbm_file, pbm_t* pix){
    int fd=-1;
    int r=-1;
    char buff = {'\0'}; //this probably should be a char
    fd = open(pbm_file, O_RDONLY);
    if (fd<0) {
        perror(NULL);
        return -1;
    }

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
        goto ret;
    }

    // skip whitespaces
    if (read(fd,&buff,1)<0) goto eeof;
    while (isspace(buff))
        if (read(fd,&buff,1)<0) goto eeof;

    // if comments skip them
    int comment = (buff == '#') ? 1 : 0;
    while (comment>0) {
        if (read(fd, &buff,1)<0) goto eeof;
        if (buff == '\n') {
            if (read(fd, &buff,1)<0) goto eeof;
            // skip whitespaces
            while (isspace(buff))
                if(read(fd,&buff,1)<0) goto eeof;
            comment = (buff == '#') ? 1 : 0;
        }
    }

    //read width
    if (isdigit(buff)){
        pix->width = 0;
        while (isdigit(buff)) {
            pix->width = (pix->width * 10) + (int) buff-'0';
            if (read(fd,&buff,1)<0){
                fprintf(stderr,"unexpected EOF: couldn't read PBM image width\n");
                goto ret;
            }
        }

        if (pix->width > MAX_PBM_LN) {
            fprintf(stderr,"format error: PBM width %d is over %d limit\n",pix->width, MAX_PBM_LN);
            goto ret;
        }
    } else {
        fprintf(stderr,"format error: did not find PBM width\n");
        goto ret;
    }

    // skip whitespaces
    while (isspace(buff))
        if(read(fd,&buff,1)<0) goto eeof;

    //read height
    if (isdigit(buff)){
        pix->height = 0;
        while (isdigit(buff)) {
            pix->height = (pix->height * 10) + (int) buff-'0';
            if (read(fd,&buff,1)<0){
                fprintf(stderr,"unexpected EOF: couldn't read PBM image height\n");
                goto ret;
            }
        }

        if (pix->height > MAX_PBM_CL) {
            fprintf(stderr,"format error: PBM height %d is over %d limit\n",pix->height, MAX_PBM_CL);
            goto ret;
        }
    } else {
        fprintf(stderr,"format error: did not find PBM height\n");
        goto ret;
    }

    // skip whitespaces
    while (isspace(buff))
        if(read(fd,&buff,1)<0) goto eeof;

    // read the image and fill the data table
    int i=0;
    int j=0;
    while(buff!=EOF && (pix->width*j)+i<pix->width*pix->height){
        if (buff=='0'||buff=='1') {
            pix->data[(pix->width*j)+i] = buff-'0';
            if ((pix->width*j)+i==pix->width*pix->height-1){
                r=0;
                break;
            }
            i++;
            if(read(fd,&buff,1)<0) goto eeof;
        }
        if (buff==EOF) {
            if ((i+1)*(j+1)!=pix->width*pix->height) {
                fprintf(stderr,"unexpected EOF: couldn't read PBM image of width %d and height %d\n",pix->width, pix->height);
                goto ret;
            }
            r=0;
            break;
        }

        // skip whitespaces
        while (isspace(buff)){
            if (buff=='\n'){
                j++;
                i=0;
            }
            if(read(fd,&buff,1)<0) goto eeof;
        }
    }
    goto ret;

eeof:
    fprintf(stderr,"format error: unexpected EOF\n");
    r=-1;
ret:
    if (close(fd)<0) {
        perror(NULL);
        r=-1;
    }
    return r;
}

int print_pbm(pbm_t* pix){
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
