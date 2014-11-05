#!/bin/bash

TOGETHERS="1 2 4 8 12 16"
REPEAT=1

#OUTPUT="omp_pttest.csv"
OUTPUT="omp_newpttest.csv"

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
		#echo run pttest with $i threads
		#RST=$RST","`./pttest $i $REPEAT | awk '{time=$3/1000000} {printf("%.2f", time);}'`
		echo run newpttest with $i togethers
		RST=$RST","`./newpttest $i $REPEAT | awk '{time=$3/1000000} {printf("%.2f", time);}'`
		sleep 10
	done
	echo $RST >> $OUTPUT

