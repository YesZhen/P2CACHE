#!/bin/bash

git submodule update --init --recursive

cd share/rocksdb
make -j

chmod +x db_bench