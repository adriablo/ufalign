
valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes   ./aligner -t target.fa -i none -d 2 -q query.fa
