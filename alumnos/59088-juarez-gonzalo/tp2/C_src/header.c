#define NCOLORS 3
#define H_MAXSIZE 512

struct header {
    char content[H_MAXSIZE];
    char magic[3];
    unsigned int cols;
    unsigned int rows;
    unsigned short maxcolor;
};

#define COLORSIZE(headerp) ({               \
    (headerp)->maxcolor & 0xff00 ? 2 : 1;   \
    })

#define BYTES_PER_PX(headerp) ({            \
    COLORSIZE(headerp) * NCOLORS;           \
    })

int headersize(struct header *headerp)
{
    char *c;
    c = headerp->content;
    while (*c++ != '\x00')
        ;
    c--;
    return c - headerp->content;
}

unsigned long bodysize(struct header *headerp)
{
    int pixelcount;
    pixelcount = headerp->rows * headerp->cols;
    return COLORSIZE(headerp) * BYTES_PER_PX(headerp) * pixelcount;
}

unsigned long filesize(struct header *headerp)
{
    return headersize(headerp) + bodysize(headerp);
}

unsigned long ppm_align(struct header *headerp, unsigned long size)
{
    int b_per_px;
    b_per_px = BYTES_PER_PX(headerp);
    if (size < b_per_px)
        return b_per_px;
    return (size / b_per_px) * b_per_px;
}

/*
#include <stdio.h>

struct header hdr = {
    .content = "AAAA",
    .rows = 512,
    .cols = 200,
    .magic = "P6",
    .maxcolor = 0xFF,
};

int main()
{
    int i;
    for (i = 0; i < H_MAXSIZE - 100; i++)
        hdr.content[i] = '\x41';
    hdr.content[i] = '\x00';
    printf("headersize %d\n", headersize(&hdr));
    printf("bodysize %lu\n", bodysize(&hdr));
    printf("filesize %lu\n", filesize(&hdr));
    printf("ppm_align(512) = %lu\n", ppm_align(&hdr, 512));
    return 0;
}
*/
