#!/bin/bash
export TTY

set -v

(
if [ -e "data/wsj.xml" ]; then
    cd bin
    ./indexer ../data/wsj.xml
fi
)

rlwrap ./readline.sh
