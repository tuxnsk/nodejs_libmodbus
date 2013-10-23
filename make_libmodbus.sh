#!/bin/sh

git clone https://github.com/stephane/libmodbus.git

cd ./libmodbus/

# reset to patch commit number
git reset --hard 79a7c1ea82e5bb2e13c8efc77cb3e8a6d7368c58

patch -c -p2 < ../mt.patch

#./autogen.sh && ./configure --enable-static=yes --enable-shared=no && make
./autogen.sh && ./configure && make
