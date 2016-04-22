#!/bin/sh
X=$1
Y=$2
C=$3
ADDR=427819008
ADDR=$(($ADDR+$X*4+$Y*5120))
#printf "devmem 0x%x 32 $C\\n" $ADDR
devmem $ADDR 32 $C
