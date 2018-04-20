#include <string.h>      //- For strcmp
#include "globals.h"
#include "comparators.h"

int posting_len_cmp(const void * a, const void * b) {
    const int * ia = (const int *)a;
    const int * ib = (const int *)b;

    int postlenA = g_posting_lens[* ia];
    int postlenB = g_posting_lens[* ib];

    if (postlenA < postlenB)
        return -1;
    else if (postlenA > postlenB)
        return 1;
    else
        return 0;
}

int compare_ranks(const void * a, const void * b) {
    const int * ia = (const int *)a;
    const int * ib = (const int *)b;

    int rankA = g_ranks[* ia];
    int rankB = g_ranks[* ib];

    if (rankA < rankB)
        return 1;
    else if (rankA > rankB)
        return -1;
    else
        return 0;
}

int compare_strings(const void * s1, const void * s2) {
    return strcmp(*(char * *) s1, *(char * *) s2);
}
