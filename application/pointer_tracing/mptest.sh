#!/bin/bash

MODE=1
LOCALITY=3

COROS=64
PROCS=2
GAP=64

OUTPUT1="mppttest_arithmatic_hugepage_gap"${GAP}"_m"$MODE"l"$LOCALITY".csv"
OUTPUT2="mppttest_harmonic_hugepage_gap"${GAP}"_m"$MODE"l"$LOCALITY".csv"

if [ -e $OUTPUT1 ]; then
	rm $OUTPUT1
fi
if [ -e $OUTPUT2 ]; then
	rm $OUTPUT2
fi

for c in $(seq $COROS); do
	let "i=c*GAP"
	RST=$RST","$i
done

echo $RST >> $OUTPUT1
echo $RST >> $OUTPUT2

./gen.sh $MODE $LOCALITY
for c in $(seq $COROS); do
	let "k=c*GAP"
#	for i in $(seq $PROCS); do
	i=$PROCS
		echo run mppttest with $k coros $i procs
		LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./mppttest $k $i 
		sleep 5
#	done
done

#for k in $(seq $PROCS); do
	k=$PROCS
	RST=proc${k}
	for c in $(seq $COROS); do
		let "i=c*GAP"
		RST=$RST","`cat mppttest_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+$3/1000000} END{ time=time/'${k}'; printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT1
#done

#for k in $(seq $PROCS); do
	k=$PROCS
	RST=proc${k}
	for c in $(seq $COROS); do
		let "i=c*GAP"
		RST=$RST","`cat mppttest_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+1000000/$3} END{ time='${k}'/time; printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT2
#done

