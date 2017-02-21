// synacor.c
//
// written by Thomas CARTON
//

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#include <memory.h>
#include <limits.h>


typedef unsigned short word;
typedef unsigned char byte;

static const unsigned int memsize = 1 << 15;
static const unsigned int stacksize = 1 << 15;

word reg[8];

word mem[memsize];
word pc, pci;

word stack[stacksize];
word sp = stacksize;

word a, b, c;

bool debug = false;
bool unasm = false;


void setup()
{
    memset(mem, 0, memsize);

    memset(reg, 0, 8 * sizeof(word));
    pc = 0;

    a = b = c = 0;
}

word value(word val)
{
	if (val < 32768)
	{
		return val;
	}
	else if (val - 32768 < 8)
	{
		return reg[val & 7];
	}

	fprintf(stderr, ">>ERR! unallowed value! %d [pc:%04d]\n", val, pci);

	return 0;
}

void set(word dest, word val)
{
    if (dest >= memsize && dest < memsize + 8)
    {
        reg[dest - memsize] = val & 32767;
    }
    else
	{
		fprintf(stderr, ">>ERR! invalid destination register for SET! %d [pc:%04d]\n", dest, pci);
	}
}

void pushStack(word v)
{
    if (sp > 0)
    {
        stack[--sp] = v;
    }
    else
    {
        fprintf(stderr, ">>ERR! stack overflow! [pc:%04d]\n", pci);
    }
}

word popStack()
{
    if (sp <= stacksize)
        return stack[sp++];

    return -1;
}

unsigned int dump(const unsigned int addr)
{
    word i = mem[addr];

    char inst[] = "???\0\0\0\0\0";
    unsigned int count = 0;

    switch (i)
    {
        case 0: strcpy(inst, "HALT"); count = 0; break;     // halt: 0
        case 1: strcpy(inst, "SET"); count = 2; break;      // set:  1 a b
        case 2: strcpy(inst, "PUSH"); count = 1; break;     // push: 2 a
        case 3: strcpy(inst, "POP"); count = 1; break;      // pop: 3 a
        case 4: strcpy(inst, "EQ"); count = 3; break;       // eq: 4 a b c
        case 5: strcpy(inst, "GT"); count = 3; break;       // gt: 5 a b c
        case 6: strcpy(inst, "JMP"); count = 1; break;      // jmp: 6 a
        case 7: strcpy(inst, "JT"); count = 2; break;       // jt: 7 a b
        case 8: strcpy(inst, "JF"); count = 2; break;       // jf: 8 a b
        case 9: strcpy(inst, "ADD"); count = 3; break;      // add: 9 a b c
        case 10: strcpy(inst, "MULT"); count = 3; break;    // mult: 10 a b c
        case 11: strcpy(inst, "MOD"); count = 3; break;     // mod: 11 a b c
        case 12: strcpy(inst, "AND"); count = 3; break;     // and: 12 a b c
        case 13: strcpy(inst, "OR"); count = 3; break;      // or: 13 a b c
        case 14: strcpy(inst, "NOT"); count = 2; break;     // not: 14 a b
        case 15: strcpy(inst, "RMEM"); count = 2; break;    // rmem: 15 a b
        case 16: strcpy(inst, "WMEM"); count = 2; break;    // wmem: 16 a b
        case 17: strcpy(inst, "CALL"); count = 1; break;    // call: 17 a
        case 18: strcpy(inst, "RET"); count = 0; break;     // ret: 18
        case 19: strcpy(inst, "OUT"); count = 1; break;     // out: 19 a
        case 20: strcpy(inst, "IN"); count = 1; break;      // in: 20 a
        case 21: strcpy(inst, "NOOP"); count = 0; break;    // noop: 21
    }

	// memory
	fprintf(stderr, "  %08d: %04X ", addr, mem[addr]);
	for (unsigned int i = 0; i < count; ++i)
	{
		fprintf(stderr, " %04X", mem[addr + i + 1]);
	}

	// instruction
    char pad[3 * 5 + 1] = "                ";
    pad[1 + (3 - count) * 5] = '\0';
	fprintf(stderr, "%s\t%s", pad, inst);

	// operands
	for (unsigned int i = 0; i < count; ++i)
	{
		word o = mem[addr + 1 + i];

		if (o < memsize)
		{
            if (mem[addr] == 19 && o > 31 && o <= 255)
            {
                fprintf(stderr, " '%c' (%d)", o, o);
            }
            else
            {
                fprintf(stderr, " %d", o);
            }
        }
		else if (o < 32776)
		{
			fprintf(stderr, " r%d", o & 7);
		}
	}

	fprintf(stderr, "\n");

    return count + 1;
}

void dumps(const unsigned int addr, unsigned int icount)
{
    unsigned int a = addr;

    while (icount--)
    {
        a += dump(a);
    }
}

void desasm(const unsigned int start, unsigned int end)
{
    for (unsigned int a = start; a < end; )
    {
        a += dump(a);
    }
}

void run()
{
	bool active = true;
	while (active)
	{
        if (debug) dump(pc);

        // 1520 // 1755 // 2125
        // if (pc == 1471)
        // {
        //     printf("*-- Breakpoint --------------------\n");
        // }

        pci = pc;
        word i = mem[pc++];

		switch (i)
		{
			case 0: // halt: 0
			{
				active = false;

				fprintf(stderr, ">>HALT: Program ended. [pc:%04d = %d]\n", pc - 1, mem[pc - 1]);

				break;
			}

			case 1: // set: 1 a b
			{
				a = mem[pc++];
				b = mem[pc++];

				set(a, value(b));

				break;
			}

            case 2: // push: 2 a
            {
				a = mem[pc++];

                pushStack(value(a));

                break;
            }

            case 3: // pop: 3 a
            {
				a = mem[pc++];

				if (a < memsize)
				{
					mem[a] = popStack();
				}
				else if (a < memsize + 8)
				{
					reg[a - memsize] = popStack();
				}

                break;
            }

			case 4: // eq: 4 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

                set(a, value(b) == value(c) ? 1 : 0);

				break;
			}

            case 5: // gt: 5 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				set(a, value(b) > value(c) ? 1 : 0);

                break;
            }

			case 6: // jmp: 6 a
			{
				a = mem[pc++];

				if (a < memsize)
				{
					pc = a;
				}
				else
				{
					fprintf(stderr, ">>ERR! invalid JMP address! %d [pc:%04d]\n", a, pci);
				}

				break;
			}

			case 7: // jt: 7 a b
			{
				a = mem[pc++];
				b = mem[pc++];

				if (value(a))
				{
					if (b < memsize)
					{
						pc = b;
					}
					else
					{
						fprintf(stderr, ">>ERR! invalid JT address! %d [pc:%04d]\n", b, pci);
					}
				}

				break;
			}

			case 8: // jf: 8 a b
			{
				a = mem[pc++];
				b = mem[pc++];

				if (value(a) == 0)
				{
					if (b < memsize)
					{
						pc = b;
					}
					else
					{
						fprintf(stderr, ">>ERR! invalid JT address! %d [pc:%04d]\n", b, pci);
					}
				}

				break;
			}

			case 9: // add: 9 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

                set(a, (unsigned int)value(b) + value(c));
			
				break;
			}

            case 10: // mult: 10 a b c
            {
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				set(a, (unsigned int)value(b) * value(c));

                break;
            }

            case 11: // mod: 11 a b c
            {
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				set(a, (unsigned int)value(b) % value(c));

                break;
            }

			case 12: // and: 12 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				set(a, value(b) & value(c));

				break;
			}

			case 13: // or: 12 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				set(a, value(b) | value(c));

				break;
			}

			case 14: // not: 14 a b
			{
				a = mem[pc++];
				b = mem[pc++];

				set(a, ~value(b));

				break;
			}

			case 15: // rmem: 15 a b
            {
				a = mem[pc++];
				b = mem[pc++];

				set(a, mem[value(b)]);

				break;
			}

			case 16: // wmem: 16 a b
			{
				a = mem[pc++];
				b = mem[pc++];

                mem[value(a)] = value(b);

				break;
			}

			case 17: // call: 17 a
			{
				a = mem[pc++];

                pushStack(pc);
                pc = value(a);

                break;
            }

            case 18: // ret: 18
            {
                pc = popStack();

                break;
            }

			case 19: // out: 19 a
			{
				a = mem[pc++];

				fprintf(stdout, "%c", value(a));

				break;
			}

			case 21: // noop: 21
			{
				break;
			}

			default:
			{
				fprintf(stderr, ">>ERR! unrecognized instruction! %d [pc:%04d]\n", i, pci);
				return;
			}
		}
	}

	printf("\n");
}

unsigned int readFile(const char *filename)
{
	FILE *f = NULL;
    if (!(f = fopen(filename, "r")))
    {
        fprintf(stderr, ">>ERR! failed to open `%s' for reading\n", filename);
        return -1;
    }

	int sizeread = (unsigned int)fread(mem, 1, 32768, f);

    fclose(f);

    return sizeread;
}

int main(int argc, const char *argv[])
{
    char filename[PATH_MAX] = "\0";

	if (argc < 2)
	   	goto usage;

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
        setup();

        unsigned int size = readFile(filename);
		if (size > 0)
		{
            if (debug && unasm)
            {
                desasm(0, size);
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
    fprintf(stderr, "     -u, --unasm    - enable debug\n\n");

    fprintf(stderr, "   example:\n");
    fprintf(stderr, "     %s example.bin\n", argv[0]);

    fprintf(stderr, "\n");

    return 0;
}
