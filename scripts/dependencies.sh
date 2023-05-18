#!/bin/bash

sudo apt-get install -y openssh-server,curl,tar,gcc,make,g++,libc6-dev,time,strace,less,psmisc
sudo apt-get install -y build-essential libncurses-dev libssl-dev libelf-dev gcc-8 g++-8 debootstrap qemu-system autotools-dev
sudo apt-get install -y ipmctl ndctl libtool m4 automake bison byacc flex tree python2 python3 python3-pip xfsprogs fio jq libsnappy-dev libgflags-dev net-tools zstd numactl
sudo update-alternatives --install /usr/bin/gcc gcc  /usr/bin/gcc-8 1
sudo pip3 install progress json5 pandas matplotlib numpy