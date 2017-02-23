//
//  processor.h
//  Synacor
//
//  Created by Thomas CARTON
//

#ifndef processor_h
#define processor_h

extern const unsigned int memsize;
extern word mem[];
extern int pc, pci;

extern const unsigned int stacksize;
extern word stack[];
extern int sp;

extern word reg[8];
extern word a, b, c;

extern bool active;
extern bool debug;

void reset();

void run();

#endif /* processor_h */
