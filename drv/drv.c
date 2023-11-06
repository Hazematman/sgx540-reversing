#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <errno.h>

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

#define DRM_IOCTL_PVR_SRVKM DRM_IOWR(PVR_DRM_SRVKM_CMD, PVRSRV_BRIDGE_PACKAGE)

#include <pprint.c>

#define UNUSED(x) (void)x

struct driver_state {
    int fd;
    IMG_HANDLE kernel_services;
    IMG_UINT32 device_idx;
    IMG_HANDLE dev_cookie;
};

static void check_pvr_error(PVRSRV_ERROR error) {
    if(error != PVRSRV_OK) {
        printf("Got error %s\n", PPRINT(NULL, error, PVRSRV_ERROR));
        assert(false);
    }
}

static void check_ioctl(int fd, long request, void *data) {
    int ret = drmIoctl(fd, request, data);
    if(ret != 0) {
        printf("IOCTL ret value %d, %d, %s\n", ret, errno, strerror(errno));
        assert(ret == 0);
    }
}

static void connect_service(struct driver_state *state) {
    PVRSRV_BRIDGE_IN_CONNECT_SERVICES in = {
        .ui32BridgeFlags = 0xDEADBEEF
    };
    PVRSRV_BRIDGE_OUT_CONNECT_SERVICES out = {.eError = 0};
    PVRSRV_BRIDGE_PACKAGE data = {
        .ui32BridgeID = PVRSRV_BRIDGE_CONNECT_SERVICES,
        .ui32Size = sizeof(data),
        .pvParamIn = &in,
        .ui32InBufferSize = sizeof(in),
        .pvParamOut = &out,
        .ui32OutBufferSize = sizeof(out),
        .hKernelServices = 0,
    };

    check_ioctl(state->fd, DRM_IOCTL_PVR_SRVKM, &data);
    //PPRINT(stdout, &out, PVRSRV_BRIDGE_OUT_CONNECT_SERVICES);
    check_pvr_error(out.eError);
    state->kernel_services = out.hKernelServices;
}

static void enum_devices(struct driver_state *state) {
    PVRSRV_BRIDGE_OUT_ENUMDEVICE out;
    PVRSRV_BRIDGE_PACKAGE data = {
        .ui32BridgeID = PVRSRV_BRIDGE_ENUM_DEVICES,
        .ui32Size = sizeof(data),
        .pvParamIn = NULL,
        .ui32InBufferSize = 0,
        .pvParamOut = &out,
        .ui32OutBufferSize = sizeof(out),
        .hKernelServices = state->kernel_services
    };

    check_ioctl(state->fd, DRM_IOCTL_PVR_SRVKM, &data);
    check_pvr_error(out.eError);

    bool gpu_found = false;
    for(uint32_t i = 0; i < out.ui32NumDevices; i++) {
        PVRSRV_DEVICE_IDENTIFIER *di = &out.asDeviceIdentifier[i];
        if(di->eDeviceType == PVRSRV_DEVICE_TYPE_SGX && di->eDeviceClass == PVRSRV_DEVICE_CLASS_3D) {
            state->device_idx = di->ui32DeviceIndex;
            gpu_found = true;
            break;
        }
    }

    assert(gpu_found);
}

static void acquire_devinfo(struct driver_state *state) {
    PVRSRV_BRIDGE_IN_ACQUIRE_DEVICEINFO in = {
        .uiDevIndex = state->device_idx,
        .eDeviceType = PVRSRV_DEVICE_TYPE_UNKNOWN,
    };
    PVRSRV_BRIDGE_OUT_ACQUIRE_DEVICEINFO out;
    PVRSRV_BRIDGE_PACKAGE data = {
        .ui32BridgeID = PVRSRV_BRIDGE_ACQUIRE_DEVICEINFO,
        .ui32Size = sizeof(data),
        .pvParamIn = &in,
        .ui32InBufferSize = sizeof(in),
        .pvParamOut = &out,
        .ui32OutBufferSize = sizeof(out),
        .hKernelServices = state->kernel_services
    };

    check_ioctl(state->fd, DRM_IOCTL_PVR_SRVKM, &data);
    check_pvr_error(out.eError);

    state->dev_cookie = out.hDevCookie;
}

static void sgx_get_misc_info(struct driver_state *state) {
    SGX_MISC_INFO misc_info = {
        .eRequest = SGX_MISC_INFO_REQUEST_DRIVER_SGXREV,
    };
    PVRSRV_BRIDGE_IN_SGXGETMISCINFO in = {
        .hDevCookie = state->dev_cookie,
        .psMiscInfo = &misc_info,
    };
    PVRSRV_BRIDGE_RETURN out;
    PVRSRV_BRIDGE_PACKAGE data = {
        .ui32BridgeID = PVRSRV_BRIDGE_SGX_GETMISCINFO,
        .ui32Size = sizeof(data),
        .pvParamIn = &in,
        .ui32InBufferSize = sizeof(in),
        .pvParamOut = &out,
        .ui32OutBufferSize = sizeof(out),
        .hKernelServices = state->kernel_services,
    };

    check_ioctl(state->fd, DRM_IOCTL_PVR_SRVKM, &data);
    check_pvr_error(out.eError);

    /* TODO figure out what we do with misc info */
    PPRINT(stdout, &in, PVRSRV_BRIDGE_IN_SGXGETMISCINFO);
    fprintf(stdout, "\n");
    PPRINT(stdout, &in.psMiscInfo->uData.sSGXFeatures, PVRSRV_SGX_MISCINFO_FEATURES);
    fprintf(stdout, "\n");
}

static void create_devmemcontext(struct driver_state *state) {
    PVRSRV_BRIDGE_IN_CREATE_DEVMEMCONTEXT in = {
        .hDevCookie = state->dev_cookie
    };
    PVRSRV_BRIDGE_OUT_CREATE_DEVMEMCONTEXT out;
    PVRSRV_BRIDGE_PACKAGE data = {
        .ui32BridgeID = PVRSRV_BRIDGE_CREATE_DEVMEMCONTEXT,
        .ui32Size = sizeof(data),
        .pvParamIn = &in,
        .ui32InBufferSize = sizeof(in),
        .pvParamOut = &out,
        .ui32OutBufferSize = sizeof(out),
        .hKernelServices = state->kernel_services
    };

    check_ioctl(state->fd, DRM_IOCTL_PVR_SRVKM, &data);
    check_pvr_error(out.eError);
    PPRINT(stdout, &out, PVRSRV_BRIDGE_OUT_CREATE_DEVMEMCONTEXT);
}

int main(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    printf("Hello world!\n");
    struct driver_state state;

    state.fd = open("/dev/dri/card0", O_RDWR);
    assert(state.fd != -1 && "Failed to device file");

    drmVersionPtr version = drmGetVersion(state.fd);
    assert(version && "drmGetVersion failed");

    printf("Driver: %s\n", version->name);
    printf("Version %d.%d.%d\n", version->version_major, version->version_minor, version->version_patchlevel);
    printf("Date: %s\n", version->date);
    printf("Desc: %s\n", version->desc);
    printf("Uniq: %s\n", drmGetBusid(state.fd));

    //printf("DRM_IOCTL_VERSION = 0x%x\n", DRM_IOCTL_VERSION);
    //printf("PVR_DRM_SRVKM_CMD = 0x%x\n", DRM_IOCTL_PVR_SRVKM);
    connect_service(&state);
    enum_devices(&state);
    acquire_devinfo(&state);
    sgx_get_misc_info(&state);
    create_devmemcontext(&state);

    close(state.fd);

    return 0;
}
