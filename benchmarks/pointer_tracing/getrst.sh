#!/bin/bash

THREADS="1 2 4"
LISTS="128 256 512 1024 2048"
#THREADS="1 2"
#LISTS="4 8 16"

OUTPUT="omp_pointer_tracing_174.csv"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

make clean

make

for i in $LISTS; do
	RST=$RST","$i
done

echo $RST >> $OUTPUT

for i in $THREADS; do
	RST="omp"$i
	for j in $LISTS; do
		echo run pointer_tracing with $i threads, $j lists
		RST=$RST","`./pointer_tracing $i $j | awk '{time=$3/1000000} {printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT
done

