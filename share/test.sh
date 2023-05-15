#!/bin/bash

gcc share/test.c -o share/test

mount -t p2cache /dev/pmem0 /mnt/data

./share/test