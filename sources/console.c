//
//  console.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "label.h"
#include "instruction.h"
#include "processor.h"

#include "debug.h"

#include "console.h"


typedef bool (*cp)(unsigned int argc, char **argv);

bool cmdHelp(unsigned int argc, char **argv);

bool cmdPrint(unsigned int argc, char **argv);

bool cmdRegisters(unsigned int argc, char **argv);
bool cmdStack(unsigned int argc, char **argv);
bool cmdLabels(unsigned int argc, char **argv);

bool cmdBreakpoints(unsigned int argc, char **argv);
bool cmdBreakpoint(unsigned int argc, char **argv);

bool cmdDisplay(unsigned int argc, char **argv);
bool cmdContinue(unsigned int argc, char **argv);
bool cmdStep(unsigned int argc, char **argv);

bool cmdLoad(unsigned int argc, char **argv);
bool cmdSave(unsigned int argc, char **argv);

bool cmdExit(unsigned int argc, char **argv);


typedef struct
{
    cp cmd;
    
    char name[16];
    char shortcut[4];
    
    char explanation[64];
    
} Command;


static const Command kCommands[] =
{
    { &cmdHelp, "help", "h", "display this help" },
    { NULL, "", "", "" },

    { &cmdPrint, "print", "p", "print a value" },
    { NULL, "", "", "" },
    
    { &cmdRegisters, "regs", "r", "display the registers" },
    { &cmdStack, "stack", "st", "display the stack" },
    { &cmdLabels, "labels", "lb", "display the labels" },
    { NULL, "", "", "" },
    
    { &cmdBreakpoints, "breakpoints", "bk", "display the breakpoints" },
    { &cmdBreakpoint, "breakpoint", "b", "switch breakpoint on/off at given address" },
    { NULL, "", "", "" },
    
    { &cmdDisplay, "display", "d", "display the current instructions" },
    { &cmdContinue, "continue", "c", "continue the execution" },
    { &cmdStep, "step", "s", "advance/execute one instruction" },
    { NULL, "", "", "" },
    
    { &cmdLoad, "load", "", "load the machine state" },
    { &cmdSave, "save", "", "save the machine state" },
    { NULL, "", "", "" },
    
    { &cmdExit, "exit", "x", "leave the debugger" },
};

static const unsigned int kCommandCount = sizeof(kCommands) / sizeof(kCommands[0]);


bool cmdHelp(unsigned int argc, char **argv)
{
    fprintf(stderr, "\n  COMMANDS\n ------------------------------------------\n");
    
    for (unsigned int i = 0; i < kCommandCount; ++i)
    {
        unsigned int p = 0;
        fprintf(stderr, "  %s", kCommands[i].name); p += strlen(kCommands[i].name) + 2;
        if (strlen(kCommands[i].shortcut) > 0)
        {
            fprintf(stderr, ", %s", kCommands[i].shortcut); p += strlen(kCommands[i].shortcut) + 2;
        }
        fprintf(stderr, "\t%s\n", kCommands[i].explanation);
    }
    
    fprintf(stderr, " ------------------------------------------\n");
    
    return true;
}

bool cmdPrint(unsigned int argc, char **argv)
{
    if (argc == 2)
    {
        int v = eval(argv[1]) & kMemMask;
        
        fprintf(stderr, "%05d 0x%04X %s '%c%c'\n", v, v, b2a(v), chr(v >> 8), chr(v));
    }
    
    return true;
}

bool cmdDisplay(unsigned int argc, char **argv)
{
    dumpInstructions(pc - 20, 20);
    
    return true;
}

bool cmdContinue(unsigned int argc, char **argv)
{
    debug = false;
    
    return false;
}

bool cmdStep(unsigned int argc, char **argv)
{
    pc += runInstructionAtAddress(pc);
    dumpInstructionAtAddress(pc);
    
    return true;
}

bool cmdRegisters(unsigned int argc, char **argv)
{
    unsigned short mask = 255;
    
    if (argc == 2)
    {
        mask = eval(argv[1]);
    }
        
    dumpRegisters(mask);
    
    return true;
}

bool cmdLabels(unsigned int argc, char **argv)
{
    listLabels();
    
    return true;
}

bool cmdStack(unsigned int argc, char **argv)
{
    dumpStack();
    
    return true;
}

bool cmdBreakpoints(unsigned int argc, char **argv)
{
    dumpBreakpoints();
    
    return true;
}

bool cmdBreakpoint(unsigned int argc, char **argv)
{
    if (argc == 2)
    {
        switchBreakpoint(eval(argv[1]));
    }
    
    return true;
}

bool cmdLoad(unsigned int argc, char **argv)
{
    char filename[128];
    sprintf(filename, "%s.sav", argv[1]);
    
    loadFromFile(filename);
    
    return false;
}

bool cmdSave(unsigned int argc, char **argv)
{
    char filename[128];
    sprintf(filename, "%s.sav", argv[1]);
    
    saveToFile(filename);
    
    return true;
}

bool cmdExit(unsigned int argc, char **argv)
{
    exit(0);
    
    return false;
}


static const unsigned int kConsoleMaxLength = 256;
static const unsigned int kConsoleMaxArgs = 4;

char commandBuffer[kConsoleMaxLength] = "\0";


unsigned int extractParameters(char **argv)
{
    unsigned int argc = 0;
    
    char *p = strtok(commandBuffer, " ");
    while (p && argc < kConsoleMaxArgs - 1)
    {
        argv[argc++] = p;
        
        p = strtok(NULL, " ");
    }
    argv[argc] = NULL;
    
    return argc;
}

unsigned int findCommand(char *label)
{
    for (unsigned int i = 0; i < kCommandCount; ++i)
    {
        if (strcmp(label, kCommands[i].shortcut) == 0)
        {
            return i;
        }
        if (strcmp(label, kCommands[i].name) == 0)
        {
            return i;
        }
    }

    return kCommandCount;
}

bool console()
{
    bool active = true;
    while (active)
    {
        printf("? ");
        
        fgets(commandBuffer, sizeof(commandBuffer), stdin);
        commandBuffer[strlen(commandBuffer) - 1] = '\0';
        char *argv[kConsoleMaxArgs];
        unsigned int argc = extractParameters(argv);

        if (argc > 0)
        {
            unsigned int n = findCommand(argv[0]);
            if (n == kCommandCount)
            {
                fprintf(stderr, "unknown command '%s' (type 'help' for a list of valid commands)\n", argv[0] ?: "");
            }
            else
            {
                active = (*(kCommands[n].cmd))(argc, argv);
            }
        }
    }

    return false;
}
