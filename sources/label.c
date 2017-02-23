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

#include "label.h"


extern word mem[];


unsigned int labelCount = 3;
Label labels[200] =
{
    {0x000000, "Start"},
    {0x00015B, "SelfTest"},
    {0x000509, "ErrNoCall"},
    
    
};


void listLabels()
{
    for (unsigned int i = 0; i < labelCount; ++i)
    {
        fprintf(stderr, "(%d) %s:\n", labels[i].address, labels[i].name);
    }
}

void dumpLabels()
{
    fprintf(stderr, "Label labels[%d] = \n{\n", labelCount);
    
    for (unsigned int i = 0; i < labelCount; ++i)
    {
        fprintf(stderr, "    {0x%04x, \"%s\"}, \n", labels[i].address, labels[i].name);
    }

    fprintf(stderr, "};\n");
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
    for (unsigned int a = addr; a < end;)
    {
        unsigned int address = 0;
        if (mem[a] == 6 || mem[a] == 17)
        {
            address = mem[a + 1];
        }
        else if (mem[a] == 7 || mem[a] == 8)
        {
            address = mem[a + 2];
        }

        if (address && labelAtAddress(address) == NULL)
        {
            labels[labelCount].address = address;
            sprintf(labels[labelCount].name, "LBL_%d", labelCount);
            
            ++labelCount;
        }
        
        a += instructionLength(mem[a]);
    }
}
