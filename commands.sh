export REPEAT_COMMAND=y

make && echo -n "year" | ./query

make && x -h -d -tmc "gdb ./query" -nto -e "GNU" -s "run < query.txt" -scc m -s1 -a