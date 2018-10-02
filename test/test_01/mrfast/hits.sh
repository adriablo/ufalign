# ignore reverse strand (flags=16)
grep -v -e '\s16\s' $1 | cut -f 1,3-4 | sort | uniq
