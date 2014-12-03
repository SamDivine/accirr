#!/bin/bash

COROS="8 16 32 64 128 256 512 1024"
REPEAT=1
MODE="0 1"
LOCALITY="0 1 2 3"

OUTPUT="pcm_test_output_locality22.txt"

HUGEFLAG="LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

echo Worker and Stack Prefetch with locality 2, 2 >> $OUTPUT

for i in $MODE; do
	for j in $LOCALITY; do
		./gen.sh $i $j
		for k in $COROS; do
			echo run pttest with $k coro, mode $i, locality $j
			echo $k $i $j normal >> $OUTPUT
			./pcm_pttest $k $REPEAT >> $OUTPUT
			echo $k $i $j hugepage >> $OUTPUT
			LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pcm_pttest $k $REPEAT >> $OUTPUT
		sleep 10
		done
	done
done

