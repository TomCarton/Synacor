// synacor.c
//
// written by Thomas CARTON
//

#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>


typedef unsigned short word;

word reg[8];
word mem[32768];

word pc;
word a, b, c;

bool debug = false;


word value(word v)
{
	if (v < 32768)
	{
		return v;
	}
	else if (v < 32776)
	{
		return reg[v - 32768];
	}

	fprintf(stderr, ">>ERR! unallowed value! [pc:%04d = %d]\n", pc, v);

	return 0;
}

void store(word v, word dest)
{
	if (dest < 32768)
	{
		mem[dest] = v & 32767;
	}
	else if (dest < 32776)
	{
		reg[dest - 32768] = v & 32767;
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
	fprintf(stderr, "\n%08X: %04X", p * (unsigned int)sizeof(word), mem[p]);
	for (unsigned int i = 0; i < count; ++i)
	{
		fprintf(stderr, " %04X", mem[p + i]);
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

		if (o < 32768)
		{
			fprintf(stderr, " %d", o);
		}
		else if (o < 32776)
		{
			fprintf(stderr, " r%d", o & 7);
		}
	}
	va_end(ap);
}

void run()
{
	pc = 0;
	
	bool active = true;
	while (active)
	{
		switch (mem[pc++])
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

			case 6: // jump: 6 a
			{
				a = mem[pc++];

				if (debug) dumpInstruction(pc, "JUMP", 1, a);

				if (a < 32768)
				{
					if (a & 1)
					{
						fprintf(stderr, ">>WARN! invalid JUMP address! [pc:%04d = %d]\n", pc, a);
						a &= ~1;
						break;
					}

					pc = a >> 1;
				}
				else
				{
					fprintf(stderr, ">>ERR! invalid JUMP address! [pc:%04d = %d]\n", pc, a);
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
					if (b < 32768)
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
					if (b < 32768)
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

	int sizeread = (int)fread(mem, 2, 32768, f);

    fclose(f);

    return sizeread;
}

int main(int argc, const char *argv[])
{
	if (argc < 2 || argc > 3)
	   	goto usage;

    // read parameters
    for (unsigned int i = 0; i < argc; ++i)
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
    }

	if (readFile(argv[1]) > 0)
	{
		run();

		return 0;
	}

	return 1;

usage:
    fprintf(stderr, "\n");
    fprintf(stderr, "   Synacor\n");
    fprintf(stderr, "   -------\n\n");

    fprintf(stderr, "   usage:\n");
    fprintf(stderr, "     %s path [-h/--help] | [-d/--debug]\n\n", argv[0]);

    fprintf(stderr, "   parameters:\n");
    fprintf(stderr, "     -h, --help     - display this\n");
    fprintf(stderr, "     -d, --debug     - enable debug\n\n");

    fprintf(stderr, "   example:\n");
    fprintf(stderr, "     %s example.bin\n", argv[0]);

    fprintf(stderr, "\n");

    return 0;
}