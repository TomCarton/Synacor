//
//  processor.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdbool.h>
#include <memory.h>
#include <stdio.h>

#include "types.h"
#include "instruction.h"

#include "processor.h"


const unsigned int memsize = 1 << 15;
const unsigned int memmask = memsize -1;

const unsigned int stacksize = 1 << 15;


word mem[memsize];
int pc, pci;

word stack[stacksize];
int sp = stacksize;

word reg[8];
word a, b, c;

bool active = false;
bool debug = false;



void reset()
{
    memset(mem, 0, memsize);
    
    memset(reg, 0, 8 * sizeof(word));
    pc = 0;
    
    a = b = c = 0;
}


word value(word val)
{
    if (val < 32768)
    {
        return val;
    }
    else if (val < 32768 + 8)
    {
        return reg[val & 7];
    }
    
    fprintf(stderr, ">>ERR! unallowed value! %d [pc:%04d]\n", val, pci);
    active = false;
    
    return 0;
}

void set(word dest, word val)
{
    if (a >= memsize || a < memsize + 8)
    {
        reg[a - memsize] = b;
    }
    else
    {
        fprintf(stderr, ">>ERR! illegal register! r%d [pc:%04d]\n", a, pci);
        active = false;
    }
}

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
    if (sp <= stacksize)
    {
        return stack[sp++];
    }
    
    fprintf(stderr, ">>ERR! stack overflow! [pc:%04d]\n", pci);
    active = false;
    
    return -1;
}

void runInstruction(unsigned int address)
{
    word i = mem[address];
    
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
            a = mem[pc++];
            b = mem[pc++];
            
            set(a, value(b));
            
            break;
        }
            
        case 2: // push: 2 a
        {
            a = mem[pc++];
            
            pushStack(value(a));
            
            break;
        }
            
        case 3: // pop: 3 a
        {
            a = mem[pc++];
            
            if (a < memsize)
            {
                mem[a] = popStack();
            }
            else if (a < memsize + 8)
            {
                reg[a - memsize] = popStack();
            }
            
            break;
        }
            
        case 4: // eq: 4 a b c
        {
            a = mem[pc++];
            b = mem[pc++];
            c = mem[pc++];
            
            set(a, value(b) == value(c) ? 1 : 0);
            
            break;
        }
            
        case 5: // gt: 5 a b c
        {
            a = mem[pc++];
            b = mem[pc++];
            c = mem[pc++];
            
            set(a, value(b) > value(c) ? 1 : 0);
            
            break;
        }
            
        case 6: // jmp: 6 a
        {
            a = mem[pc++];
            
            if (a < memsize)
            {
                pc = a;
            }
            else
            {
                fprintf(stderr, ">>ERR! invalid JMP address! %d [pc:%04d]\n", a, pci);
            }
            
            break;
        }
            
        case 7: // jt: 7 a b
        {
            a = mem[pc++];
            b = mem[pc++];
            
            if (value(a))
            {
                if (b < memsize)
                {
                    pc = b;
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
            a = mem[pc++];
            b = mem[pc++];
            
            if (value(a) == 0)
            {
                if (b < memsize)
                {
                    pc = b;
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
            a = mem[pc++];
            b = mem[pc++];
            c = mem[pc++];
            
            set(a, (unsigned int)value(b) + value(c));
            
            break;
        }
            
        case 10: // mult: 10 a b c
        {
            a = mem[pc++];
            b = mem[pc++];
            c = mem[pc++];
            
            set(a, (unsigned int)value(b) * value(c));
            
            break;
        }
            
        case 11: // mod: 11 a b c
        {
            a = mem[pc++];
            b = mem[pc++];
            c = mem[pc++];
            
            set(a, (unsigned int)value(b) % value(c));
            
            break;
        }
            
        case 12: // and: 12 a b c
        {
            a = mem[pc++];
            b = mem[pc++];
            c = mem[pc++];
            
            set(a, value(b) & value(c));
            
            break;
        }
            
        case 13: // or: 12 a b c
        {
            a = mem[pc++];
            b = mem[pc++];
            c = mem[pc++];
            
            set(a, value(b) | value(c));
            
            break;
        }
            
        case 14: // not: 14 a b
        {
            a = mem[pc++];
            b = mem[pc++];
            
            set(a, ~value(b));
            
            break;
        }
            
        case 15: // rmem: 15 a b
        {
            a = mem[pc++];
            b = mem[pc++];
            
            set(a, mem[value(b)]);
            
            break;
        }
            
        case 16: // wmem: 16 a b
        {
            a = mem[pc++];
            b = mem[pc++];
            
            mem[value(a)] = value(b);
            
            break;
        }
            
        case 17: // call: 17 a
        {
            a = mem[pc++];
            
            pushStack(pc);
            pc = value(a);
            
            break;
        }
            
        case 18: // ret: 18
        {
            pc = popStack();
            
            break;
        }
            
        case 19: // out: 19 a
        {
            a = mem[pc++];
            
            char str[8];
            str[0] = str[1];
            str[1] = str[2];
            str[2] = str[3];
            str[3] = str[4];
            str[4] = str[5];
            str[5] = str[6];
            str[6] = a;
            str[7] = 0;
            
            if (a > 9 && a < 128)
            {
                fprintf(stdout, "%c", a);
            }
            
            break;
        }
            
        case 21: // noop: 21
        {
            break;
        }
            
        default:
        {
            fprintf(stderr, ">>ERR! unrecognized instruction! %d [pc:%04d]\n", i, pci);
            return;
        }
    }
}

void run()
{
    active = true;
    while (active)
    {
        if (debug) dumpInstructionAt(pc);
        
        int breakpoints[] = {590, };//1471, 1520, 1755, 2125};
        for (unsigned int i = 0; i < sizeof(breakpoints) / sizeof(breakpoints[0]); ++i)
        {
            if (pc == breakpoints[i])
            {
                printf("*-- Breakpoint --------------------\n");
                
                debug = true;
                //                active = false;
            }
        }
        
        pci = pc;
        runInstruction(pc++);
    }
    
    printf("\n");
}
