#!/bin/bash

LISTS="128 256 512 1024 2048"
MODE="1"
LOCALITY="0 1"

OUTPUT="accirr_pointer_tracing.csv"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

for i in $LISTS; do
	RST=$RST","$i
done

echo $RST >> $OUTPUT

for i in $MODE; do
	for j in $LOCALITY; do
		RST="m"$i"l"$j
		./gen.sh $i $j
		for k in $LISTS; do
			echo run pointer_tracing with $k lists, mode $i, locality $j
			RST=$RST","`./pointer_tracing $k | awk '{time=$3/1000000} { printf("%.2f", time);}'`
		done
		echo $RST >> $OUTPUT
	done
done

