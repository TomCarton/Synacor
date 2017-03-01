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

#include "console.h"

#include "instruction.h"


Instruction instructions[] =
{
    // op   name    oc in sp
    { 0x00, "HALT", 0, 0, 1, }, // halt: 0
    
    { 0x01, "SET",  2, 2, 0, }, // set: 1 a b
    
    { 0x02, "PUSH", 1, 0, 0, }, // push: 2 a
    { 0x03, "POP",  1, 0, 0, }, // pop: 3 a
    
    { 0x04, "EQ",   3, 0, 0, }, // eq: 4 a b c
    { 0x05, "GT",   3, 0, 0, }, // gt: 5 a b c
    
    { 0x06, "JMP",  1, 1, 1, }, // jmp: 6 a
    { 0x07, "JT",   2, 2, 1, }, // jt: 7 a b
    { 0x08, "JF",   2, 2, 1, }, // jf: 8 a b
    
    { 0x09, "ADD",  3, 0, 0, }, // add: 9 a b c
    { 0x0A, "MULT", 3, 0, 0, }, // mult: 10 a b c
    { 0x0B, "MOD",  3, 0, 0, }, // mod: 11 a b c
    
    { 0x0C, "AND",  3, 0, 0, }, // and: 12 a b c
    { 0x0D, "OR",   3, 0, 0, }, // or: 13 a b c
    { 0x0E, "NOT",  2, 0, 0, }, // not: 14 a b

    { 0x0F, "RMEM", 2, 0, 0, }, // rmem: 15 a b
    { 0x10, "WMEM", 2, 0, 0, }, // wmem: 16 a b

    { 0x11, "CALL", 1, 1, 1, }, // call: 17 a
    { 0x12, "RET",  0, 0, 1, }, // ret: 18

    { 0x13, "OUT",  1, 1, 0, }, // out: 19 a
    { 0x14, "IN",   1, 0, 0, }, // in: 20 a

    { 0x15, "NOOP", 0, 0, 0, }, // noop: 21

    { 0xffff, "???", 0, 0, 0, },
};

unsigned int instructionCount = sizeof(instructions) / sizeof(instructions[0]);


unsigned int instructionLength(word opcode)
{
    return (opcode <= 0x15) ? instructions[opcode].operandCount + 1: 1;
}


// MARK: - Execution

unsigned int runInstructionAtAddress(unsigned int address)
{
    word i = memory[address++];
    
    switch (i)
    {
        case 0: // halt: 0
        {
            active = false;
            
            fprintf(stderr, ">>HALT: Program ended. [pc:%04d = %d]\n", pc - 1, memory[pc - 1]);
            
            break;
        }
            
        case 1: // set: 1 a b
        {
            word a = memory[address++];
            word b = memory[address++];
            
            setMemory(a, getValue(b));

            break;
        }
            
        case 2: // push: 2 a
        {
            word a = memory[address++];
            
            pushStack(getValue(a));
            
            break;
        }
            
        case 3: // pop: 3 a
        {
            word a = memory[address++];

            setMemory(a, popStack());
            
            break;
        }
            
        case 4: // eq: 4 a b c
        {
            word a = memory[address++];
            word b = memory[address++];
            word c = memory[address++];
            
            setMemory(a, getValue(b) == getValue(c) ? 1 : 0);
            
            break;
        }
            
        case 5: // gt: 5 a b c
        {
            word a = memory[address++];
            word b = memory[address++];
            word c = memory[address++];
            
            setMemory(a, getValue(b) > getValue(c) ? 1 : 0);
            
            break;
        }
            
        case 6: // jmp: 6 a
        {
            word a = memory[address++];
            
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
            word a = memory[address++];
            word b = memory[address++];
            
            if (getValue(a))
            {
                pc = getValue(b);
                return 0;
            }
            
            break;
        }
            
        case 8: // jf: 8 a b
        {
            word a = memory[address++];
            word b = memory[address++];
            
            if (getValue(a) == 0)
            {
                pc = getValue(b);
                return 0;
            }
            
            break;
        }
            
        case 9: // add: 9 a b c
        {
            word a = memory[address++];
            word b = memory[address++];
            word c = memory[address++];
            
            setMemory(a, (getValue(b) + getValue(c)) & 32767);
            
            break;
        }
            
        case 10: // mult: 10 a b c
        {
            word a = memory[address++];
            word b = memory[address++];
            word c = memory[address++];
            
            setMemory(a, (getValue(b) * getValue(c)) & 32767);
            
            break;
        }
            
        case 11: // mod: 11 a b c
        {
            word a = memory[address++];
            word b = memory[address++];
            word c = memory[address++];
            
            setMemory(a, (unsigned int)getValue(b) % getValue(c));
            
            break;
        }
            
        case 12: // and: 12 a b c
        {
            word a = memory[address++];
            word b = memory[address++];
            word c = memory[address++];
            
            setMemory(a, getValue(b) & getValue(c));
            
            break;
        }
            
        case 13: // or: 12 a b c
        {
            word a = memory[address++];
            word b = memory[address++];
            word c = memory[address++];
            
            setMemory(a, getValue(b) | getValue(c));
            
            break;
        }
            
        case 14: // not: 14 a b
        {
            word a = memory[address++];
            word b = memory[address++];
            
            setMemory(a, getValue(b) ^ kMemMask);
            
            break;
        }
            
        case 15: // rmem: 15 a b
        {
            word a = memory[address++];
            word b = memory[address++];
            
            setMemory(a, memory[getValue(b)]);
            
            break;
        }
            
        case 16: // wmem: 16 a b
        {
            word a = memory[address++];
            word b = memory[address++];
            
            memory[getValue(a)] = getValue(b) & kMemMask;
            
            break;
        }
            
        case 17: // call: 17 a
        {
            word a = memory[address++];
            
            pushStack(address);
            pc = getValue(a);
            
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
            while (memory[ad] == 19 && (c = getValue(memory[ad + 1])))
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
            
        case 20: // in: 20 a
        {
            word a = memory[address++];

            char c = getchar();

            if (c == '>')
            {
                getchar();

                if (startConsole())
                    return 0;

                c = getchar();
            }
            
            setMemory(a, c);
            
            break;
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

