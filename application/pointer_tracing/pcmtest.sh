#!/bin/bash

MODE="1"
LOCALITY="0 1 2 3"
COROS=64
GAP="1 8 64"

for gap in $GAP; do
OUTPUTHUGEPAGE="pcm_test_gap${gap}_hugepage.txt"

STARTTIME=`date`

if [ -e $OUTPUTHUGEPAGE ]; then
	rm $OUTPUTHUGEPAGE
fi

#echo $STARTTIME >> $OUTPUTHUGEPAGE

for i in $MODE; do
	for j in $LOCALITY; do
		./gen.sh $i $j
		for c in $(seq $COROS); do
			let "k=c*gap"
			echo run pcm_pttest with $k coros, mode $i locality $j
			echo coros $k mode $i locality $j >> $OUTPUTHUGEPAGE
			LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pcm_pttest $k >> $OUTPUTHUGEPAGE
		sleep 5
		done
	done
done

ENDTIME=`date`

#echo $ENDTIME >> $OUTPUTHUGEPAGE

done

