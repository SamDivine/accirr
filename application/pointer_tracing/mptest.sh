#!/bin/bash

MODE="1"
LOCALITY="2"

COROS=32
PROCS=3

OUTPUT1="accirr_mppttest_arithmatic_normal_east.tk.csv"
OUTPUT2="accirr_mppttest_harmonic_normal_east.tk.csv"
#OUTPUT1="accirr_mppttest_arithmatic_hugepage_east.tk.csv"
#OUTPUT2="accirr_mppttest_harmonic_hugepage_east.tk.csv"

if [ -e $OUTPUT1 ]; then
	rm $OUTPUT1
fi
if [ -e $OUTPUT2 ]; then
	rm $OUTPUT2
fi

for c in $(seq $COROS); do
	let "i=c*8"
	RST=$RST","$i
done

echo $RST >> $OUTPUT1
echo $RST >> $OUTPUT2

./gen.sh $MODE $LOCALITY
for c in $(seq $COROS); do
	let "k=c*8"
	for i in $(seq $PROCS); do
		echo run mppttest with $k coros $i procs
		./mppttest $k $i 
#		LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./mppttest $k $i 
		sleep 5
	done
done

for k in $(seq $PROCS); do
	RST=proc${k}
	for c in $(seq $COROS); do
		let "i=c*8"
		RST=$RST","`cat mppttest_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+$3/1000000} END{ time=time/'${k}'; printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT1
done

for k in $(seq $PROCS); do
	RST=proc${k}
	for c in $(seq $COROS); do
		let "i=c*8"
		RST=$RST","`cat mppttest_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+1000000/$3} END{ time='${k}'/time; printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT2
done

#rm mppttest*.log
