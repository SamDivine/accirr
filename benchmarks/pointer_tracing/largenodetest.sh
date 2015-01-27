#!/bin/bash

THREADS=32
ALINES="0 1 2 3"

OUTPUT="omp_largenode_pttest.txt"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

for alines in $ALINES; do
	let "LOCALNUM=14+16*alines"
	sed -i 's/#define LOCAL_NUM [0-9]*/#define LOCAL_NUM '$LOCALNUM'/g' loadspttest.cpp
	make
	echo alines $alines >> $OUTPUT
	for i in $(seq $THREADS); do
		echo run loadspttest with $i threads $alines alines
		LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./loadspttest $i >> $OUTPUT
		sleep 5
	done
done

