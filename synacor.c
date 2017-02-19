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
word sp;

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

void dumpInstruction(const unsigned int addr, const char *inst, const unsigned int count, ...)
{
	va_list ap;

	// memory
	unsigned int p = addr - count - 1;
	fprintf(stderr, "  %08d: %04X", p, mem[p]);
	for (unsigned int i = 0; i < count; ++i)
	{
		fprintf(stderr, " %04X", mem[p + i + 1]);
	}

	// instruction
    char pad[3 * 5] = "               ";
    pad[(3 - count) * 5] = '\0';
	fprintf(stderr, "%s\t%s", pad, inst);

	// operands
	va_start(ap, count);
	for (unsigned int i = 0; i < count; ++i)
	{
		word o = va_arg(ap, int);

		if (o < memsize)
		{
            if (mem[p] == 19 && o > 31 && o <= 255)
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
	va_end(ap);

    fprintf(stderr, "\n");
}

void run()
{
	bool active = true;
	while (active)
	{
        word i = mem[pc++];

		switch (i)
		{
			case 0: // halt: 0
			{
				if (debug) dumpInstruction(pc, "HALT", 0);

				active = false;

				fprintf(stderr, ">>HALT: Program ended. [pc:%04d = %d]\n", pc, mem[pc - 1]);

				break;
			}

			case 1: // set: 1 a b
			{
				a = mem[pc++];
				b = mem[pc++];

				if (debug) dumpInstruction(pc, "SET", 2, a, b);

				store(b, a);

				break;
			}

			case 4: // eq: 4 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				if (debug) dumpInstruction(pc, "EQ", 3, a, b, c);

				store(a, value(b) == value(c) ? 1 : 0);

				break;
			}

			case 6: // jmp: 6 a
			{
				a = mem[pc++];

				if (debug) dumpInstruction(pc, "JMP", 1, a);

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

                if (debug) dumpInstruction(pc, "JT", 2, a, b);

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

                if (debug) dumpInstruction(pc, "JF", 2, a, b);

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

                if (debug) dumpInstruction(pc, "ADD", 3, a, b, c);

				store((value(b) + value(c)) % 32767, a);
			
				break;
			}

			case 12: // and: 12 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

                if (debug) dumpInstruction(pc, "AND", 3, a, b, c);

				store(value(b) & value(c), a);

				break;
			}

			case 13: // or: 12 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

                if (debug) dumpInstruction(pc, "OR", 3, a, b, c);

				store(value(b) | value(c), a);

				break;
			}

			case 14: // not: 14 a b
			{
				a = mem[pc++];
				b = mem[pc++];

                if (debug) dumpInstruction(pc, "NOT", 2, a, b);


				store(~value(b), a);

				break;
			}

			case 19: // out: 19 a
			{
				a = mem[pc++];

				if (debug) dumpInstruction(pc, "OUT", 1, a);

				fprintf(stdout, "%c", value(a));

				break;
			}

			case 21: // noop: 21
			{
				if (debug) dumpInstruction(pc, "NOOP", 0);

				break;
			}

			default:
			{
				fprintf(stderr, ">>ERR! unrecognized instruction! [pc:%04d = %d]\n", pc, mem[pc - 1]);
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