#include "parse.c"

int main(int argc, char **argv)
{
    struct arguments *argp;
    struct header *headerp;
    struct header *out_headerp;

    argp = malloc(sizeof(struct arguments));
    memset(argp, '\x00', sizeof(struct arguments));
    parse_args(argc, argv, argp);

    headerp = malloc(sizeof(struct header));
    memset(headerp, '\x00', sizeof(struct header));
    search_fileheader(argp->filepath, headerp);

    out_headerp = malloc(sizeof(struct header));
    memcpy(out_headerp, headerp, sizeof(struct header));

    if (argp->rotopt != WALSH)
        swap_rc(out_headerp);

    free(out_headerp);
    free(headerp);
    free(argp);
}
