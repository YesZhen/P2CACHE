#!/bin/bash

mountpoint -q /mnt/data

if [ $? = 1 ]; then
    echo "/mnt/data/is not a mountpoint. exit!"
    exit 1
fi


./share/minio/minio server /mnt/data > share/minio_out.log 2>&1 &

echo "minio started"

# mcli alias set myminio http://127.0.0.1:9000 minioadmin minioadmin