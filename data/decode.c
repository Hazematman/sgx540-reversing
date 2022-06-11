#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#define S(x)  (sizeof(x) / sizeof((x)[0]))

#define OPC_FMATH 0
#define OPC_DOTP  1

char* fmath_subops[] = {
    "fmad"
};

uint32_t get_dest_num(uint32_t op0, uint32_t op1) {
    return(op0 >> 21) & 127;
}

uint32_t get_src0_num(uint32_t op0, uint32_t op1) {
    return(op0 >> 14) & 127;
}

uint32_t get_src1_num(uint32_t op0, uint32_t op1) {
    return(op0 >> 7) & 127;
}
uint32_t get_src2_num(uint32_t op0, uint32_t op1) {
    return(op0) & 127;
}

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
        //__builtin_bswap32
        uint32_t inst0 = (file[i]);
        uint32_t inst1 = (file[i+1]);

        uint8_t opcode = inst1 >> 27;

//        printf("opcode: %x\n", inst1 >> 27);

        //encoding 1 seems to be mostly relevant to arithmetic
        //encoding 1 checklist

        switch (opcode) {
            //TODO find more sub-opcodes, find sub-opcodes for instructions where they're not known yet
            //also there seems to be a "repeat" feature, find how it is encoded
            //check the correctness of the register sources/dests fully and handle register (banks|types)
            case OPC_FMATH: {
                uint32_t subop = (inst1 >> 9) & 0x3;
                if (subop < S(fmath_subops)) {
                    printf("%s r%d, r%d, r%d, r%d\n", fmath_subops[subop], get_dest_num(inst0, inst1), get_src0_num(inst0, inst1), get_src1_num(inst0, inst1), get_src2_num(inst0, inst1));
                } else {
                    printf("opcode: %x, fmath sub-opcode: %x\n", opcode, subop);
                }
                break;
            }
            case OPC_DOTP: {
                printf("fdot\n");
            }
            default:
                printf("opcode: %x\n", inst1 >> 27);
        }
    }
}
