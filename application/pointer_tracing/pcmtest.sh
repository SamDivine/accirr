#!/bin/bash

COROS="8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768"
REPEAT=1
MODE="0 1"
LOCALITY="0 1 2 3"

OUTPUTNORMAL="pcm_test_only_worker_prefetch1_normal.txt"
OUTPUTHUGEPAGE="pcm_test_only_worker_prefetch1_hugepage.txt"

if [ -e $OUTPUTNORMAL ]; then
	rm $OUTPUTNORMAL
fi
if [ -e $OUTPUTHUGEPAGE ]; then
	rm $OUTPUTHUGEPAGE
fi

for i in $MODE; do
	for j in $LOCALITY; do
		./gen.sh $i $j
		for k in $COROS; do
			echo run pttest with $k coro, mode $i, locality $j
			echo $k $i $j >> $OUTPUTNORMAL
			./pcm_pttest $k $REPEAT >> $OUTPUTNORMAL
			echo $k $i $j >> $OUTPUTHUGEPAGE
			LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pcm_pttest $k $REPEAT >> $OUTPUTHUGEPAGE
		sleep 10
		done
	done
done

