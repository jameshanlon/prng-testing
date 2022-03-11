#!/usr/bin/env bash

set -e

# Install
PREFIX=`pwd`/install
echo "Installing in $PREFIX"
rm -rf $PREFIX
mkdir -p $PREFIX
cd $PREFIX

# Grab the sources
wget http://simul.iro.umontreal.ca/testu01/TestU01.zip
wget https://sourceforge.net/projects/pracrand/files/PractRand-pre0.95.zip
wget https://sourceforge.net/projects/gjrand/files/gjrand/gjrand.4.3.0/gjrand.4.3.0.tar.bz2
git clone https://github.com/jameshanlon/random123.git
git clone https://github.com/imneme/pcg-cpp.git

# TestU01
unzip TestU01.zip
pushd TestU01-1.2.3
./configure --prefix=`pwd`/../TestU01
make -j8
make install
popd

# PractRand
unzip PractRand-pre0.95.zip -d PractRand-pre0.95
pushd PractRand-pre0.95
g++ -std=c++14 -c src/*.cpp src/RNGs/*.cpp src/RNGs/other/*.cpp -O3 -Iinclude -pthread
ar rcs libPractRand.a *.o
rm *.o
g++ -std=c++14 -o RNG_test tools/RNG_test.cpp libPractRand.a -O3 -Iinclude -pthread
popd

# gjrand
tar -xvf gjrand.4.3.0.tar.bz2
(cd gjrand.4.3.0.0/src/; ./compile)
(cd gjrand.4.3.0.0/testunif/src; ./compile)
