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

#include "processor.h"


const unsigned int kMemSize = 1 << 15;
const unsigned int kMemMask = kMemSize - 1;

const unsigned int kStackSize = 1 << 15;

const unsigned int kRegisterCount = 8;


word mem[kMemSize];
int pc, pci;

word stack[kStackSize];
int sp = kStackSize;

word reg[kRegisterCount];
word a, b, c;

bool active = false;
bool debug = false;



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
            fprintf(stderr, "  R%i: 0x%04X %s %d\n", i, reg[i], b2a(reg[i]), reg[i]);
        }
        mask <<= 1;
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
    
    fprintf(stderr, ">>ERR! unallowed value! %d [pc:%04d]\n", val, pci);
    active = false;
    
    return 0;
}

void set(word dest, word val)
{
    if (a >= kMemSize || a < kMemSize + kRegisterCount)
    {
        reg[a - kMemSize] = b;
    }
    else
    {
        fprintf(stderr, ">>ERR! illegal register! r%d [pc:%04d]\n", a, pci);
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
        fprintf(stderr, ">>ERR! stack overflow! [pc:%04d]\n", pci);
        active = false;
    }
}

word popStack()
{
    if (sp <= kStackSize)
    {
        return stack[sp++];
    }
    
    fprintf(stderr, ">>ERR! stack overflow! [pc:%04d]\n", pci);
    active = false;
    
    return -1;
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
            a = mem[address++];
            b = mem[address++];
            
            set(a, value(b));
            
            break;
        }
            
        case 2: // push: 2 a
        {
            a = mem[address++];
            
            pushStack(value(a));
            
            break;
        }
            
        case 3: // pop: 3 a
        {
            a = mem[address++];
            
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
            a = mem[address++];
            b = mem[address++];
            c = mem[address++];
            
            set(a, value(b) == value(c) ? 1 : 0);
            
            break;
        }
            
        case 5: // gt: 5 a b c
        {
            a = mem[address++];
            b = mem[address++];
            c = mem[address++];
            
            set(a, value(b) > value(c) ? 1 : 0);
            
            break;
        }
            
        case 6: // jmp: 6 a
        {
            a = mem[address++];
            
            if (a < kMemSize)
            {
                pc = a;
                return 0;
            }
            else
            {
                fprintf(stderr, ">>ERR! invalid JMP address! %d [pc:%04d]\n", a, pci);
            }
            
            break;
        }
            
        case 7: // jt: 7 a b
        {
            a = mem[address++];
            b = mem[address++];
            
            if (value(a))
            {
                if (b < kMemSize)
                {
                    pc = b;
                    return 0;
                }
                else
                {
                    fprintf(stderr, ">>ERR! invalid JT address! %d [pc:%04d]\n", b, pci);
                }
            }
            
            break;
        }
            
        case 8: // jf: 8 a b
        {
            a = mem[address++];
            b = mem[address++];
            
            if (value(a) == 0)
            {
                if (b < kMemSize)
                {
                    pc = b;
                    return 0;
                }
                else
                {
                    fprintf(stderr, ">>ERR! invalid JT address! %d [pc:%04d]\n", b, pci);
                }
            }
            
            break;
        }
            
        case 9: // add: 9 a b c
        {
            a = mem[address++];
            b = mem[address++];
            c = mem[address++];
            
            set(a, (unsigned int)value(b) + value(c));
            
            break;
        }
            
        case 10: // mult: 10 a b c
        {
            a = mem[address++];
            b = mem[address++];
            c = mem[address++];
            
            set(a, (unsigned int)value(b) * value(c));
            
            break;
        }
            
        case 11: // mod: 11 a b c
        {
            a = mem[address++];
            b = mem[address++];
            c = mem[address++];
            
            set(a, (unsigned int)value(b) % value(c));
            
            break;
        }
            
        case 12: // and: 12 a b c
        {
            a = mem[address++];
            b = mem[address++];
            c = mem[address++];
            
            set(a, value(b) & value(c));
            
            break;
        }
            
        case 13: // or: 12 a b c
        {
            a = mem[address++];
            b = mem[address++];
            c = mem[address++];
            
            set(a, value(b) | value(c));
            
            break;
        }
            
        case 14: // not: 14 a b
        {
            a = mem[address++];
            b = mem[address++];
            
            set(a, ~value(b));
            
            break;
        }
            
        case 15: // rmem: 15 a b
        {
            a = mem[address++];
            b = mem[address++];
            
            set(a, mem[value(b)]);
            
            break;
        }
            
        case 16: // wmem: 16 a b
        {
            a = mem[address++];
            b = mem[address++];
            
            mem[value(a)] = value(b);
            
            break;
        }
            
        case 17: // call: 17 a
        {
            a = mem[address++];
            
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
            while (mem[ad] == 19 && (c = mem[ad + 1]))
            {
                fprintf(stderr, "%c", c);
                
                ad += 2;
                
                if (c == 10)
                    break;
            }
            
            return ad - address + 1;
        }
            
        case 21: // noop: 21
        {
            return 1;
        }
            
        default:
        {
            fprintf(stderr, ">>ERR! unrecognized instruction! %d [pc:%04d]\n", i, pci);

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
        
        int breakpoints[] = {590};//1471, 1520, 1755, 2125};
        for (unsigned int i = 0; i < sizeof(breakpoints) / sizeof(breakpoints[0]); ++i)
        {
            if (pc == breakpoints[i])
            {
                printf("*-- Breakpoint --------------------\n");

//                raise(SIGINT);
//                active = false;

                debug = true;
            }
        }
        
        pci = pc;
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
    
    a = b = c = 0;
}
