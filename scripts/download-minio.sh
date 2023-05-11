#!/bin/bash

mkdir -p share/minio

rm -f share/minio/minio
wget https://dl.min.io/server/minio/release/linux-amd64/minio -P share/minio/
chmod +x minio

rm -f share/minio/warp_Darwin_x86_64.tar.gz
wget https://github.com/minio/warp/releases/download/v0.6.8/warp_Darwin_x86_64.tar.gz -P share/minio/
tar -xf share/minio/warp_Darwin_x86_64.tar.gz -C share/minio
chmod +x share/minio/warp