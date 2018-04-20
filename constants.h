#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <limits.h>
#include <stdint.h>

#define SCUL static const uint32_t

    // Make sure this is even
    SCUL max_terms = 300000ul;                //- ULONG_MAX UINT_MAX USHRT_MAX
    SCUL max_docs = 200000ul;
    SCUL std_term_strlen = 80ul;
    SCUL max_term_strlen = 256ul;

    SCUL htable_capacity = 300000ul;          //- doc_tf, idmap, htable capacity

    SCUL FALSE = 0;
    SCUL TRUE = 1;

    SCUL SUCCESS = 0;
    SCUL FAILED = -1;

    SCUL EXIT_MISSING_TERM = 2;
    SCUL EXIT_BUG = 3;
    SCUL EXIT_EMPTY_QUERY = 4;


#define SCC static const char

    SCC index_path[] = "index.bin";           //- The postings lists
    SCC dictionary_path[] = "dictionary.bin";
    SCC map_path[] = "documentmap.bin";


#define bytes_offset_t off_t

#endif