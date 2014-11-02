#!/bin/bash

if [ -n "$1" ]; then
	MODE=$1
else
	MODE=0
fi

if [ -n "$2" ]; then
	LOCALITY=$2
else
	LOCALITY=3
fi

sed -i 's/MODE=\([0-9]\)/MODE='$MODE'/g' Makefile
sed -i 's/LOCALITY=\([0-9]\)/LOCALITY='$LOCALITY'/g' Makefile

make clean

make

