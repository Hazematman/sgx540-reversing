#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

//TODO (low prio) figure out what any of this information means
//seems like it is needed for compilation too, so it's possible
//to extract for any pvr gpu in theory.

const uint32_t a[] = {0x13, 0x26, 0x22};
uint32_t a_s = 3;

struct info_unk0
{
	uint32_t				unk1;
	uint32_t				unk2;
	uint32_t				unk3;
	uint32_t				unk4;
	uint32_t				unk5;
	uint32_t				unk6;
	uint32_t				unk7;
	uint32_t				unk8;
	uint32_t				unk9;
	uint32_t const*	    	unk10;
	uint32_t const*	    	unk10_len;
	char     const*			name;
};

struct info_unk {
    uint32_t funny;
};

struct gpuinfo
{
	uint32_t	             type;
	uint32_t			     revision;
	struct info_unk          flags;
    uint32_t padding;
	const struct info_unk0*	 gpu_info;
};


static const struct info_unk0 thingy =
{
	 0x24000,
	 0,
	 1,
	 0,
	 3,
	 1,
	 4,
	 12,
	 3,
	 &a_s,
	 a,
	 "SGX540",
};

#define SGX540 5

static const struct gpuinfo thing2 = {
		SGX540,
		101,
		{
            0x16b800
		},
        0,
		&thingy,
	};

int main_hook(int argc, char **argv, char **envp) { 

    if (argc != 2) {
        printf("Please pass an USSE binary file as an argument\n");
        return 1;
    }

    FILE *ptr;
    printf("dumping: %s\n", argv[1]);
    ptr = fopen(argv[1],"rb");
    fseek(ptr, 0L, SEEK_END);
    size_t size = ftell(ptr);
    rewind(ptr);

    //file size is always divisible by 8
    uint32_t* file = malloc(size);
    fread(file, size, 1, ptr);

    size_t instruction_count = size / sizeof(uint32_t);

    //trust me
    ((void(*) (const void*, uint32_t, uint32_t*))0x082994cb)(&thing2, instruction_count, file);
    //note that this address may need to be adjusted for different versions of the compiler

    return 0;
}

//hijack the libc main function
int __libc_start_main(
    int (*main)(int, char **, char **),
    int argc,
    char **argv,
    int (*init)(int, char **, char **),
    void (*fini)(void),
    void (*rtld_fini)(void),
    void *stack_end) {
    /* Save the real main function address */

    /* Find the real __libc_start_main()... */
    typeof(&__libc_start_main) orig = dlsym(RTLD_NEXT, "__libc_start_main");

    /* ... and call it with our custom main function */
    return orig(main_hook, argc, argv, init, fini, rtld_fini, stack_end);
}
