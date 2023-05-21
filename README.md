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

> **Warning!**
> Some figures may be different from the figures in our paper. In this repo, we change the Y-AXIS from regular-scale to log-scale for better reading in some figures.

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
./scripts/dependencies.sh
```

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

#### Build Nova
```
# Use our script to build Nova for host
./scripts/build-nova-host.sh

# Different kernel version may cause failure of compilation and installation

```

#### Build FxMark
```
# We use fxmark for microbenchmark and filebench

./scripts/build-fxmark.sh

```

#### Build Filebench
```

./scripts/build-filebench.sh

```

#### Build db_bench (official bench tool from RocksDB)

```
./scripts/build-rocksdb.sh
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
If everything is good, you should be able to see the following output or similar output with all green-check marks. Then, you can start to reproduce results.

![image](https://drive.google.com/uc?export=view&id=1-RPvReFxN2JyA079NiVazPV9cFHroEAF)

If something is wrong, the script will tell you which component is not good. You can try to follow the instruction to fix it or feel free to contact us for help!

![image](https://drive.google.com/uc?export=view&id=1-Racp7G4r1p9pBJ_zYe2nuT9qhm0Y1Y_)


### Run experiments

- Run experiment with script, e.g., `.share/artifact/meta_without_fsync.sh`. The experiment scripts wil run experiment, parse log to formatted csv, and plot figure.
- You can see the log from raw log path, e.g., `tmp/meta_without_fsync.log`.
- When the experiment is done, you can see the formatted csv from the formatted data path, e.g., `output/meta_without_fsync.csv`.
- When the experiment is done, you can see the figure from the graph path, e.g., `output/meta_without_fsync.png`.

### (E1) Figure 6a. P²CACHE significantly accelerates metadata operations as against other cases except for TMPFS (Without fsync).
Expected running time: 10 minutes
|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/meta_without_fsync.sh`  |
| Raw log path  | `tmp/meta_without_fsync.log`  |
| Formatted data path  | `output/meta_without_fsync.csv`  |
| Figure path  | `output/meta_without_fsync.png`  |
| Plot script path  | `share/artifact/plot/meta_without_fsync.py`  |


### (E2) Figure 6b. P²CACHE significantly accelerates metadata operations as against other cases except for TMPFS (With fsync).
Expected running time: 30 minutes
|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/meta_with_fsync.sh`  |
| Raw log path  | `tmp/meta_with_fsync.log`  |
| Formatted data path  | `output/meta_with_fsync.csv`  |
| Figure path  | `output/meta_with_fsync.png`  |
| Plot script path  | `share/artifact/plot/meta_with_fsync.py`  |


### (E3) Figure 7a. P²CACHE accelerates data operations, especially for small, partial writes, as against other cases (Without fdatasync).
Expected running time: 30 minutes
|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/data_without_fdatasync.sh`  |
| Raw log path  | `tmp/data_without_fdatasync.log`  |
| Formatted data path  | `output/data_without_fdatasync.csv`  |
| Figure path  | `output/data_without_fdatasync.png`  |
| Plot script path  | `share/artifact/plot/data_without_fdatasync.py`  |


### (E4) Figure 7b. P²CACHE accelerates data operations, especially for small, partial writes, as against other cases (With fdatasync).
Expected running time: 30 minutes
|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/data_with_fdatasync.sh`  |
| Raw log path  | `tmp/data_with_fdatasync.log`  |
| Formatted data path  | `output/data_with_fdatasync.csv`  |
| Figure path  | `output/data_with_fdatasync.png`  |
| Plot script path  | `share/artifact/plot/data_with_fdatasync.py`  |

### (E5) Figure 8.  Comparisons of performance for reads.
Expected running time: 30 minutes
|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/read.sh`  |
| Raw log path  | `tmp/read.log`  |
| Formatted data path  | `output/read.csv`  |
| Figure path  | `output/read.png`  |
| Plot script path  | `share/artifact/plot/read.py`  |

### (E6) Figure 9.  Scalability test with 4 KB append operations.
Expected running time: 20 minutes
|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/scalability.sh`  |
| Raw log path  | `tmp/scalability.log`  |
| Formatted data path  | `output/scalability.csv`  |
| Figure path  | `output/scalability.png`  |
| Plot script path  | `share/artifact/plot/scalability.py`  |

### (E7) Figure 10a. Performance comparisons of using real-world applications - Filebench
Expected running time: 15 minutes
|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/filebench.sh`  |
| Raw log path  | `tmp/filebench.log`  |
| Formatted data path  | `output/filebench.csv`  |
| Figure path  | `output/filebench.png`  |
| Plot script path  | `share/artifact/plot/filebench.py`  |

### (E8) Figure 10b. Performance comparisons of using real-world applications - RocksDB
Expected running time: 15 minutes
|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/rocksdb.sh`  |
| Raw log path  | `tmp/rocksdb.log`  |
| Formatted data path  | `output/rocksdb.csv`  |
| Figure path  | `output/rocksdb.png`  |
| Plot script path  | `share/artifact/plot/rocksdb.py`  |

### (E9) Figure 10c. Performance comparisons of using real-world applications - MinIO
Expected running time: 15 minutes
|  Figure info   | Contents  |
|  ----  | ----  |
| Experiments script path  | `share/artifact/minio.sh`  |
| Raw log path  | `tmp/minio.log`  |
| Formatted data path  | `output/minio.csv`  |
| Figure path  | `output/minio.png`  |
| Plot script path  | `share/artifact/plot/minio.py`  |
