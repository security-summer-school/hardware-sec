#!/bin/bash

RDRAND=$(grep -o "rdrand" /proc/cpuinfo | uniq)
RDSEED=$(grep -o "rdseed" /proc/cpuinfo | uniq)

if [[ $RDRAND == "" ]] || [[ $RDSEED == "" ]]
then
	echo "RDRAND or RDSEED not found; must emulate"
	if ! command -v qemu-x86_64 &> /dev/null
	then
		echo "qemu-user not installed; exiting"
		exit
	fi
	qemu-x86_64 -cpu max ./main
else
	./main
fi
