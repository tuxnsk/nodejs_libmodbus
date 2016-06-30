#!/bin/sh

git clone https://github.com/stephane/libmodbus.git

cd ./libmodbus/

# reset to patch commit number
git reset --hard f935846

# patch -c -p2 < ../mt.patch

#./autogen.sh && ./configure --enable-static=yes --enable-shared=no && make
./autogen.sh && ./configure && make
