#!/bin/bash

COROS=64
MODE="1"
LOCALITY="2"
GAP="1 8 64"

for gap in $GAP; do

OUTPUTHUGEPAGE="hugepage_newnode_onlydata_gap${gap}.txt"

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
			echo run newnodepttest with $k coro, mode $i, locality $j gap $gap
			echo $k >> $OUTPUTHUGEPAGE
			LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./newnodepttest $k >> $OUTPUTHUGEPAGE 
			sleep 5
		done
	done
done
done

