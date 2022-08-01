/*
 * This code is the initial implementation of an assembler library for the SGX 540 architecture
 *
 * it currently lacks a lot of error checking but the opcodes have been tested working, this will
 * probably be converted to a more easily readable template file.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "asm.h"

//FOUND: 0xF804014000000000 nop.end

#define DUMP(name, area, size) char filename[strlen(name) + 10];\
        sprintf(filename, "./" name);\
        printf("writing to %s\n", filename);\
        FILE* d = fopen(filename, "wb");\
        fwrite((const void*)area, size, 1, d);\
        fclose(d);

//we pass the entire instruction in case some modifiers are found to be unavailable in the future
void encode_standard_argument(Instruction instruction, int regnum, uint32_t* i0, uint32_t* i1) {
    //register arguments 1 and 2 are encoded very similarly
    if (regnum >= 2) {
        uint32_t bank = instruction.arguments[regnum].bank;
        uint32_t ext = 0;
        bool src1 = regnum == 2;

        if (bank > ASM_BANK_SECONDARY_ATTRIBUTE) {
            ext = src1 ? SWITCH_SOURCE1_BANK : SWITCH_SOURCE2_BANK;
            bank -= 4;
        } else {
            ext = 0;
        }

        *i0 |= (instruction.arguments[regnum].num << (src1 ? SRC1_SHIFT : SRC2_SHIFT)) |
            (bank << (src1 ? SRC1_BANK_SHIFT : SRC2_BANK_SHIFT));
        *i1 |= ext;
    } else if (regnum == 1) {
        //For some source0 is special, as it only has one bit for its bank
        //TODO there might be more to this? i can't seem to find a bank switcher for this
        uint32_t bank = instruction.arguments[1].bank;

        *i0 |= (instruction.arguments[1].num << SRC0_SHIFT);
        *i1 |= ((bank == ASM_BANK_PRIMARY_ATTRIBUTE) << SRC0_BANK_SHIFT);
    } else if (regnum == 0) {
        uint32_t ext = 0;
        uint32_t bank = bank = instruction.arguments[0].bank;

        //encode dest0
        //higher banks require bank extensions
        if (instruction.arguments[0].bank > ASM_BANK_SECONDARY_ATTRIBUTE) {
            ext = SWITCH_DEST_BANK;
            bank -= 4;
        }

        *i0 = (instruction.arguments[0].num << DST_SHIFT);
        *i1 = (bank << DST_BANK_SHIFT) | ext;
    } else {
        fprintf(stderr, "standard instruction only have 4 arguments");
    }
}

//TODO implement shorter predicate fields and add error checking
void encode_predicate(uint32_t* i1, bool shortpred, uint8_t pred) {
    if (!shortpred) {
        *i1 |= (pred << PRED_SHIFT);
    }
}

//TODO this is very preliminary and mostly for debugging purposes
void encode_instruction(uint32_t* buffer, Instruction instruction) {
    //two parts of the instruction
    uint32_t* i0 = &buffer[0];
    uint32_t* i1 = &buffer[1];
    *i0 = *i1 = 0;

    switch (instruction.opcode) {
        //move a register to another register
        case ASM_OP_FMAD : {

            encode_standard_argument(instruction, 0, i0, i1);
            encode_standard_argument(instruction, 1, i0, i1);
            encode_standard_argument(instruction, 2, i0, i1);
            encode_standard_argument(instruction, 3, i0, i1);

            *i1 |= (0 << OP_SHIFT);
            //unknown purpose for now, the internal assembler seems to do it so we do it too
            *i1 |= (1 << 12);// | (1 << 16);
            break;
        }

        case ASM_OP_MOV : {
            encode_standard_argument(instruction, 0, i0, i1);
            encode_standard_argument(instruction, 2, i0, i1);

            *i1 |= (OP_MOV << OP_SHIFT);
            //not sure what these bits do just yet, seems like they're needed for some reason
            *i0 |= 0x20000000;
            *i1 |= (1 << 12) | (1 << 16);
            break;
        }

        case ASM_OP_BA:
        case ASM_OP_BR : {
            //TODO a few flags seem to be supported for branches, including saving the link register
            uint32_t off = instruction.arguments[1].num;

            if (off > 4095) {
                fprintf(stderr, "%x is over the branch offset limit\n", off);
            }

            *i0 = off;
            *i1 = (OP_EXT << OP_SHIFT) | ((instruction.opcode == ASM_OP_BR) << FLOW_OP_SHIFT);
            encode_predicate(i1, false, instruction.arguments[0].num);
            break;
        }

        //TODO: the exact nature of this instruction isn't clear, there could be a way
        //to disable the "test" portion of the instruction, additionally it's probably
        //possible to encode other predicate registers and conditions, but at the mom-
        //ent this is not necessary.
        //
        //register bank encoding seem to behave a bit weirdly here.
        case ASM_OP_TEST_FSUB : {
            encode_standard_argument(instruction, 0, i0, i1);
//            encode_standard_argument(instruction, 1, i0, i1);
            encode_standard_argument(instruction, 2, i0, i1);
            encode_standard_argument(instruction, 3, i0, i1);

            *i0 |= (0b111 << FLOAT_SUBOP_SHIFT) | (1 << 20);
            *i1 |= (OP_FOP << OP_SHIFT) | (0b11 << 7) | (1 << 12);
            break;
        }

        case ASM_OP_LDDW:
        case ASM_OP_STDW : {
            bool store = instruction.opcode == ASM_OP_STDW;

            if (!store) {
                encode_standard_argument(instruction, 0, i0, i1);
            }

            encode_standard_argument(instruction, 1, i0, i1);
            encode_standard_argument(instruction, 2, i0, i1);

            if (store) {
                encode_standard_argument(instruction, 3, i0, i1);
            }

            //the purpose of these bits is unknown, the disassembly doesn't change when they change
            //but the instruction doesn't actually seem to run if i don't set them.
            if (!store) {
                *i0 |= (1 << 29);
                *i1 |= (1 << 16);
            }

            *i1 |= ((store ? OP_ST : OP_LD) << OP_SHIFT) | BPCACHE;
            break;
        }

        case ASM_OP_WDF : {
            *i1 = (OP_EXT << OP_SHIFT) |
                (2 << 20) | // these bits may be part of another sub-operation
                (1 << EXTEND_OP_SHIFT) |
                instruction.arguments[0].num;
        }

        case ASM_OP_MOV_LONG : {
            uint32_t val = instruction.arguments[1].num;

            encode_standard_argument(instruction, 0, i0, i1);

            *i0 |= (val) & ~0xFFE00000U;
            *i1 |= (31 << 27) | (4 << 24) | (2 << 20) | 
                (((val >> 21) << 4) & ~0xFFFFFE0FU) |
                (((val >> 26) << 12) & ~0xFFFC0FFFU);
        }
    }
}
