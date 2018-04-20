#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "mylib.h"

/* node := a pointer to a node struct */
/* list := a pointer to a list struct */

/* this is a list of doc ids */

/* constructor for a doc id node */
node node_new(int _document_id) {
    node n = emalloc(sizeof * n);
    {
        n->document_id = _document_id;
        n->next  = 0;
        n->count = 1;
    }
    return n;
}

void ll_delete(list _l) {
    if (_l) {
        if (_l->first) {
            node_delete(_l->first);
        }
        free(_l);
        _l = 0;
    }
}

/* constructor for list */
list ll_new(char const * _item) {
    /* Allocate memory for the list */
    list l = emalloc(sizeof * l);
    {
        l->last = 0;
        l->word = _item;
        l->first = 0;
        /* l->size = strlen(_item);*/
        l->last_document_id = 0;
    }
    return l;
}

/* send docnumber and count to the _fout and then print the next one 
   The returned length is the total number of bytes written by the entire linked list */

int node_print(node _n, FILE * _fout) {
    int len = 0; 
    if (_n) {
        len += vbencode(_n->document_id, _fout);
        len += vbencode(_n->count, _fout);
        len += node_print(_n->next, _fout);
    }
    return len;
}

/* Deletes all nodes starting with the first */
void node_delete(node _n) {
    if (_n) {
        if (_n->next) {
            node_delete(_n->next);
        }
        free(_n);
        _n = 0;
    }
}

/* this increases the count of existing node or adds a new node */
void ll_add(list _l, int _document_id) {
    /* node n = ll_search(_l, _document_id);*/
    node n;
    /* if last exists and _document_id is the _document_id of the last one */
    if ((_l->last != 0) && (_document_id == _l->last_document_id)) {
        /* select the last node of the list */
        n = _l->last;
    }
    /* if nothing was selected then do a normal append */
    if (n == 0) {
        /* create a new entry for a document if it doesnt exist in the list */
        ll_append(_l, _document_id);
    } else {
        /* if the document already exists in the list, then increase its count */
        n->count++;
    }
}

/* create a new entry with unique _document_id in the list */
void ll_append(list _l, int _document_id) {
    /* why are we subtracting the last document_id from the _document_id? */
    /* query will segfault if it's changed to _document_id */
    node n = node_new(_document_id - _l->last_document_id);

    /* if there is nothing in the list, make the first and last point to the new one */
    if (_l->first == 0) {
        _l->first = _l->last = n;
    } else {
        /* otherwise, extend the list */
        _l->last = n;
        _l->last->next = n;
    }
    _l->last_document_id = _document_id;
}

node ll_search(list _l, int _document_id) {
    int total = 0; 
    for (node n = _l->first; n != 0; n = n->next, total += n->document_id) {
        if (total == _document_id) {
            return n;
        }
    }
    return 0;
}

/* variable encode the doc id list to a file */
int ht_out(list _l, FILE * _fout) {
    if (_l) {
        return node_print(_l->first, _fout);
    }
    return 0;
}
