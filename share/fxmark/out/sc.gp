# LOG_FILE = logs/2022-11-02-02-29-10.707652/fxmark.log
# SYSTEM = Linux syzkaller 5.4.219 #5 SMP Thu Oct 27 00:20:53 UTC 2022 x86_64 GNU/Linux
# DISK_SIZE = 60G
# DURATION = 10s
# DIRECTIO = bufferedio,directio
# MEDIA_TYPES = ssd,pmem,dram,mem
# FS_TYPES = pCache,nova,ext4_dax,xfs_dax,ext4,xfs,tmpfs
# BENCH_TYPES = N_CREATE_FSYNC,N_MKDIR_FSYNC,N_RENAME_FSYNC,N_UNLINK_FSYNC,N_LINK_FSYNC
# NCORES = 1,2,4,8
# CORE_SEQ = 0,1,2,3,4,5,6,7
# MODEL_NAME = Intel(R) Xeon(R) Gold 5317 CPU @ 3.00GHz
# PHYSICAL_CHIPS = 1
# CORE_PER_CHIP = 8
# SMT_LEVEL = 1
# NUM_TEST_CONF = 120

set term pdfcairo size 6.8999999999999995in,9.2in font ',10'
set_out='set output "`if test -z $OUT; then echo sc.pdf; else echo $OUT; fi`"'
eval set_out
set multiplot layout 4,3

set title 'pmem:N_CREATE_FSYNC:bufferedio'
set xlabel '# cores'
set ylabel 'M ops/sec'
plot [0:][0:] 'pmem:ext4_dax:N_CREATE_FSYNC:bufferedio.dat' using 1:2 title 'ext4_dax' with lp ps 0.5, 'pmem:nova:N_CREATE_FSYNC:bufferedio.dat' using 1:2 title 'nova' with lp ps 0.5, 'pmem:pCache:N_CREATE_FSYNC:bufferedio.dat' using 1:2 title 'pCache' with lp ps 0.5, 'pmem:xfs_dax:N_CREATE_FSYNC:bufferedio.dat' using 1:2 title 'xfs_dax' with lp ps 0.5

set title 'pmem:N_LINK_FSYNC:bufferedio'
set xlabel '# cores'
set ylabel 'M ops/sec'
plot [0:][0:] 'pmem:ext4_dax:N_LINK_FSYNC:bufferedio.dat' using 1:2 title 'ext4_dax' with lp ps 0.5, 'pmem:nova:N_LINK_FSYNC:bufferedio.dat' using 1:2 title 'nova' with lp ps 0.5, 'pmem:pCache:N_LINK_FSYNC:bufferedio.dat' using 1:2 title 'pCache' with lp ps 0.5, 'pmem:xfs_dax:N_LINK_FSYNC:bufferedio.dat' using 1:2 title 'xfs_dax' with lp ps 0.5

set title 'pmem:N_MKDIR_FSYNC:bufferedio'
set xlabel '# cores'
set ylabel 'M ops/sec'
plot [0:][0:] 'pmem:ext4_dax:N_MKDIR_FSYNC:bufferedio.dat' using 1:2 title 'ext4_dax' with lp ps 0.5, 'pmem:nova:N_MKDIR_FSYNC:bufferedio.dat' using 1:2 title 'nova' with lp ps 0.5, 'pmem:pCache:N_MKDIR_FSYNC:bufferedio.dat' using 1:2 title 'pCache' with lp ps 0.5, 'pmem:xfs_dax:N_MKDIR_FSYNC:bufferedio.dat' using 1:2 title 'xfs_dax' with lp ps 0.5

set title 'pmem:N_RENAME_FSYNC:bufferedio'
set xlabel '# cores'
set ylabel 'M ops/sec'
plot [0:][0:] 'pmem:ext4_dax:N_RENAME_FSYNC:bufferedio.dat' using 1:2 title 'ext4_dax' with lp ps 0.5, 'pmem:nova:N_RENAME_FSYNC:bufferedio.dat' using 1:2 title 'nova' with lp ps 0.5, 'pmem:pCache:N_RENAME_FSYNC:bufferedio.dat' using 1:2 title 'pCache' with lp ps 0.5, 'pmem:xfs_dax:N_RENAME_FSYNC:bufferedio.dat' using 1:2 title 'xfs_dax' with lp ps 0.5

set title 'pmem:N_UNLINK_FSYNC:bufferedio'
set xlabel '# cores'
set ylabel 'M ops/sec'
plot [0:][0:] 'pmem:ext4_dax:N_UNLINK_FSYNC:bufferedio.dat' using 1:2 title 'ext4_dax' with lp ps 0.5, 'pmem:nova:N_UNLINK_FSYNC:bufferedio.dat' using 1:2 title 'nova' with lp ps 0.5, 'pmem:pCache:N_UNLINK_FSYNC:bufferedio.dat' using 1:2 title 'pCache' with lp ps 0.5, 'pmem:xfs_dax:N_UNLINK_FSYNC:bufferedio.dat' using 1:2 title 'xfs_dax' with lp ps 0.5

set title 'ssd:N_CREATE_FSYNC:bufferedio'
set xlabel '# cores'
set ylabel 'M ops/sec'
plot [0:][0:] 'ssd:ext4:N_CREATE_FSYNC:bufferedio.dat' using 1:2 title 'ext4' with lp ps 0.5, 'ssd:xfs:N_CREATE_FSYNC:bufferedio.dat' using 1:2 title 'xfs' with lp ps 0.5

set title 'ssd:N_LINK_FSYNC:bufferedio'
set xlabel '# cores'
set ylabel 'M ops/sec'
plot [0:][0:] 'ssd:ext4:N_LINK_FSYNC:bufferedio.dat' using 1:2 title 'ext4' with lp ps 0.5, 'ssd:xfs:N_LINK_FSYNC:bufferedio.dat' using 1:2 title 'xfs' with lp ps 0.5

set title 'ssd:N_MKDIR_FSYNC:bufferedio'
set xlabel '# cores'
set ylabel 'M ops/sec'
plot [0:][0:] 'ssd:ext4:N_MKDIR_FSYNC:bufferedio.dat' using 1:2 title 'ext4' with lp ps 0.5, 'ssd:xfs:N_MKDIR_FSYNC:bufferedio.dat' using 1:2 title 'xfs' with lp ps 0.5

set title 'ssd:N_RENAME_FSYNC:bufferedio'
set xlabel '# cores'
set ylabel 'M ops/sec'
plot [0:][0:] 'ssd:ext4:N_RENAME_FSYNC:bufferedio.dat' using 1:2 title 'ext4' with lp ps 0.5, 'ssd:xfs:N_RENAME_FSYNC:bufferedio.dat' using 1:2 title 'xfs' with lp ps 0.5

set title 'ssd:N_UNLINK_FSYNC:bufferedio'
set xlabel '# cores'
set ylabel 'M ops/sec'
plot [0:][0:] 'ssd:ext4:N_UNLINK_FSYNC:bufferedio.dat' using 1:2 title 'ext4' with lp ps 0.5, 'ssd:xfs:N_UNLINK_FSYNC:bufferedio.dat' using 1:2 title 'xfs' with lp ps 0.5

unset multiplot
set output
