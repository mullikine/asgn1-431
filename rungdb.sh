#!/bin/bash
export TTY

# rsv
make && x -h -d -tmc "gdb ./query" -nto -e "GNU" -s "run < query.txt | tee /tmp/output-\$(vc branch).txt" -scc m -s1 -a

# tf-idf
# make && x -h -d -tmc "gdb ./query" -nto -e "GNU" -s "run -t < query.txt | tee /tmp/output-\$(vc branch).txt" -scc m -s1 -a