#!/bin/sh
NUM=$1
ON=0
for i in 0 1 2 3 4 5 6 7;
do
	ON=$(($NUM % 2));
	led=$(($i+61));
	echo $ON > /sys/class/gpio/gpio$led/value;
	echo $ON
	NUM=$(($NUM / 2));
done;
