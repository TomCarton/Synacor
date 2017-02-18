// synacor.c
//
// written by Thomas CARTON
//

#include <stdio.h>
#include <memory.h>


unsigned int reg[8];
unsigned int mem[32768];
unsigned int pc;

void start()
{
	pc = 0;

	unsigned int a, b, c, i;
	
	unsigned int active = 1;
	while (active)
	{
		i = mem[pc]; pc += 2;

		switch (i)
		{
			case 0: // halt 0
				active = 0;
				break;

			case 19: // out 19 a
				a = mem[pc]; pc += 2;
				printf("%c", a);
				break;

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
	unsigned int prg[] = { 9, 32768, 32769, 4, 19, 32768 };

	memcpy(mem, prg, sizeof(prg));
	start();

	return 0;
}