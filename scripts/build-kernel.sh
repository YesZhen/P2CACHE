#!/bin/bash

cores=$1

if [ -z "$cores" ]
then
    echo "Usage: build_kernel.sh cores"
    exit
fi

cd share/linux-5.4.225
cp ../../P2CACHE_CONFIG .config 
make -j $cores
