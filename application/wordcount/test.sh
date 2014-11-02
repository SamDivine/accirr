#!/bin/bash

CORO="1 2 4 8 12 16"
#CORO="1"

OUTPUT=wc.csv

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

for i in $CORO; do
	RST=$RST","$i
done

echo $RST >> $OUTPUT

RST=coro

for i in $CORO; do
	echo run wordcount with coros $i
	RST=$RST","`./wordcount $i | awk '{printf("%.2f", $3);}'`
done
echo $RST >> $OUTPUT
