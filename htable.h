#ifndef HTABLE_H_
#define HTABLE_H_

#include <stdio.h>
#include "list.h"

typedef struct htablerec * htable;

extern htable htable_new(int capacity);
extern list htable_search_list(htable h, char const * s);
extern node htable_search_node(htable h, char const * s, int document_id);
extern void htable_print_entire_table(htable h, FILE * stream);
extern void htable_print(htable h, FILE * postfile, FILE * dict);
extern void htable_delete(htable h);
extern void htable_print_stats(htable h, FILE * stream, int snapshots);
// extern int htable_insert(htable h, char const * s, int document_id);

#endif
