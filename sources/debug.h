//
//  debug.h
//  Synacor
//
//  Created by Thomas CARTON
//

#ifndef debug_h
#define debug_h

void dumpLabels();
void dumpStack();
void dumpRegisters(unsigned short rbitfield);

void dumpMemory(const unsigned int start, unsigned int end);

void dumpInstructions(const unsigned int addr, unsigned int icount);
void dumpInstructionsFromRange(const unsigned int start, unsigned int end);

void loadFromFile(const char *filename);
void saveToFile(const char *filename);

#endif /* debug_h */
