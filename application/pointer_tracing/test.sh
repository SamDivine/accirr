#!/bin/bash

COROS=64
MODE="1"
LOCALITY="0 1 2 3"

#OUTPUTNORMAL="accirr_pttest_east.tk.csv"
#OUTPUTHUGEPAGE="accirr_pttest_hugepage_east.tk.csv"
OUTPUTHUGEPAGE="hugepage_4GB_gap64.txt"

#if [ -e $OUTPUTNORMAL ]; then
#	rm $OUTPUTNORMAL
#fi

if [ -e $OUTPUTHUGEPAGE ]; then
	rm $OUTPUTHUGEPAGE
fi

#for k in $(seq $COROS); do
#	let "i=k*8"
#	RST=$RST","$i
#done

#echo $RST >> $OUTPUTNORMAL
#echo $RST >> $OUTPUTHUGEPAGE

for i in $MODE; do
	for j in $LOCALITY; do
		RSTN="m"$i"l"$j
		RSTH="m"$i"l"$j
		./gen.sh $i $j
		echo $RSTH >> $OUTPUTHUGEPAGE
		for c in $(seq $COROS); do
			let "k=c*64"
#			echo run pttest with $k coro, mode $i, locality $j normal
#			RSTN=$RSTN","`./pttest $k | awk '{time=$3} { printf("%.2f", time);}'`
			echo run pttest with $k coro, mode $i, locality $j hugepage
			echo $k >> $OUTPUTHUGEPAGE
			LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./pttest $k >> $OUTPUTHUGEPAGE 
			sleep 5
		done
#		echo $RSTN >> $OUTPUTNORMAL
#		echo $RSTH >> $OUTPUTHUGEPAGE
	done
done

