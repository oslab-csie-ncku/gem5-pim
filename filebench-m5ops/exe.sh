#!/bin/bash
make clean
libtoolize
aclocal
autoheader
automake --add-missing
autoconf
./configure
make
