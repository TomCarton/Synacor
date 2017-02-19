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

	printf("\n\n>>ERR! unallowed value! [pc:%04d = %d]\n\n", pc, v);

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
		printf("\n\n>>ERR! unallowed destination! [pc:%04d = %d]\n\n", pc, v);
	}

}

void start()
{
	pc = 0;

	word a, b, c, i;
	
	unsigned int active = 1;
	while (active)
	{
		i = mem[pc++];

		switch (i)
		{
			case 0: // halt: 0
			{
				active = 0;

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

			case 6: // jump: 6 a
			{
				a = mem[pc++];

				if (a < 32768)
				{
					pc = a;
				}
				else
				{
					printf("\n\n>>ERR! invalid JUMP address! [pc:%04d = %d]\n\n", pc, a);
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
						printf("\n\n>>ERR! invalid JT address! [pc:%04d = %d]\n\n", pc, b);
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
						printf("\n\n>>ERR! invalid JT address! [pc:%04d = %d]\n\n", pc, b);
					}
				}

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

				printf("%d", value(a));

				break;
			}

			case 21: // noop: 21
				break;

			default:
			{
				printf("\n\n>>ERR! unrecognized instruction! [pc:%04d = %d]\n\n", pc, i);

				break;
			}
		}
	}

	printf("\n");
}


int main(int argc, const char *argv[])
{
	if (argc != 2)
    	goto usage;

    FILE *f = NULL;
    if (!(f = fopen(argv[1], "r")))
    {
        fprintf(stderr, "\n\n>>ERR! failed to open `%s' for reading\n\n", argv[1]);
        return 1;
    }

	int clen = fread(mem, 2, 16384, f);

    fclose(f);

	start();

	return 0;

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