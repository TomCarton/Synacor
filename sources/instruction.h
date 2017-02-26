//
//  instruction.h
//  Synacor
//
//  Created by Thomas CARTON
//

#ifndef instruction_h
#define instruction_h


typedef struct
{
    word opcode;
    unsigned char name[5];
    unsigned int operandCount;
    unsigned int lind;
    unsigned int space;
} Instruction;

extern unsigned int instructionCount;
extern Instruction instructions[];


unsigned int instructionLength(word instruction);

unsigned int dumpInstructionAtAddress(const unsigned int addr);

#endif /* instruction_h */
