#!/bin/bash

THREADS=64
REPEAT="1 2 4 8 12 16"

OUTPUT="omp_repeat_pttest.txt"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

make clean

make

for i in $(seq $THREADS); do
	for repeat in $REPEAT; do
		echo run repeatpttest with $i threads $repeat repeat
		./repeatpttest $i $repeat >> $OUTPUT
		sleep 5
	done
done

