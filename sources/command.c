//
//  command.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdio.h>
#include <string.h>


#include "label.h"

#include "debug.h"

#include "processor.h"

#include "debug.h"

#include "command.h"


bool funcHelp(char *param);

bool funcRegisters(char *param);
bool funcStack(char *param);
bool funcLabels(char *param);

bool funcLoad(char *param);
bool funcSave(char *param);

bool funcExit(char *param);


Command commands[] =
{
    { &funcHelp, "help", "display this" },
    
    { &funcRegisters, "regs", "display the registers" },
    { &funcStack, "stack", "display the stack" },
    { &funcLabels, "labels", "display the labels" },
    
    { &funcLoad, "load", "load the machine state" },
    { &funcSave, "save", "save the machine state" },
    
    { &funcExit, "exit", "leave the debugger" },
};

unsigned int commandCount = sizeof(commands) / sizeof(commands[0]);



bool funcRegisters(char *param)
{
    dumpRegisters(0b11111111);
    
    return true;
}

bool funcLabels(char *param)
{
    listLabels();
    
    return true;
}

bool funcStack(char *param)
{
    dumpStack();
    
    return true;
}

bool funcLoad(char *param)
{
    char filename[128];
    sprintf(filename, "%s.sav", &param[1]);
    
    loadFromFile(filename);
    
    return false;
}

bool funcSave(char *param)
{
    char filename[128];
    sprintf(filename, "%s.sav", &param[1]);
    
    saveToFile(filename);
    
    return true;
}

bool funcExit(char *param)
{
    return false;
}
