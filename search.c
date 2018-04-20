#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>       //- For memset
#include <linux/limits.h>
#include <sys/types.h>

#include "comparators.h"
#include "constants.h"
#include "cosc242-search.h"
#include "globals.h"
#include "mylib.h"

int main(int argc, const char * argv[]) {
    int exit_code = EXIT_SUCCESS;

    int tfidf_enabled = 0;
    int c;

    while ((c = getopt (argc, argv, "t")) != -1) {
        switch (c) {
        case 't':
            tfidf_enabled = 1;
            break;
        default:
            abort ();
        }
    }

    /* There are no newlines in these documents */

    char * terms[max_terms];
    int dict_tf[max_terms];
    int n_terms = get_tf (dictionary_path, terms, dict_tf, max_terms) - 1;
    //  ^^^^^^^                            ^^^^^  ^^^^^^^  ^^^^^^^^^

    char * docnames[max_docs];
    int doc_tf[max_docs];
    int n_docs = get_tf (map_path, docnames, doc_tf, max_docs);
    //                             ^^^^^^^^  ^^^^^^  ^^^^^^^^


    int n_query_terms = 0;
    char * query_terms[std_term_strlen];
    char query[max_term_strlen];


    /* user_query:; */

    if (0 == fgets(query, sizeof query, stdin)) {
        exit_code = EXIT_EMPTY_QUERY;
    } else {
        query_terms[0] = strtok(query, " ");
        while(query_terms[n_query_terms])
            query_terms[++n_query_terms] = strtok(NULL, " ");

        if (!query_terms[0]) {
            exit_code = EXIT_EMPTY_QUERY;
        }
    }

    int * * postings_lists[n_query_terms];

    int posting_lens[n_query_terms]; //- the length of each postings list.

    //- TODO: make this a struct
    for (int index = 0; index < n_query_terms; index++) {
        postings_lists[index] = 0;
        posting_lens[index] = 0;
    }

    /* load_postings:; */

    FILE * findex = fopen(index_path, "rb");  
    for (int qw_i = 0; qw_i < n_query_terms; qw_i++) {        //- words in query
        char * * found = (char * *) bsearch(&query_terms[qw_i], terms, n_terms, sizeof(char *), compare_strings);
        int word_id = found - terms;

        if (found == NULL || word_id < 0) {
            exit_code = EXIT_MISSING_TERM;
            break;

        } else if (fseek(findex, (bytes_offset_t)dict_tf[word_id], SEEK_SET) != (bytes_offset_t)(-1)) {
            // [int] term frequency difference
            int tf_d = dict_tf[word_id + 1] - dict_tf[word_id];

            /* Allocate space for loading of postings list for the current query word */
            postings_lists[qw_i] = emalloc(max_docs * sizeof postings_lists[qw_i][0]);

            uint32_t c = 0; // counter (enumerate while decoding postings)
            // Can't be bothered working out the type
#define pl postings_lists[qw_i][c]
            while (tf_d && c < max_docs) {
                pl = postings_lists[qw_i][c];
                pl = emalloc(2 * sizeof postings_lists[qw_i][0]);
                pl[0] = vbdecode(&tf_d, findex);
                pl[1] = vbdecode(&tf_d, findex);

                c++;
            }

            uint32_t n_postings_for_word = c;
            for (uint32_t i = 1; i < n_postings_for_word; i++) {
                postings_lists[qw_i][i][0] += postings_lists[qw_i][i - 1][0];
            }

            posting_lens[qw_i] = n_postings_for_word;
        } else {
            exit_code = EXIT_BUG;
            break;
        }
    }
    fclose(findex);


    if (exit_code == EXIT_SUCCESS) {
        //- sort index
        int indices[n_query_terms];
        for (int index = 0; index < n_query_terms; index++)
            indices[index] = index;

        g_posting_lens = posting_lens; qsort(indices, n_query_terms, sizeof (int), posting_len_cmp);

        /* merge:; // merge query results */

        int * temp[posting_lens[indices[0]]];

        int n_ranked_results = posting_lens[indices[0]];
        int * results[n_ranked_results]; 
        memcpy(results, postings_lists[indices[0]], posting_lens[indices[0]] * sizeof postings_lists[indices[0]][0]);

        for (int index = 1; index < n_query_terms; index++) {

            // Merge postings_lists[indices[index]] and posting_lens[indices[index]]
            // Merge the results with the postings lists


            int * * postings_for_index = postings_lists[indices[index]]; //- A
            int posting_len = posting_lens[indices[index]];              //- B
            
            int i = 0;
            int j = 0;
            int n_inserted = 0; // A counter
            while (i < n_ranked_results && j < posting_len) {
                if (results[i][0] < postings_for_index[j][0]) {
                    i++;
                } else if (results[i][0] > postings_for_index[j][0]) {
                    j++;
                } else {
                    temp[n_inserted] = results[i++];
                    temp[n_inserted++][1] += postings_for_index[j++][1];
                }
            }
        
            memcpy(results, temp, n_inserted * sizeof (int));
            n_ranked_results = n_inserted;
        }

        /* rank:; Relevance scoring */

        /* Retrieval Status Value (RSV)  */
        /* https://nlp.stanford.edu/IR-book/pdf/11prob.pdf */

        double ranks[n_ranked_results];
        if (tfidf_enabled) {
            for (int index = 0; index < n_ranked_results; index++) {
                /* double word_density = atof(temp1) / n_terms; */
                /* word_density *= (log(((double)n_docs / (double)n_ranked_results)) / log(2)); */
                /* ranks[index] = word_density; */
                0;
            }
        } else {
            for (int index = 0; index < n_ranked_results; index++) {
                ranks[index] = results[index][1] / ((double) doc_tf[results[index][0]]);
            }
        }

        /* Sort the results based on ranking */
        memset(&indices, 0, sizeof(indices));
        for (int index = 0; index < n_ranked_results; index++) {
            indices[index] = index;
        }

        g_ranks = ranks; qsort(indices, n_ranked_results, sizeof indices[0], compare_ranks);

        for (int index = 0; index < n_ranked_results; index++) {
            printf("%s %f\n", docnames[results[indices[index]][0]], ranks[indices[index]]);
        }
    }

    /* cleanup:; */

    for (int qw_i = 0; qw_i < n_query_terms; qw_i++) {        //- words in query
        for (int pl_i = 0; pl_i < posting_lens[qw_i]; pl_i++)
            free(postings_lists[qw_i][pl_i]);
        free(postings_lists[qw_i]);
    }

    for (int index = 0; index < n_terms + 1; index++) {       //- words in dictionary
        free(terms[index]);
    }

    for (int index = 0; index < n_docs; index++) {
        free(docnames[index]);
    }

    return EXIT_SUCCESS;
}

/* Get term frequency tuples */
int get_tf(char * fpath, char * items[], int * values, uint32_t read_limit) {
    FILE * file = fopen (fpath, "r");

    char item[max_term_strlen];
    uint32_t count = 0, index = 0;

    //void read_words (FILE * f) {
    //    char x[1024];
    //    /* assumes no word exceeds length of 1023 */
    //    while (fscanf(f, " %1023s", x) == 1) {
    //        puts(x);
    //    }
    //}

    // asprintf(&string, "%d", max_term_strlen - 1); // or not
    /* while (fscanf(file, " %255s", item) == 1) { */

    while (getword(item, sizeof item, file) != EOF && index < read_limit) {
        if (count++ % 2) {
            values[index++] = atoi(item);
        } else {
            items[index] = emalloc((strlen(item) + 1) * sizeof items[0][0]);
            strcpy(items[index], item);
        }
    }

    fclose(file);
    return index;
}
