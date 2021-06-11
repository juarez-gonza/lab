#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <fcntl.h>

#include "header.c"


#define CCW 1
#define CW 2
#define WALSH 3

#define R_FLAG 1
#define G_FLAG 2
#define B_FLAG 4
#define RGB_FLAG 7

#define MAX_FILENAME 128
#define MAX_FILEPATH 2048

struct arguments {
    char filename[MAX_FILENAME];
    char filepath[MAX_FILEPATH];
    unsigned long rsize;
    unsigned int rotopt;
    unsigned int colorfilter;
};

void usagendie()
{
    printf("Error\n");
    exit(-1);
}

/* expects pointer to 0 initialized arg_struct */
void parse_args(int argc, char **argv, struct arguments *arg_struct)
{
    int opt;
    arg_struct->rotopt = CCW;

    while ((opt = getopt(argc, argv, "f:s:c:w")) != -1) {
        switch (opt) {
            case 'f':
                /* MAX_FILENAME may fall short but well that's on you for having such a large filename */
                strncpy(arg_struct->filename, optarg, MAX_FILENAME-1);
                arg_struct->filename[MAX_FILENAME-1] = '\x00';
                break;
            case 's':
                arg_struct->rsize = atol(optarg);
                break;
            case 'c':
                arg_struct->colorfilter = atoi(optarg) & RGB_FLAG;
                break;
            case 'w':
                arg_struct->rotopt = WALSH;
                break;
            default:
                usagendie();
        }
    }

    if (strlen(arg_struct->filename) == 0 || arg_struct->rsize <= 0)
        usagendie();

    /* this may overflow depending on MAX_FILEPATH */
    if (realpath(arg_struct->filename, arg_struct->filepath) == NULL)
        usagendie();
}

#define INFONL 3
/* expects pointer to 0 initialized struct header */
void parseheader(char *str, struct header *headerp)
{
    char *uncmmnt;
    int in_cmmnt;
    int nl_count;
    char *uncmmnt_ptr;
    char *c;

    uncmmnt = malloc(H_MAXSIZE);
    memset(uncmmnt, '\x00', H_MAXSIZE);
    uncmmnt_ptr = uncmmnt;

    in_cmmnt = 0;
    nl_count = 0;

    for (c = str; c-str < H_MAXSIZE; c++) {
        switch (*c) {
            case '#':

                in_cmmnt |= 1;

                break;
            case '\n':

                if (in_cmmnt) {
                    in_cmmnt &= 0;
                }
                else {
                    nl_count++;
                    *uncmmnt_ptr = *c;
                    uncmmnt_ptr++;
                }

                break;
            default:
                if (in_cmmnt)
                    break;
                *uncmmnt_ptr = *c;
                uncmmnt_ptr++;
        }
        if (nl_count == INFONL) {
            c++;
            break;
        }
    }
    /* c-str+1 because of null-termination of snprintf */
    snprintf(headerp->content, c-str+1, "%s", str);

    uncmmnt_ptr = uncmmnt;
    c = strchr(uncmmnt_ptr, '\n');
    snprintf(headerp->magic, c-uncmmnt_ptr+1, "%s", uncmmnt);

    uncmmnt_ptr = c + 1;
    c = strchr(uncmmnt_ptr, '\x20');
    *c = '\x00';
    headerp->cols = atoi(uncmmnt_ptr);

    uncmmnt_ptr = c + 1;
    c = strchr(uncmmnt_ptr, '\n');
    *c = '\x00';
    headerp->rows = atoi(uncmmnt_ptr);

    uncmmnt_ptr = c + 1;
    c = strchr(uncmmnt_ptr, '\n');
    *c = '\x00';
    headerp->maxcolor = atoi(uncmmnt_ptr);

    free(uncmmnt);
    uncmmnt = c = uncmmnt_ptr = NULL;
}

void search_fileheader(char *filepath, struct header *headerp)
{
    char *str;
    unsigned int rb;
    unsigned int fd;

    rb = 0;
    str = malloc(H_MAXSIZE);
    memset(str, '\x00', H_MAXSIZE);

    fd = open(filepath, O_RDONLY);
    while (rb < H_MAXSIZE)
        rb += read(fd, str, H_MAXSIZE-rb);

    /* just in case null-terminate, one never knows */
    str[H_MAXSIZE-1] = '\x00';

    close(fd);
    fd = -1;

    parseheader(str, headerp);

    free(str);
    str = NULL;
}

/*
int main(int argc, char **argv)
{
    struct arguments *arg_struct;
    arg_struct = malloc(sizeof(struct arguments));
    memset(arg_struct, '\x00', sizeof(struct arguments));
    parse_args(argc, argv, arg_struct);

    printf("filename: %s\n", arg_struct->filename);
    printf("filepath: %s\n", arg_struct->filepath);
    printf("rsize: %lu\n", arg_struct->rsize);
    printf("rotopt: %d\n", arg_struct->rotopt);
    printf("colorfilter: %d\n", arg_struct->colorfilter);
}
*/

/*
char headerbytes[] = "P6\n# Imagen ppm\n200 298\n255\n";
int main()
{

    struct header *headerp;
    char *str;
    int i;

    headerp = malloc(sizeof(struct header));
    memset(headerp, '\x00', sizeof(struct header));

    str = malloc(H_MAXSIZE);
    for (i = 0; i < H_MAXSIZE; i++) {
        if (i < strlen(headerbytes))
            str[i] = headerbytes[i];
        else
            str[i] = '\x41';
    }

    parseheader(str, headerp);

    printf("content: %s\n", headerp->content);
    printf("magic: %s, strlen(magic): %lu\n", headerp->magic, strlen(headerp->magic));
    printf("rows: %d\n", headerp->rows);
    printf("cols: %d\n", headerp->cols);
    printf("maxcolor: %d\n", headerp->maxcolor);

    free(str);
    str = NULL;
}
*/
