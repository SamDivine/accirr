#!/bin/bash

COROS=64
MODE="1"
LOCALITY="2"

GAPS="1 8 64"
ALINES="0 1 2 3"

for gap in $GAPS; do
for alines in $ALINES; do

OUTPUTHUGEPAGE="loads_alines${alines}_gap${gap}.txt"

if [ -e $OUTPUTHUGEPAGE ]; then
	rm $OUTPUTHUGEPAGE
fi

for i in $MODE; do
	for j in $LOCALITY; do
		RSTH="m"$i"l"$j
		./genloads.sh $i $j $alines
		echo $RSTH >> $OUTPUTHUGEPAGE
		for c in $(seq $COROS); do
			let "k=c*gap"
			echo run pttest with $k coro, mode $i, locality $j gap $gap alines $alines
			echo $k >> $OUTPUTHUGEPAGE
			LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./loadspttest $k >> $OUTPUTHUGEPAGE 
			sleep 5
		done
	done
done

done
done
