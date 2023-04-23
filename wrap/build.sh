#!/bin/bash
C_FLAGS="-g -I/home/ci20/Documents/pvr_us/usr/local/XSGX/include -I/home/ci20/Documents/pvr_us/usr/local/XSGX/include/libdrm "
#C_FLAGS="-DLINUX -I/home/ci20/Documents/eurasia_km/include4 -I/home/ci20/Documents/eurasia_km/services4/include -I/usr/include/mipsel-linux-gnu/sys -I/home/ci20/Documents/eurasia_km/services4/srvkm/hwdefs "
gcc -std=gnu99 -D_GNU_SOURCE ${C_FLAGS} -ldl -shared -Wall -Wextra wrap.c -o libwrap.so -fPIC -pthread
gcc -std=gnu99 -D_GNU_SOURCE dump_ioctl.c -o dump_ioctl
