#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mylib.h"
#include "constants.h"

void * emalloc(size_t s) {
    void * o = malloc(s);
    if (!o) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    return o;
}

unsigned long int vbdecode(int * bytes, FILE * stream) {
    unsigned long int x, b;
    int shift;

    x = 0, shift = 0, b = 0;
    while ((* bytes)-- && (b = getc(stream)) > 127 && !feof(stream)) {
        x |= (b & 127) << shift;
        shift += 7;
    }
    return x | (b << shift);
}

int vbyteread(FILE * fp)
{
    char tmp = 0x1;
    int val = 0;

    while((tmp & 0x1) == 0x1)
    {
        if (fread(&tmp, sizeof(char), 1, fp) == 0)
        {
            if (feof(fp))
                return(-1);
            else
                return(0);
        }
        val = (val << 7) + ((tmp >> 1) & 127);
    }
    return(val);
}

int vbytewrite(int number, FILE * fp)
{
    char bytearray[4];
    char tmp = 0;
    int x, started = FALSE;
    int charswritten = 0;

    for(x=0;x<4;x++)
    {
        tmp = (number%128) << 1;
        bytearray[x] = tmp;
        number /= 128;
    }
    for(x=3;x>0;x--)
    {
        if (bytearray[x] != 0 || started == TRUE)
        {
            started = TRUE;
            bytearray[x] |= 0x1;
            fwrite(&bytearray[x], sizeof(char), 1, fp);
            charswritten++;
        }
    }
    bytearray[0] |= 0x0;
    fwrite(&bytearray[0], sizeof(char), 1, fp);
    charswritten++;
    return(charswritten);
}


/* When the linked list prints to the file stream, it encodes the doc number
 * and count for each item in the linked list to the stream */
int vbencode(unsigned long int x, FILE * stream) {
    int bytes_written = 0;
    while (x > 127) {
        putc((x & 127) | 128, stream);
        x >>= 7;
        bytes_written++;
    }
    putc(x, stream);
    return ++bytes_written;
}

