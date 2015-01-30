#!/bin/bash

TOGETHERS="8 16 32 64 128 256 512 1024"
REPEAT=1

OUTPUT="omp_pttest.csv"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

make clean

make

for i in $TOGETHERS; do
	RST=$RST","$i
done

echo $RST >> $OUTPUT

RST="rst"
	for i in $TOGETHERS; do
		echo run pttest with $i threads
		RST=$RST","`./pttest $i $REPEAT | awk '{time=$3} {printf("%.2f", time);}'`
		sleep 5
	done
	echo $RST >> $OUTPUT

