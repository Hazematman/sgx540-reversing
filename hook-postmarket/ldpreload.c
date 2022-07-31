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
#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))

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

//TODO we rely on the fact that all the programs we're testing are single-threaded
typedef struct Watchpoint {
    uintptr_t address;
    size_t len;
    //instruction to restore
    uint16_t instr;
    uint32_t bp_addr;
    //address that caused the last fault
    uint32_t addr;
    //last offset from the base of a library
    size_t addr_off;
    const char* lib_name;
    void (*callback_pre) (uintptr_t address, struct Watchpoint hit);
    void (*callback_post) (uintptr_t addr, struct Watchpoint hit);
} Watchpoint;

static Allocation* map;
static Library*    libraries;
static Watchpoint* watchpoints;

static int num_changes = 0;
static size_t addr_test = 0;

static void sev_handler(int, siginfo_t* siginfo, void* uap) {
    uintptr_t addr = (uintptr_t)siginfo->si_addr;
    ucontext_t *context = (ucontext_t *)uap;
    printf("fault at address %p\n", siginfo->si_addr);

    for (int i = 0; i < arrlen(watchpoints); i++) {
        Watchpoint cur = watchpoints[i];

        if (IN_RANGE((size_t)addr, cur.address, cur.len)) {
            printf("hit watchpoint %d\n", i);

            uintptr_t page = ALIGN_DOWN(((uintptr_t)addr), (getpagesize()));
            mprotect((void*)page, cur.len, PROT_READ | PROT_WRITE);

            uintptr_t codepage = ALIGN_DOWN(((uintptr_t)context->uc_mcontext.arm_pc), (getpagesize()));
            mprotect((void*)codepage, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC);

            uint16_t* ins = (uint16_t*)((uintptr_t)context->uc_mcontext.arm_pc);
            uint16_t* c_instruction = (uint16_t*)0;

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

            printf("in lib %s\n", lib);

            if (((*ins >> 13) & 0b111) == 0b111) {
                c_instruction = (uint16_t*)((uintptr_t)context->uc_mcontext.arm_pc + 4);
            } else {
                c_instruction = (uint16_t*)((uintptr_t)context->uc_mcontext.arm_pc + 2);
            }
            watchpoints[i].instr = (*c_instruction);
            watchpoints[i].bp_addr = (uint32_t)(c_instruction);
            watchpoints[i].addr = addr;
            watchpoints[i].addr_off = off;
            watchpoints[i].lib_name = lib;

            if (cur.callback_pre) {
                cur.callback_pre(addr, cur);
            }

            *c_instruction = 0xde01;
            __clear_cache((char*)(codepage), (char*)(codepage + getpagesize() * 10));

            return;
        }
    }

    printf("real segmentation fault\n");
    exit(1);
}

static void trap_handler(int, siginfo_t*, void* uap) {
    ucontext_t *context = (ucontext_t *)uap;
    uint32_t addr = (uint32_t)(context->uc_mcontext.arm_pc);

    for (int i = 0; i < arrlen(watchpoints); i++) {
        Watchpoint cur = watchpoints[i];
        if (cur.bp_addr == addr) {
            uint16_t* c_instruction = (uint16_t*)((uintptr_t)context->uc_mcontext.arm_pc);
            *c_instruction = cur.instr;
//            printf("\nreplacing instruction at %p %x\n", c_instruction, *c_instruction);
            uintptr_t codepage = ALIGN_DOWN(((uintptr_t)context->uc_mcontext.arm_pc), (getpagesize()));
            __clear_cache((char*)(codepage), (char*)(codepage + getpagesize() * 10));
            mprotect((void*)ALIGN_DOWN(((uintptr_t)cur.addr), (getpagesize())), cur.len, PROT_READ);

            if (cur.callback_post) {
                cur.callback_post(cur.addr, cur);
            }
            break;
        }
    }
}

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

void* thread() {
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

void gpu_mem_prewrite(uintptr_t address, struct Watchpoint hit) {
    printf("callback\n");
    return;
}

void gpu_mem_postwrite(uintptr_t address, struct Watchpoint hit) {
    printf("write in library: %s at offset %x\n", hit.lib_name, hit.addr_off);
    return;
}

void add_watchpoint(uintptr_t addr, size_t size,
    void (*callback_pre) (uintptr_t address, struct Watchpoint hit),
    void (*callback_post) (uintptr_t address, struct Watchpoint hit) ) {
        struct Watchpoint new;
        new.address = (uintptr_t)addr;
        new.len = size;
        new.callback_pre = callback_pre;
        new.callback_post = callback_post;

        arrput(watchpoints, new);
        mprotect((void*)addr, size, PROT_READ);
}

HOOK(PVRSRVAllocDeviceMem, (void* data, void* handle, uint32_t attribs, uint32_t size, uint32_t align, PVRSRV_CLIENT_MEM_INFO** info), int, {
    int ret = real_PVRSRVAllocDeviceMem(data, handle, attribs, size, align, info);
    printf("allocated %x bytes of memory at %p : %x (%s)\n", size, (*info)->pvLinAddr, (*info)->sDevVAddr, addr_to_name((*info)->sDevVAddr));

    memset((*info)->pvLinAddr, 0, size);

    struct Allocation new;
    new.cpu_addr = (*info)->pvLinAddr;
    new.gpu_addr = (*info)->sDevVAddr;
    new.size = size;
    arrput(map, new);

    if (!strcmp(addr_to_name((*info)->sDevVAddr), "VertexShaderCode") && ((*info)->sDevVAddr == 0xfc00000)) {
//    if (!strcmp(addr_to_name((*info)->sDevVAddr), "KernelCode")/* && ((*info)->sDevVAddr == 0xe429000)*/) {
        printf("setting breakpoint %x\n", size);
        add_watchpoint((*info)->pvLinAddr, size, gpu_mem_prewrite, gpu_mem_postwrite);
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

void* c_memcpy(const void *restrict dest, const void *restrict src, size_t n) {
    // Typecast src and dest addresses to (char *)
    char *csrc = (char *)src;
    char *cdest = (char *)dest;
     
    // Copy contents of src[] to dest[]
    for (size_t i = 0; i<n; i++)
        cdest[i] = csrc[i];

    return (void*)dest;
}

HOOK(memset, (void *s, int c, size_t n), void*, {
    unsigned char* p=s;
    while(n--) {
        *p++ = (unsigned char)c;
    }
    return s;
})

HOOK(memcpy, (void *restrict dest, const void *restrict src, size_t n), void*, {
    uintptr_t d = (uintptr_t)dest;

    bool found = false;
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
        }
    }


    if (!found) {
        printf("external memcpy\n");
    }

    void* res = c_memcpy(dest, src, n);

    return res;
})

static int (*main_orig)(int, char **, char **);

int main_hook(int argc, char **argv, char **envp) { 
    sigset_t set;
    //install the signal handler
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = sev_handler;
    act.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigaction(SIGSEGV, &act, NULL);

    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = trap_handler;
    act.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigaction(SIGTRAP, &act, NULL);

    sigemptyset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    pthread_t pthread;
    if (pthread_create(&pthread, NULL, thread, (void*)0)) {
        printf("Could not create thread\n");
        exit(1);
    }

    sigemptyset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    return main_orig(argc, argv, envp);
}

int __libc_start_main(
    int (*main)(int, char **, char **),
    int argc,
    char **argv,
    int (*init)(int, char **, char **),
    void (*fini)(void),
    void (*rtld_fini)(void),
    void *stack_end) {

    main_orig = main;

    typeof(&__libc_start_main) orig = dlsym(RTLD_NEXT, "__libc_start_main");

    return orig(main_hook, argc, argv, init, fini, rtld_fini, stack_end);
}
