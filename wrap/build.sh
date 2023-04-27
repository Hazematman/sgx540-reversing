#!/bin/bash
#C_FLAGS="-g -I/home/ci20/Documents/pvr_us/usr/local/XSGX/include -I/home/ci20/Documents/pvr_us/usr/local/XSGX/include/libdrm "
C_FLAGS="-g -DSUPPORT_DRI_DRM_EXT -I/home/ci20/Documents/eurasia_km/include4 -I/home/ci20/Documents/eurasia_km/services4/include -I/home/ci20/Documents/eurasia_km/services4/srvkm/hwdefs -I/home/ci20/Documents/eurasia_km/eurasiacon/binary2_jz4780_linux_release -I/home/ci20/Documents/eurasia_km/services4/include/env/linux -I/home/ci20/Documents/eurasia_km/services4/srvkm/env/linux -I/home/ci20/Documents/eurasia_km/services4/srvkm/devices/sgx -I/home/ci20/Documents/pvr_us/usr/local/XSGX/include -I/home/ci20/Documents/pvr_us/usr/local/XSGX/include/libdrm -I./ -I../wrap"
gcc -std=gnu99 -D_GNU_SOURCE ${C_FLAGS} -ldl -shared -Wall -Wextra wrap.c -o libwrap.so -fPIC -pthread
#gcc -std=gnu99 -D_GNU_SOURCE dump_ioctl.c -o dump_ioctl
