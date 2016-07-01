#!/bin/bash

git clone git://git.savannah.gnu.org/libjit.git libjit/
cd libjit/
./bootstrap
./configure
make
make install && popd
cp /usr/local/lib/libjit* /usr/lib/
