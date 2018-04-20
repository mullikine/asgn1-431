#ifndef MYLIB_H_
#define MYLIB_H_

#include <stddef.h>
#include <stdio.h>
// #include <limits.h>

extern void clean_word(char * word);
extern void * emalloc(size_t);
extern void * calloc(size_t n, size_t s);
extern void * realloc(void *, size_t);
extern FILE * fopen(const char * path, const char * mode);
extern int fclose(FILE * fp);
extern int vbencode(unsigned long int x, FILE * stream);
extern unsigned long int vbdecode(int * bytes, FILE * stream);

#endif
