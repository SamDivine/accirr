#!/bin/bash

THREADS="8 16 32 64 128 256 512 1024"
REPEAT=1

OUTPUT="omp_pttest_hugepage.csv"
#OUTPUT="omp_newpttest_hugepage.csv"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

make clean

make

for i in $THREADS; do
	RST=$RST","$i
done

echo $RST >> $OUTPUT

RST="rst"
	for i in $THREADS; do
		echo run pttest with $i threads
		RST=$RST","`LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pttest $i $REPEAT | awk '{time=$3} {printf("%.2f", time);}'`
#echo run newpttest with $i togethers
#RST=$RST","`LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./newpttest $i $REPEAT | awk '{time=$3} {printf("%.2f", time);}'`
		sleep 10
	done
	echo $RST >> $OUTPUT

