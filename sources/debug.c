//
//  debug.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdio.h>

#include "label.h"
#include "instruction.h"
#include "processor.h"

#include "debug.h"


// MARK: - Helpers

const char *b2a(unsigned short val)
{
    static char binaryString[] = "0b................";
    
    unsigned int mask = 0x8000;
    for (unsigned int i = 0; i < 16; ++i)
    {
        binaryString[i + 2] = val & mask ? '1' : '0';
        mask >>= 1;
    }
    
    return binaryString;
}


// MARK: -

void dumpLabels()
{
    fprintf(stderr, "Label labels[%d] = \n{\n", labelCount);
    
    for (unsigned int i = 0; i < labelCount; ++i)
    {
        fprintf(stderr, "    {0x%04x, \"%s\"}, \n", labels[i].address, labels[i].name);
    }
    
    fprintf(stderr, "};\n");
}

void dumpRegisters(unsigned short rbitfield)
{
    if (rbitfield == 0)
    {
        rbitfield = 255;
    }

    fprintf(stderr, "\n  REGISTERS\n -----------------------------------------\n");
    
    short mask = 1;
    
    for (unsigned int i = 0; i < 8; ++i)
    {
        if (rbitfield & mask)
        {
            byte c1 = reg[i] >> 8;
            if (c1 < 11 || c1 > 127) c1 = '.';
            byte c2 = reg[i];
            if (c2 < 11 || c2 > 127) c2 = '.';
            
            fprintf(stderr, "  R%i: %05d 0x%04X %s  '%c%c'\n", i, reg[i], reg[i], b2a(reg[i]), c1, c2);
        }
        mask <<= 1;
    }
    
    fprintf(stderr, " -----------------------------------------\n");
}

void dumpStack()
{
    fprintf(stderr, "\n  STACK\n -----------------------------------------\n");
    
    unsigned int count = kStackSize - sp;
    for (unsigned int i = 0; i < count; ++i)
    {
        word val = stack[kStackSize - i];
        
        fprintf(stderr, " %3d: 0x%04X %5d  %s\n", i, val, val, b2a(val));
    }
    
    fprintf(stderr, " -----------------------------------------\n");
}

void dumpInstructions(const unsigned int addr, unsigned int icount)
{
    unsigned int a = addr;
    
    while (icount--)
    {
        a += dumpInstructionAtAddress(a);
    }
}

void dumpAllInstructions(const unsigned int start, unsigned int end)
{
    for (unsigned int a = start; a < end; )
    {
        a += dumpInstructionAtAddress(a);
    }
}

void dumpMemory(const unsigned int start, unsigned int end)
{
    fprintf(stderr, "\n  MEMORY\n ---------------------------------------------------------------------------------------\n");
    
    char line[81];
    
    unsigned int p = start;
    while (p < end)
    {
        fprintf(stderr, "  0x%06X: ", p);
        
        unsigned int j, size = end - p > 10 ? 10 : end - p;
        for (j = 0; j < size; ++j)
        {
            unsigned int js = j * 6;
            word val = mem[p + j];
            
            unsigned char c = (val & 0x00F0) >> 4;
            c += c < 10 ? '0' : 'A' - 10;
            line[js] = c;

            c = (val & 0x000F) >> 0;
            c += c < 10 ? '0' : 'A' - 10;
            line[js + 1] = c;
            
            line[js + 2] = ' ';

            c = (val & 0xF000) >> 12;
            c += c < 10 ? '0' : 'A' - 10;
            line[js + 3] = c;
            
            c = (val & 0x0F00) >> 8;
            c += c < 10 ? '0' : 'A' - 10;
            line[js + 4] = c;
            
            line[js + 5] = ' ';
            
            
            js = 60 + (j * 2);

            c = val & 0x00FF;
            line[js] = (c > 10 && c < 128) ? c : '.';
            c = (val & 0xFF) >> 8;
            line[js + 1] = (c > 10 && c < 128) ? c : '.';
        }
        line[80] = '\0';
        
        fprintf(stderr, "%s\n", line);
        p += size;
    }
    
    fprintf(stderr, " ---------------------------------------------------------------------------------------\n");
}
