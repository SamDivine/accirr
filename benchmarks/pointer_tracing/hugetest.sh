#!/bin/bash

THREADS="1 2 4 8 12 16"
REPEAT=1

OUTPUT="omp_pttest_hugepage_172.csv"
#OUTPUT="omp_newpttest_hugepage_172.csv"

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
		RST=$RST","`LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=yes ./pttest $i $REPEAT | awk '{time=$3/1000000} {printf("%.2f", time);}'`
		#echo run newpttest with $i togethers
		#RST=$RST","`LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=yes ./newpttest $i $REPEAT | awk '{time=$3/1000000} {printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT

