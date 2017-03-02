//
//  debug.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "label.h"
#include "instruction.h"
#include "processor.h"

#include "debug.h"


static const unsigned int kMaxOperandCount = 3;


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

const char rchar(unsigned char c)
{
    return ((c & 0xFF) > 31 && (c & 0xFF) < 127) ? c : '.';
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


// MARK: - Labels

void dumpLabels()
{
    fprintf(stderr, "Label labels[%d] = \n{\n", labelCount);
    
    for (unsigned int i = 0; i < labelCount; ++i)
    {
        fprintf(stderr, "    {0x%04x, \"%s\"}, \n", labels[i].address, labels[i].name);
    }
    
    fprintf(stderr, "};\n");
}


// MARK: - Registers

void dumpRegisters(unsigned short rbitfield)
{
    if (rbitfield == 0)
    {
        rbitfield = 255;
    }

    fprintf(stderr, "\n  REGISTERS\n ------------------------------------------\n");
    
    short mask = 1;
    
    for (unsigned int i = 0; i < 8; ++i)
    {
        if (rbitfield & mask)
        {
            word reg = memory[32768 + i];
            fprintf(stderr, "  R%i: %5d 0x%04X %s '%c%c'\n", i, reg, reg, b2a(reg), rchar(reg >> 8), rchar(reg));
        }
        mask <<= 1;
    }
    
    fprintf(stderr, " ------------------------------------------\n");
}


// MARK: - Stack

void dumpStack()
{
    fprintf(stderr, "\n  STACK\n -----------------------------------------\n");
    
    unsigned int count = kStackSize - sp;
    for (unsigned int i = 0; i < count; ++i)
    {
        word val = stack[sp + i];
        
        fprintf(stderr, " %3d: %5d 0x%04X %s  '%c%c'\n", i, val, val, b2a(val), rchar(val >> 8), rchar(val));
    }
    
    fprintf(stderr, " -----------------------------------------\n");
}


// MARK: - Memory

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
            word val = memory[p + j];
            
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
            
            line[js] = rchar(val);
            line[js + 1] = rchar(val >> 8);
        }
        line[80] = '\0';
        
        fprintf(stderr, "%s\n", line);
        p += size;
    }
    
    fprintf(stderr, " ---------------------------------------------------------------------------------------\n");
}


// MARK: - Instructions

unsigned int dumpInstructionAtAddress(const unsigned int addr)
{
    word opcode = memory[addr];
    
    if (opcode > instructionCount - 1)
        opcode = instructionCount - 1;
    
    unsigned int operandCount = instructions[opcode].operandCount;
    
    
    // memory
    fprintf(stderr, "%c %c 0x%04X-%05d: %04X ", addr == pc ? '>' : ' ', isBreakpointAtAddress(addr) ? '*' : ' ', addr, addr, memory[addr]);
    for (unsigned int i = 0; i < operandCount; ++i)
    {
        fprintf(stderr, " %04X", memory[addr + i + 1]);
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
    fprintf(stderr, "%s", instructions[opcode].name);
    
    // output string
    if (memory[addr] == 19 && memory[addr + 1] < kMemSize)
    {
        unsigned int ad = addr;
        
        fprintf(stderr, " '");
        
        char c = 0;
        while (memory[ad] == 19)
        {
            c = memory[ad + 1];
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
            word o = memory[addr + 1 + i];
            
            Label *lbl = NULL;
            if (o >= kMemSize && o < kMemSize + kRegisterCount)
            {
                fprintf(stderr, " r%d", o & 7);
            }
            else if (o < kMemSize)
            {
                if (instructions[opcode].lind && i == instructions[opcode].lind - 1 && o > 16 && (lbl = labelAtAddress(o)))
                {
                    fprintf(stderr, " %s(0x%04X)", lbl->name, lbl->address);
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
    
    if (instructions[opcode].space)
        fprintf(stderr, "\n");
    
    return operandCount + 1;
}

void dumpInstructions(const unsigned int addr, unsigned int icount)
{
    unsigned int a = addr;
    
    while (icount--)
    {
        a += dumpInstructionAtAddress(a);
    }
}

void dumpInstructionsFromRange(const unsigned int start, unsigned int end)
{
    for (unsigned int a = start; a < end; )
    {
        a += dumpInstructionAtAddress(a);
    }
}


// MARK: load/save

void loadFromFile(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file)
    {
        // load memory + registers
        fread(memory, sizeof(word), kMemSize + kRegisterCount, file);
        fread(&pc, sizeof(word), 1, file);
        
        // stack
        fread(stack, sizeof(word), kStackSize, file);
        
        fclose(file);
        
        printf(">>> load fom file %s\n", filename);
    }
}

void saveToFile(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (file)
    {
        // save memory + registers
        fwrite(memory, sizeof(word), kMemSize + kRegisterCount, file);
        fwrite(&pc, sizeof(word), 1, file);

        // stack
        fwrite(stack, sizeof(word), kStackSize, file);
        
        fclose(file);
    
        printf(">>> saved to file %s\n", filename);
    }
}

