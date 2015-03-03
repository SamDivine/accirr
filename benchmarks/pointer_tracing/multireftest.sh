#!/bin/bash

BOUND=11

TMPFILE="tmp.txt"
OUTPUT="hugepage_remote_multiref.csv"

if [ -e $TMPFILE ]; then
	rm $TMPFILE
fi

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

for ((i=0;i<=$BOUND;i++)); do
	let "k=2**i"
	./genmultiref.py $k
	make
	echo run with $k multiref
	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./multiref_source >> $TMPFILE
	sleep 5
done

for ((i=0;i<=$BOUND;i++)); do
	let "k=2**i"
	RST=$RST","$k
done
echo $RST >> $OUTPUT
RST="rst"`awk 'BEGIN {str=""} {str=sprintf("%s,%.2f", str, $3)} END {printf("%s", str)}' $TMPFILE`

echo $RST >> $OUTPUT

