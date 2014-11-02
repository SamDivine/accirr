#!/bin/bash

CORO="1 2 4 8 12 16"
PROC="1 2 3 4"
#CORO="1"
#PROC="4"

OUTPUT1=mpwordcount_arithmatic.csv
OUTPUT2=mpwordcount_harmonic.csv

if [ -e $OUTPUT1 ]; then
	rm $OUTPUT1
fi
if [ -e $OUTPUT2 ]; then
	rm $OUTPUT2
fi

for i in $CORO; do
	RST=$RST","$i
done

echo $RST >> $OUTPUT1
echo $RST >> $OUTPUT2
for k in $PROC; do
	for i in $CORO; do
		echo run mpwordcount with coros $i, procs $k
		./mpwordcount $i $k
		sleep 5
	done
done
sleep 30
for k in $PROC; do
	RST=${k}
	for i in $CORO; do
		RST=$RST","`cat mpwordcount_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+$3} END{time=time/'${k}';printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT1
done
for k in $PROC; do
	RST=${k}
	for i in $CORO; do
		RST=$RST","`cat mpwordcount_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+1/$3} END{time='${k}'/time;printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT2
done

