#!/bin/bash

if [ -n "$1" ]; then
	MODE=$1
else
	MODE=1
fi

if [ -n "$2" ]; then
	LOCALITY=$2
else
	LOCALITY=3
fi

if [ -n "$3" ]; then
	ALINES=$3
else
	ALINES=0
fi

let "LOCALNUM=14+16*ALINES"

sed -i 's/MODE=\([0-9]\)/MODE='$MODE'/g' Makefile
sed -i 's/LOCALITY=\([0-9]\)/LOCALITY='$LOCALITY'/g' Makefile

sed -i 's/#define LOCAL_NUM [0-9]*/#define LOCAL_NUM '$LOCALNUM'/g' loadspttest.cpp

make clean

make

