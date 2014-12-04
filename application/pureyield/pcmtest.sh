#!/bin/bash

COROS="8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768"

OUTPUT="pureyield_with_prefetch_locality00.txt"
#OUTPUT="pureyield_without_prefetch.txt"

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

./gen.sh
for k in $COROS; do
	echo run pttest with $k coro
	echo $k normal >> $OUTPUT
	./pureyield $k >> $OUTPUT
	echo $k hugepage >> $OUTPUT
	LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pureyield $k >> $OUTPUT
	sleep 10
done

