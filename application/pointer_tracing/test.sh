#!/bin/bash

COROS="8 16 32 64 128 256 512 1024"
REPEAT=1
MODE="0 1"
LOCALITY="0 1 2 3"

OUTPUT="accirr_pttest.csv"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

for i in $COROS; do
	RST=$RST","$i
done

echo $RST >> $OUTPUT

for i in $MODE; do
	for j in $LOCALITY; do
		RST="m"$i"l"$j
		./gen.sh $i $j
		for k in $COROS; do
			echo run pttest with $k coro, mode $i, locality $j
			RST=$RST","`./pttest $k $REPEAT | awk '{time=$3} { printf("%.2f", time);}'`
			sleep 10
		done
		echo $RST >> $OUTPUT
	done
done

