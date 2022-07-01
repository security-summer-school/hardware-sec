FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update -y && apt-get install -y tpm2-tools

COPY ./tpm-aes/ /root/tpm-aes
COPY ./tpm-rsa/ /root/tpm-rsa
COPY ./tpm-seal/ /root/tpm-seal
