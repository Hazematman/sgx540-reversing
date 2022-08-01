#pragma once

#include <stdint.h>

#define OP_SHIFT 27

#define OP_ST    30
//floating point multiply-add
#define OP_FMAD  0
//move
#define OP_MOV   5
#define OP_FOP   9
#define OP_LD    29
#define OP_ST    30
#define OP_EXT   31

#define SRC0_SHIFT 14
#define SRC1_SHIFT 7
#define SRC0_BANK_SHIFT 2
#define SRC1_BANK_SHIFT 30
#define SRC2_SHIFT      0
#define SRC2_BANK_SHIFT 28
#define DST_SHIFT       21
#define DST_BANK_SHIFT  0

#define FLOW_OP_SHIFT     6
#define PRED_SHIFT        24
#define FLOAT_SUBOP_SHIFT 15

#define EXTEND_OP_SHIFT   24

#define FOP_FSUB 7

#define BPCACHE (0b101 << 19)

#define LONG_IMM_MASK1 0x1FFFFF
#define LONG_IMM_SHIFT2_1 17
#define LONG_IMM_MASK2_1 0x1F0
#define LONG_IMM_SHIFT2_2 14
#define LONG_IMM_MASK2_2 0x3F000

#define REGISTER_BANK_SOURCE1_SHIFT 30
#define SWITCH_SOURCE1_BANK         0x00020000U
#define SWITCH_SOURCE2_BANK         0x00010000U
#define SWITCH_DEST_BANK            0x00080000U

//register bank definitions
//it seems like some general register and some internal registers are overlapping, so not all of the 127 registers are actually available
#define BANK_GENERAL              0
#define BANK_OUTPUT               1
#define BANK_PRIMARY_ATTRIBUTE    2
#define BANK_SECONDARY_ATTRIBUTE  3
//extended banks, setting the SWITCH_(x)_BANK bits in the second part of the opcode switches between them
#define BANK_INDEXED              0
//the top bit of the register number determines if the register is a constant or a global (0 and 1 respectively)
#define BANK_GLOBAL_CONSTANT      1
#define BANK_IMMEDIATE            2
#define BANK_INTERNAL             3

#define ASM_BANK_GENERAL              0
#define ASM_BANK_OUTPUT               1
#define ASM_BANK_PRIMARY_ATTRIBUTE    2
#define ASM_BANK_SECONDARY_ATTRIBUTE  3
#define ASM_BANK_INDEXED              4
#define ASM_BANK_GLOBAL_CONSTANT      5
#define ASM_BANK_IMMEDIATE            6
#define ASM_BANK_INTERNAL             7
//the following are special banks
#define ASM_BANK_PREDICATE            8
//32-bit immediate
#define ASM_BANK_EXTIMM               9
#define ASM_BANK_DRC                  10

//writing to an immediate is for setting index registers
#define ASM_BANK_INDEX_DEST           6
#define INDEX_WRITE_LOW  1
#define INDEX_WRITE_HIGH 2
#define INDEX_WRITE_BOTH 3

#define ASM_OP_FMAD      0
#define ASM_OP_TEST_FSUB 1
#define ASM_OP_MOV       5
#define ASM_OP_MOV_LONG  6
#define ASM_OP_LDDW      29
#define ASM_OP_STDW      30
#define ASM_OP_WDF       148
#define ASM_OP_BR        149
#define ASM_OP_BA        150

#define ASM_SKIPINV 0x00800000U

//this structure will probably be changed quite a bit as i find more information
typedef struct Reg {
    uint32_t num;
    uint8_t bank;
    //32 bit immediates seem to only be supported for mov instructions
    //only for indexed register
    bool low_index;
} Reg;

typedef struct Instruction {
    uint16_t opcode;
    Reg arguments[4]; //probably more in some cases
    //add support for more flags later
} Instruction;

void encode_instruction(uint32_t* buffer, Instruction instruction);
