#!/bin/bash

THREADS="8 16 32 64 128 256 512 1024"
REPEAT=1

#OUTPUT="omp_pcmtest.txt"
OUTPUT="omp_newpcmtest.txt"
HUGEPAGE_FLAG='LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g'

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

make clean

make

for i in $THREADS; do
	echo run pcmtest with $i threads

#	echo $i threads normal >> $OUTPUT
#	./pcm_pttest $i $REPEAT >> $OUTPUT
	echo $i together normal >> $OUTPUT
	./newpcm_pttest $i $REPEAT >> $OUTPUT
#	echo $i threads hugepage >> $OUTPUT
#	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pcm_pttest $i $REPEAT >> $OUTPUT
	echo $i together hugepage >> $OUTPUT
	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./newpcm_pttest $i $REPEAT >> $OUTPUT
	sleep 10
done

