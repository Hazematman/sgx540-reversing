#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>

#include <asm-generic/ioctl.h>

#define TRANSFER_QUEUE
#define SUPPORT_MEMINFO_IDS
#include "pvr_types.h"
#include "pvr_heaps.h"

/* NOTE make sure defines match between this and dump_ioctl.c */
/* You need to have matching defines inorder to get the right ioctl number
   for the ioctl name */
#include "pvr_ioctl.h"

#define MAX_FDS 128
#define PROLOG(func) \
    static typeof(func) *orig_##func = NULL; \
    if(!orig_##func) \
        orig_##func = dlsym(RTLD_NEXT, #func);

#define DRM_COMMAND_BASE 0x40

#define DEBUG_SYSCALL(msg) write(-1, (msg), sizeof(msg))

enum pvr_ioctl {
    DRM_IOCTL_VERSION = 0x00,
    DRM_IOCTL_GET_UNIQUE = 0x01,
    DRM_IOCTL_GET_MAGIC = 0x02,
    DRM_IOCTL_SET_VERSION = 0x07,
    PVR_DRM_SRVKM_CMD = DRM_COMMAND_BASE + 0x00,
    PVR_DRM_IS_MASTER_CMD = DRM_COMMAND_BASE + 0x03,
};

static struct fd_info {
    bool open;
    bool is_pvr;
} fds[MAX_FDS];

static void check_fd(int fd, const char *pathname) {
    assert(fd < MAX_FDS && "fd is larger than MAX_FDS");
    fds[fd].open = true;
    if(strcmp("/dev/dri/card0", pathname) == 0) {
        //printf("FD %d is pvr\n", fd);
        fds[fd].is_pvr = true;
    }
}

static bool is_pvr(int fd) {
    assert(fd < MAX_FDS && "fd is larger than MAX_FDS");
    assert(fds[fd].open && "fd is not open");
    return fds[fd].is_pvr;
}

static void pvrsrv_ioctl_post(PVRSRV_BRIDGE_PACKAGE *bridge_package) {
    int ioctl_nr = _IOC_NR(bridge_package->ui32BridgeID);
    switch(ioctl_nr) {
        case PVRSRV_BRIDGE_MHANDLE_TO_MMAP_DATA:
            break;
    }
}

static void pvrsrv_ioctl(PVRSRV_BRIDGE_PACKAGE *bridge_package) {
    int ioctl_nr = _IOC_NR(bridge_package->ui32BridgeID);
    printf(">>> pvr_ioctl(%s)\n", pvrsrv_ioctl_names[ioctl_nr]);
    switch(ioctl_nr) {
        case PVRSRV_BRIDGE_MHANDLE_TO_MMAP_DATA:
            /* TODO track memory */
            DEBUG_SYSCALL("MMAP call");
            break;
        case PVRSRV_BRIDGE_ALLOC_DEVICEMEM:
            /* TODO track memory */
            break;
        case PVRSRV_BRIDGE_SGX_REGISTER_HW_RENDER_CONTEXT:
            /* TODO figure out if we can find cmd submits from this */
            break;
        case PVRSRV_BRIDGE_SGX_REGISTER_HW_TRANSFER_CONTEXT:
            /* TODO figure out if we can find cmd submits from this */
            break;
        case PVRSRV_BRIDGE_MAP_DEVICECLASS_MEMORY:
            /* TODO figure out if we need to track this memory */
            break;
        case PVRSRV_BRIDGE_RELEASE_MMAP_DATA:
            /* TODO track memory */
            break;
        case PVRSRV_BRIDGE_UNMAP_DEVICECLASS_MEMORY:
            /* TODO track memory */
            break;
        case PVRSRV_BRIDGE_MAP_DEV_MEMORY:
            /* TODO track memory */
            break;
        case PVRSRV_BRIDGE_UNMAP_DEV_MEMORY:
            /* TODO track memory */
            break;
        case PVRSRV_BRIDGE_FREE_DEVICEMEM:
            /* TODO track memory */
            break;
        case PVRSRV_BRIDGE_SGX_DOKICK:
            /* Figure out if this starts command */
            break;
        case PVRSRV_BRIDGE_SGX_SET_RENDER_CONTEXT_PRIORITY:
        case PVRSRV_BRIDGE_SGX_SET_TRANSFER_CONTEXT_PRIORITY:
            /* TODO figure out if this is important */
            break;
        case PVRSRV_BRIDGE_EVENT_OBJECT_WAIT:
        case PVRSRV_BRIDGE_SYNC_OPS_FLUSH_TO_TOKEN:
        case PVRSRV_BRIDGE_SYNC_OPS_TAKE_TOKEN:
        case PVRSRV_BRIDGE_OPEN_DISPCLASS_DEVICE:
        case PVRSRV_BRIDGE_GET_DISPCLASS_INFO:
        case PVRSRV_BRIDGE_CLOSE_DISPCLASS_DEVICE:
        case PVRSRV_BRIDGE_ENUM_DISPCLASS_FORMATS:
        case PVRSRV_BRIDGE_ENUM_DISPCLASS_DIMS:
        case PVRSRV_BRIDGE_GET_DISPCLASS_SYSBUFFER:
        case PVRSRV_BRIDGE_ENUM_CLASS:
        case PVRSRV_BRIDGE_SGX_GETINTERNALDEVINFO:
        case PVRSRV_BRIDGE_CONNECT_SERVICES:
        case PVRSRV_BRIDGE_ENUM_DEVICES:
        case PVRSRV_BRIDGE_ACQUIRE_DEVICEINFO:
        case PVRSRV_BRIDGE_SGX_GETMISCINFO:
        case PVRSRV_BRIDGE_DISCONNECT_SERVICES:
        case PVRSRV_BRIDGE_CREATE_DEVMEMCONTEXT:
        case PVRSRV_BRIDGE_SGX_GETCLIENTINFO:
        case PVRSRV_BRIDGE_GET_MISC_INFO:
        case PVRSRV_BRIDGE_EVENT_OBJECT_OPEN:
        case PVRSRV_BRIDGE_GET_DEVMEM_HEAPINFO:
        case PVRSRV_BRIDGE_SGX_UNREGISTER_HW_RENDER_CONTEXT:
            /* TODO figure out if these ioctl need to be handled */
            break;
        default:
            printf("Unimplemented pvrsrv ioctl %d, may be %s\n", ioctl_nr, pvrsrv_ioctl_names[ioctl_nr]);
            assert(false);
            break;
    }
}

static void pvr_ioctl_pre(int fd, int request, void *ptr) {
    //printf("ioctl is 0x%x\n", _IOC_NR(request));
    int ioctl_nr = _IOC_NR(request);
    switch(ioctl_nr) {
        case DRM_IOCTL_VERSION:
        case DRM_IOCTL_GET_MAGIC:
        case DRM_IOCTL_GET_UNIQUE:
        case DRM_IOCTL_SET_VERSION:
            /* TODO figure out if this needs to be handled */
            break;
        case PVR_DRM_SRVKM_CMD: 
            pvrsrv_ioctl(ptr);
            break;
        case PVR_DRM_IS_MASTER_CMD:
            /* From KMD source code this seems to always return 0 */
            break;
        default:
            printf("Unimplemented ioctl 0x%x\n", ioctl_nr);
            assert(false);
            break;
    }
}

static void pvr_ioctl_post(int fd, int rquest, void *ptr, int ret) {
}

int ioctl(int fd, int request, ...) {
    PROLOG(ioctl);
    int ioc_size = _IOC_SIZE(request);
    bool pvr = is_pvr(fd);

    //printf("Size is %d\n", ioc_size);
    void *ptr = NULL;
    if(ioc_size) {
        va_list args;
        va_start(args, request);
        ptr = va_arg(args, void *);
        va_end(args);
    }

    if(pvr) {
        pvr_ioctl_pre(fd, request, ptr);
    }

    int ret = orig_ioctl(fd, request, ptr);

    if(pvr) {
        pvr_ioctl_post(fd, request, ptr, ret);
    }

    return ret;
}

int close(int fd) {
    PROLOG(close);
    /* Mark file as closed */
    memset(&fds[fd], 0, sizeof(fds[fd]));
    return orig_close(fd);
}

int open64(const char *pathname, int flags, ...) {
    //printf("Called open64 on %s (%d)\n", pathname, flags);
    PROLOG(open64);
    int fd = orig_open64(pathname, flags);
    check_fd(fd, pathname);
    return fd;
}

int open(const char *pathname, int flags, ...) {
    //printf("Called open on %s (%d)\n", pathname, flags);
    PROLOG(open);
    int fd = orig_open(pathname, flags);
    check_fd(fd, pathname);
    return fd;
}

int openat(int dirfd, const char *pathname, int flags, ...) {
    assert(false && "openat not implemented");
    printf("Called openat on %s (%d) (%d)\n", pathname, dirfd, flags);
    PROLOG(openat);
    return orig_openat(dirfd, pathname, flags);
}

#define INRANGE(val, min, len) (val >= min && val < (min+len))
char *get_heap_name(IMG_DEV_VIRTADDR vaddr) {
    if(INRANGE(vaddr.uiAddr, SGX_VERTEXSHADER_HEAP_BASE, SGX_VERTEXSHADER_HEAP_SIZE)) {
        return "VertexShader";
    } else if(INRANGE(vaddr.uiAddr, SGX_PIXELSHADER_HEAP_BASE, SGX_PIXELSHADER_HEAP_SIZE)) {
        return "PixelShader";
    } else if(INRANGE(vaddr.uiAddr, SGX_PDSVERTEX_CODEDATA_HEAP_BASE, SGX_PDSVERTEX_CODEDATA_HEAP_SIZE)) {
        return "PDSVertexCodeData";
    } else if(INRANGE(vaddr.uiAddr, SGX_PDSPIXEL_CODEDATA_HEAP_BASE, SGX_PDSPIXEL_CODEDATA_HEAP_SIZE)) {
        return "PDSPixelCodeData";
    } else if(INRANGE(vaddr.uiAddr, SGX_GENERAL_HEAP_BASE, SGX_GENERAL_HEAP_SIZE)) {
        return "General";
    } else if(INRANGE(vaddr.uiAddr, SGX_SYNCINFO_HEAP_BASE, SGX_SYNCINFO_HEAP_SIZE)) {
        return "SyncInfo";
    } else {
        printf("Unhandled addr 0x%X\n", vaddr.uiAddr);
        assert(false);
    }
}

/* We need to hijack this function to get memory mappings since the pvr driver
 * calls `mmap2` through `syscall` meaning we can't hijack it with LD_PRELOAD
 */
int PVRSRVAllocDeviceMem(void* data, void* handle, uint32_t attribs, uint32_t size, uint32_t align, PVRSRV_CLIENT_MEM_INFO** info) {
    PROLOG(PVRSRVAllocDeviceMem);
    int ret = orig_PVRSRVAllocDeviceMem(data, handle, attribs, size, align, info);

    printf("allocated %x bytes of memory at %p : %x (%s)\n", size, (*info)->pvLinAddr, (*info)->sDevVAddr.uiAddr,
    get_heap_name((*info)->sDevVAddr));

    /* Clear memory so comparing dumps will be more consistent */
    memset((*info)->pvLinAddr, 0, size);

    return ret;
}
