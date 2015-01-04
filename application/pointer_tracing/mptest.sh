#!/bin/bash

COROS="8 16 32 64 128"
PROCS="1 2 3 4"
REPEAT=1
MODE="1"
LOCALITY="1 2"

OUTPUT1="accirr_mppttest_arithmatic_east.tk.csv"
OUTPUT2="accirr_mppttest_harmonic_east.tk.csv"

if [ -e $OUTPUT1 ]; then
	rm $OUTPUT1
fi
if [ -e $OUTPUT2 ]; then
	rm $OUTPUT2
fi

for i in $COROS; do
	RST=$RST","$i
done

echo $RST >> $OUTPUT1
echo $RST >> $OUTPUT2

for m in $MODE; do
for l in $LOCALITY; do
./gen.sh $m $l
for k in $COROS; do
	for i in $PROCS; do
		echo run mppttest with $k coros $i procs
		./mppttest $k $i 
		sleep 5
	done
done
done
done

for k in $PROCS; do
	RST=proc${k}
	for i in $COROS; do
		RST=$RST","`cat mppttest_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+$3/1000000} END{ time=time/'${k}'; printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT1
done

for k in $PROCS; do
	RST=proc${k}
	for i in $COROS; do
		RST=$RST","`cat mppttest_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+1000000/$3} END{ time='${k}'/time; printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT2
done

rm mppttest*.log
