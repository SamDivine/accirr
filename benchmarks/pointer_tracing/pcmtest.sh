#!/bin/bash

THREADS="8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768"
REPEAT=1

#OUTPUTNORMAL="omp_pcmtest_normal.txt"
#OUTPUTHUGEPAGE="omp_pcmtest_hugepage.txt"
OUTPUTNORMAL="omp_newpcmtest_normal.txt"
OUTPUTHUGEPAGE="omp_newpcmtest_hugepage.txt"

if [ -e $OUTPUTNORMAL ]; then
	rm $OUTPUTNORMAL
fi
if [ -e $OUTPUTHUGEPAGE ]; then
	rm $OUTPUTHUGEPAGE
fi

make clean

make

for i in $THREADS; do
	echo run pcmtest with $i threads

#	echo $i threads normal >> $OUTPUTNORMAL
#	./pcm_pttest $i $REPEAT >> $OUTPUTNORMAL
	echo $i together normal >> $OUTPUTNORMAL
	./newpcm_pttest $i $REPEAT >> $OUTPUTNORMAL
#	echo $i threads hugepage >> $OUTPUTHUGEPAGE
#	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pcm_pttest $i $REPEAT >> $OUTPUTHUGEPAGE
	echo $i together hugepage >> $OUTPUTHUGEPAGE
	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./newpcm_pttest $i $REPEAT >> $OUTPUTHUGEPAGE
	sleep 10
done

