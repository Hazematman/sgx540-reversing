#!/bin/sh

C_FLAGS="-g -I/home/ci20/Documents/pvr_us/usr/local/XSGX/include -L/home/ci20/Documents/pvr_us/usr/local/XSGX/lib"
gcc -g -std=c99 test.c ${C_FLAGS} -o test -lX11 -lGL
