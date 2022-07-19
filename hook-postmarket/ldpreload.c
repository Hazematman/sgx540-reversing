#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
//TODO alternative for non musl/glibc (does this driver even run on other systems?)
#include <link.h>
#include <signal.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <sys/mman.h>
#include <ucontext.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

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

#define DUMP(name, num, area, size) char filename[strlen(name) + 10];\
        sprintf(filename, "./" name "%d", num);\
        printf("writing to %s\n", filename);\
        FILE* d = fopen(filename, "wb");\
        fwrite((const void*)area, size, 1, d);\
        fclose(d);

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))
#define ALIGN_DOWN(x, a)    ALIGN((x) - ((a) - 1), (a))

typedef struct {
    /* CPU Virtual Address */
    void*                   pvLinAddr;
    /* CPU Virtual Address (for kernel mode) */
    void*                   pvLinAddrKM;
    /* Device Virtual Address */
    uint32_t                sDevVAddr;
    /* allocation flags */
    uint32_t                ui32Flags;
    /* client allocation flags */
    uint32_t                ui32ClientFlags;
    /* allocation size in bytes */
    size_t                  uAllocSize;
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

//TODO this entire part of code should be reworked to support multiple memory areas ans generally cleaned up

static uint16_t saved = 0;
static uintptr_t saved_addr = 0;
static int num_changes = 0;
static uintptr_t monitor_cpu_addr = 0;
static bool running = false;
static size_t last_break = 0;
static size_t last_addr = 0;

static size_t addr_test = 0;

static void handler(int, siginfo_t* siginfo, void* uap) {
    running = true;
    void* addr = siginfo->si_addr;
    ucontext_t *context = (ucontext_t *)uap;
    const char* lib;
    size_t off;


    int j = 0;
    for (j = 0; j < arrlen(libraries); j++)  {
        if (IN_RANGE((context->uc_mcontext.arm_pc), libraries[j].base, libraries[j].size)) {
            lib = libraries[j].name;
            off = (context->uc_mcontext.arm_pc) - libraries[j].base;
            break;
        }
    }

    printf("\nwrite in library::: %s offset: %x ip: %lx\n", lib, off, context->uc_mcontext.arm_pc);
    printf("\nat: %x %p %x\n", monitor_cpu_addr, siginfo->si_addr, (uintptr_t)siginfo->si_addr - monitor_cpu_addr);

    last_break = off;
    last_addr = (uint32_t)siginfo->si_addr;

    uintptr_t page = ALIGN_DOWN(((uintptr_t)addr), (getpagesize()));
    mprotect((void*)page, 0x8000, PROT_READ | PROT_WRITE);
    uintptr_t codepage = ALIGN_DOWN(((uintptr_t)context->uc_mcontext.arm_pc), (getpagesize()));
    mprotect((void*)codepage, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC);

    saved_addr = page;

    if (context->uc_mcontext.arm_cpsr & 0x20) {
        printf("\nTHUMB\n");
    } else {
        printf("\nARM\n");
    }
    
    uint16_t* ins = (uint16_t*)((uintptr_t)context->uc_mcontext.arm_pc);
    uint16_t* c_instruction = (uint16_t*)0;

    printf("checking instruction: %x\n", *ins);
    if (((*ins >> 13) & 0b111) == 0b111) {
        printf("\nthumb32\n");
        c_instruction = (uint16_t*)((uintptr_t)context->uc_mcontext.arm_pc + 4);
    } else {
        printf("\nthumb16\n");
        c_instruction = (uint16_t*)((uintptr_t)context->uc_mcontext.arm_pc + 2);
    }

    saved = (*c_instruction);
    printf("\nreplacing instruction at %p %x off: %x\n", c_instruction, saved, (uintptr_t)c_instruction - libraries[j].base);
    *c_instruction = 0xde01;
    __clear_cache((char*)(codepage - getpagesize() * 10), (char*)(codepage + getpagesize() * 10));

    for (int i = 0; i < arrlen(map); i++) {
        Allocation cur = map[i];

        if (IN_RANGE(((uintptr_t)addr), ((uintptr_t)cur.cpu_addr), cur.size)) {
        }
    }
    running = false;
    return;
}

#define INSTR(hex) *d6 = hex; d6++;

static void trap_handler(int, siginfo_t*, void* uap) {
    static int num = 0;
    running = true;
    if (last_break == 0x419e6) {
        if (num > 1) {
            printf("\nrewriting epilogue\n");
            uint64_t *d6 = (uint64_t*)(last_addr - 4);
            INSTR(0xFC2030C000603004);//address to r3

            INSTR(0xFC20000000000000);//write output
            INSTR(0xFC20000000200002);//write output
            INSTR(0x2801700020000080);//write output
            INSTR(0xF02A00008000C002);//write output

            INSTR(0xF02B0000A000C101);//signal ready
            INSTR(0xFB275000A0200000);
        }
        num++;
    }

    ucontext_t *context = (ucontext_t *)uap;
    uint16_t* c_instruction = (uint16_t*)((uintptr_t)context->uc_mcontext.arm_pc);
    *c_instruction = saved;
    printf("\nreplacing instruction at %p %x\n", c_instruction, *c_instruction);
    uintptr_t codepage = ALIGN_DOWN(((uintptr_t)context->uc_mcontext.arm_pc), (getpagesize()));
    __clear_cache((char*)(codepage - getpagesize() * 10), (char*)(codepage + getpagesize() * 10));
    mprotect((void*)saved_addr, 0x8000, PROT_READ);

    DUMP("vertex_shader", num_changes, saved_addr, 0x8000);
    num_changes++;

    running = false;
    return;
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

HOOK(mprotect, (void *addr, size_t len, int prot), int, {
        uintptr_t addr_k = (uintptr_t)addr;
        if (IN_RANGE(addr_k, monitor_cpu_addr, 0x8000)) {
            if (running) {
                int ret = real_mprotect(addr, len, prot);
                return ret;
            } else {
                int ret = real_mprotect(addr, len, PROT_READ);
                printf("\nexternal mprotect\n");
                return ret;
            }
        } else {
            int ret = real_mprotect(addr, len, prot);
            return ret;
        }
})

bool thread_spawned = false;
static bool installed = false;

void* thread(void* data) {
    printf("thread\n");

    sigset_t set;
    sigemptyset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    while (!addr_test) {sched_yield();}

    printf("\naddress is ready, starting monitor\n");

    volatile uint32_t* signal = ((uint32_t*)(addr_test + 8));

    while (*signal != 0x1) { sched_yield(); }

    printf("\ngot signal\n");

    volatile uint32_t* dumped = ((uint32_t*)(addr_test));
    printf("\ndumped register: %x\n", *dumped);

    volatile float* ack = ((float*)(addr_test + 4));
    *ack = 1.0;

    while (1) { sched_yield(); }
}

HOOK(PVRSRVAllocDeviceMem, (void* data, void* handle, uint32_t attribs, uint32_t size, uint32_t align, PVRSRV_CLIENT_MEM_INFO** info), int, {
//  printf("\nallocating %x bytes of device memory\n", size);
    int ret = real_PVRSRVAllocDeviceMem(data, handle, attribs, size, align, info);
    printf("allocated %x bytes of memory at %p : %x (%s)\n", size, (*info)->pvLinAddr, (*info)->sDevVAddr, addr_to_name((*info)->sDevVAddr));
    sigset_t set;

    if (!installed) {
        //install the signal handler
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        sigemptyset(&act.sa_mask);
        act.sa_sigaction = handler;
        act.sa_flags = SA_SIGINFO | SA_ONSTACK;
        installed = true;
        sigaction(SIGSEGV, &act, NULL);

        memset(&act, 0, sizeof(struct sigaction));
        sigemptyset(&act.sa_mask);
        act.sa_sigaction = trap_handler;
        act.sa_flags = SA_SIGINFO | SA_ONSTACK;
        installed = true;
        sigaction(SIGTRAP, &act, NULL);
    }

    if (!thread_spawned) {
        sigemptyset(&set);
        pthread_sigmask(SIG_BLOCK, &set, NULL);

        pthread_t pthread;
        if (pthread_create(&pthread, NULL, thread, (void*)0)) {
            printf("Could not create thread\n");
            exit(1);
        }
        thread_spawned = true;

        sigemptyset(&set);
//        sigaddset(&set, SIGINT);
        pthread_sigmask(SIG_BLOCK, &set, NULL);
    }

    memset((*info)->pvLinAddr, 0, size);

    struct Allocation new;
    new.cpu_addr = (*info)->pvLinAddr;
    new.gpu_addr = (*info)->sDevVAddr;
    new.size = size;
    arrput(map, new);
    if (!strcmp(addr_to_name(new.gpu_addr), "VertexShaderCode")) {
        //raise a signal and then we attach gdb
//        raise(SIGSEGV);
    }

    if (!strcmp(addr_to_name((*info)->sDevVAddr), "VertexShaderCode") && ((*info)->sDevVAddr == 0xfc00000)) {
        monitor_cpu_addr = (uint32_t)(*info)->pvLinAddr;
        mprotect((void*)(*info)->pvLinAddr, size, PROT_READ);
//        addr_test = (uint32_t)(*info)->pvLinAddr + 100 * 8;
    } else if (!strcmp(addr_to_name((*info)->sDevVAddr), "CacheCoherent") && ((*info)->sDevVAddr == 0xd803000)) {
        PVRSRV_CLIENT_MEM_INFO* nInfo = malloc(0x8000);
        real_PVRSRVAllocDeviceMem(data, handle, attribs, 12, align, &nInfo);
        printf("cache coherent hook, new allocation: %p %x\n", nInfo->pvLinAddr, nInfo->sDevVAddr);
        memset((nInfo)->pvLinAddr, 0xaa, 12);
//        memset((void*)((uintptr_t)(nInfo)->pvLinAddr + 4), 0xaa, 4);
        *(float*)((uintptr_t)(nInfo)->pvLinAddr + 4) = 3.0;
        addr_test = (uint32_t)(nInfo)->pvLinAddr;
    }

    fflush(stdout);

    return ret;
});

HOOK(memcpy, (void *restrict dest, const void *restrict src, size_t n), void*, {
    uintptr_t d = (uintptr_t)dest;

    void* area_base = 0;
    size_t area_size = 0;

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

            printf("\nmemcpy to gpu area (%s), offset %x, size: %x, function addr: %x, library name: %s, area base: %x, area size: %x\n",
                    addr_to_name(cur.gpu_addr), mem_off, n, off, lib, cur.gpu_addr, cur.size);

           // uint8_t* source = (uint8_t*)src;
           
            if (cur.gpu_addr == 0xfc00000) {
                area_size = cur.size;
                area_base = cur.cpu_addr;
            }

        }
    }

    return real_memcpy(dest, src, n);;
})
