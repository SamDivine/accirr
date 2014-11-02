#!/bin/bash

CORO="1 2 4 8 12 16"
#CORO="1"
FILENAME=gen_graph/graph_22_16
SOURCE=0

OUTPUT=bfs.csv

if [ -e $OUTPUT ]; then
	rm $OUTPUT
fi

for i in $CORO; do
	RST=$RST","$i
done

echo $RST >> $OUTPUT

RST=coro

for i in $CORO; do
	echo run bfs with input $FILENAME, source $SOURCE, coros $i
	RST=$RST","`./bfs $FILENAME $SOURCE $i | awk '{printf("%.2f", $2);}'`
done
echo $RST >> $OUTPUT

