//
//  processor.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdbool.h>
#include <memory.h>
#include <stdio.h>

#include <signal.h>

#include "types.h"
#include "instruction.h"
#include "debug.h"
#include "console.h"

#include "processor.h"


const unsigned int kMemSize = 1 << 15;
const unsigned int kMemMask = kMemSize - 1;
const unsigned int kRegisterCount = 8;

const unsigned int kStackSize = 256;


word memory[kMemSize + kRegisterCount];

word stack[kStackSize];
int sp = kStackSize;

int pc;

bool active = false;
bool debug = false;


byte breakpoint[kMemSize];


// MARK: - Value

word getMemory(word address)
{
    return memory[address];
}

void setMemory(word address, word value)
{
    memory[address] = value;
}

word getValue(word value)
{
    if (value >= kMemSize)
    {
        return getMemory(value);
    }
    
    return value;
}


// MARK: - Stack

void pushStack(word v)
{
    if (sp > 0)
    {
        stack[--sp] = v;
    }
    else
    {
        fprintf(stderr, ">>ERR! stack overflow! [pc:%04d]\n", pc);
        active = false;
    }
}

word popStack()
{
    if (sp <= kStackSize)
    {
        return stack[sp++];
    }
    
    fprintf(stderr, ">>ERR! stack overflow! [pc:%04d]\n", pc);
    active = false;
    
    return -1;
}


// MARK: - Breakpoints

void removeAllBreakpoints()
{
    memset(breakpoint, 0, kMemSize / 2);
}

void setBreakpoint(unsigned int address, byte active)
{
    breakpoint[address >> 1] = active;
}

bool isBreakpointAtAddress(unsigned int address)
{
    return breakpoint[address >> 1];
}


// MARK: - Execution

void run()
{
    setBreakpoint(0x6e2, 1);
    
    active = true;
    while (active)
    {
        if (isBreakpointAtAddress(pc))
        {
            dumpInstructionAtAddress(pc);

//            active = false;
            debug = true;
            
            startConsole();
        }
        else if (debug)
        {
            dumpInstructionAtAddress(pc);
        }

        pc += runInstructionAtAddress(pc);
    }
    
    printf("\n");
}

void reset()
{
    memset(memory, 0, (kMemSize + kRegisterCount) << 1);

    pc = 0;
        
    memset(breakpoint, 0, kMemSize);
}
