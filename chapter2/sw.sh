#!/bin/sh
while [ 1 ];
do
	for i in 0 1 2 3 4 5 6 7;
	do
		led=$(($i+61));
		sw=$(($i+69));
		cat /sys/class/gpio/gpio$sw/value > /sys/class/gpio/gpio$led/value;
	done;
	sleep 1;
done;
