#!/bin/bash

REPEAT=1
MODE="0 1"
LOCALITY="0 1 2 3"

#OUTPUTNORMAL="pcm_test_only_worker_prefetch3_normal.txt"
#OUTPUTHUGEPAGE="pcm_test_only_worker_prefetch3_hugepage.txt"
#OUTPUTNORMAL="pcm_test_output_without_worker_stack_prefetch_normal.txt"
#OUTPUTHUGEPAGE="pcm_test_output_without_worker_stack_prefetch_hugepage.txt"
OUTPUTNORMAL="pcm_test_worker_stack1_prefetch33_normal.txt"
OUTPUTHUGEPAGE="pcm_test_worker_stack1_prefetch33_hugepage.txt"

STARTTIME=`date`


if [ -e $OUTPUTNORMAL ]; then
	rm $OUTPUTNORMAL
fi
if [ -e $OUTPUTHUGEPAGE ]; then
	rm $OUTPUTHUGEPAGE
fi

echo $STARTTIME >> $OUTPUTNORMAL
echo $STARTTIME >> $OUTPUTHUGEPAGE

for i in $MODE; do
	for j in $LOCALITY; do
		./gen.sh $i $j
		for c in {1..32}; do
			let "k=c*8"
			echo run pttest with $k coro, mode $i, locality $j
			echo $k $i $j >> $OUTPUTNORMAL
			./pcm_pttest $k $REPEAT >> $OUTPUTNORMAL
			echo $k $i $j >> $OUTPUTHUGEPAGE
			LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pcm_pttest $k $REPEAT >> $OUTPUTHUGEPAGE
		sleep 2
		done
	done
done

ENDTIME=`date`

echo $ENDTIME >> $OUTPUTNORMAL
echo $ENDTIME >> $OUTPUTHUGEPAGE

