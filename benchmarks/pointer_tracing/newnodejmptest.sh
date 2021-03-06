#!/bin/bash

JMPDIS=128

OUTPUT="omp_newnodejmp_pttest.txt"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

make

if [ -e jmppttest ]; then
	echo run newnodejmppttest with max distance $JMPDIS
	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./newnodejmppttest $JMPDIS >> $OUTPUT
else
	echo perhaps procedure make is failed
fi

