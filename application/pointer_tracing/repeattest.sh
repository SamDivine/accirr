#!/bin/bash

COROS=64
MODE="1"
LOCALITY="2"

GAPS="1 8 64"
REPEATS="4 8 12 16"

for gap in $GAPS; do
for repeat in $REPEATS; do

OUTPUTHUGEPAGE="loads_4GB_repeat${repeat}_gap${gap}.txt"

if [ -e $OUTPUTHUGEPAGE ]; then
	rm $OUTPUTHUGEPAGE
fi

for i in $MODE; do
	for j in $LOCALITY; do
		RSTH="m"$i"l"$j
		./gen.sh $i $j
		echo $RSTH >> $OUTPUTHUGEPAGE
		for c in $(seq $COROS); do
			let "k=c*gap"
			echo run pttest with $k coro, mode $i, locality $j gap $gap repeat $repeat
			echo $k >> $OUTPUTHUGEPAGE
			LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pttest $k $repeat >> $OUTPUTHUGEPAGE 
			sleep 5
		done
	done
done

done
done
