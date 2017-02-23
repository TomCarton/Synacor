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

#include "instruction.h"


extern word mem[];
extern const unsigned int memsize;
extern const unsigned int stacksize;


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
    // pad
    char padding[20 + 1] = "                    ";

    char pad[20 + 1];
    strcpy(pad, padding);
    pad[count] = '\0';
    
    fprintf(stderr, "%s", pad);
}

unsigned int dumpInstructionAt(const unsigned int addr)
{
    word i = mem[addr];
    
    char inst[] = "???\0\0\0\0\0";
    unsigned int count = 0;
    
    switch (i)
    {
        case 0: strcpy(inst, "HALT"); count = 0; break;     // halt: 0
        case 1: strcpy(inst, "SET"); count = 2; break;      // set:  1 a b
        case 2: strcpy(inst, "PUSH"); count = 1; break;     // push: 2 a
        case 3: strcpy(inst, "POP"); count = 1; break;      // pop: 3 a
        case 4: strcpy(inst, "EQ"); count = 3; break;       // eq: 4 a b c
        case 5: strcpy(inst, "GT"); count = 3; break;       // gt: 5 a b c
        case 6: strcpy(inst, "JMP"); count = 1; break;      // jmp: 6 a
        case 7: strcpy(inst, "JT"); count = 2; break;       // jt: 7 a b
        case 8: strcpy(inst, "JF"); count = 2; break;       // jf: 8 a b
        case 9: strcpy(inst, "ADD"); count = 3; break;      // add: 9 a b c
        case 10: strcpy(inst, "MULT"); count = 3; break;    // mult: 10 a b c
        case 11: strcpy(inst, "MOD"); count = 3; break;     // mod: 11 a b c
        case 12: strcpy(inst, "AND"); count = 3; break;     // and: 12 a b c
        case 13: strcpy(inst, "OR"); count = 3; break;      // or: 13 a b c
        case 14: strcpy(inst, "NOT"); count = 2; break;     // not: 14 a b
        case 15: strcpy(inst, "RMEM"); count = 2; break;    // rmem: 15 a b
        case 16: strcpy(inst, "WMEM"); count = 2; break;    // wmem: 16 a b
        case 17: strcpy(inst, "CALL"); count = 1; break;    // call: 17 a
        case 18: strcpy(inst, "RET"); count = 0; break;     // ret: 18
        case 19: strcpy(inst, "OUT"); count = 1; break;     // out: 19 a
        case 20: strcpy(inst, "IN"); count = 1; break;      // in: 20 a
        case 21: strcpy(inst, "NOOP"); count = 0; break;    // noop: 21
    }
    
    // memory
    fprintf(stderr, "  0x%06X: %04X ", addr, mem[addr]);
    for (unsigned int i = 0; i < count; ++i)
    {
        fprintf(stderr, " %04X", mem[addr + i + 1]);
    }
    
    // pad
    addPadding(2 + (3 - count) * 5);
    
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
        addPadding(14 - (unsigned int)strlen(label->name));
    }
    else
    {
        addPadding(15);
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
            count += 2;
        }
        
        fprintf(stderr, "' (%d)", c);
    }
    else
    {
        // label
        unsigned int lind = count;
        if (mem[addr] == 6 || mem[addr] == 17)
        {
            lind = 0;
        }
        else if (mem[addr] == 7 || mem[addr] == 8)
        {
            lind = 1;
        }
        
        // operands
        for (unsigned int i = 0; i < count; ++i)
        {
            word o = mem[addr + 1 + i];

            Label *lbl = NULL;
            if (i == lind && (lbl = labelAtAddress(o)))
            {
                fprintf(stderr, " %s (0x%06X)", lbl->name, lbl->address);
            }
            else if (o < memsize)
            {
                fprintf(stderr, " %d", o);
            }
            else if (o < 32776)
            {
                fprintf(stderr, " r%d", o & 7);
            }
        }
    }
    
    fprintf(stderr, "\n");
    
    return count + 1;
}
