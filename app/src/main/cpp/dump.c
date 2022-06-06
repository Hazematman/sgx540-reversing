#include <stdio.h>
#include <android/log.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <string.h>
#include <linux/ioctl.h>
#include <stdint.h>

#include "ioctl-map.h"
#include "structs.h"

static int (*real_ioctl)(int d, unsigned long request, char *argp)=NULL;

#define GPU_FILE "/dev/pvrsrvkm"
#define ASSERT_ARRAY(a) \
    sizeof(char[1-2*__builtin_types_compatible_p(__typeof__(a), __typeof__(&(a)[0]))])

#define S(a) \
    (ASSERT_ARRAY(a)*0 + sizeof(a)/sizeof((a)[0]))

static void mtrace_init(void) {
    real_ioctl = dlsym(RTLD_NEXT, "ioctl");
    if (NULL == real_ioctl) {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    }
}

extern ssize_t readlink(const char *restrict pathname, char *restrict buf,
                size_t bufsiz);


bool is_gpu(int fd) {
    char in[128] = { 0 }, out[128] = { 0 };
    snprintf(in, sizeof(in), "/proc/self/fd/%d", fd);

    int count = readlink(in, out, sizeof(out) - 1);
    return count == strlen(GPU_FILE) && strncmp(out, GPU_FILE, count) == 0;
}

int ioctl(int d, unsigned long request, void *argp) {
    if(real_ioctl==NULL) {
        mtrace_init();
    }

    int p = real_ioctl(d, request, argp);
    //this second check is technically not necessary
    if (is_gpu(d) && (_IOC_TYPE(request) == 0x67)) {
        PVRSRV_BRIDGE_PACKAGE* pack = (PVRSRV_BRIDGE_PACKAGE*)argp;
        uint32_t io = pack->ui32BridgeID;
        if (_IOC_NR(request) != _IOC_NR(io)) {
            __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "pack and bridge id different");
            return p;
        }

        int callnum = _IOC_NR(io);
        if (callnum < S(ioctl_names1)) {
            switch (callnum) {
                case 0x6: {
                    PVRSRV_BRIDGE_IN_ALLOCDEVICEMEM* sin = (PVRSRV_BRIDGE_IN_ALLOCDEVICEMEM*)pack->pvParamIn;
                    PVRSRV_BRIDGE_OUT_ALLOCDEVICEMEM* sout = (PVRSRV_BRIDGE_OUT_ALLOCDEVICEMEM*)pack->pvParamOut;
                    if (sout->eError != PVRSRV_OK) {
                        __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "error: %x", sout->eError);
                    }
                    __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "allocating %x bytes of memory %x", sin->uSize, sout->sClientMemInfo.uAllocSize);
                    break;
                }
                default:
                    __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "ioctl: type: %x number: %s size: %x", _IOC_TYPE(request), ioctl_names1[callnum], _IOC_SIZE(request));
            }
        } else {
            __android_log_print(ANDROID_LOG_VERBOSE, "PVR: UNKNOWN", "ioctl: type: %x number: %x size: %x", _IOC_TYPE(request), callnum, _IOC_SIZE(request));
        }
    }

    return p;
}
