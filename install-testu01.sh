#!/usr/bin/env bash

set -e
wget http://simul.iro.umontreal.ca/testu01/TestU01.zip
unzip TestU01.zip
mkdir TestU01
cd TestU01-1.2.3
./configure --prefix=`pwd`/../TestU01
make -j8
make install
