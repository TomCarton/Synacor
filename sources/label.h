//
//  label.h
//  Synacor
//
//  Created by Thomas CARTON
//

#ifndef label_h
#define label_h

#include "types.h"


typedef struct { word address; char name[10]; } Label;

extern unsigned int labelCount;
extern Label labels[];


void listLabels();
Label *labelAtAddress(const unsigned int address);

void scanForLabels(const unsigned int addr, const unsigned int end);


#endif /* label_h */
