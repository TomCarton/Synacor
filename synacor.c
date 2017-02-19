// synacor.c
//
// written by Thomas CARTON
//

#include <stdio.h>
#include <memory.h>


typedef unsigned short word;

word reg[8];
word mem[32768];

word pc;
word a, b, c;


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

void run()
{
	pc = 0;

	
	unsigned int active = 1;
	while (active)
	{
		switch (mem[pc++])
		{
			case 0: // halt: 0
			{
				active = 0;

				fprintf(stderr, ">>HALT: Program ended. [pc:%04d = %d]\n", pc, mem[pc - 1]);

				break;
			}

			case 6: // jump: 6 a
			{
				a = mem[pc++];

				if (a < 32768)
				{
					pc = a;
				}
				else
				{
					fprintf(stderr, "\n\n>>ERR! invalid JUMP address! [pc:%04d = %d]\n", pc, a);
				}

				break;
			}

			case 7: // jt: 7 a b
			{
				a = mem[pc++];
				b = mem[pc++];

				if (a)
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

				if (a == 0)
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

				store((value(b) + value(c)) % 32767, a);
			
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

			case 19: // out: 19 a
			{
				a = mem[pc++];

				fprintf(stdout, "%d", value(a));

				break;
			}

			case 21: // noop: 21
			{
				break;
			}

			default:
			{
				fprintf(stderr, ">>ERR! unrecognized instruction! [pc:%04d = %d]\n", pc, mem[pc - 1]);

				break;
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

	int sizeread = fread(mem, 2, 32768, f);

    fclose(f);

    return sizeread;
}

int main(int argc, const char *argv[])
{
	if (argc != 2)
    	goto usage;

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
    fprintf(stderr, "     %s path\n\n", argv[0]);

    fprintf(stderr, "   example:\n");
    fprintf(stderr, "     %s example.bin\n", argv[0]);

    fprintf(stderr, "\n");

    return 0;
}