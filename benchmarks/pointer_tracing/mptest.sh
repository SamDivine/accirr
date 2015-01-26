#!/bin/bash

PROCS=14

ALLOC="diff"

OUTPUT1="multiproc_hugepage_arithmatic_${ALLOC}.csv"
OUTPUT2="multiproc_hugepage_harmonic_${ALLOC}.csv"

if [ -e $OUTPUT1 ]; then
	rm $OUTPUT1
fi

if [ -e $OUTPUT2 ]; then
	rm $OUTPUT2
fi

make clean

make

for p in $(seq $PROCS); do
	echo run mppttest with $p procs
	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./mppttest $p
	sleep 5
done

for p in $(seq $PROCS); do
	RST=$RST","$p
done

echo $RST >> $OUTPUT1
echo $RST >> $OUTPUT2

RST="arithmatic"
for p in $(seq $PROCS); do
	RST=$RST","`cat multiproc_${p}_*.log | awk 'BEGIN{time=0;} {time=time+$3} END{time=time/'${p}'; printf("%.2f", time);}'`
done
echo $RST >> $OUTPUT1

RST="harmonic"
for p in $(seq $PROCS); do
	RST=$RST","`cat multiproc_${p}_*.log | awk 'BEGIN{time=0;} {time=time+1/$3} END{time='${p}'/time; printf("%.2f", time);}'`
done
echo $RST >> $OUTPUT2

