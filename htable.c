#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"
#include "mylib.h"
#include "list.h"
#include "constants.h"


/* struct htablerec { */
/*     int capacity; */
/*     int num_keys; */
/*     int * freq; */
/*     int * stats; */
/*     char * * keys; */
/* }; */

struct htablerec {
  int capacity;
  int num_keys;
  int * freq;
  list * lists;
  char * * keys;
  char * * idmap;
  int * stats;
};


/* global variable for comparator function */
htable this = 0;


/* *
 * Prints a line of data indicating the state of the hash-table when
 * it is a given percentage full.
 *
 * The data is printed out in this order:
 *
 * - How full the hash-table is as a percentage
 * - How many keys are in the hash-table at that point
 * - What percentage of those keys were placed 'at home'
 * - The average number of collisions per key placed
 * - The maximum number of collisions while placing a key
 *
 * @param h the hash-table to get data from.
 * @param stream the place to send output to.
 * @param percent_full the point at which to print the statistics.
 *                     If the hashtable is less full than that, then
 *                     nothing will be printed.
 */
static void print_stats_line(htable h, FILE * stream, int percent_full) {
    int current_entries = h->capacity * percent_full / 100;
    double average_collisions = 0.0;
    int at_home = 0;
    int max_collisions = 0;
    int i = 0;

    if (current_entries > 0 && current_entries <= h->num_keys) {
        for (i = 0; i < current_entries; i++) {
            if (h->stats[i] == 0) {
                at_home++;
            }
            if (h->stats[i] > max_collisions) {
                max_collisions = h->stats[i];
            }
            average_collisions += h->stats[i];
        }

        fprintf(stream, "%4d %10d %10.1f %10.2f %11d\n", percent_full,
                current_entries, at_home * 100.0 / current_entries,
                average_collisions / current_entries, max_collisions);
    }
}

unsigned int htable_word_to_int(const char * word) {
    unsigned int result = 0;

    while(* word != '\0') {
        result = (* word++ + 31 * result);
    }
    return result;
}

unsigned int htable_step(htable h, unsigned int key) {
    return (key % (h->capacity - 1)) + 1;
}

htable htable_new(int capacity) {
    int i;
    htable result = (htable)emalloc(sizeof * result);
    result->capacity = capacity;
    result->num_keys = 0;
    result->freq = (int *)emalloc(result->capacity * sizeof result->freq[0]);
    result->stats = (int *)emalloc(result->capacity * sizeof result->stats[0]);
    result->lists = emalloc(capacity * sizeof result->lists[0]);
    result->keys = (char * *)emalloc(result->capacity * sizeof result->keys[0]);
    for(i = 0; i < capacity; i++) {
        result->freq[i] = 0;
        result->stats[i] = 0;
        result->keys[i] = NULL;

      result->lists[i] = 0;
    }

    return result;
}

int htable_insert(htable h, char const * s, int document_id) {
  unsigned int index;
  int count = 0;

  index = htable_step(h, htable_word_to_int(s)) % h->capacity;

  while (count < h->capacity) {
      if (h->keys[index] == NULL) {
          h->keys[index] = (char *)emalloc((strlen(s) + 1) * sizeof h->keys[0][0]);
          strcpy(h->keys[index], s);
          h->freq[index] = 1;
          h->stats[h->num_keys++] = count;
          h->lists[index] = ll_new(h->keys[index]);
          ll_append(h->lists[index], document_id);
          return SUCCESS;
      } else if (strcmp(h->keys[index], s) == 0) {
          ll_add(h->lists[index], document_id);
          return SUCCESS;
      }
      count++;

    index = htable_step(h, index) % h->capacity;
  }
  return FAILED;
}

int htable_search(htable h, char * s) {
    int bangs = 0;
    unsigned int index;

    index = htable_step(h, htable_word_to_int(s)) % h->capacity;

    while((h->keys[index] == NULL || strcmp(h->keys[index], s) != 0) && bangs < h->capacity) {
        index = htable_step(h, index) % h->capacity;
        bangs++;
    }
    if(bangs == h->capacity) {
        return 0;
    } else {
        return h->freq[index];
    }
}

void htable_print_entire_table(htable h, FILE * stream) {
    int i;

    fprintf(stream, "%7s %7s %7s %7s\n", "Pos", "Freq", "Stats", "Word");
    fprintf(stream, "---------------------------------\n");
    for(i = 0; i < h->capacity; i++) {
        fprintf(stream, "%7d %7d %7d    %-s\n", i, h->freq[i], h->stats[i], h->keys[i] == NULL ? "" : h->keys[i]);
    }

}

int htable_index_cmp(const void * a, const void * b) {
     const int * ia = (const int *)a;
     const int * ib = (const int *)b;
     return strcmp(this->keys[* ia], this->keys[* ib]);
}

/*
void htable_print(htable h, FILE * stream) {
    int i;

    for(i = 0; i < h->capacity; i++) {
        if(h->freq[i] != 0) {
            fprintf(stream, "%-3d %s\n", h->freq[i], h->keys[i]);
        }
    }
}
*/

void htable_print(htable h, FILE * termslist, FILE * dictionary) {
  int index, count = 0, len = 0;
  int * items = emalloc(h->num_keys * sizeof items[0]);
  this = h;

  for (index = 0; index < h->capacity; index++)
    if (h->lists[index])
      items[count++] = index;
  qsort(items, h->num_keys, sizeof items[0], htable_index_cmp);

  for (index = 0; index < h->num_keys; index++) {
    fprintf(dictionary, "%s %d\n", h->keys[items[index]], len);
    len += ht_out(h->lists[items[index]], termslist);
  }
  /* Bogus value for calculating length of the last postfile list */
  fprintf(dictionary, "%s %d\n", "bogus", len);

  free(items);
  items = 0;
  this = 0;
}

void htable_print_stats(htable h, FILE * stream, int num_stats) {
    int i;

    fprintf(stream,"\nUsing ");
    fprintf(stream,"Percent   Current   Percent    Average      Maximum\n");
    fprintf(stream," Full     Entries   At Home   Collisions   Collisions\n");
    fprintf(stream,"-----------------------------------------------------\n");
    for(i = 1; i <= num_stats; i++) {
        print_stats_line(h, stream, 100 * i / num_stats);
    }
    fprintf(stream,"-----------------------------------------------------\n");
}

void htable_delete(htable h) {

    int i;
    for(i = 0; i < h->capacity; i++) {
        if(h->keys[i] != NULL) {
            free(h->keys[i]);
        }
    }
    free(h->keys);
    
    for (i = 0; i < h->capacity; i++) {
      ll_delete(h->lists[i]);
    }
    free(h->lists);

    free(h->stats);
    free(h->freq);
    free(h);
}
