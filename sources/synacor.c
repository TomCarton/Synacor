// synacor.c
//
// written by Thomas CARTON
//

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#include <memory.h>
#include <limits.h>

#include "types.h"
#include "label.h"
#include "instruction.h"
#include "processor.h"

#include "debug.h"


bool unasm = false;


unsigned int readFile(const char *filename)
{
    FILE *f = NULL;
    if (!(f = fopen(filename, "r")))
    {
        fprintf(stderr, ">>ERR! failed to open `%s' for reading\n", filename);
        return -1;
    }
    
    int sizeread = (unsigned int)fread(memory, 1, 65536, f);
    
    fclose(f);
    
    return sizeread;
}

int main(int argc, const char *argv[])
{
    char filename[PATH_MAX] = "\0";
    
#   if DEBUG
        strcpy(filename, "/Users/thomascarton/Work/Tom/Synacor/challenge.bin");
#   else
        if (argc < 2)
            goto usage;
#   endif
    
    // read parameters
    for (unsigned int i = 1; i < argc; ++i)
    {
        // debug
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0)
        {
            debug = true;
        }
        
        // unasm
        else if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--unasm") == 0)
        {
            unasm = true;
        }
        
        // help
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            goto usage;
        }
        
        // unknown parameter
        else if (argv[i][0] == '-')
        {
            fprintf(stderr, "\n!  Unknown parameter \"%s\"\n", argv[i]);
            
            goto usage;
        }
        
        // input
        else
        {
            strcpy(filename, argv[i]);
        }
    }
    
    if (filename[0] != '\0')
    {
        reset();
        
        unsigned int size = readFile(filename);
        if (size > 0)
        {            
            if (debug && unasm)
            {
                scanForLabels(0, size >> 1);
                
                dumpInstructionsFromRange(0, size >> 1);
            }
            else
            {
                run();
            }
            
            return 0;
        }
        
        return 1;
    }
    
usage:
    fprintf(stderr, "\n");
    fprintf(stderr, "   Synacor\n");
    fprintf(stderr, "   -------\n\n");
    
    fprintf(stderr, "   usage:\n");
    fprintf(stderr, "     %s path [-h/--help] | [-d/--debug|-u/--unasm]\n\n", argv[0]);
    
    fprintf(stderr, "   parameters:\n");
    fprintf(stderr, "     -h, --help     - display this\n\n");
    
    fprintf(stderr, "     -d, --debug    - enable debug\n");
    fprintf(stderr, "     -u, --unasm    - unasm program\n\n");
    
    fprintf(stderr, "   example:\n");
    fprintf(stderr, "     %s example.bin\n", argv[0]);
    
    fprintf(stderr, "\n");
    
    return 0;
}
