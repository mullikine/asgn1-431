#include <stdio.h>
#include <assert.h>

// cosc242 asgn2 -> with some tweaks
int getword(char * s, int limit, FILE * stream) {
    int c;
    char * w = s;
    assert(limit > 0 && s != NULL && stream != NULL);

    /* skip to the start of the word */
    while (!isalnum(c = getc(stream)) && EOF != c);

    if(EOF == c) {
        return EOF;
    } else if(--limit > 0) { /* reduce limit by 1 to allow for the \0 */
        * w++ = tolower(c);
    }

    while (--limit > 0) {
        // ' and - *
        if (isalnum(c = getc(stream)) || c == 39 || c == 45) {
            * w++ = tolower(c);
        } else if ('\'' == c) {
            limit++;
        } else {
            break;
        }
    }
    * w = '\0';

    return w - s;
}
