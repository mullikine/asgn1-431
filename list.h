#ifndef LIST_H_
#define LIST_H_

#include <stdio.h>

typedef struct listrec * list;
typedef struct noderec * node;

struct noderec {
   node next;
   int document_id;
   int count;
};

struct listrec {
   node first, last;
   char const * word;
   int last_document_id;
};

extern int ht_out(list l, FILE * stream);
extern list ll_new(char const * item);
extern node ll_search(list l, int document_id);
extern void ll_add(list l, int document_id);
extern void ll_append(list l, int document_id);
extern void ll_delete(list l);

#endif
