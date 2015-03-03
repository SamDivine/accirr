#!/bin/bash

COROS=64
GAP=64

for gap in $GAP; do
OUTPUT="pureyield_gap${gap}.txt"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

./gen.sh
for c in $(seq $COROS); do
	let "k=c*gap"
	echo run pttest with $k coro
	echo $k hugepage >> $OUTPUT
	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pureyield $k >> $OUTPUT
	sleep 2
done
done

