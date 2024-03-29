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

#include "pprint.c"

#define MAX_FDS 128
#define PROLOG(func) \
    static typeof(func) *orig_##func = NULL; \
    if(!orig_##func) \
        orig_##func = dlsym(RTLD_NEXT, #func);

#define DRM_COMMAND_BASE 0x40

#define DEBUG_SYSCALL(msg) write(-1, (msg), sizeof(msg))

int ioctl(int fd, int request, ...);

enum pvr_heap {
    PVR_HEAP_VERTEX_SHADER,
    PVR_HEAP_PIXEL_SHADER,
    PVR_HEAP_PDS_VERTEX_CODE_DATA,
    PVR_HEAP_PDS_PIXEL_CODE_DATA,
    PVR_HEAP_GENERAL,
    PVR_HEAP_SYNC_INFO,
    PVR_HEAP_KERNEL_DATA,
    PVR_HEAP_TA_DATA,
    PVR_HEAP_SHARED_3DPARAMETERS,
    PVR_HEAP_PERCONTEXT_3DPARAMETERS,
};

static const char *pvr_heap_names[] = {
    [PVR_HEAP_VERTEX_SHADER] = "VertexShader",
    [PVR_HEAP_PIXEL_SHADER] = "PixelShader",
    [PVR_HEAP_PDS_VERTEX_CODE_DATA] = "PDSVertexCodeData",
    [PVR_HEAP_PDS_PIXEL_CODE_DATA] = "PDSPixelCodeData",
    [PVR_HEAP_GENERAL] = "General",
    [PVR_HEAP_SYNC_INFO] = "SyncInfo",
    [PVR_HEAP_KERNEL_DATA] = "KernelData",
    [PVR_HEAP_TA_DATA] = "TAData",
    [PVR_HEAP_SHARED_3DPARAMETERS] = "Shared3DParameters",
    [PVR_HEAP_PERCONTEXT_3DPARAMETERS] = "PerContext3DParameters",
};

enum mem_type {
    MEM_TYPE_NORMAL,
    MEM_TYPE_DISP,
    MEM_TYPE_MAP,
};

#define MAX_BUFFERS_TO_TRACK 256

static struct mem_entry {
    bool in_use;
    bool has_mmap[2];
    enum mem_type type;
    void *data[2];
    int prot[2];
    PVRSRV_CLIENT_MEM_INFO mem_info;
    PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA mmap_data[2];
} mem_trackings[MAX_BUFFERS_TO_TRACK], *last_mem_entry = NULL;

static struct fd_info {
    bool open;
    bool is_pvr;
} fds[MAX_FDS];

static FILE *log_file;
static void open_log_file() {
    if(log_file == NULL) {
        log_file = fopen("log.bin", "wb");
        assert(log_file);
    }
}

#define INRANGE(val, min, len) (val >= min && val < (min+len))
static enum pvr_heap get_heap(IMG_DEV_VIRTADDR vaddr) {
    if(INRANGE(vaddr.uiAddr, SGX_VERTEXSHADER_HEAP_BASE, SGX_VERTEXSHADER_HEAP_SIZE)) {
        return PVR_HEAP_VERTEX_SHADER;
    } else if(INRANGE(vaddr.uiAddr, SGX_PIXELSHADER_HEAP_BASE, SGX_PIXELSHADER_HEAP_SIZE)) {
        return PVR_HEAP_PIXEL_SHADER;
    } else if(INRANGE(vaddr.uiAddr, SGX_PDSVERTEX_CODEDATA_HEAP_BASE, SGX_PDSVERTEX_CODEDATA_HEAP_SIZE)) {
        return PVR_HEAP_PDS_VERTEX_CODE_DATA;
    } else if(INRANGE(vaddr.uiAddr, SGX_PDSPIXEL_CODEDATA_HEAP_BASE, SGX_PDSPIXEL_CODEDATA_HEAP_SIZE)) {
        return PVR_HEAP_PDS_PIXEL_CODE_DATA;
    } else if(INRANGE(vaddr.uiAddr, SGX_GENERAL_HEAP_BASE, SGX_GENERAL_HEAP_SIZE)) {
        return PVR_HEAP_GENERAL;
    } else if(INRANGE(vaddr.uiAddr, SGX_SYNCINFO_HEAP_BASE, SGX_SYNCINFO_HEAP_SIZE)) {
        return PVR_HEAP_SYNC_INFO;
    } else if(INRANGE(vaddr.uiAddr, SGX_KERNEL_DATA_HEAP_BASE, SGX_KERNEL_DATA_HEAP_SIZE)) {
        return PVR_HEAP_KERNEL_DATA;
    } else if(INRANGE(vaddr.uiAddr, SGX_TADATA_HEAP_BASE, SGX_TADATA_HEAP_SIZE)) {
        return PVR_HEAP_TA_DATA;
    } else if(INRANGE(vaddr.uiAddr, SGX_SHARED_3DPARAMETERS_HEAP_BASE, SGX_SHARED_3DPARAMETERS_HEAP_SIZE)) {
        return PVR_HEAP_SHARED_3DPARAMETERS;
    } else if(INRANGE(vaddr.uiAddr, SGX_PERCONTEXT_3DPARAMETERS_HEAP_BASE, SGX_PERCONTEXT_3DPARAMETERS_HEAP_SIZE)) {
        return PVR_HEAP_PERCONTEXT_3DPARAMETERS;
    }else {
        printf("Unhandled addr 0x%X\n", vaddr.uiAddr);
        assert(false);
    }
}

static bool bincmp32(uint8_t *ptr, uint32_t value) {
    uint8_t *bytes = &value;

    return /*ptr[0] == bytes[0] &&*/ ptr[1] == bytes[1] && ptr[2] == bytes[2] && ptr[3] == bytes[3];
}

static void clear_mem(struct mem_entry *mem) {
    enum pvr_heap heap = get_heap(mem->mem_info.sDevVAddr);
    bool valid_heap = heap == PVR_HEAP_VERTEX_SHADER
                      || heap == PVR_HEAP_PIXEL_SHADER
                      || heap == PVR_HEAP_PDS_VERTEX_CODE_DATA
                      || heap == PVR_HEAP_PDS_PIXEL_CODE_DATA
                      || heap == PVR_HEAP_GENERAL
                      //|| heap == PVR_HEAP_KERNEL_DATA
                      || heap == PVR_HEAP_PERCONTEXT_3DPARAMETERS
                      || heap == PVR_HEAP_SYNC_INFO;
    IMG_HANDLE handle = mem->mem_info.hKernelMemInfo;
    bool is_special_heap = handle == 0x14;
    if(mem->type == MEM_TYPE_NORMAL && (valid_heap || is_special_heap)) {
        for (int i = 0; i < 2; i++)
            if (mem->data[i])
                memset(mem->data[i], 0x00, mem->mem_info.uAllocSize);
    }

/* TODO, it might be help to track all writes to the command buffer
 * in order to do this we would need to handle SEGV to figure out
 * what is being written to memory and then recover from it.
 * One thing to note is that 0x14 (which appears to be the memory for
 * the CCB in my test app) gets written to by memcpy. so in theory
 * we might be able to just hook into memcpy and see if the addr
 * is part of CCB.
 */
#if 0
    if(is_special_heap) {
        mprotect(mem->data, mem->mem_info.uAllocSize, PROT_READ);
    }
#endif
}

static struct mem_entry *add_mmap_pointer(void* data, int prot, size_t length, uintptr_t pgoffset) {
    int track_arr_size = sizeof(mem_trackings)/sizeof(mem_trackings[0]);
    for(int i = 0; i < track_arr_size; i++) {
        struct mem_entry *mem = &mem_trackings[i];

        for (int j = 0; j < 2; j++) {
            if (mem->in_use && mem->has_mmap[j]) {
                if (mem->mmap_data[j].uiMMapOffset == pgoffset && mem->mmap_data[j].uiRealByteSize == length) {
                    mem->data[j] = data;
                    mem->prot[j] = prot;
                    return mem;
                }
            }
        }
    }

    printf("Failed to find matching tracking entry\n");
    assert(false);
}

static void track_buffer(PVRSRV_CLIENT_MEM_INFO *mem_info, enum mem_type type) {
    int track_arr_size = sizeof(mem_trackings)/sizeof(mem_trackings[0]);
    for(int i = 0; i < track_arr_size; i++) {
        struct mem_entry *mem = &mem_trackings[i];
        if(!mem->in_use) {
            mem->mem_info = *mem_info;
            mem->in_use = true;
            mem->type = type;

            printf("Allocated %x bytes of memory at %p : %x (%s)\n",
                   mem_info->uAllocSize, mem_info->pvLinAddr, mem_info->sDevVAddr.uiAddr,
                   pvr_heap_names[get_heap(mem_info->sDevVAddr)]);
            if(type == MEM_TYPE_DISP) {
                /* Override handle for tracking system */
                /* TODO this might not be the best approach, modify it causes problems with
                 * disp class memory */
                mem->mem_info.hKernelMemInfo = mem->mem_info.hMappingInfo;
            }
            printf("TRACKING %p\n", mem->mem_info.hMappingInfo);
            printf(" Next is %p\n", mem->mem_info.psNext);
            return;
        }
    }

    printf("Ran out of slot to track memory\n");
    assert(false);
}

static void add_mmap_data(int fd, IMG_HANDLE handle, PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA *mmap_data) {
    int track_arr_size = sizeof(mem_trackings)/sizeof(mem_trackings[0]);
    for(int i = 0; i < track_arr_size; i++) {
        struct mem_entry *mem = &mem_trackings[i];
        bool primary_handle_valid = mem->mem_info.hKernelMemInfo == handle;
        /* TODO assume next handle is just incremented of out handle
         * We do this because psNext doesn't seem to be a virt addr but we need to track the handle
         * Because this handle will be used mmap_info iotctl
         * this assumption may be wrong */
        bool secondary_handle_valid = mem->mem_info.psNext && (mem->mem_info.hKernelMemInfo+1 == handle);
        if(mem->in_use && !mem->has_mmap[0] && primary_handle_valid) {
            mem->mmap_data[0] = *mmap_data;
            mem->has_mmap[0] = true;
            last_mem_entry = mem;
            return;
        } else if(mem->in_use && !mem->has_mmap[1] && secondary_handle_valid) {
            mem->mmap_data[1] = *mmap_data;
            mem->has_mmap[1] = true;
            last_mem_entry = mem;
            return;
        }
    }

    printf("Could not find mem with handle %p\n", handle);
    assert(false);
}

#if 0
static void add_mmap_addr(IMG_HANDLE handle, void *data) {
    int track_arr_size = sizeof(mem_trackings)/sizeof(mem_trackings[0]);
    for(int i = 0; i < track_arr_size; i++) {
        struct mem_entry *mem = &mem_trackings[i];
        if(mem->in_use && mem->mem_info.hKernelMemInfo == handle) {
            mem->data = data;
            return;
        }
    }

    printf("Could not find mem with handle %p\n", handle);
    assert(false);
}
#endif

static char str_buf[4096];

static void dump_buffer(struct mem_entry *mem, int buf_index, const char *name) {
    FILE *fp = fopen(name, "wb");
    if(!fp) {
        printf("Failed to open buffer file %s", name);
        assert(false);
    }
    fwrite(mem->data[buf_index], 1, mem->mem_info.uAllocSize, fp);
    fclose(fp);
}

static void dump_tracked_buffers() {
    struct stat st = {0};
    if(stat("buffers", &st) == -1) {
        mkdir("buffers", 0700);
    }
    int track_arr_size = sizeof(mem_trackings)/sizeof(mem_trackings[0]);
    for(int i = 0; i < track_arr_size; i++) {
        struct mem_entry *mem = &mem_trackings[i];
        for (int j = 0; j < 2; j++) {
            if(mem->in_use && mem->data[j]) {
                sprintf(str_buf, "buffers/0x%x_%s.bin", (uintptr_t)mem->mem_info.hKernelMemInfo + j,
                        pvr_heap_names[get_heap(mem->mem_info.sDevVAddr)]);
                dump_buffer(mem, j, str_buf);
            }
        }
    }
}

static struct mem_entry *get_buffer(IMG_HANDLE mem_handle) {
    int track_arr_size = sizeof(mem_trackings)/sizeof(mem_trackings[0]);
    for(int i = 0; i < track_arr_size; i++) {
        struct mem_entry *mem = &mem_trackings[i];
        //for (int j = 0; j < 2; j++) {
            if (mem->in_use && (mem->mem_info.hKernelMemInfo) == mem_handle) {
                return mem;
            }
        //}
    }
    return NULL;
}

static void create_buffer(int fd, PVRSRV_BRIDGE_PACKAGE *in_bridge, IMG_HANDLE cookie, int heap, int size, int attrib)
{
    PROLOG(ioctl);
    PVRSRV_BRIDGE_IN_ALLOCDEVICEMEM in_alloc = {
        //.ui32BridgeFlags = 0xffffffff,
        .hDevCookie = cookie,
        .hDevMemHeap = heap,
        .ui32Attribs = attrib,
        .uSize = size,
        .uAlignment = 64,
    };

    PVRSRV_BRIDGE_OUT_ALLOCDEVICEMEM out_alloc = {0};

    PVRSRV_BRIDGE_PACKAGE alloc = {
        .ui32BridgeID = PVRSRV_BRIDGE_ALLOC_DEVICEMEM,
        .ui32Size = sizeof(alloc),
        .pvParamIn = &in_alloc,
        .ui32InBufferSize = sizeof(in_alloc),
        .pvParamOut = &out_alloc,
        .ui32OutBufferSize = sizeof(out_alloc),
        .hKernelServices = in_bridge->hKernelServices
    };

    long ret = orig_ioctl(fd, DRM_IOCTL_PVR_SRVKM, &alloc);

    if (ret != 0 || out_alloc.eError != PVRSRV_OK) {
        printf("Failed to alloc patch buffer\n");
    } else {
        printf("Created alloc buffer\n");
    }
}

static IMG_HANDLE new_magic_handle = NULL;
static PVRSRV_CLIENT_MEM_INFO new_magic_mem;
static IMG_HANDLE create_patchable_buffer(int fd, PVRSRV_BRIDGE_PACKAGE *in_bridge, IMG_HANDLE cookie) {
    PROLOG(ioctl);
    PROLOG(syscall);
    PVRSRV_BRIDGE_IN_ALLOCDEVICEMEM in_alloc = {
        //.ui32BridgeFlags = 0xffffffff,
        .hDevCookie = cookie,
        .hDevMemHeap = 0x9,
        .ui32Attribs = 0x9,
        .uSize = 524288,
        .uAlignment = 64,
    };

    PVRSRV_BRIDGE_OUT_ALLOCDEVICEMEM out_alloc = {0};

    PVRSRV_BRIDGE_PACKAGE alloc = {
        .ui32BridgeID = PVRSRV_BRIDGE_ALLOC_DEVICEMEM,
        .ui32Size = sizeof(alloc),
        .pvParamIn = &in_alloc,
        .ui32InBufferSize = sizeof(in_alloc),
        .pvParamOut = &out_alloc,
        .ui32OutBufferSize = sizeof(out_alloc),
        .hKernelServices = in_bridge->hKernelServices
    };

    long ret = orig_ioctl(fd, DRM_IOCTL_PVR_SRVKM, &alloc);

    if (ret != 0 || out_alloc.eError != PVRSRV_OK) {
        printf("Failed to alloc patch buffer\n");
    } else {
        printf("Created alloc buffer\n");
    }

    PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA in_to_mmap = {
        //.ui32BridgeFlags = 0xffffffff,
        .hMHandle = out_alloc.sClientMemInfo.hKernelMemInfo,
    };

    PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA out_to_mmap = {0};

    PVRSRV_BRIDGE_PACKAGE to_mmap = {
        .ui32BridgeID = PVRSRV_BRIDGE_MHANDLE_TO_MMAP_DATA,
        .ui32Size = sizeof(to_mmap),
        .pvParamIn = &in_to_mmap,
        .ui32InBufferSize = sizeof(in_to_mmap),
        .pvParamOut = &out_to_mmap,
        .ui32OutBufferSize = sizeof(out_to_mmap),
        .hKernelServices = in_bridge->hKernelServices
    };

    ret = orig_ioctl(fd, DRM_IOCTL_PVR_SRVKM, &to_mmap);

    if (ret != 0 || out_alloc.eError != PVRSRV_OK) {
        printf("Failed to mmap data patch buffer\n");
    } else {
        printf("Got mmap data\n");
    }


    ret = orig_syscall(SYS_mmap2, 0, out_to_mmap.uiRealByteSize, 3, 1, fd, out_to_mmap.uiMMapOffset);

    if (ret == MAP_FAILED) {
        printf("Failed to mmap patch buffer\n");
    } else {
        printf("Mapped patch buffer\n");
    }
    uint8_t *addr = (uint8_t*)ret;
    new_magic_mem = out_alloc.sClientMemInfo;

#if 1
    FILE *fp = fopen("./buffers_red/0x73_PDSPixelCodeData.bin", "rb");
    fread(addr, 1, 524288, fp);
    fclose(fp);
#endif

#if 1
    for (size_t i = 0; i < 524288; i += 4) {
        uint32_t *data = ((uint8_t*)addr) + i;
        if ((*data & 0xffffff00) == 0x0dc13000) {
            //printf("Memory %p is patched at 0x%x\n", mem->mem_info.hKernelMemInfo, i);
            *data = ((uint32_t)new_magic_mem.sDevVAddr.uiAddr) | (*data & 0xff);
            //cacheflush(mem->data, mem->mem_info.uAllocSize, DCACHE);
        }

    }
#endif

    munmap(addr, out_to_mmap.uiRealByteSize);

    printf("Created patchable buffer %d %d\n", out_alloc.eError, out_to_mmap.eError);


    return out_alloc.sClientMemInfo.hKernelMemInfo;
}

#include <sys/cachectl.h>

static void patch_buffers() {
    int track_arr_size = sizeof(mem_trackings)/sizeof(mem_trackings[0]);
    for(int i = 0; i < track_arr_size; i++) {
        struct mem_entry *mem = &mem_trackings[i];
#if 0
        if (mem->in_use && mem->mem_info.hKernelMemInfo == 0x73) {
            //memset(mem->data[0], 0x00, mem->mem_info.uAllocSize);
            uint32_t *magic_ptr = mem->data[0] + 0xc0;
            //*magic_ptr = 0x0;
            //*magic_ptr = new_magic_mem.sDevVAddr.uiAddr | 0xa0;
            *magic_ptr = 0xdeadbeef;
        }
#endif
#if 0
        if(mem->in_use && mem->mem_info.hKernelMemInfo == 0x73) {
            float *clear_color = ((uint8_t*)mem->data) + 0xac;
            clear_color[0] = 1.0f;
            clear_color[1] = 0.5f;
        }
#endif
#if 0
        else if(mem->in_use && mem->mem_info.hKernelMemInfo == 0x14) {
            uint32_t *cmds = ((uint8_t*)mem->data) + 0x00;
            cmds[0]= 0xDEADBEEF;
        }
#endif
        if(mem->in_use && mem->data[0] && mem->mem_info.hKernelMemInfo == 0x71) {
            printf("Patched buffer\n");
            uint8_t *data = ((uint8_t*)mem->data[0]) + 0x142;
            uint32_t important_bits = (new_magic_mem.sDevVAddr.uiAddr & 0x00FFF000) >> 12;
            uint8_t *ptr_bits = &important_bits;
            data[2] = ptr_bits[1];
            data[3] = ptr_bits[0];
            uint32_t *data32 = (uint32_t*)data;
            printf("Patched data is 0x%x real addr is 0x%x\n", *data32, new_magic_mem.sDevVAddr.uiAddr);
        }
#if 0
        int count = 0;
        if(mem->in_use && mem->data && (mem->prot & PROT_WRITE) && !mem->disp_mem/*mem->mem_info.hKernelMemInfo == 0x25*/) {
            //if (mem->mem_info.hKernelMemInfo == 0x73)
            //    continue;
            for (size_t i = 0; i < mem->mem_info.uAllocSize; i += 4) {
                uint32_t *data = ((uint8_t*)mem->data) + i;
                if ((*data & 0xffffff00) == 0x0dc13000) {
                    //if (count == 0)
                    {
                        printf("Memory %p is patched at 0x%x\n", mem->mem_info.hKernelMemInfo, i);
                        *data = ((uint32_t)new_magic_mem.sDevVAddr.uiAddr) | (*data & 0xff);
                    }
                    //count += 1;
                }
            }
        }
#endif

#if 0
        for (int j = 0; j < 2; j++) {
            if(mem->in_use && !mem->disp_mem && mem->data[j]) {
                for (size_t k = 0; k < mem->mem_info.uAllocSize; k += 2) {
                    uint16_t *data = ((uint8_t*)mem->data[j]) + k;
                    if ((*data) == 0x0130) {
                        printf("Memory %p is patched at 0x%x\n", mem->mem_info.hKernelMemInfo+j, k);
                        *data = (new_magic_mem.sDevVAddr.uiAddr - SGX_PDSPIXEL_CODEDATA_HEAP_BASE) >> 8;
                    }

                }
            }
        }
#endif
    }
}

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

static void pvrsrv_ioctl_post(int fd, PVRSRV_BRIDGE_PACKAGE *bridge_package, int ret) {
    int ioctl_nr = _IOC_NR(bridge_package->ui32BridgeID);
    switch(ioctl_nr) {
        case _IOC_NR(PVRSRV_BRIDGE_CONNECT_SERVICES):
            PPRINT(stdout, bridge_package->pvParamOut, PVRSRV_BRIDGE_OUT_CONNECT_SERVICES);
            break;
        case _IOC_NR(PVRSRV_BRIDGE_ENUM_DEVICES):
            PPRINT(stdout, bridge_package->pvParamOut, PVRSRV_BRIDGE_OUT_ENUMDEVICE);
            break;
        case _IOC_NR(PVRSRV_BRIDGE_ACQUIRE_DEVICEINFO):
            PPRINT(stdout, bridge_package->pvParamOut, PVRSRV_BRIDGE_OUT_ACQUIRE_DEVICEINFO);
            break;
        case _IOC_NR(PVRSRV_BRIDGE_SGX_REGISTER_HW_RENDER_CONTEXT):
            /* TODO figure out if we can find cmd submits from this */
            {
                PVRSRV_BRIDGE_OUT_SGX_REGISTER_HW_RENDER_CONTEXT *data = bridge_package->pvParamOut;
                printf("HW Render Context handle %p\n", data->hHWRenderContext);
                printf("HW Render Context %p\n", data);
                break;
            }
        case _IOC_NR(PVRSRV_BRIDGE_ALLOC_DEVICEMEM):
            {
                PVRSRV_BRIDGE_OUT_ALLOCDEVICEMEM *mem_data = bridge_package->pvParamOut;
                PVRSRV_CLIENT_MEM_INFO *mem = &mem_data->sClientMemInfo;
                track_buffer(mem, MEM_TYPE_NORMAL);
                printf("Alloc %p\n", mem->hKernelMemInfo);
            }
            break;
        case _IOC_NR(PVRSRV_BRIDGE_MHANDLE_TO_MMAP_DATA):
            {
                PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA *in_data = bridge_package->pvParamIn;
                PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA *out_data = bridge_package->pvParamOut;
                add_mmap_data(fd, in_data->hMHandle, out_data);
                printf("MHANDLE to MMAP data %p\n", in_data->hMHandle);
            }
            break;
        case _IOC_NR(PVRSRV_BRIDGE_MAP_DEVICECLASS_MEMORY):
            {
                PVRSRV_BRIDGE_OUT_MAP_DEVICECLASS_MEMORY *out_data = bridge_package->pvParamOut;
                track_buffer(&out_data->sClientMemInfo, MEM_TYPE_DISP);
                printf("Disp class %p\n", out_data->sClientMemInfo.hMappingInfo);
            }
            break;
        case _IOC_NR(PVRSRV_BRIDGE_MAP_DEV_MEMORY):
            {
                PVRSRV_BRIDGE_IN_MAP_DEV_MEMORY *in = bridge_package->pvParamIn;
                PVRSRV_BRIDGE_OUT_MAP_DEV_MEMORY *out_data = bridge_package->pvParamOut;
                track_buffer(&out_data->sDstClientMemInfo, MEM_TYPE_MAP);
                printf("Map dev mem %p\n", out_data->sDstClientMemInfo.hKernelMemInfo);
                printf("Mapping %p %p\n", in->hKernelMemInfo, in->hDstDevMemHeap);
            }
            break;
        case _IOC_NR(PVRSRV_BRIDGE_CREATE_DEVMEMCONTEXT):
            {
                PVRSRV_BRIDGE_OUT_CREATE_DEVMEMCONTEXT *out = bridge_package->pvParamOut;
                printf("Devmem context %p\n", out);
                //PPRINT(stdout, bridge_package->pvParamIn, PVRSRV_BRIDGE_IN_CREATE_DEVMEMCONTEXT);
                //fwrite(bridge_package->pvParamIn, 1, sizeof(PVRSRV_BRIDGE_IN_CREATE_DEVMEMCONTEXT), log_file);
            }
            break;

        case _IOC_NR(PVRSRV_BRIDGE_GET_MISC_INFO):
            {
                PVRSRV_BRIDGE_IN_GET_MISC_INFO *in = bridge_package->pvParamIn;
                printf("Getting misc info %u\n", in->sMiscInfo.ui32StateRequest);
            }
            break;
        case _IOC_NR(PVRSRV_BRIDGE_SGX_GETMISCINFO):
            {
                PVRSRV_BRIDGE_IN_SGXGETMISCINFO *in = bridge_package->pvParamIn;
                printf("Getting SGX misc info %u\n", in->psMiscInfo->eRequest);
            }
            break;
        case _IOC_NR(PVRSRV_BRIDGE_SGX_DOKICK):
            /* This is the command to kick off command execution */
            {
                PVRSRV_BRIDGE_IN_DOKICK *ccb = bridge_package->pvParamIn;

                printf("CCB offset 0x%x\n", ccb->sCCBKick.ui32CCBOffset);
                printf("Cookie %p\n", ccb->hDevCookie);
                printf("Dev Mem Context %p\n", ccb->sCCBKick.hDevMemContext);
                printf("Kernel mem handle %p\n", ccb->sCCBKick.hCCBKernelMemInfo);
                struct mem_entry *cmd_buf = get_buffer(ccb->sCCBKick.hCCBKernelMemInfo);
                printf("Dumping %p as ccb buffer\n", cmd_buf->mem_info.hKernelMemInfo);
                dump_buffer(cmd_buf, 0, "./cmd.bin");
                SGXMKIF_CMDTA_SHARED *ccb_data = (cmd_buf->data[0] + ccb->sCCBKick.ui32CCBOffset);
                printf("CCB data %p\n", ccb_data);
            }
            break;
    }
}

static bool pvrsrv_ioctl(int fd, PVRSRV_BRIDGE_PACKAGE *bridge_package) {
    int ioctl_nr = _IOC_NR(bridge_package->ui32BridgeID);
    printf(">>> pvr_ioctl(%s)\n", pvrsrv_ioctl_names[ioctl_nr]);
    switch(ioctl_nr) {
        case _IOC_NR(PVRSRV_BRIDGE_MHANDLE_TO_MMAP_DATA):
            /* TODO track memory */
            break;
        case _IOC_NR(PVRSRV_BRIDGE_ALLOC_DEVICEMEM):
            {
                PVRSRV_BRIDGE_IN_ALLOCDEVICEMEM *mem_data = bridge_package->pvParamIn;
#if 0
                if (mem_data->hDevMemHeap == 0x9 && mem_data->uSize == 524288) {
                    create_buffer(fd, bridge_package, mem_data->hDevCookie, 0x9, 524288, 0x9);
                    printf("Created buffer to offset\n");
                }
#endif
                printf("Alloc %p\n", mem_data);
            }
            break;
        case _IOC_NR(PVRSRV_BRIDGE_SGX_REGISTER_HW_RENDER_CONTEXT):
            /* TODO figure out if we can find cmd submits from this */
            {
                PVRSRV_BRIDGE_IN_SGX_REGISTER_HW_RENDER_CONTEXT *data = bridge_package->pvParamIn;
                printf("HW Render Context %p\n", data);
                for (IMG_UINT32 i = 0; i < data->ui32HWRenderContextSize; i += sizeof(IMG_UINT32)) {
                    IMG_UINT32 *t = (IMG_UINT32*)&data->pHWRenderContextCpuVAddr[i];
                    printf("\tvalue is 0x%x\n", *t);
                }
                break;
            }
        case _IOC_NR(PVRSRV_BRIDGE_SGX_REGISTER_HW_TRANSFER_CONTEXT):
            /* TODO figure out if we can find cmd submits from this */
            break;
        case _IOC_NR(PVRSRV_BRIDGE_MAP_DEVICECLASS_MEMORY):
            /* TODO figure out if we need to track this memory */
            break;
        case _IOC_NR(PVRSRV_BRIDGE_RELEASE_MMAP_DATA):
            /* TODO track memory */
            break;
        case _IOC_NR(PVRSRV_BRIDGE_UNMAP_DEVICECLASS_MEMORY):
            /* TODO track memory */
            break;
        case _IOC_NR(PVRSRV_BRIDGE_MAP_DEV_MEMORY):
            /* TODO track memory */
            break;
        case _IOC_NR(PVRSRV_BRIDGE_UNMAP_DEV_MEMORY):
            /* TODO track memory */
            break;
        case _IOC_NR(PVRSRV_BRIDGE_FREE_DEVICEMEM):
            /* TODO track memory */
            break;
        case _IOC_NR(PVRSRV_BRIDGE_SGX_DOKICK):
            /* This is the command to kick off command execution */
            {
                int num_buffers = 0;
                int track_arr_size = sizeof(mem_trackings)/sizeof(mem_trackings[0]);
                for(int i = 0; i < track_arr_size; i++) {
                    struct mem_entry *mem = &mem_trackings[i];
                    if(mem->in_use) {
                        num_buffers += 1;
                    }
                }
                PVRSRV_BRIDGE_IN_DOKICK *ccb = bridge_package->pvParamIn;
                printf("Tracking %d buffers\n", num_buffers);
                new_magic_handle = create_patchable_buffer(fd, bridge_package, ccb->hDevCookie);
                dump_tracked_buffers();
                patch_buffers();

                printf("CCB offset 0x%x\n", ccb->sCCBKick.ui32CCBOffset);
                printf("Cookie %p\n", ccb->hDevCookie);
                printf("Dev Mem Context %p\n", ccb->sCCBKick.hDevMemContext);
                printf("Kernel mem handle %p\n", ccb->sCCBKick.hCCBKernelMemInfo);
                //SGXMKIF_CMDTA_SHARED *ccb_data = get_buffer(ccb->sCCBKick.hCCBKernelMemInfo);
                //printf("CCB data %p\n", ccb_data);
            }
            break;
        case _IOC_NR(PVRSRV_BRIDGE_SGX_SET_RENDER_CONTEXT_PRIORITY):
        case _IOC_NR(PVRSRV_BRIDGE_SGX_SET_TRANSFER_CONTEXT_PRIORITY):
            /* TODO figure out if this is important */
            break;
        case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_WAIT):
        case _IOC_NR(PVRSRV_BRIDGE_SYNC_OPS_FLUSH_TO_TOKEN):
        case _IOC_NR(PVRSRV_BRIDGE_SYNC_OPS_TAKE_TOKEN):
        case _IOC_NR(PVRSRV_BRIDGE_OPEN_DISPCLASS_DEVICE):
        case _IOC_NR(PVRSRV_BRIDGE_GET_DISPCLASS_INFO):
        case _IOC_NR(PVRSRV_BRIDGE_CLOSE_DISPCLASS_DEVICE):
        case _IOC_NR(PVRSRV_BRIDGE_ENUM_DISPCLASS_FORMATS):
        case _IOC_NR(PVRSRV_BRIDGE_ENUM_DISPCLASS_DIMS):
        case _IOC_NR(PVRSRV_BRIDGE_GET_DISPCLASS_SYSBUFFER):
        case _IOC_NR(PVRSRV_BRIDGE_ENUM_CLASS):
        case _IOC_NR(PVRSRV_BRIDGE_SGX_GETINTERNALDEVINFO):
            break;
        case _IOC_NR(PVRSRV_BRIDGE_CONNECT_SERVICES):
            PPRINT(stdout, bridge_package->pvParamIn, PVRSRV_BRIDGE_IN_CONNECT_SERVICES);
            //fwrite(bridge_package->pvParamIn, 1, sizeof(PVRSRV_BRIDGE_IN_CONNECT_SERVICES), log_file);
            break;
        case _IOC_NR(PVRSRV_BRIDGE_ENUM_DEVICES):
            break;
        case _IOC_NR(PVRSRV_BRIDGE_ACQUIRE_DEVICEINFO):
            PPRINT(stdout, bridge_package->pvParamIn, PVRSRV_BRIDGE_IN_ACQUIRE_DEVICEINFO);
            //fwrite(bridge_package->pvParamIn, 1, sizeof(PVRSRV_BRIDGE_IN_ACQUIRE_DEVICEINFO), log_file);
            break;
        case _IOC_NR(PVRSRV_BRIDGE_SGX_GETMISCINFO):
            PPRINT(stdout, bridge_package->pvParamIn, PVRSRV_BRIDGE_IN_SGXGETMISCINFO);
            //fwrite(bridge_package->pvParamIn, 1, sizeof(PVRSRV_BRIDGE_IN_SGXGETMISCINFO), log_file);
            break;
        case _IOC_NR(PVRSRV_BRIDGE_DISCONNECT_SERVICES):
            break;
        case _IOC_NR(PVRSRV_BRIDGE_CREATE_DEVMEMCONTEXT):
            PPRINT(stdout, bridge_package->pvParamIn, PVRSRV_BRIDGE_IN_CREATE_DEVMEMCONTEXT);
            //fwrite(bridge_package->pvParamIn, 1, sizeof(PVRSRV_BRIDGE_IN_CREATE_DEVMEMCONTEXT), log_file);
            break;
        case _IOC_NR(PVRSRV_BRIDGE_SGX_GETCLIENTINFO):
            PPRINT(stdout, bridge_package->pvParamIn, PVRSRV_BRIDGE_IN_GETCLIENTINFO);
            //fwrite(bridge_package->pvParamIn, 1, sizeof(PVRSRV_BRIDGE_IN_GETCLIENTINFO), log_file);
            break;
        case _IOC_NR(PVRSRV_BRIDGE_GET_MISC_INFO):
        case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_OPEN):
        case _IOC_NR(PVRSRV_BRIDGE_GET_DEVMEM_HEAPINFO):
        case _IOC_NR(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_RENDER_CONTEXT):
        case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_CLOSE):
        case _IOC_NR(PVRSRV_BRIDGE_SGX_RELEASECLIENTINFO):
        case _IOC_NR(PVRSRV_BRIDGE_DESTROY_DEVMEMCONTEXT):
            /* TODO figure out if these ioctl need to be handled */
            break;
        default:
            printf("Unimplemented pvrsrv ioctl %d, may be %s\n", ioctl_nr, pvrsrv_ioctl_names[ioctl_nr]);
            assert(false);
            break;
    }
    return false;
}

static bool pvr_ioctl_pre(int fd, int request, void *ptr) {
    //fwrite(&request, 1, sizeof(request), log_file);
    int ioctl_nr = _IOC_NR(request);
    switch(ioctl_nr) {
        case _IOC_NR(DRM_IOCTL_VERSION):
            printf(">>> ioctl(DRM_IOCTL_VERSION)\n");
            break;
        case _IOC_NR(DRM_IOCTL_GET_MAGIC):
            printf(">>> ioctl(DRM_IOCTL_GET_MAGIC)\n");
            break;
        case _IOC_NR(DRM_IOCTL_GET_UNIQUE):
            printf(">>> ioctl(DRM_IOCTL_GET_UNIQUE)\n");
            break;
        case _IOC_NR(DRM_IOCTL_SET_VERSION):
            printf(">>> ioctl(DRM_IOCTL_SET_VERSION)\n");
            {
                struct drm_set_version *data = ptr;
                printf("\t%d %d %d %d\n", data->drm_di_major,
                                        data->drm_di_minor,
                                        data->drm_dd_major,
                                        data->drm_dd_minor);
            }
            break;
        case PVR_DRM_SRVKM_CMD:
            //fwrite(ptr, 1, sizeof(PVRSRV_BRIDGE_PACKAGE), log_file);
            //PPRINT(stdout, ptr, PVRSRV_BRIDGE_PACKAGE);
            return pvrsrv_ioctl(fd, ptr);
            break;
        case PVR_DRM_IS_MASTER_CMD:
            printf(">>> ioctl(PVR_DRM_IS_MASTER_CMD)\n");
            /* From KMD source code this seems to always return 0 */
            break;
        default:
            printf("Unimplemented ioctl 0x%x\n", ioctl_nr);
            assert(false);
            break;
    }
    return false;
}

static void pvr_ioctl_post(int fd, int request, void *ptr, int ret) {
    int ioctl_nr = _IOC_NR(request);
    switch(ioctl_nr) {
        case PVR_DRM_SRVKM_CMD:
            pvrsrv_ioctl_post(fd, ptr, ret);
            break;
    }
}

int ioctl(int fd, int request, ...) {
    PROLOG(ioctl);
    //open_log_file();
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

    bool skip = false;
    if(pvr) {
        skip = pvr_ioctl_pre(fd, request, ptr);
    }

    int ret = 0;
    if(!skip)
        ret = orig_ioctl(fd, request, ptr);

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

long syscall(long number, ...) {
    PROLOG(syscall);
    if(number == SYS_mmap2) {
        va_list args;
        va_start(args, number);
        unsigned long addr = va_arg(args, unsigned long);
        unsigned long length = va_arg(args, unsigned long);
        unsigned long prot = va_arg(args, unsigned long);
        unsigned long flags = va_arg(args, unsigned long);
        unsigned long fd = va_arg(args, unsigned long);
        unsigned long pgoffset = va_arg(args, unsigned long);
        long ret = orig_syscall(number, addr, length, prot, flags, fd, pgoffset);
        printf("mmap2 called with 0x%lx\n", ret);
        last_mem_entry = add_mmap_pointer(ret, prot, length, pgoffset);
        clear_mem(last_mem_entry);
        return ret;
    } else {
        printf("Unhandled syscall 0x%lx\n", number);
    }

    assert(false);
    return 0;
}



#if 0
void * memcpy ( void * destination, const void * source, size_t num ) {
    PROLOG(memcpy);

    for(size_t i = 0; i < num; i += 1) {
        uint32_t *data = ((uint8_t*)source) + i;
        //if ((*data & 0xffffff00) == 0x0dc13000) {
        if (bincmp32(data, 0x0dc13000)) {
            printf("Found pointer at %p\n", data);
        }
    }

    return orig_memcpy(destination, source, num);
}
#endif

#if 0
drmVersionPtr drmGetVersion(int fd) {
    PROLOG(drmGetVersion);
    printf(">>> drmGetVersion(%d)\n", fd);
    return orig_drmGetVersion(fd);
}

int drmSetInterfaceVersion(int fd, drmSetVersion *version) {
    PROLOG(drmSetInterfaceVersion);
    printf(">>> drmSetInterfaceVersion(%d, ...)", fd);
    return orig_drmSetInterfaceVersion(fd);
}
#endif

#if 0
/* We need to hijack this function to get memory mappings since the pvr driver
 * calls `mmap2` through `syscall` meaning we can't hijack it with LD_PRELOAD
 * This function does not get every memory mapping though
 */
int PVRSRVAllocDeviceMem(void* data, void* handle, uint32_t attribs, uint32_t size, uint32_t align, PVRSRV_CLIENT_MEM_INFO** info) {
    PROLOG(PVRSRVAllocDeviceMem);
    int ret = orig_PVRSRVAllocDeviceMem(data, handle, attribs, size, align, info);

    printf("allocated %x bytes of memory at %p : %x (%s)\n", size, (*info)->pvLinAddr, (*info)->sDevVAddr.uiAddr,
    pvr_heap_names[get_heap((*info)->sDevVAddr)]);
    printf("Heap handle %p\n", handle);

    /* Clear memory so comparing dumps will be more consistent */
    memset((*info)->pvLinAddr, 0, size);

    /* Track userspace pointer here since this avoids the double mmap issue */
    add_mmap_addr((*info)->hKernelMemInfo, (*info)->pvLinAddr);

    return ret;
}
#endif
