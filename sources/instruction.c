//
//  instruction.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "types.h"
#include "label.h"
#include "processor.h"

#include "instruction.h"


Instruction instructions[] =
{
    // op   name    oc in sp
    { 0x00, "HALT", 0, 0, 1, }, // halt: 0
    { 0x01, "SET",  2, 0, 0, }, // set: 1 a b
    { 0x02, "PUSH", 1, 0, 0, }, // push: 2 a
    { 0x03, "POP",  1, 0, 0, }, // pop: 3 a
    { 0x04, "EQ",   3, 0, 0, }, // eq: 4 a b c
    { 0x05, "GT",   3, 0, 0, }, // gt: 5 a b c
    { 0x06, "JMP",  1, 1, 1, }, // jmp: 6 a
    { 0x07, "JT",   2, 2, 1, }, // jt: 7 a b
    { 0x08, "JF",   2, 2, 1, }, // jf: 8 a b
    { 0x09, "ADD",  3, 0, 0, }, // add: 9 a b c
    { 0x0A, "MULT", 3, 0, 0, }, // mult: 10 a b c
    { 0x0B, "MOD",  3, 0, 0, }, // mod: 11 a b c
    { 0x0C, "AND",  3, 0, 0, }, // and: 12 a b c
    { 0x0D, "OR",   3, 0, 0, }, // or: 13 a b c
    { 0x0E, "NOT",  2, 0, 0, }, // not: 14 a b
    { 0x0F, "RMEM", 2, 0, 0, }, // rmem: 15 a b
    { 0x10, "WMEM", 2, 0, 0, }, // wmem: 16 a b
    { 0x11, "CALL", 1, 1, 1, }, // call: 17 a
    { 0x12, "RET",  0, 0, 1, }, // ret: 18
    { 0x13, "OUT",  1, 1, 0, }, // out: 19 a
    { 0x14, "IN",   1, 0, 0, }, // in: 20 a
    { 0x13, "NOOP", 0, 0, 0, }, // noop: 21

    { 0xffff, "???", 0, 0, 0, },
};

unsigned int instructionCount = sizeof(instructions) / sizeof(instructions[0]);


unsigned int instructionLength(word opcode)
{
    return (opcode <= 0x13) ? instructions[opcode].operandCount + 1: 1;
}
