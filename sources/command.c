//
//  command.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdlib.h>

#include <stdio.h>
#include <string.h>


#include "label.h"

#include "instruction.h"
#include "processor.h"

#include "debug.h"

#include "command.h"


bool funcHelp(char *param);

bool funcRegisters(char *param);
bool funcStack(char *param);
bool funcLabels(char *param);

bool funcDisplay(char *param);
bool funcContinue(char *param);
bool funcStep(char *param);

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

    { &funcDisplay, "d", "display the current instructions" },
    { &funcContinue, "c", "continue the execution" },
    { &funcStep, "s", "advance/execute one instruction" },

    { &funcExit, "exit", "leave the debugger" },
};

unsigned int commandCount = sizeof(commands) / sizeof(commands[0]);


bool funcDisplay(char *param)
{
    dumpInstructions(pc - 20, 20);
    
    return true;
}

bool funcContinue(char *param)
{
    debug = false;
    
    return false;
}

bool funcStep(char *param)
{
    pc += runInstructionAtAddress(pc);
    dumpInstructionAtAddress(pc);

    return true;
}

bool funcRegisters(char *param)
{
    dumpRegisters(0b1111111111);
    
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
    exit(0);
    return false;
}
