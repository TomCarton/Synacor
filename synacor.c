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

	printf("ERR! unallowed value! [pc:%04d = %d]\n", pc, v);

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
		printf("ERR! unallowed destination! [pc:%04d = %d]\n", pc, v);
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
			case 0: // halt 0
			{
				active = 0;

				break;
			}

			case 9: // add 9 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				store((value(b) + value(c)) % 32767, a);
			
				break;
			}

			case 19: // out 19 a
			{
				a = mem[pc++];

				printf("%d", value(a));

				break;
			}

			case 21: // noop 21
				break;

			default:
			{
				printf("ERR! unrecognized instruction! [pc:%04d = %d]\n", pc, i);

				break;
			}
		}
	}

	printf("\n");
}


int main(int argc, const char *argv[])
{
	word prg[] = { 9, 32768, 32769, 4, 19, 32768 };

	memcpy(mem, prg, sizeof(prg));
	start();

	return 0;
}