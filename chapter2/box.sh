#!/bin/sh
X0=$1
Y0=$2
X1=$3
Y1=$4
C=$5

X=$X0
while [ $X -ne $X1 ];
do
	./dot.sh $X $Y0 $C
	./dot.sh $X $Y1 $C
	X=$(($X+1));
done

Y=$Y0
while [ $Y -ne $Y1 ];
do
	./dot.sh $X0 $Y $C
	./dot.sh $X1 $Y $C
	Y=$(($Y+1));
done
