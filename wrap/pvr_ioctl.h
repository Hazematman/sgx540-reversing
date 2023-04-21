#ifndef PVR_IOCTL_H
#define PVR_IOCTL_H

#define PVRSRV_IOWR(INDEX) (INDEX)
#define PVRSRV_IOCTL(NAME, VALUE) NAME = (VALUE),

enum pvrsrv_ioctl {
#include "pvr_ioctl.inc"
};

#undef PVRSRV_IOCTL
#define PVRSRV_IOCTL(NAME, VALUE) [VALUE] = #NAME,

const char *pvrsrv_ioctl_names[] = {
#include "pvr_ioctl.inc"
};

#endif
