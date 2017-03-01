//
//  console.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdio.h>
#include <string.h>

#include "label.h"

#include "debug.h"

#include "console.h"


typedef bool (*fp)(char *);

typedef struct
{
    char name[16];
    fp function;
} Command;


bool funcHelp(char *param);
bool funcRegisters(char *param);
bool funcLabels(char *param);
bool funcStack(char *param);
bool funcExit(char *param);


Command commands[] =
{
    { "help", &funcHelp },
    
    { "registers", &funcRegisters },
    { "stack", &funcStack },
    { "labels", &funcLabels },
    
    { "exit", &funcExit },
};

unsigned int commandCount = sizeof(commands) / sizeof(commands[0]);

char commandBuffer[256] = "\0";


bool funcHelp(char *param)
{
    fprintf(stderr, "\n  COMMANDS\n ------------------------------------------\n");

    for (unsigned int i = 0; i < commandCount; ++i)
    {
        fprintf(stderr, "  %s\n", commands[i].name);
    }

    fprintf(stderr, " ------------------------------------------\n");
    
    return true;
}

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

bool funcExit(char *param)
{
    return false;
}


bool isCommand(const char *command)
{
    return strncmp(commandBuffer, command, strlen(command)) == 0;
}

bool startConsole()
{
    bool active = true;
    while (active)
    {
        printf("> ");

        fgets(commandBuffer, sizeof(commandBuffer), stdin);
        size_t len = strlen(commandBuffer);
        commandBuffer[len - 1] = '\0';

        unsigned int i;
        for (i = 0; i < commandCount; ++i)
        {
            if (isCommand(commands[i].name))
            {
                active = (*(commands[i].function))(commandBuffer + strlen(commands[i].name));

                break;
            }
        }
        
        if (i == commandCount)
        {
            fprintf(stderr, "unknown command '%s' (type 'help' for a list of valid commands)\n", commandBuffer);
        }
    }

    return false;
}
