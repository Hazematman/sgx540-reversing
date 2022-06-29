#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
//TODO alternative for non musl/glibc (does this driver even run on other systems?)
#include <link.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define HOOK(fn_name, fn_args, fn_type, code) fn_type fn_name fn_args {\
    static fn_type (*real_ ## fn_name) fn_args ;\
    if (real_ ## fn_name == NULL) {\
        real_ ## fn_name = dlsym(RTLD_NEXT, #fn_name);\
        if (real_ ## fn_name == NULL) {\
            fprintf(stderr, "could not dlsym %s\n", #fn_name);\
        }\
    }\
    code\
}

typedef struct {
	/* CPU Virtual Address */
	void*	    			pvLinAddr;

	/* CPU Virtual Address (for kernel mode) */
	void*	    			pvLinAddrKM;

	/* Device Virtual Address */
	uint32_t        		sDevVAddr;

	/* allocation flags */
	uint32_t				ui32Flags;

	/* client allocation flags */
	uint32_t				ui32ClientFlags;

	/* allocation size in bytes */
	size_t  				uAllocSize;
    //...
    //this actually contains more data but we don't really need it
} PVRSRV_CLIENT_MEM_INFO;

#define IN_RANGE(val, min, len) (val >= min && val <= (min + len))
char* addr_to_name(size_t addr) {
    if (IN_RANGE(addr, 0x1000, 0x87fe000)) {
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

//given the name of the 
bool is_usse_code(size_t addr) {
    if (IN_RANGE(addr, 0xf400000, 0x4ff000)) {
        return "PixelShaderCode";
    } else if (IN_RANGE(addr, 0xfc00000, 0x1ff000)) {
        return "VertexShaderCode";
    } else {
        return false;
    }
}

typedef struct Allocation {
    void* cpu_addr;
    uint32_t gpu_addr;
    uint32_t size;
} Allocation;

typedef struct Library {
    uintptr_t base;
    size_t size;
    const char* name;
} Library;

static Allocation* map;
static Library*    libraries;
static int iterate_callback (struct dl_phdr_info *info, size_t, void *) {
    size_t last_addr = 0;

    for (int i = 0; i < info->dlpi_phnum; i++) {
        ElfW(Phdr) cur = info->dlpi_phdr[i];
        uintptr_t  first = info->dlpi_addr + cur.p_vaddr;
        uintptr_t  last  = first + cur.p_memsz - 1;

        if (last > last_addr) {
            last_addr = last;
        }
    }
    Library new = { .base = info->dlpi_addr, .size = (last_addr - info->dlpi_addr), .name = (info->dlpi_name) };

    //TODO reduce memory waste with a hashmap to avoid having the same library multiple times since they are all listed upon dlopen
    arrput(libraries, new);

    return 0;
}

HOOK(dlopen, (const char *filename, int flags), void*, { 
    //HACK, this probably should be changed
    void* ret = real_dlopen(filename, flags | RTLD_GLOBAL);
    if (ret) {
        printf("\ndlopened: %s\n", filename);
    }

    dl_iterate_phdr(iterate_callback, 0);
    return ret;
})

HOOK(PVRSRVAllocDeviceMem, (void* data, void* handle, uint32_t attribs, uint32_t size, uint32_t align, PVRSRV_CLIENT_MEM_INFO** info), int, {
	printf("\nallocating %x bytes of device memory\n", size);
    int ret = real_PVRSRVAllocDeviceMem(data, handle, attribs, size, align, info);
    printf("allocated memory at %p : %x (%s)\n", (*info)->pvLinAddr, (*info)->sDevVAddr, addr_to_name((*info)->sDevVAddr));

    struct Allocation new;
    new.cpu_addr = (*info)->pvLinAddr;
    new.gpu_addr = (*info)->sDevVAddr;
    new.size = size;
    arrput(map, new);

    return ret;
});

HOOK(memcpy, (void *restrict dest, const void *restrict src, size_t n), void*, {
    uintptr_t d = (uintptr_t)dest;

    for (int i = 0; i < arrlen(map); i++) {
        Allocation cur = map[i];
        if (IN_RANGE(d, ((uintptr_t)cur.cpu_addr), cur.size)) {
            uintptr_t ret = (uintptr_t)__builtin_return_address(0);
            uintptr_t off = 0;
            const char* lib = NULL;
            size_t mem_off = d - (uintptr_t)cur.cpu_addr;

            for (int j = 0; j < arrlen(libraries); j++)  {
                if (IN_RANGE(ret, libraries[j].base, libraries[j].size)) {
                    lib = libraries[j].name;
                    off = ret - libraries[j].base;
                }
            }

            printf("\nmemcpy to gpu area (%s), offset %x, size: %x, function addr: %x, library name: %s\n",
                    addr_to_name(cur.gpu_addr), mem_off, n, off, lib);
        }
    }

    void* res = real_memcpy(dest, src, n);
    return res;
})
