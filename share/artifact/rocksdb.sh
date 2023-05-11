#!/bin/bash

###### 2>&1 &


# total_size=17179869184 # 16G
# total_size=10737418240
# total_size=67108864
# total_size=16384
# total_size=536870912 # 512M
# total_size=274877906944 # 256G
total_size=68719476736 # 64G
runtime=60
now=$(date +"%m_%d_%y_%H_%M_%S")
workspace=$(dirname "$0")
path="$workspace/$now"

outfile=1

dry_run=1


# mount -t ext4 /dev/sdb /mnt/sdb
if [ $outfile == 1 ]; then
    echo $path
    mkdir -p $path
fi

# 0) mkfs.ext4 -F /dev/sdc && mount -t ext4 /dev/sdc /mnt/data
#     ;;
# 3) mkfs.ext4 -F /dev/nvme0n1 && mount -t ext4 /dev/nvme0n1 /mnt/data
    # ;;

function prepare_storage() {
    # mountpoint -q /mnt/data
    # if [ $? == 1 ]; then
    #     umount /mnt/data
    # fi

    if mountpoint -q /mnt/data
    then
        umount /mnt/data
    fi
    # umount /mnt/data || /bin/true
    # umount /mnt/pmem0 || /bin/true
    # mkdir -p /mnt/data

    storage_mode=$1

    
    case $storage_mode in
    0) mount -t tmpfs -o size=160G,mpol=bind:0 tmpfs /mnt/data # mem tmpfs
    ;;
    1) mkfs.ext4 -F /dev/sdb && mount -t ext4 /dev/sdb /mnt/data # ssd ext4
    ;;
    2) mkfs.ext4 -F /dev/sdb && mount -t ext4 -o data=journal /dev/sdb /mnt/data # ssd ext4_dj (ext4 data=journal)
    ;;
    3) mkfs.xfs -m reflink=0 -f /dev/sdb && mount -t xfs /dev/sdb /mnt/data # ssd xfs
    ;;
    4) mkfs.ext4 -F /dev/pmem0 && mount -t ext4 -o dax /dev/pmem0 /mnt/data # pmem ext4_dax
    ;;
    5) mkfs.xfs -m reflink=0 -f /dev/pmem0 && mount -t xfs -o dax /dev/pmem0 /mnt/data # pmem xfs_dax 
    ;;
    6) mount -t NOVA -o init,dax,data_cow /dev/pmem0 /mnt/data # pmem NOVA 
    ;;
    7) mount -t ovl /dev/pmem0 /mnt/data # pmem p2cache
    ;;
    esac
}


# if [ $outfile == 1 ]; then
#     echo 'storage_mode,access_mode,bs,numjobs,fsync,direct,single_file,read_bw,read_iops,read_lat_min,read_lat_mean,read_lat_max,write_bw,write_iops,write_lat_min,write_lat_mean,write_lat_max' > $path/result.csv
# fi

for((storage_mode=0;storage_mode<=7;storage_mode++));
do
    prepare_storage $storage_mode
    ./share/rocksdb/db_bench --benchmarks="fillrandom" --wal_dir=/mnt/data --db=/mnt/data --disable_auto_compactions --disable_wal=false --sync --key_size=20 --value_size=100 --memtablerep=skip_list --num=1000000 --threads=1
    umount /mnt/data || /bin/true
done

for((storage_mode=0;storage_mode<=7;storage_mode++));
do
    prepare_storage $storage_mode
    ./share/rocksdb/db_bench --benchmarks="fillrandom,compact" --wal_dir=/mnt/data --db=/mnt/data --disable_wal=false --key_size=20 --value_size=100 --memtablerep=skip_list --num=10000000 --threads=1
    ./share/rocksdb/db_bench --benchmarks="readrandom" --threads=8 --wal_dir=/mnt/data --db=/mnt/data --disable_auto_compactions --disable_wal=false --key_size=20 --value_size=100 --use_existing_db --num=10000000 --memtablerep=skip_list --verify_checksum
    umount /mnt/data || /bin/true
done

for((storage_mode=0;storage_mode<=7;storage_mode++));
do
    prepare_storage $storage_mode
    ./share/rocksdb/db_bench --benchmarks="fillrandom" --wal_dir=/mnt/data --db=/mnt/data --disable_auto_compactions --disable_wal=false --sync --key_size=20 --value_size=1024 --memtablerep=skip_list --num=1000000 --threads=1
    umount /mnt/data || /bin/true
done

for((storage_mode=0;storage_mode<=7;storage_mode++));
do
    prepare_storage $storage_mode
    ./share/rocksdb/db_bench --benchmarks="fillrandom,compact" --wal_dir=/mnt/data --db=/mnt/data --disable_wal=false --key_size=20 --value_size=100 --memtablerep=skip_list --num=10000000 --threads=1
    ./share/rocksdb/db_bench --benchmarks="readrandom" --threads=8 --wal_dir=/mnt/data --db=/mnt/data --disable_auto_compactions --disable_wal=false --key_size=20 --value_size=1024 --use_existing_db --num=10000000 --memtablerep=skip_list --verify_checksum
    umount /mnt/data || /bin/true
done

echo "========================================================================================================================="
echo "=+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++="
echo "=+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++="
echo "=+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++="
echo "=+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++="
echo "=+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++="
echo "=+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++="
echo "=+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++="
echo "========================================================================================================================="