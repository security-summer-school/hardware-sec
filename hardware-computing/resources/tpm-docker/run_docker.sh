#!/bin/bash

DOCKER_IMG_NAME="sss-hardware-tpm"

if [[ $EUID !=0 ]]
then
	echo "Please run as root"
	exit
fi

if ! command -v docker &> /dev/null
then
	echo "Docker not installed; run \"sudo apt-get update && sudo apt-get install docker\" to install it"
	exit
fi

pushd ../../activities || exit

docker docker build -t "$DOCKER_IMG_NAME" -f ../resources/tpm-docker/Dockerfile .
docker run --privileged -it "$DOCKER_IMG_NAME"

popd
