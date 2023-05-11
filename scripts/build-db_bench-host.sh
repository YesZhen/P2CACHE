#!/bin/bash

cd share/rocksdb
make -j

chmod +x db_bench

# test it
./db_bench --version && echo "ok"