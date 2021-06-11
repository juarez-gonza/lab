#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    return strlen(headerp->content);
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

void _swap_rc_content(char *str)
{
    char newstr[20];
    char *space;
    char *nl;
    char *rowstr;
    char *colstr;

    space = strchr(str, '\x20');
    nl = strchr(str, '\n');
    *space = '\x00';
    *nl = '\x00';

    rowstr = space + 1;
    colstr = str;

    /* yes it is intended not to be null terminated because a '\n' is what should follow next
     * because the header still goes on
     */
    snprintf(newstr, 20, "%s %s", rowstr, colstr);
    strncpy(str, newstr, strlen(newstr));
    *nl = '\n';
}


#define RCLINE 2
/*
 * Heavily relies on headerp->content being lines separated by
 *'\n' and terminating '\x00' after the final '\n'
 */
void swap_rc_content(struct header *headerp)
{
    unsigned int rows, cols, count;
    char *c;
    char *nl;
    int in_cmmnt;

    in_cmmnt = 0;
    count = RCLINE;
    for (c = headerp->content; *c != '\x00'; c = nl + 1) {
        /* puts '\x00' at '\n' address -> isolates line */
        nl = strchr(c, '\n');
        *nl = '\x00';
        if (strchr(c, '#') == NULL)
            count--;

        /* restores '\n' to what it was before isolating the line */
        *nl = '\n';

        if (count == 0) {
            _swap_rc_content(c);
            break;
        }
    }
}

void _swap_rc(struct header *headerp)
{
    unsigned int tmp;

    tmp = headerp->rows;
    headerp->rows = headerp->cols;
    headerp->cols = tmp;
}

void swap_rc(struct header *headerp)
{
    swap_rc_content(headerp);
    _swap_rc(headerp);
}

/*
struct header hdr = {
    .content = "P6\n# Imagen ppm\n200 298\n255\n",
    .rows = 512,
    .cols = 200,
    .magic = "P6",
    .maxcolor = 0xFF,
};

int main()
{
    printf("headersize %d\n", headersize(&hdr));
    printf("bodysize %lu\n", bodysize(&hdr));
    printf("filesize %lu\n", filesize(&hdr));
    printf("ppm_align(512) = %lu\n", ppm_align(&hdr, 512));
    printf("%s\tstrlen(hdr): %lu\n", hdr.content, strlen(hdr.content));
    swap_rc(&hdr);
    printf("%s\tstrlen(hdr): %lu\n", hdr.content, strlen(hdr.content));

    return 0;
}
*/
