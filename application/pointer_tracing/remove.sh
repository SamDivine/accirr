#!/bin/bash

COROS=64

GAP="1 8 64"

for i in $(seq $COROS); do
	for gap in $GAP; do
		let "k=i*gap"
		rm -f mtpttest_$k*.log
	done
done


