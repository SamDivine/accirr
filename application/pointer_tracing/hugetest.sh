#!/bin/bash

COROS="1 2 4 8 12 16"
REPEAT=1
MODE="0 1"
LOCALITY="0 1 2 3"

OUTPUT="accirr_pttest_hugepage.csv"

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
			RST=$RST","`LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pttest $k $REPEAT | awk '{time=$3/1000000} { printf("%.2f", time);}'`
		sleep 10
		done
		echo $RST >> $OUTPUT
	done
done

