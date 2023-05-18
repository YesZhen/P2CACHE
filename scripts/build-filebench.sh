#!/bin/bash

git submodule update --init --recursive

cd share/filebench

libtoolize
aclocal
autoheader
automake --add-missing
autoconf

./configure
make
sudo make install