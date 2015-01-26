#!/bin/bash

THREADS=32

OUTPUT="multithread_samesocket.csv"
TMPFILE="tmp.txt"
if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi
if [ -e $TMPFILE ]; then
	rm $TMPFILE
fi

make clean

make

for t in $(seq $THREADS); do
	echo run pttest with $t threads
	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pttest $t >> $TMPFILE
	sleep 5
done

for t in $(seq $THREADS); do
	RST=$RST","$t
done

echo $RST >> $OUTPUT

RST="multithread"`awk 'BEGIN {str=""} {str=sprintf("%s,%.2f", str, $3)} END{print str}' $TMPFILE`

echo $RST >> $OUTPUT

