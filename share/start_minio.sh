#!/bin/bash

mountpoint -q /mnt/data

if [ $? = 1 ]; then
    echo "/mnt/data/is not a mountpoint. exit!"
    exit 1
fi

rm -rf /root/.mc /root/.minio

./share/minio/minio server /mnt/data > share/tmp/minio_out.log 2>&1 &

sleep 5s

echo "minio started"

./share/minio/mc alias set myminio http://127.0.0.1:9000 minioadmin minioadmin