#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#define S(x)  (sizeof(x) / sizeof((x)[0]))

#define OPC_FMATH 0

char* fmath_subops[] = {
    "fmad"
};

int main(int argc, char *argv[]) {
    FILE *ptr;
    printf("dumping: %s\n", argv[1]);
    ptr = fopen(argv[1],"rb");
    fseek(ptr, 0L, SEEK_END);
    size_t size = ftell(ptr);
    rewind(ptr);

    //file size is always divisible by 8
    uint32_t* file = malloc(size);
    fread(file, size, 1, ptr);

//    printf("%x\n", file[1]);
    size_t instruction_count = size / sizeof(uint32_t);

    for (int i = 0; i < instruction_count; i += 2) {
        uint32_t inst0 = (file[i]);
        uint32_t inst1 = (file[i+1]);

        uint8_t opcode = inst1 >> 27;

//        printf("opcode: %x\n", inst1 >> 27);

        switch (opcode) {
            case OPC_FMATH: {
                uint32_t subop = (inst1 >> 9) % 0x3;
                if (subop < S(fmath_subops)) {
                    printf("%s\n", fmath_subops[subop]);
                } else {
                    printf("opcode: %x, fmath sub-opcode: %x\n", opcode, subop);
                }
                break;
            }
            default:
                printf("opcode: %x\n", inst1 >> 27);
        }
    }
}
