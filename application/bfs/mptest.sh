#!/bin/bash

CORO="1 2 4 8 12 16"
PROC="1 2 3 4"
#CORO="1"
#PROC="4"
FILENAME=gen_graph/graph_22_16
SOURCE=0

OUTPUT1=mpbfs_arithmatic.csv
OUTPUT2=mpbfs_harmonic.csv

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
		echo run mpbfs with input $FILENAME, source $SOURCE, coros $i, procs $k
		./mpbfs $FILENAME $SOURCE $i $k
	done
	sleep 10
done
sleep 30
for k in $PROC; do
	RST=${k}
	for i in $CORO; do
		RST=$RST","`cat mpbfs_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+$2} END{time=time/'${k}';printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT1
done
for k in $PROC; do
	RST=${k}
	for i in $CORO; do
		RST=$RST","`cat mpbfs_${i}_${k}_*.log | awk 'BEGIN{time=0;} {time=time+1/$2} END{time='${k}'/time;printf("%.2f", time);}'`
	done
	echo $RST >> $OUTPUT2
done

