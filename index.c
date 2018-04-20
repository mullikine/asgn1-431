#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "htable.h"
#include "mylib.h"


void clean_word(char * word) {
    if (word) {
        int len = strlen(word);
        while (39 == word[len - 1]) {
            word[--len] = '\0';
        }
        if (len > 2 && word[len - 2] == 39 && word[len - 1] == 's') {
            len -= 2;
            word[len] = '\0';
        }
    }
}


int main(int argc, const char * argv[]) {
    FILE * xfile = fopen(argv[1], "r");

    int insert = 0, mapping = 0, document_id = -1, wc = 0;
    int doc_tf[htable_capacity];
    char * idmap[htable_capacity];

    htable h = htable_new(htable_capacity);

    for (uint32_t index = 0; index < htable_capacity; index++)
        idmap[index] = 0;

    char buffer[max_term_strlen];
    int c = 0, index = 0;

    while (EOF != (c = tolower(getc(xfile)))) {
        switch (c) {
        case '<':
            {
                if (index) {
                    buffer[index] = '\0';
                    clean_word(buffer);
                    word(buffer, mapping, idmap, insert, h, document_id, wc);
                    index = 0;
                }

                c = getc(xfile);
                switch (c) {
                case '/':
                    {
                        while (EOF != (c = tolower(getc(xfile))) && '>' != c) {
                            buffer[index++] = c;
                        }

                        buffer[index++] = '\0';
                        if (!strcmp(buffer, "docno")) {
                            if (!mapping) {
                                exit(EXIT_BUG);
                            }
                            mapping = 0;
                        } else if (!strcmp(buffer, "doc")) {
                            doc_tf[document_id] = wc;
                            wc = 0;
                        }
                        break;
                    }
                default:
                    c = ungetc(c, xfile);
                    if (EOF != c) {
                        int c, index = 0;
                        while (EOF != (c = tolower(getc(xfile))) && '>' != c) {
                            buffer[index++] = c;
                        }

                        buffer[index++] = '\0';
                        if (!strcmp(buffer, "docno")) {
                            wc = 0;
                            mapping = 1;
                            ++document_id;
                        }
                    }
                    break;
                }
                break;
            case '&': // html entities
                {
                    int ret = 0;
                    int c = tolower(getc(xfile));

                    switch (c) {
                    case 'a':
                        {
                            c = tolower(getc(xfile));
                            if (c == 'p') ret = 39;
                            else if (c == 'm') ret = '&';
                            else {
                                exit(EXIT_BUG);
                            }
                            break;
                        }
                    case 'q':
                        ret = '"';
                        break;
                    case 'l':
                        ret = '<';
                        break;
                    case 'g':
                        ret = '>';
                        break;
                    default:
                        ungetc(c, xfile);
                        return ret; /* Don't want to consume until ; if there was no entity ref */
                    }
                    while (EOF != c && ';' != c)
                        c = getc(xfile);

                    return ret;
                    break;
                }
            }
        default:
            {
                if (index == max_term_strlen) {
                    exit(EXIT_BUG);
                }

                if (isalnum(c) || c == 39) {
                    if (c != 39 || index) { /* no ' allowed at the start */
                        buffer[index++] = c;
                    }
                } else if (index) {
                    buffer[index] = '\0';
                    clean_word(buffer);
                    word(buffer, mapping, idmap, insert, h, document_id, wc);
                    index = 0;
                }
                break;
            }
        }
    }

    if (!feof(xfile)) {
        exit(EXIT_BUG);
    }



    { // write index
        int count;
        FILE * findex = fopen(index_path, "wb");
        FILE * fdictionary = fopen(dictionary_path, "w");
        FILE * mfile = fopen(map_path, "w");

        htable_print(h, findex, fdictionary);
        fflush(findex);
        fflush(fdictionary);

        for (count = 0; count <= document_id; count++) {
            fprintf(mfile, "%s %d\n", idmap[count], doc_tf[count]);
        }

        fflush(mfile);

        fclose(findex);
        fclose(fdictionary);
        fclose(mfile);
    }


    { // end indexing
        htable_delete(h);
        h = 0;
        for (uint32_t index = 0; index <= document_id; index++)
            free(idmap[index]);
    }

    fclose(xfile);

    return EXIT_SUCCESS;
}


void word(char const * spelling, int mapping, char * idmap[], int insert, htable h, int document_id, int wc) {
    if (mapping) {
        if (!insert) {
            idmap[document_id] = emalloc(15 * sizeof idmap[0][0]);
            strcpy(idmap[document_id], spelling);
            idmap[document_id][9] = '-';
            insert = 10;
        } else {
            strcpy(idmap[document_id] + insert, spelling);
            insert = 0;
        }
    }
    int error = htable_insert(h, spelling, document_id);
    wc++;
}
