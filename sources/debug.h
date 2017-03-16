//
//  debug.h
//  Synacor
//
//  Created by Thomas CARTON
//

#ifndef debug_h
#define debug_h


// helpers
char *b2a(unsigned short val);
char chr(unsigned char c);
int eval(const char *str);

// dump
void dumpLabels();
void dumpStack();
void dumpRegisters(unsigned short rbitfield);

void dumpMemory(const unsigned int start, unsigned int end);

unsigned int dumpInstructionAtAddress(const unsigned int addr);
void dumpInstructions(const unsigned int addr, unsigned int icount);
void dumpInstructionsFromRange(const unsigned int start, unsigned int end);

void dumpBreakpoints();

// load/save state
void loadFromFile(const char *filename);
void saveToFile(const char *filename);


#endif /* debug_h */
