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

#include "processor.h"


const unsigned int kMemSize = 1 << 15;
const unsigned int kMemMask = kMemSize - 1;

const unsigned int kStackSize = 32; //1 << 15;

const unsigned int kRegisterCount = 8;


word mem[kMemSize];
word reg[kRegisterCount];

word stack[kStackSize];
int sp = kStackSize;

int pc;

bool active = false;
bool debug = false;


byte breakpoint[kMemSize / 2];




// MARK: - Value

word value(word val)
{
    if (val < 32768)
    {
        return val;
    }
    else if (val < 32768 + kRegisterCount)
    {
        return reg[val & 7];
    }
    
    fprintf(stderr, ">>ERR! unallowed value! %d [pc:%04d]\n", val, pc);
    active = false;
    
    return 0;
}

void setRegister(unsigned int address, word value)
{
    if (address >= kMemSize && address < kMemSize + kRegisterCount)
    {
        reg[address - kMemSize] = value & kMemMask;
    }
    else
    {
        fprintf(stderr, ">>ERR! illegal register! r%d [pc:%04d]\n", address, pc);
        active = false;
    }
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

unsigned int runInstructionAtAddress(unsigned int address)
{
    word i = mem[address++];
    
    switch (i)
    {
        case 0: // halt: 0
        {
            active = false;
            
            fprintf(stderr, ">>HALT: Program ended. [pc:%04d = %d]\n", pc - 1, mem[pc - 1]);
            
            break;
        }
            
        case 1: // set: 1 a b
        {
            word a = mem[address++];
            word b = mem[address++];
            
            setRegister(a, value(b));
            
            break;
        }
            
        case 2: // push: 2 a
        {
            word a = mem[address++];
            
            pushStack(value(a));
            
            break;
        }
            
        case 3: // pop: 3 a
        {
            word a = mem[address++];
            
            if (a < kMemSize)
            {
                mem[a] = popStack();
            }
            else if (a < kMemSize + kRegisterCount)
            {
                reg[a - kMemSize] = popStack();
            }
            
            break;
        }
            
        case 4: // eq: 4 a b c
        {
            word a = mem[address++];
            word b = mem[address++];
            word c = mem[address++];
            
            setRegister(a, value(b) == value(c) ? 1 : 0);
            
            break;
        }
            
        case 5: // gt: 5 a b c
        {
            word a = mem[address++];
            word b = mem[address++];
            word c = mem[address++];
            
            setRegister(a, value(b) > value(c) ? 1 : 0);
            
            break;
        }
            
        case 6: // jmp: 6 a
        {
            word a = mem[address++];
            
            if (a < kMemSize)
            {
                pc = a;
                return 0;
            }
            else
            {
                fprintf(stderr, ">>ERR! invalid JMP address! %d [pc:%04d]\n", a, pc);
            }
            
            break;
        }
            
        case 7: // jt: 7 a b
        {
            word a = mem[address++];
            word b = mem[address++];
            
            if (value(a))
            {
                if (b < kMemSize)
                {
                    pc = b;
                    return 0;
                }
                else
                {
                    fprintf(stderr, ">>ERR! invalid JT address! %d [pc:%04d]\n", b, pc);
                }
            }
            
            break;
        }
            
        case 8: // jf: 8 a b
        {
            word a = mem[address++];
            word b = mem[address++];
            
            if (value(a) == 0)
            {
                if (b < kMemSize)
                {
                    pc = b;
                    return 0;
                }
                else
                {
                    fprintf(stderr, ">>ERR! invalid JT address! %d [pc:%04d]\n", b, pc);
                }
            }
            
            break;
        }
            
        case 9: // add: 9 a b c
        {
            word a = mem[address++];
            word b = mem[address++];
            word c = mem[address++];
            
            setRegister(a, (unsigned int)value(b) + value(c));
            
            break;
        }
            
        case 10: // mult: 10 a b c
        {
            word a = mem[address++];
            word b = mem[address++];
            word c = mem[address++];
            
            setRegister(a, (unsigned int)value(b) * value(c));
            
            break;
        }
            
        case 11: // mod: 11 a b c
        {
            word a = mem[address++];
            word b = mem[address++];
            word c = mem[address++];
            
            setRegister(a, (unsigned int)value(b) % value(c));
            
            break;
        }
            
        case 12: // and: 12 a b c
        {
            word a = mem[address++];
            word b = mem[address++];
            word c = mem[address++];
            
            setRegister(a, value(b) & value(c));
            
            break;
        }
            
        case 13: // or: 12 a b c
        {
            word a = mem[address++];
            word b = mem[address++];
            word c = mem[address++];
            
            setRegister(a, value(b) | value(c));
            
            break;
        }
            
        case 14: // not: 14 a b
        {
            word a = mem[address++];
            word b = mem[address++];
            
            setRegister(a, ~value(b));
            
            break;
        }
            
        case 15: // rmem: 15 a b
        {
            word a = mem[address++];
            word b = mem[address++];
            
            setRegister(a, mem[value(b)]);
            
            break;
        }
            
        case 16: // wmem: 16 a b
        {
            word a = mem[address++];
            word b = mem[address++];
            
            mem[value(a)] = value(b) & kMemMask;
            
            break;
        }
            
        case 17: // call: 17 a
        {
            word a = mem[address++];
            
            pushStack(address);
            pc = value(a);

            return 0;
        }
            
        case 18: // ret: 18
        {
            pc = popStack();
            
            return 0;
        }
            
        case 19: // out: 19 a
        {
            unsigned int ad = address - 1;
            
            char c;
            while (mem[ad] == 19 && (c = value(mem[ad + 1])))
            {
                if (c > 8 && c < 127)
                    fprintf(stderr, "%c", c);
                
                ad += 2;
                
                if (c == 10)
                    break;
            }

            int d = ad - address + 1;
            if (d < 2) d = 2;
            return d;
        }
            
        case 21: // noop: 21
        {
            return 1;
        }
            
        default:
        {
            fprintf(stderr, ">>ERR! unrecognized instruction! %d [pc:%04d]\n", i, pc);

            return 1;
        }
    }
    
    return instructionLength(i);
}

void run()
{
    active = true;
    while (active)
    {
        if (debug) dumpInstructionAtAddress(pc);
        
        if (isBreakpointAtAddress(pc))
        {
            printf("*-- Breakpoint --------------------\n");
            dumpInstructionAtAddress(pc);
            printf("*-- Breakpoint --------------------\n");

            dumpStack();
            
//            raise(SIGINT);
//            active = false;
            debug = true;
        }
        
        unsigned int sz = runInstructionAtAddress(pc);
        pc += sz;
    }
    
    printf("\n");
}

void reset()
{
    memset(mem, 0, kMemSize);
    
    memset(reg, 0, kRegisterCount * sizeof(word));
    pc = 0;
        
    memset(breakpoint, 0, kMemSize / 2);
}
