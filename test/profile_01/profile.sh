valgrind --tool=callgrind   ./aligner -t target.fa -i none -d 2 -q query.fa

# kcachegrind profile.callgrind
