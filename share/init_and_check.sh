#!/bin/bash

mkdir -p share/tmp share/output

lsmod | grep nova > /dev/null
if [ $? -ne 0 ]; then
    insmod share/nova/nova.ko
    if [ $? -ne 0 ]; then
        echo -e "\e[31m \u2717 Fail to insert kernel module nova! \e[0m"
        echo -e "\e[33m ! Please run ./scripts/build-nova-host to build nova kernel module on host! \e[0m"
    else
        echo -e "\e[32m \u2714 Kernel module nova loaded successfully! \e[0m"
    fi
else
    echo -e "\e[32m \u2714 Kernel module nova already loaded ! \e[0m"
fi

lsmod | grep p2cache > /dev/null
if [ $? -ne 0 ]; then
    insmod share/p2cache/p2cache.ko
    if [ $? -ne 0 ]; then
        echo -e "\e[31m \u2717 Fail to insert kernel module p2cache! \e[0m"
        echo -e "\e[33m ! Please run ./scripts/build-p2cache-host to build p2cache kernel module on host! \e[0m"
    else
        echo -e "\e[32m \u2714 Kernel module p2cache loaded successfully! \e[0m"
    fi
else
    echo -e "\e[32m \u2714 Kernel module 'p2cache' already loaded ! \e[0m"
fi

./share/fxmark/bin/run-fxmark.py -h > /dev/null
if [ $? -ne 0 ]; then
    echo -e "\e[31m \u2717 Fail to run './share/fxmark/bin/run-fxmark.py -h' ! \e[0m"
    echo -e "\e[33m ! Please run './scripts/dependencies' to install necessary dependencies! \e[0m"
else
    echo -e "\e[32m \u2714 Run './share/fxmark/bin/run-fxmark.py -h' successfully! \e[0m"
fi

./share/fxmark/bin/fxmark -h > /dev/null 2>&1
if [ $? -ne 99 ]; then
    echo -e "\e[31m \u2717 Fail to run './share/fxmark/bin/fxmark -h' ! \e[0m"
    echo -e "\e[33m ! Please run './scripts/build-fxmark.sh' to to build fxmark! \e[0m"
else
    echo -e "\e[32m \u2714 Run './share/fxmark/bin/fxmark -h' successfully! \e[0m"
fi

./share/filebench/filebench -h > /dev/null
if [ $? -ne 0 ]; then
    echo -e "\e[31m \u2717 Fail to run './share/filebench/filebench -h' ! \e[0m"
    echo -e "\e[33m ! Please run './scripts/build-filebench.sh' to to build filebench! \e[0m"
else
    echo -e "\e[32m \u2714 Run './share/filebench/filebench -h' successfully! \e[0m"
fi

./share/rocksdb/db_bench --version > /dev/null
if [ $? -ne 0 ]; then
    echo -e "\e[31m \u2717 Fail to run './share/rocksdb/db_bench --version' ! \e[0m"
    echo -e "\e[33m ! Please run './scripts/build-rocksdb.sh' to to build rocksdb! \e[0m"
else
    echo -e "\e[32m \u2714 Run './share/rocksdb/db_bench --version' successfully! \e[0m"
fi

./share/minio/minio -h > /dev/null
if [ $? -ne 0 ]; then
    echo -e "\e[31m \u2717 Fail to run './share/minio/minio -h' ! \e[0m"
    echo -e "\e[33m ! Please run './scripts/download-minio.sh' to download minio! \e[0m"
else
    echo -e "\e[32m \u2714 Run './share/minio/minio -h' successfully! \e[0m"
fi

./share/minio/warp -h > /dev/null
if [ $? -ne 0 ]; then
    echo -e "\e[31m \u2717 Fail to run './share/minio/warp -h' ! \e[0m"
    echo -e "\e[33m ! Please run './scripts/download-minio.sh' to download warp bench tool! \e[0m"
else
    echo -e "\e[32m \u2714 Run './share/minio/warp -h' successfully! \e[0m"
fi

./share/minio/mc -v > /dev/null
if [ $? -ne 0 ]; then
    echo -e "\e[31m \u2717 Fail to run './share/minio/mc -v' ! \e[0m"
    echo -e "\e[33m ! Please run './scripts/download-minio.sh' to download minio client! \e[0m"
else
    echo -e "\e[32m \u2714 Run './share/minio/mc -v' successfully! \e[0m"
fi