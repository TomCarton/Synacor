//
//  label.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#include <memory.h>
#include <limits.h>

#include "types.h"
#include "instruction.h"
#include "processor.h"

#include "label.h"


static const unsigned int kMaxLabelCount = 300;

Label labels[kMaxLabelCount] =
{
    {0x0000, "Start"},
    
    {0x015b, "SelfTest"},

    {0x0166, "TestJump"},
    {0x0170, "ErrJumpM2"},
    {0x018d, "ErrJumpM1"},
    {0x01a8, "ErrJumpP1"},
    {0x01c5, "ErrJumpP2"},
    {0x01e4, "TestJtJf"},
    {0x01ef, "TestJtJf2"},
    {0x01f4, "TestRegs"},
    {0x0234, "TestEqOp"},
    {0x024e, "TestStack"},
    {0x02ac, "TestNot"},
    {0x02c4, "BackCallOp"},
    {0x02db, "BackCallOp2"},
    {0x034b, "TestMemSlot"},
    {0x034d, "TestMem"},
    {0x03ad, "ErrWmem"},
    {0x03D2, "Begin"},
    {0x03ff, "_end"},
    {0x03e8, "_loop"},
    {0x0432, "ErrNoJtJf"},
    {0x0445, "ErrRegs"},
    {0x045e, "ErrSetOp"},
    {0x0486, "ErrStack"},
    {0x0473, "ErrGtOp"},
    {0x0499, "ErrAnd"},
    {0x04b8, "ErrNot"},
    {0x04d7, "ErrRmemOp"},
    {0x04ee, "ErrWmemOp"},
    {0x0505, "TestCallOp"},
    {0x0507, "TestCallOp2"},
    {0x0509, "ErrCallOp"},
    {0x0520, "ErrModulo"},
    {0x0565, "ErrHitchhiking"},
    {0x0586, "ErrMultOp"},
    {0x059d, "ErrModOp"},
    
    {0x05b2, "ForEach"},
    {0x05c8, "_loop"},
    {0x05e3, "_end"},
    {0x05ee, "Print"},

    {0x05f8, "PrintChar"},
    {0x061b, "Toto"},
    
    {0x0645, "Method2"},
    {0x0652, "_end"},

    {0x06bb, "Decipher"},
    {0x06c2, "_loop"},

    {0x0726, "_end"},

    {0x0840, "_end"},
    
    {0x084d, "Xor"},
};

unsigned int labelCount = 50;


void listLabels()
{
    fprintf(stderr, "\n  LABELS\n ------------------------------------------\n");
    
    for (unsigned int i = 0; i < labelCount; ++i)
    {
        fprintf(stderr, "  0x%04x:%05d - %s\n", labels[i].address, labels[i].address, labels[i].name);
    }
    
    fprintf(stderr, " ------------------------------------------\n");
}

Label *labelAtAddress(const unsigned int address)
{
    for (unsigned int i = 0; i < labelCount; ++i)
    {
        if (labels[i].address == address)
        {
            return &labels[i];
        }
    }

    return NULL;
}


void scanForLabels(const unsigned int addr, const unsigned int end)
{
    for (unsigned int a = addr; a < end && labelCount < kMaxLabelCount;)
    {
        unsigned int address = 0;
        // JUMP/CALL
        if (memory[a] == 6 || memory[a] == 17)
        {
            address = memory[a + 1];
        }
        // JT/JF
        else if (memory[a] == 7 || memory[a] == 8)
        {
            address = memory[a + 2];
        }

        if (address > 0 && address < kMemSize && labelAtAddress(address) == NULL)
        {
            labels[labelCount].address = address;
            sprintf(labels[labelCount].name, "Label%d", labelCount);
            
            ++labelCount;
        }
        
        a += instructionLength(memory[a]);
    }
}
