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
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>

static int (*real_ioctl)(int d, unsigned long request, char *argp)=NULL;

#define GPU_FILE "/dev/pvrsrvkm"
#define ASSERT_ARRAY(a) \
    sizeof(char[1-2*__builtin_types_compatible_p(__typeof__(a), __typeof__(&(a)[0]))])

#define S(a) \
    (ASSERT_ARRAY(a)*0 + sizeof(a)/sizeof((a)[0]))

extern ssize_t readlink(const char *restrict pathname, char *restrict buf,
                size_t bufsiz);
extern long syscall(long number, ...);


bool is_gpu(int fd) {
    char in[128] = { 0 }, out[128] = { 0 };
    snprintf(in, sizeof(in), "/proc/self/fd/%d", fd);

    int count = readlink(in, out, sizeof(out) - 1);
    return count == strlen(GPU_FILE) && strncmp(out, GPU_FILE, count) == 0;
}

void** handles;
int* fds;
PVRSRV_BRIDGE_PACKAGE* packs;
PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA* mapped_addresses;

typedef struct { uint32_t key; uint32_t value; } HandleDevAddr;

HandleDevAddr* handle_addresses = NULL;

int count = 0;

#define IN_RANGE(val, min, len) (val >= min && val <= (min + len))
char* addr_to_name(size_t addr) {
    if (IN_RANGE(addr, 0x1000, 0xb7fe000)) {
        return "General";
    } else if (IN_RANGE(addr, 0xc800000, 0xfff000)){
        return "TAData";
    } else if (IN_RANGE(addr, 0xe400000, 0x7f000)) {
        return "KernelCode";
    } else if (IN_RANGE(addr, 0xf000000, 0x3ff000)) {
        return "KernelData";
    } else if (IN_RANGE(addr, 0xf400000, 0x4ff000)) {
        return "PixelShaderCode";
    } else if (IN_RANGE(addr, 0xfc00000, 0x1ff000)) {
        return "VertexShaderCode";
    } else if (IN_RANGE(addr, 0xdc00000, 0x7ff000)) {
        return "PDSPPixelCodeData";
    } else if (IN_RANGE(addr, 0xe800000, 0x7ff000)) {
        return "PDSPVertexCodeData";
    } else if (IN_RANGE(addr, 0xd800000, 0x3ff000)) {
        return "CacheCoherent";
    } else if (IN_RANGE(addr, 0xb800000, 0)) { //seemingly unused?
        return "Shared3DParameters";
    } else if (IN_RANGE(addr, 0xb800000, 0xfff000)) {
        return "PerContext3DParameters";
    } else {
        return "Unknown";
    }
}

void signal_catcher(int signo, siginfo_t *info, void *context) {
    __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "Called signal");
    return;
    for (int i = 0; i < arrlen(mapped_addresses); i++) {
        if (mapped_addresses[i].hMHandle != 0) {
            PVRSRV_BRIDGE_PACKAGE new_pack = packs[i];
            PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA new_mmap = {0};
            new_pack.pvParamIn  = &mapped_addresses[i];
            new_pack.pvParamOut = &new_mmap;
            real_ioctl(fds[i], new_pack.ui32BridgeID, (char*)&new_pack);
            if (new_mmap.eError != PVRSRV_OK) {
                __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "error with mmap hack");
                continue;
            }
            __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "mmap addr: %x handle: %x, gpu addr: %x", new_mmap.uiUserVAddr, mapped_addresses[i].hMHandle, hmget(handle_addresses, mapped_addresses[i].hMHandle));

            char* filename[4096];
            char* dirname[4096];

            sprintf(dirname, "/sdcard/memory/%d", count);
            mkdir(dirname, 0777);
//            sprintf(filename, "/sdcard/memory/%d/%x-%x", count, (uint32_t)hmget(handle_addresses, mapped_addresses[i].hMHandle), new_mmap.uiRealByteSize);
            sprintf(filename, "/sdcard/memory/%d/%s-%x-%x", count, addr_to_name((uint32_t)hmget(handle_addresses, mapped_addresses[i].hMHandle)), i , new_mmap.uiRealByteSize);
            __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "filename: %s handle: %x", filename, mapped_addresses[i].hMHandle);
            FILE* pFile = fopen((const char*)filename, "wb");
            fwrite((const void*)new_mmap.uiUserVAddr, new_mmap.uiRealByteSize, 1, pFile);
            fclose(pFile);
        }
    }
    count++;
}

int ioctl(int d, unsigned long request, void *argp) {
    if(real_ioctl==NULL) {
        real_ioctl = dlsym(RTLD_NEXT, "ioctl");
        if (NULL == real_ioctl) {
            fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
        }

        //also set up our signal handlers
        struct sigaction act = {0};
        struct sigaction old = {0};
        act.sa_sigaction = signal_catcher;
        act.sa_flags = SA_SIGINFO;
        if (0 != sigaction(SIGWINCH, &act, &old)) {
            __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "Failure setting signal handler");
        }
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
                    __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "allocating %x bytes of memory, handle: %x", sin->uSize, sout->sClientMemInfo.hKernelMemInfo);
                    hmput(handle_addresses, (uint32_t)sout->sClientMemInfo.hKernelMemInfo, sout->sClientMemInfo.sDevVAddr);
                    __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "base address of allocation: %x", sout->sClientMemInfo.sDevVAddr);
                    
                    hmput(handle_addresses, (uint32_t)sout->sClientMemInfo.hKernelMemInfo, sout->sClientMemInfo.sDevVAddr);

                    break;
                }
                case 0xB: {
                    PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA* mmap_data = (PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA*)pack->pvParamOut;
                    if (mmap_data->eError != PVRSRV_OK) {
                        return p;
                    }
                    //TODO we should remove stuff that is unmapped
                    arrput(fds, d);
                    arrput(packs, *pack);
                    arrput(mapped_addresses, (*(PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA*)pack->pvParamIn));
                    __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "the handle is: %x", ((PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA*)pack->pvParamIn)->hMHandle);

                    break;
                }
                default:
                    __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "ioctl: type: %x number: %s (%d) size: %x", _IOC_TYPE(request), ioctl_names1[callnum], callnum, _IOC_SIZE(request));
            }
        } else {
            __android_log_print(ANDROID_LOG_VERBOSE, "PVR: UNKNOWN", "ioctl: type: %x number: %x size: %x", _IOC_TYPE(request), callnum, _IOC_SIZE(request));
        }
    }

    return p;
}
