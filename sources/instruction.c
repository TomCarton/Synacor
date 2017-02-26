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


static const unsigned int kMaxOperandCount = 3;


Instruction Instructions[] =
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

unsigned int instructionCount = sizeof(Instructions) / sizeof(Instructions[0]);


unsigned int instructionLength(word opcode)
{
    return (opcode <= 0x13) ? Instructions[opcode].operandCount + 1: 1;
}


// MARK: - Helpers

void addPadding(unsigned int count)
{
    count = count > kLabelMaxSize ? kLabelMaxSize : count;
    
    // pad
    char padding[kLabelMaxSize + 1];
    
    for (unsigned int i = 0; i < kLabelMaxSize; ++i)
    {
        padding[i] = ' ';
    }
    padding[kLabelMaxSize] = '\0';
    
    char pad[kLabelMaxSize + 1];
    strcpy(pad, padding);
    pad[count] = '\0';
    
    fprintf(stderr, "%s", pad);
}

unsigned int dumpInstructionAtAddress(const unsigned int addr)
{
    word opcode = mem[addr];
    
    if (opcode > instructionCount - 1)
        opcode = instructionCount - 1;

    unsigned int operandCount = Instructions[opcode].operandCount;
    
    
    // memory
    fprintf(stderr, "  0x%06X: %04X ", addr, mem[addr]);
    for (unsigned int i = 0; i < operandCount; ++i)
    {
        fprintf(stderr, " %04X", mem[addr + i + 1]);
    }
    
    // pad
    addPadding(2 + (kMaxOperandCount - operandCount) * 5);

    
    // label
    Label *label = NULL;
    for (unsigned int i = 0; i < labelCount; ++i)
    {
        if (labels[i].address == addr)
        {
            label = &labels[i];
            break;
        }
    }
    if (label)
    {
        fprintf(stderr, "%s:", label->name);
        addPadding(kLabelMaxSize - (unsigned int)strlen(label->name) - 1);
    }
    else
    {
        addPadding(kLabelMaxSize);
    }
    
    
    // instruction
    fprintf(stderr, "%s", Instructions[opcode].name);
    
    // output string
    if (mem[addr] == 19 && mem[addr + 1] < kMemSize)
    {
        unsigned int ad = addr;
        
        fprintf(stderr, " '");
        
        char c = 0;
        while (mem[ad] == 19)
        {
            c = mem[ad + 1];
            if (c == 0 || c == 10)
                break;
            
            fprintf(stderr, "%c", c);
            
            ad += 2;
            operandCount += 2;
        }
        
        fprintf(stderr, "' (%d)", c);
    }
    else
    {
        // operands
        for (unsigned int i = 0; i < operandCount; ++i)
        {
            word o = mem[addr + 1 + i];

            Label *lbl = NULL;
            if (o >= kMemSize && o < kMemSize + kRegisterCount)
            {
                fprintf(stderr, " r%d", o & 7);
            }
            else if (o < kMemSize)
            {
                if (Instructions[opcode].lind && i == Instructions[opcode].lind - 1 && (lbl = labelAtAddress(o)))
                {
                    fprintf(stderr, " %s (0x%04X)", lbl->name, lbl->address);
                }
                else
                {
                    fprintf(stderr, " 0x%04X", o);
                }
            }
            else
            {
                fprintf(stderr, " ??");
            }
        }
    }
    
    fprintf(stderr, "\n");
    
    if (Instructions[opcode].space)
        fprintf(stderr, "\n");
    
    return operandCount + 1;
}
