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

static int (*real_ioctl)(int d, unsigned long request, char *argp)=NULL;
static void* (*real_mmap)(void *addr, size_t len, int prot, int flags,
           int fildes, off_t off);

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

void *mmap(void *addr, size_t len, int prot, int flags,
           int fildes, off_t off) {
    if(real_ioctl==NULL) {
        real_mmap = dlsym(RTLD_NEXT, "mmap");
        if (NULL == real_mmap) {
            fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
        }
    }

    void* ret = real_mmap(addr, len, prot, flags, fildes, off);

    if (fildes != 0 && is_gpu(fildes)) {
        __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "mmap call on gpu file descriptor %x", fildes);
    }

    return ret;
}

void** handles;
int* fds;
PVRSRV_BRIDGE_PACKAGE* packs;
PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA* mapped_addresses;

typedef struct { uint32_t key; uint32_t value; } HandleDevAddr;

HandleDevAddr* handle_addresses = NULL;

void signal_catcher(int signo, siginfo_t *info, void *context) {
    __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "Called signal");
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
            __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "mmap addr: %x handle: %x", new_mmap.uiUserVAddr, mapped_addresses[i].hMHandle);

            char* filename[4096];
/*
            uint32_t dev_addr = hmgetp_null(handle_addresses, (uint32_t)mapped_addresses[i].hMHandle)->value;
            if (dev_addr == 0) {
                __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "error with hashtable lookup");
                continue;
            }
            */
            sprintf(filename, "/sdcard/memory/%x-%x", (uint32_t)mapped_addresses[i].hMHandle, new_mmap.uiRealByteSize);
            __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "filename: %s handle: %x", filename, mapped_addresses[i].hMHandle);
            FILE* pFile = fopen((const char*)filename, "wb");
            fwrite((const void*)new_mmap.uiUserVAddr, new_mmap.uiRealByteSize, 1, pFile);
            fclose(pFile);
        }
    }
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
                    break;
                }
                case 0xB: {
                    //The driver seems to call mmap directly, so we cannot intercept it via LD_PRELOAD
                    //so we will simply map the memory for ourselves
                    PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA* mmap_data = (PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA*)pack->pvParamOut;
                    if (mmap_data->eError != PVRSRV_OK) {
                        return p;
                    }
                    //TODO we should remove stuff that is unmapped
                    arrput(fds, d);
                    arrput(packs, *pack);
                    arrput(mapped_addresses, (*(PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA*)pack->pvParamIn));
                    __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "the handle is: %x", ((PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA*)pack->pvParamIn)->hMHandle);
                    /*
                    if (last_mmap_data.hMHandle != 0) {
                        PVRSRV_BRIDGE_PACKAGE new_pack = *pack;
                        PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA new_mmap = {0};
                        new_pack = *pack;
                        new_pack.pvParamIn  = &last_mmap_data;
                        new_pack.pvParamOut = &new_mmap;
                        real_ioctl(d, request, (char*)&new_pack);
                        if (new_mmap.eError != PVRSRV_OK) {
                            __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "error with mmap hack");
                            return p;
                        }
                        __android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "mmap addr: %x", new_mmap.uiUserVAddr);
                    }
                    last_mmap_data = *(PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA*)(pack->pvParamIn);
                    */
                    //#define MMAP2 192
                    //__android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "get mmap data size: %x", mmap_data->uiRealByteSize);
                    //void* addr = (void*)syscall(MMAP2, mmap_data->uiRealByteSize, 0x1, 0x1, d, mmap_data->uiMMapOffset);
                    //__android_log_print(ANDROID_LOG_VERBOSE, "PVR:", "mmaped gpu memory address: %x, %s", addr, strerror(errno));
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
