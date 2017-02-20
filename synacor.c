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
word pc;

word stack[stacksize];
word sp = stacksize;

word a, b, c;

bool debug = false;


void setup()
{
    memset(mem, 0, memsize);

    memset(reg, 0, 8 * sizeof(word));
    pc = 0;

    a = b = c = 0;
}

word value(word v)
{
	if (v < 32768)
	{
		return v;
	}
	else if (v - 32768 < 8)
	{
		return reg[v & 7];
	}

	fprintf(stderr, ">>ERR! unallowed value! [pc:%04d = %d]\n", pc, v);

	return 0;
}

void store(word v, word dest)
{
    if (dest < memsize)
	{
        v &= 32767;

		mem[dest + 0] = v & 0x00FF;
		mem[dest + 1] = v & 0xFF00;
	}
	else if (dest < 32776)
	{
		reg[dest - memsize] = v & 32767;
	}
	else
	{
		fprintf(stderr, ">>ERR! unallowed destination! [pc:%04d = %d]\n", pc, v);
	}
}

void push(word v)
{
    stack[--sp] = v;
}

word pop()
{
    return stack[sp++];
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
	fprintf(stderr, "  %08d: %04X", addr, mem[addr]);
	for (unsigned int i = 0; i < count; ++i)
	{
		fprintf(stderr, " %04X", mem[addr + i + 1]);
	}

	// instruction
    char pad[3 * 5] = "               ";
    pad[(3 - count) * 5] = '\0';
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

void run()
{
	bool active = true;
	while (active)
	{
        if (debug) dump(pc);

        word i = mem[pc++];

		switch (i)
		{
			case 0: // halt: 0
			{
				active = false;

				fprintf(stderr, ">>HALT: Program ended. [pc:%04d = %d]\n", pc, mem[pc - 1]);

				break;
			}

			case 1: // set: 1 a b
			{
				a = mem[pc++];
				b = mem[pc++];

				store(b, a);

				break;
			}

            case 2: // push: 2 a
            {
				a = mem[pc++];

                push(value(a));

                break;
            }

            case 3: // pop: 3 a
            {
				a = mem[pc++];

                store(pop(), a);

                break;
            }

			case 4: // eq: 4 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				store(value(b) == value(c) ? 1 : 0, a);

				break;
			}

            case 5: // gt: 5 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				store(value(b) > value(c) ? 1 : 0, a);

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
					fprintf(stderr, ">>ERR! invalid JMP address! [pc:%04d = %d]\n", pc, a);
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
						fprintf(stderr, ">>ERR! invalid JT address! [pc:%04d = %d]\n", pc, b);
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
						fprintf(stderr, ">>ERR! invalid JT address! [pc:%04d = %d]\n", pc, b);
					}
				}

				break;
			}

			case 9: // add: 9 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				store((unsigned int)value(b) + value(c), a);
			
				break;
			}

            case 10: // mult: 10 a b c
            {
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				store((unsigned int)value(b) * value(c), a);

                break;
            }

            case 11: // mod: 11 a b c
            {
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				store((unsigned int)value(b) % value(c), a);

                break;
            }

			case 12: // and: 12 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				store(value(b) & value(c), a);

				break;
			}

			case 13: // or: 12 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				store(value(b) | value(c), a);

				break;
			}

			case 14: // not: 14 a b
			{
				a = mem[pc++];
				b = mem[pc++];

				store(~value(b), a);

				break;
			}

			case 15: // rmem: 15 a b
            {
				a = mem[pc++];
				b = mem[pc++];

				store(mem[value(b)], a);

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

                push(pc);
                pc = value(a);

                break;
            }

            case 18: // ret: 18
            {
                pc = pop();

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
				fprintf(stderr, ">>ERR! unrecognized instruction! [pc:%04d = %d]\n", pc - 1, mem[pc - 1]);
				return;
			}
		}
	}

	printf("\n");
}

int readFile(const char *filename)
{
	FILE *f = NULL;
    if (!(f = fopen(filename, "r")))
    {
        fprintf(stderr, ">>ERR! failed to open `%s' for reading\n", filename);
        return -1;
    }

	int sizeread = (int)fread(mem, 1, 32768, f);

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

		if (readFile(filename) > 0)
		{
			run();

			return 0;
		}

		return 1;
	}

usage:
    fprintf(stderr, "\n");
    fprintf(stderr, "   Synacor\n");
    fprintf(stderr, "   -------\n\n");

    fprintf(stderr, "   usage:\n");
    fprintf(stderr, "     %s path [-h/--help] | [-d/--debug]\n\n", argv[0]);

    fprintf(stderr, "   parameters:\n");
    fprintf(stderr, "     -h, --help     - display this\n");
    fprintf(stderr, "     -d, --debug    - enable debug\n\n");

    fprintf(stderr, "   example:\n");
    fprintf(stderr, "     %s example.bin\n", argv[0]);

    fprintf(stderr, "\n");

    return 0;
}