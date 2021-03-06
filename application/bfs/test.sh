#!/bin/bash

COROS=64
GAPS="1 8"
FILENAME=graph_23_16
SOURCE=0

make clean

make

for gap in $GAPS; do
OUTPUT="bfs_gap${gap}.txt"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

for i in $(seq $COROS); do
	let "k=i*gap"
	echo run bfs with input $FILENAME, source $SOURCE, coros $k
	./bfs $FILENAME $SOURCE $k >> $OUTPUT
done

done

