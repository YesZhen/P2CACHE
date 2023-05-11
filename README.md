# P²CACHE
Implementation of the paper:

Zhen Lin, Lingfeng Xiang, Jia Rao and Hui Lu, [**“P²CACHE: Exploring Tiered Memory for In-Kernel File Systems Caching”**](https://www.usenix.org/conference/atc23/presentation/lin) in Proceedings of the 2023 USENIX Annual Technical Conference. July 10-12, 2023 • Boston, MA, USA

This repository contains the code for P²CACHE, microbenchmarks and macrobenchmarks.

For artifact evaluation, we provide the following two options.
- Run experiments on our testbed. We already built and installed all the tools and dependencies. Please see [Quick Start on Our Testbed](#quick-start-on-our-testbed).
- Build all the source code on your machine. Then, run experiments in your environment. For this option, to fully reproduce the results in the papers, the machine requires at least 512 GB Intel Optane 200 series PMEM and 256 GB DRAM.

> **Warning!**
> Running experiments on your own machine may lead to machine panic. Please save all the important data before running experiments! Thanks!

> **Warning!**
> Please run all the scripts we provid only under the top-level directory of this repo.

## Table of Contents

<!-- - [Background](#background) -->
- [Quick Start on Our Testbed](#quick-start-on-our-testbed)
- [Prerequisites](#prerequisites)
- [Usage](#usage)
	- [Before Run](#before-run)
  - [Reproduce results from the paper](#reproduce-results-from-the-paper)
<!-- ## Background -->


## Quick Start on Our Testbed

We provide the hardware details of our testbed.
|  Component   | Model Specifications  |
|  ----  | ----  |
|  Platform  | ASUS RS700-E10-RS12U  |
|  CPU  | 2 * Intel Xeon Gold 5317 processors (3.0 GHz and 18M Cache)  |
|  DRAM  | 256 GB for each socket |
|  PMEM  | 4 x 128 GB (total 512 GB) Intel Optane 200 for each socket |
|  SSD  | 2-TB Samsung PM883 |

To access our testbed, please feel free to contact us on HotCRP. We would create a VPN account for you to access our machine remotely. On our testbed, we have already built and installed all the code and depencencies. Once you log into to our testbed, please run the following commands to switch the working directory to P²CACHE.

```
cd /root/p2cache
```

Then, you can jump to the step [Reproduce results from the paper](#reproduce-results-from-the-paper)

## Prerequisites

### Hardware Requirements

Your machine needs to have Intel Optane DC PMM installed.
Besides, your machine also needs AVX512 and CLWB instruction sets.


### Software Requirements

We run the code on Ubuntu 20.04 LTS (https://releases.ubuntu.com/focal/ubuntu-20.04.6-live-server-amd64.iso), and the compatibility on other Linux distros are not verified. It's not recommended to run the current code on bare-metal machine with other kernel versions because incompatible kernel code may crash the entire machine.

To run the code you need to install these packages:

```
sudo apt install build-essentials libgflags-dev libssl-dev numactl cmake
```

#### Set PM in interleaved mode
All the Intel Optane DC PMM devices must be unmounted before namespaces are destroyed.
```
# destroy current namespaces on Intel Optane DC PMM
ndctl destroy-namespace -f all
# reboot is required after this
ipmctl create -goal
# execute this after reboot
ndctl create-namespace
```
For more details, please refer https://docs.pmem.io/persistent-memory/getting-started-guide.

## Usage
### Before run
Before you run, something must be set up. You need to

#### Init git submodule
```
git submodule update --init --recursive
```

#### Build P²CACHE
```
# Use our script to build P²CACHE for host
./scripts/build-p2cache-host.sh

# Different kernel version may cause failure of compilation and installation

```

#### Build FxMark
```
# We use fxmark for microbenchmark and filebench

./scripts/build-fxmark.sh

```

#### Build db_bench (official bench tool from RocksDB)

```
./scripts/build-db_bench-host.sh
```

#### Build MinIO

```
# We download the binary executables of MinIO and Warp from
# https://dl.min.io/server/minio/release/linux-amd64/minio
# https://github.com/minio/warp/releases/download/v0.6.8/warp_Darwin_x86_64.tar.gz
# You can use the following command to quickly download executables.

./scripts/download-minio.sh

```



## Reproduce results from the paper

Before reproducing results, please run the following command to make sure that the environment is ok.

```
./share/init_and_check.sh
```

### (E1) Figure 6a. P²CACHE significantly accelerates metadata operations as against other cases except for TMPFS (Without fsync).

|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/meta_without_fsync.sh`  |
| Raw log path  | `tmp/meta_without_fsync.log`  |
| Formatted data path  | `output/meta_without_fsync.csv`  |
| Graph path  | `output/meta_without_fsync.png`  |
| Plot script path  | `share/artifact/plot/meta_without_fsync.py`  |


### (E2) Figure 6b. P²CACHE significantly accelerates metadata operations as against other cases except for TMPFS (With fsync).

|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/meta_with_fsync.sh`  |
| Raw log path  | `tmp/meta_with_fsync.log`  |
| Formatted data path  | `output/meta_with_fsync.csv`  |
| Graph path  | `output/meta_with_fsync.png`  |
| Plot script path  | `share/artifact/plot/meta_with_fsync.py`  |


### (E3) Figure 7a. P²CACHE accelerates data operations, especially for small, partial writes, as against other cases (Without fdatasync).

|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/data_without_fdatasync.sh`  |
| Raw log path  | `tmp/data_without_fdatasync.log`  |
| Formatted data path  | `output/data_without_fdatasync.csv`  |
| Graph path  | `output/data_without_fdatasync.png`  |
| Plot script path  | `share/artifact/plot/data_without_fdatasync.py`  |


### (E4) Figure 7b. P²CACHE accelerates data operations, especially for small, partial writes, as against other cases (With fdatasync).

|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/data_with_fdatasync.sh`  |
| Raw log path  | `tmp/data_with_fdatasync.log`  |
| Formatted data path  | `output/data_with_fdatasync.csv`  |
| Graph path  | `output/data_with_fdatasync.png`  |
| Plot script path  | `share/artifact/plot/data_with_fdatasync.py`  |

### (E5) Figure 8.  Comparisons of performance for reads.

|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/read.sh`  |
| Raw log path  | `tmp/read.log`  |
| Formatted data path  | `output/read.csv`  |
| Graph path  | `output/read.png`  |
| Plot script path  | `share/artifact/plot/read.py`  |

### (E6) Figure 9.  Scalability test with 4 KB append operations.

|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/scalability.sh`  |
| Raw log path  | `tmp/scalability.log`  |
| Formatted data path  | `output/scalability.csv`  |
| Graph path  | `output/scalability.png`  |
| Plot script path  | `share/artifact/plot/scalability.py`  |

### (E7) Figure 10a. Performance comparisons of using real-world applications - Filebench

|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/filebench.sh`  |
| Raw log path  | `tmp/filebench.log`  |
| Formatted data path  | `output/filebench.csv`  |
| Graph path  | `output/application.png`  |
| Plot script path  | `share/artifact/plot/application.py`  |

### (E8) Figure 10b. Performance comparisons of using real-world applications - RocksDB

|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/rocksdb.sh`  |
| Raw log path  | `tmp/rocksdb.log`  |
| Formatted data path  | `output/rocksdb.csv`  |
| Graph path  | `output/application.png`  |
| Plot script path  | `share/artifact/plot/application.py`  |

### (E9) Figure 10c. Performance comparisons of using real-world applications - MinIO

|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/minio.sh`  |
| Raw log path  | `tmp/minio.log`  |
| Formatted data path  | `output/minio.csv`  |
| Graph path  | `output/application.png`  |
| Plot script path  | `share/artifact/plot/application.py`  |
