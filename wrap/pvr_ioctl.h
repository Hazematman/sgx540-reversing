#ifndef PVR_IOCTL_H
#define PVR_IOCTL_H

#if 0
#define PVRSRV_IOWR(INDEX) (INDEX)
#define PVRSRV_IOCTL(NAME, VALUE) NAME = (VALUE),

enum pvrsrv_ioctl {
#include "pvr_ioctl.inc"
};

#undef PVRSRV_IOCTL
#endif
#define PVRSRV_IOCTL(NAME, VALUE) [ _IOC_NR(NAME) ] = #NAME,

const char *pvrsrv_ioctl_names[] = {
#include "pvr_ioctl.inc"
};

#endif
