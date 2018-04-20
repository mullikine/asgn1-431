INDEXER_TARGET=indexer
SEARCH_TARGET=search

CC    = gcc
#CC   = clang
COPTS = -W -Wall -ansi -pedantic -g -std=c99
LOPTS = -L. -Wl,-rpath=.

INDEXER_CFILES=index.c htable.c list.c mylib.c
INDEXER_OFILES=$(INDEXER_CFILES:.c=.o)

SEARCH_CFILES=search.c mylib.c cosc242-search.c comparators.c globals.c
SEARCH_OFILES=$(SEARCH_CFILES:.c=.o)

%.o: %.c
	$(CC) -c $< -o $@

default: cleanall indexer search
	mkdir -p bin
	mv -f indexer bin/
	mv -f search bin/

test:
	bin/indexer

indexer: $(INDEXER_OFILES)
	$(CC) -o $(INDEXER_TARGET) $(INDEXER_OFILES) $(LOPTS)

search: $(SEARCH_OFILES)
	$(CC) -o $(SEARCH_TARGET) $(SEARCH_OFILES) $(LOPTS)

clean: cleanall

cleanall:
	rm -f $(INDEXER_OFILES) $(SEARCH_OFILES)
	rm -f build query
