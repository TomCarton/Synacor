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
extern const unsigned int kMemMask;

extern const unsigned int kStackSize;

extern const unsigned int kRegisterCount;

extern word memory[];
extern int pc;

extern word stack[];
extern int sp;

extern bool active;
extern bool debug;


extern byte breakpoint[];


// memory

void setMemory(word address, word value);
word getMemory(word address);
word getValue(word value);

// stack
void pushStack(word v);
word popStack();

// breakpoint
bool isBreakpointAtAddress(unsigned int address);
void setBreakpoint(unsigned int address, byte active);
void switchBreakpoint(unsigned int address);

// execution
void reset();
void run();


#endif /* processor_h */
