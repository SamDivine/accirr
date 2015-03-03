#!/bin/bash

COROS=64
GAPS="1 8"
FILENAME=graph_22_16
SOURCE=0

for gap in $GAPS; do
OUTPUT="bfs_gap${gap}.txt"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

for i in $(seq $COROS); do
	let "k=i*gap"
	echo run bfs with input $FILENAME, source $SOURCE, coros $k
	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./bfs $FILENAME $SOURCE $k >> $OUTPUT
done

done

