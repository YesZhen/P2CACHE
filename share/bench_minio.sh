#!/bin/bash

./share/minio/warp mixed --host=127.0.0.1:9000 --access-key=minioadmin --secret-key=minioadmin --duration=10s --obj.size=256 --concurrent=8

# sleep 5

# ./share/minio/warp get --host=127.0.0.1:9000 --access-key=minioadmin --secret-key=minioadmin --duration=30s --obj.randsize --concurrent=20

# sleep 5

# ./share/minio/warp mixed --host=127.0.0.1:9000 --access-key=minioadmin --secret-key=minioadmin --duration=30s --obj.randsize --concurrent=20