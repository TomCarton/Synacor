//
//  processor.h
//  Synacor
//
//  Created by Thomas CARTON
//

#ifndef processor_h
#define processor_h

#include <stdbool.h>


extern const unsigned int kMemSize;
extern const unsigned int kStackSize;

extern const unsigned int kRegisterCount;

extern word mem[];
extern int pc, pci;

extern word stack[];
extern int sp;

extern word reg[8];

extern bool active;
extern bool debug;


void reset();

void dumpRegisters(unsigned short rbitfield);
void dumpInstructions(const unsigned int addr, unsigned int icount);
void dumpAllInstructions(const unsigned int start, unsigned int end);
unsigned int runInstructionAtAddress(unsigned int address);

void setBreakpoint(unsigned int address, byte active);

void run();


#endif /* processor_h */