//
//  console.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdio.h>
#include <string.h>

#include "command.h"

#include "console.h"


char commandBuffer[256] = "\0";


bool funcHelp(char *param)
{
    fprintf(stderr, "\n  COMMANDS\n ------------------------------------------\n");

    for (unsigned int i = 0; i < commandCount; ++i)
    {
        fprintf(stderr, "  %s:     \t%s\n", commands[i].name, commands[i].explanation);
    }

    fprintf(stderr, " ------------------------------------------\n");
    
    return true;
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
