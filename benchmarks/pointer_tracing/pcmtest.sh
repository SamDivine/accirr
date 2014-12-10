#!/bin/bash

THREADS="8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768"
REPEAT=1

#OUTPUTNORMAL="omp_pcmtest_normal.txt"
#OUTPUTHUGEPAGE="omp_pcmtest_hugepage.txt"
OUTPUTNORMAL="pcm_multiref_compiler_normal.txt"
OUTPUTHUGEPAGE="pcm_multiref_compiler_hugepage.txt"

if [ -e $OUTPUTNORMAL ]; then
	rm $OUTPUTNORMAL
fi
if [ -e $OUTPUTHUGEPAGE ]; then
	rm $OUTPUTHUGEPAGE
fi

make clean

make

for i in $THREADS; do
	echo run with $i concurrency

#	echo $i threads normal >> $OUTPUTNORMAL
#	./pcm_pttest $i $REPEAT >> $OUTPUTNORMAL
	echo $i together normal >> $OUTPUTNORMAL
	./pcm_multiref_compiler $i $REPEAT >> $OUTPUTNORMAL
#	echo $i threads hugepage >> $OUTPUTHUGEPAGE
#	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pcm_pttest $i $REPEAT >> $OUTPUTHUGEPAGE
	echo $i together hugepage >> $OUTPUTHUGEPAGE
	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pcm_multiref_compiler $i $REPEAT >> $OUTPUTHUGEPAGE
	sleep 10
done

