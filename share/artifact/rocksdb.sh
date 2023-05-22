#!/bin/bash

ulimit -n 1048576

function prepare_storage() {
    # mountpoint -q /mnt/data
    # if [ $? == 1 ]; then
    #     umount /mnt/data
    # fi

    if mountpoint -q /mnt/data
    then
        umount /mnt/data
    fi

    storage_mode=$1

    
    case $storage_mode in
    0) mount -t tmpfs -o size=160G,mpol=bind:0 tmpfs /mnt/data && echo "[media]:[mem],[fs]:[tmpfs]" | tee -a share/tmp/rocksdb.log # mem tmpfs
    ;;
    1) mkfs.ext4 -F /dev/sdb && mount -t ext4 /dev/sdb /mnt/data && echo "[media]:[ssd],[fs]:[ext4]" | tee -a share/tmp/rocksdb.log # ssd ext4
    ;;
    2) mkfs.ext4 -F /dev/sdb && mount -t ext4 -o data=journal /dev/sdb /mnt/data && echo "[media]:[ssd],[fs]:[ext4_dj]" | tee -a share/tmp/rocksdb.log # ssd ext4_dj (ext4 data=journal)
    ;;
    3) mkfs.xfs -m reflink=0 -f /dev/sdb && mount -t xfs /dev/sdb /mnt/data && echo "[media]:[ssd],[fs]:[xfs]" | tee -a share/tmp/rocksdb.log # ssd xfs
    ;;
    4) mkfs.ext4 -F /dev/pmem0 && mount -t ext4 -o dax /dev/pmem0 /mnt/data && echo "[media]:[pmem],[fs]:[ext4_dax]" | tee -a share/tmp/rocksdb.log # pmem ext4_dax
    ;;
    5) mkfs.xfs -m reflink=0 -f /dev/pmem0 && mount -t xfs -o dax /dev/pmem0 /mnt/data && echo "[media]:[pmem],[fs]:[xfs_dax]" | tee -a share/tmp/rocksdb.log # pmem xfs_dax 
    ;;
    6) mount -t NOVA -o init,dax,data_cow /dev/pmem0 /mnt/data && echo "[media]:[pmem],[fs]:[nova]" | tee -a share/tmp/rocksdb.log # pmem NOVA 
    ;;
    7) mount -t p2cache /dev/pmem0 /mnt/data && echo "[media]:[pmem],[fs]:[p2cache]" | tee -a share/tmp/rocksdb.log # pmem p2cache
    ;;
    esac
}


rm -f share/tmp/rocksdb.log

# the amount in paper
# insert_num=1000000
# read_num=10000000

# to save time, we can reduce amount
insert_num=100000
read_num=1000000

for((storage_mode=0;storage_mode<=7;storage_mode++));
do
    echo "-----------start-----------" | tee -a share/tmp/rocksdb.log
    echo "[bench_name]:[insert_100B]" | tee -a share/tmp/rocksdb.log
    prepare_storage $storage_mode
    result=$(numactl -C 0-7 ./share/rocksdb/db_bench --benchmarks="fillrandom" --wal_dir=/mnt/data --db=/mnt/data --disable_auto_compactions --disable_wal=false --sync --key_size=20 --value_size=100 --memtablerep=skip_list --num=$insert_num --threads=1)
    ops=$(echo $result | grep -o "micros/op .* ops/sec" | tr -dc '[. [:digit:]]' | xargs)
    echo "[ops]:[$ops]" | tee -a share/tmp/rocksdb.log
    umount /mnt/data || /bin/true
    echo "---------------------------" | tee -a share/tmp/rocksdb.log
done

for((storage_mode=0;storage_mode<=7;storage_mode++));
do
    echo "-----------start-----------" | tee -a share/tmp/rocksdb.log
    echo "[bench_name]:[read_100B]" | tee -a share/tmp/rocksdb.log
    prepare_storage $storage_mode
    numactl -C 0-7 ./share/rocksdb/db_bench --benchmarks="fillrandom,compact" --wal_dir=/mnt/data --db=/mnt/data --disable_wal=false --key_size=20 --value_size=100 --memtablerep=skip_list --num=$read_num --threads=1
    result=$(numactl -C 0-7 ./share/rocksdb/db_bench --benchmarks="readrandom" --threads=8 --wal_dir=/mnt/data --db=/mnt/data --disable_auto_compactions --disable_wal=false --key_size=20 --value_size=100 --use_existing_db --num=$read_num --memtablerep=skip_list --verify_checksum)
    ops=$(echo $result | grep -o "micros/op .* ops/sec" | tr -dc '[. [:digit:]]' | xargs)
    echo "[ops]:[$ops]" | tee -a share/tmp/rocksdb.log
    umount /mnt/data || /bin/true
    echo "---------------------------" | tee -a share/tmp/rocksdb.log
done

for((storage_mode=0;storage_mode<=7;storage_mode++));
do
    echo "-----------start-----------" | tee -a share/tmp/rocksdb.log
    echo "[bench_name]:[insert_1KB]" | tee -a share/tmp/rocksdb.log
    prepare_storage $storage_mode
    result=$(numactl -C 0-7 ./share/rocksdb/db_bench --benchmarks="fillrandom" --wal_dir=/mnt/data --db=/mnt/data --disable_auto_compactions --disable_wal=false --sync --key_size=20 --value_size=1024 --memtablerep=skip_list --num=$insert_num --threads=1)
    ops=$(echo $result | grep -o "micros/op .* ops/sec" | tr -dc '[. [:digit:]]' | xargs)
    echo "[ops]:[$ops]" | tee -a share/tmp/rocksdb.log
    umount /mnt/data || /bin/true
    echo "---------------------------" | tee -a share/tmp/rocksdb.log
done

for((storage_mode=0;storage_mode<=7;storage_mode++));
do
    echo "-----------start-----------" | tee -a share/tmp/rocksdb.log
    echo "[bench_name]:[read_1KB]" | tee -a share/tmp/rocksdb.log
    prepare_storage $storage_mode
    numactl -C 0-7 ./share/rocksdb/db_bench --benchmarks="fillrandom,compact" --wal_dir=/mnt/data --db=/mnt/data --disable_wal=false --key_size=20 --value_size=100 --memtablerep=skip_list --num=$read_num --threads=1
    result=$(numactl -C 0-7 ./share/rocksdb/db_bench --benchmarks="readrandom" --threads=8 --wal_dir=/mnt/data --db=/mnt/data --disable_auto_compactions --disable_wal=false --key_size=20 --value_size=1024 --use_existing_db --num=$read_num --memtablerep=skip_list --verify_checksum)
    ops=$(echo $result | grep -o "micros/op .* ops/sec" | tr -dc '[. [:digit:]]' | xargs)
    echo "[ops]:[$ops]" | tee -a share/tmp/rocksdb.log
    umount /mnt/data || /bin/true
    echo "---------------------------" | tee -a share/tmp/rocksdb.log
done

./share/artifact/log_parser.py --log_path share/tmp/rocksdb.log --config_path share/artifact/log_parser.config --output_path share/output/rocksdb.csv

./share/artifact/plot/rocksdb.py 