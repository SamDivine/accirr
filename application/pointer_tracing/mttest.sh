#!/bin/bash

MODE=1
LOCALITY=2

COROS=64
PROCS="2 4 6 8 10 12 14"
GAP="1 8 64"

for mode in $MODE; do
	for locality in $LOCALITY; do
		for gap in $GAP; do
			OUTPUT1="mtpttest_arithmatic_hugepage_gap"${gap}"_m"$mode"l"$locality".csv"
			OUTPUT2="mtpttest_harmonic_hugepage_gap"${gap}"_m"$mode"l"$locality".csv"

			OUTPUTL="mtpttest_longest_hugepage_gap"${gap}"_m"$mode"l"$locality".csv"

			if [ -e $OUTPUT1 ]; then
				rm $OUTPUT1
			fi
			if [ -e $OUTPUT2 ]; then
				rm $OUTPUT2
			fi
			if [ -e $OUTPUTL ]; then
				rm $OUTPUTL
			fi
			RST=""

			for c in $(seq $COROS); do
				let "i=c*gap"
				RST=$RST","$i
			done

			echo $RST >> $OUTPUT1
			echo $RST >> $OUTPUT2
			echo $RST >> $OUTPUTL

			./gen.sh $mode $locality
			for c in $(seq $COROS); do
				let "k=c*gap"
				for i in $PROCS; do
					echo run mtpttest with $k coros $i procs mode $mode locality $locality gap $gap
					LD_PRELOAD=../../lib/libhugetlbfs.so HUGETLB_MORECORE=1g ./mtpttest $k $i 
					sleep 5
				done
			done

			for k in $PROCS; do
				RST=proc${k}
				for c in $(seq $COROS); do
					let "i=c*gap"
					RST=$RST","`cat mtpttest_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+$3/1000000} END{ time=time/'${k}'; printf("%.2f", time);}'`
				done
				echo $RST >> $OUTPUT1
			done

			for k in $PROCS; do
				RST=proc${k}
				for c in $(seq $COROS); do
					let "i=c*gap"
					RST=$RST","`cat mtpttest_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+1000000/$3} END{ time='${k}'/time; printf("%.2f", time);}'`
				done
				echo $RST >> $OUTPUT2
			done
			for k in $PROCS; do
				RST=proc${k}
				for c in $(seq $COROS); do
					let "i=c*gap"
					RST=$RST","`cat mtpttest_${i}_${k}_*.log | awk 'BEGIN {time=0;} {if ($3>time){time=$3;}} END {printf("%.2f", time);}'`
				done
				echo $RST >> $OUTPUTL
			done	
		done
	done
done

