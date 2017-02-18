// synacor.c
//
// written by Thomas CARTON
//

#include <stdio.h>
#include <memory.h>


unsigned short reg[8];
unsigned short mem[32768];
unsigned short pc;


unsigned short value(unsigned short v)
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

void start()
{
	pc = 0;

	unsigned short a, b, c, i;
	
	unsigned int active = 1;
	while (active)
	{
		i = mem[pc++];

		switch (i)
		{
			case 0: // halt 0
				active = 0;
				break;

			case 9: // add 9 a b c
			{
				a = mem[pc++];
				b = mem[pc++];
				c = mem[pc++];

				unsigned short tmp = value(b) + value(c);

				if (a < 32768)
				{
					mem[a] = tmp & 32767;
				}
				else if (a < 32776)
				{
					reg[a - 32768] = tmp & 32767;
				}
			
				break;
			}

			case 19: // out 19 a
			{
				a = mem[pc++];
				printf("%d\n", value(a));

				break;
			}

			case 21: // noop 21
				break;

			default:
				printf("ERR! unrecognized instruction! [pc:%04d = %d]\n", pc, i);
				break;
		}
	}
}


int main(int argc, const char *argv[])
{
	unsigned short prg[] = { 9, 32768, 32769, 4, 19, 32768 };

	memcpy(mem, prg, sizeof(prg));
	start();

	return 0;
}