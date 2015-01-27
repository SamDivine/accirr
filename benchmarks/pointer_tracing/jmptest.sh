#!/bin/bash

JMPDIS=16

OUTPUT="omp_jmp_pttest.txt"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

make

if [ -e jmppttest ]; then
	echo run jmppttest with max distance $JMPDIS
	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./repeatpttest $JMPDIS >> $OUTPUT
else
	echo perhaps procedure make is failed
fi

