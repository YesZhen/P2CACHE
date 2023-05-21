#!/bin/bash

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
    0) mount -t tmpfs -o size=160G,mpol=bind:0 tmpfs /mnt/data && echo "[media]:[mem],[fs]:[tmpfs]" | tee -a share/tmp/minio.log # mem tmpfs
    ;;
    1) mkfs.ext4 -F /dev/sdb && mount -t ext4 /dev/sdb /mnt/data && echo "[media]:[ssd],[fs]:[ext4]" | tee -a share/tmp/minio.log # ssd ext4
    ;;
    2) mkfs.ext4 -F /dev/sdb && mount -t ext4 -o data=journal /dev/sdb /mnt/data && echo "[media]:[ssd],[fs]:[ext4_dj]" | tee -a share/tmp/minio.log # ssd ext4_dj (ext4 data=journal)
    ;;
    3) mkfs.xfs -m reflink=0 -f /dev/sdb && mount -t xfs /dev/sdb /mnt/data && echo "[media]:[ssd],[fs]:[xfs]" | tee -a share/tmp/minio.log # ssd xfs
    ;;
    4) mkfs.ext4 -F /dev/pmem0 && mount -t ext4 -o dax /dev/pmem0 /mnt/data && echo "[media]:[pmem],[fs]:[ext4_dax]" | tee -a share/tmp/minio.log # pmem ext4_dax
    ;;
    5) mkfs.xfs -m reflink=0 -f /dev/pmem0 && mount -t xfs -o dax /dev/pmem0 /mnt/data && echo "[media]:[pmem],[fs]:[xfs_dax]" | tee -a share/tmp/minio.log # pmem xfs_dax 
    ;;
    6) mount -t NOVA -o init,dax,data_cow /dev/pmem0 /mnt/data && echo "[media]:[pmem],[fs]:[nova]" | tee -a share/tmp/minio.log # pmem NOVA 
    ;;
    7) mount -t p2cache /dev/pmem0 /mnt/data && echo "[media]:[pmem],[fs]:[p2cache]" | tee -a share/tmp/minio.log # pmem p2cache
    ;;
    esac
}


rm -f share/tmp/minio.log


for((storage_mode=0;storage_mode<=7;storage_mode++));
do
    echo "-----------start-----------" | tee -a share/tmp/minio.log
    echo "[bench_name]:[put]" | tee -a share/tmp/minio.log
    prepare_storage $storage_mode
    ./share/start_minio.sh
    numactl -C 9 ./share/minio/warp put --host=127.0.0.1:9000 --access-key=minioadmin --secret-key=minioadmin --duration=30s --obj.size=16K,256k --concurrent=8 --noclear --quiet --no-color --benchdata=share/tmp/warp
    printf "\n"
    result=$(./share/minio/warp analyze --analyze.op=PUT --analyze.v share/tmp/warp.csv.zst --quiet --no-color)
    ops=$(echo $result | grep -o "Average: .*B/s, .* obj/s Throughput," | grep -o ", .* obj/s Throughput" | tr -dc '[. [:digit:]]' | xargs)
    echo "[ops]:[$ops]" | tee -a share/tmp/minio.log
    ./share/minio/mc admin service stop myminio
    umount /mnt/data || /bin/true
    echo "---------------------------" | tee -a share/tmp/minio.log
done

for((storage_mode=0;storage_mode<=7;storage_mode++));
do
    echo "-----------start-----------" | tee -a share/tmp/minio.log
    echo "[bench_name]:[get]" | tee -a share/tmp/minio.log
    prepare_storage $storage_mode
    ./share/start_minio.sh
    numactl -C 9 ./share/minio/warp get --host=127.0.0.1:9000 --access-key=minioadmin --secret-key=minioadmin --duration=30s --obj.size=16k,1m --concurrent=8 --noclear --quiet --no-color --benchdata=share/tmp/warp
    printf "\n"
    result=$(./share/minio/warp analyze --analyze.op=GET --analyze.v share/tmp/warp.csv.zst --quiet --no-color)
    ops=$(echo $result | grep -o "Average: .*B/s, .* obj/s Throughput," | grep -o ", .* obj/s Throughput" | tr -dc '[. [:digit:]]' | xargs)
    echo "[ops]:[$ops]" | tee -a share/tmp/minio.log
    ./share/minio/mc admin service stop myminio
    umount /mnt/data || /bin/true
    echo "---------------------------" | tee -a share/tmp/minio.log
done

for((storage_mode=0;storage_mode<=7;storage_mode++));
do
    echo "-----------start-----------" | tee -a share/tmp/minio.log
    echo "[bench_name]:[mixed]" | tee -a share/tmp/minio.log
    prepare_storage $storage_mode
    ./share/start_minio.sh
    numactl -C 9 ./share/minio/warp mixed --host=127.0.0.1:9000 --access-key=minioadmin --secret-key=minioadmin --duration=30s --obj.size=16K,256k --concurrent=8 --noclear --quiet --no-color --benchdata=share/tmp/warp
    printf "\n"
    result=$(./share/minio/warp analyze --analyze.v share/tmp/warp.csv.zst --quiet --no-color)
    ops=$(echo $result | grep -o "Cluster Total: .*B/s, .* obj/s" | grep -o ", .* obj/s" | tr -dc '[. [:digit:]]' | xargs)
    echo "[ops]:[$ops]" | tee -a share/tmp/minio.log
    ./share/minio/mc admin service stop myminio
    umount /mnt/data || /bin/true
    echo "---------------------------" | tee -a share/tmp/minio.log
done


./share/artifact/log_parser.py --log_path share/tmp/minio.log --config_path share/artifact/log_parser.config --output_path share/output/minio.csv

./share/artifact/plot/minio.py 