#!/bin/bash

AES=$(grep -o "aes" /proc/cpuinfo | uniq)
SSE2=$(grep -o "sse2" /proc/cpuinfo | uniq)

if [[ $AES == "" ]] || [[ $SSE2 == "" ]]
then
	echo "AES or SSE2 not found; must emulate"
	if ! command -v qemu-x86_64 &> /dev/null
	then
		echo "qemu-user not installed; exiting"
		exit
	fi
	qemu-x86_64 -cpu max ./aes_in_c
else
	./aes_in_c
fi
