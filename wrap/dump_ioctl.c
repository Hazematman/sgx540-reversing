#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>

#include <sys/mman.h>
#include <sys/syscall.h>

#include <asm-generic/ioctl.h>

#include <xf86drm.h>

#define DRM_PVR_RESERVED1 (DRM_COMMAND_BASE + 0)
#define DRM_PVR_RESERVED2 (DRM_COMMAND_BASE + 1)
#define DRM_PVR_RESERVED3 (DRM_COMMAND_BASE + 2)
#define DRM_PVR_RESERVED4 (DRM_COMMAND_BASE + 3)
#define DRM_PVR_RESERVED5 (DRM_COMMAND_BASE + 4)
#define DRM_PVR_RESERVED6 (DRM_COMMAND_BASE + 5)

/* PVR includes */
#define SUPPORT_MEMINFO_IDS
#include <config_kernel.h>
#include <sgxfeaturedefs.h>
#include <pvr_bridge.h>
#include <sgx_bridge.h>
#include <pvr_drm_shared.h>
#include <sgxconfig.h>

#define DRM_IOCTL_PVR_SRVKM DRM_IOWR(PVR_DRM_SRVKM_CMD, PVRSRV_BRIDGE_PACKAGE)

#include "pvr_ioctl.h"

int main() {
    int arr_size = sizeof(pvrsrv_ioctl_names)/sizeof(pvrsrv_ioctl_names[0]);
    for(int i = 0; i < arr_size; i++) {
        printf("%d = %s\n", i, pvrsrv_ioctl_names[i]);
    }

    return 0;
}
