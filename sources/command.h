//
//  command.h
//  Synacor
//
//  Created by Thomas CARTON
//

#ifndef command_h
#define command_h

#include <stdbool.h>


typedef bool (*fp)(char *);


typedef struct
{
    fp function;

    char name[16];
    char explanation[64];
} Command;

extern unsigned int commandCount;
extern Command commands[];

#endif /* command_h */
