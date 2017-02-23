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


extern word mem[];
extern const unsigned int kMemSize;

static const unsigned int kMaxOperandCount = 3;


unsigned int instructionLength(word instruction)
{
    switch (instruction)
    {
        case 0: return 1; // halt: 0
        case 1: return 3; // set:  1 a b
        case 2: return 2; // push: 2 a
        case 3: return 2; // pop: 3 a
        case 4: return 4; // eq: 4 a b c
        case 5: return 4; // gt: 5 a b c
        case 6: return 2; // jmp: 6 a
        case 7: return 3; // jt: 7 a b
        case 8: return 3; // jf: 8 a b
        case 9: return 4; // add: 9 a b c
        case 10: return 4; // mult: 10 a b c
        case 11: return 4; // mod: 11 a b c
        case 12: return 4; // and: 12 a b c
        case 13: return 4; // or: 13 a b c
        case 14: return 3; // not: 14 a b
        case 15: return 3; // rmem: 15 a b
        case 16: return 3; // wmem: 16 a b
        case 17: return 2; // call: 17 a
        case 18: return 1; // ret: 18
        case 19: return 2; // out: 19 a
        case 20: return 2; // in: 20 a
        case 21: return 1; // noop: 21
    }
    
    return 1;
}

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
    word i = mem[addr];
    
    char inst[] = "???\0\0";
    unsigned int operandCount = 0;
    
    switch (i)
    {
        case 0: strcpy(inst, "HALT"); operandCount = 0; break;     // halt: 0
        case 1: strcpy(inst, "SET"); operandCount = 2; break;      // set:  1 a b
        case 2: strcpy(inst, "PUSH"); operandCount = 1; break;     // push: 2 a
        case 3: strcpy(inst, "POP"); operandCount = 1; break;      // pop: 3 a
        case 4: strcpy(inst, "EQ"); operandCount = 3; break;       // eq: 4 a b c
        case 5: strcpy(inst, "GT"); operandCount = 3; break;       // gt: 5 a b c
        case 6: strcpy(inst, "JMP"); operandCount = 1; break;      // jmp: 6 a
        case 7: strcpy(inst, "JT"); operandCount = 2; break;       // jt: 7 a b
        case 8: strcpy(inst, "JF"); operandCount = 2; break;       // jf: 8 a b
        case 9: strcpy(inst, "ADD"); operandCount = 3; break;      // add: 9 a b c
        case 10: strcpy(inst, "MULT"); operandCount = 3; break;    // mult: 10 a b c
        case 11: strcpy(inst, "MOD"); operandCount = 3; break;     // mod: 11 a b c
        case 12: strcpy(inst, "AND"); operandCount = 3; break;     // and: 12 a b c
        case 13: strcpy(inst, "OR"); operandCount = 3; break;      // or: 13 a b c
        case 14: strcpy(inst, "NOT"); operandCount = 2; break;     // not: 14 a b
        case 15: strcpy(inst, "RMEM"); operandCount = 2; break;    // rmem: 15 a b
        case 16: strcpy(inst, "WMEM"); operandCount = 2; break;    // wmem: 16 a b
        case 17: strcpy(inst, "CALL"); operandCount = 1; break;    // call: 17 a
        case 18: strcpy(inst, "RET"); operandCount = 0; break;     // ret: 18
        case 19: strcpy(inst, "OUT"); operandCount = 1; break;     // out: 19 a
        case 20: strcpy(inst, "IN"); operandCount = 1; break;      // in: 20 a
        case 21: strcpy(inst, "NOOP"); operandCount = 0; break;    // noop: 21
    }
    
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
    fprintf(stderr, "%s", inst);
    
    // output string
    if (mem[addr] == 19)
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
        // label
        unsigned int lind = operandCount;
        if (mem[addr] == 6 || mem[addr] == 17)
        {
            lind = 0;
        }
        else if (mem[addr] == 7 || mem[addr] == 8)
        {
            lind = 1;
        }
        
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
                if (i == lind && (lbl = labelAtAddress(o)))
                {
                    fprintf(stderr, " %s (0x%06X)", lbl->name, lbl->address);
                }
                else
                {
                    fprintf(stderr, " %d", o);
                }
            }
            else
            {
                fprintf(stderr, " ??");
            }
        }
    }
    
    fprintf(stderr, "\n");
    
    return operandCount + 1;
}
