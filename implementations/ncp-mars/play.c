/* play.c ---- main program for the interpreter.
   1.	play player 1
   2.	play player 2
   3.   output
   4.   check deaths
   5.   repeat

   --- 12/17/88   NCP                             */
#include <stdio.h>
#include "interp.h"

extern	stream *exe[];
extern  cell	a[];

int	dead(i)
int	i;
{
	if (exe[i])
		return 0;
	else
		return 1;
}


int	alive()
{
	int	i, j = 1;

	for (i = 1; i < MAXPLAY; i ++)
	{
		if (!exe[i])
		{
			j = 0;
			break;
		}
	}

	return(j);
}

int play(cycles)
long cycles;
{
	int	i;

	while (alive() && cycles)
	{
		for ( i = 1; i < MAXPLAY; i++)
		{
			execute(i);	/* play player n */
#ifdef DEBUG
			output();
#endif
#ifdef BIG
			output(cycles);
#endif
		}
#ifdef SUN
		if (!(cycles % 50))
			xcore_display(a);
#endif
#ifdef SMALL
		if (!(cycles % 100))
			output(cycles);
#endif
		cycles --;
	}

	if (dead(1) && !dead(2))
		return(2);		/* process 2 won */
	else if (dead(2) && !dead(1))
		return(1);		/* process 1 won */
	else
		return(0);		/* nobody won */
}

execute(i)
int	(i);
{
	instr	temp;			/* instruction */
	int	x;			/* cell to execute */
	int	pc;

	if (exe[i] == NULL)
		return;

	correct(&(exe[i] -> pc));
	x = pc = exe[i] -> pc;
	temp = a[x].inst;

	/* this really should have been a switch-case, but I'm using an
	   elongated if-else because the compiler doesn't accept it.   */

	if (temp == dat)
		Dat(i);
	else if (temp == mov)
		Mov(x, &pc, i);
	else if (temp == add)
		Add(x, &pc, i);
	else if (temp == sub)
		Sub(x, &pc, i);
	else if (temp == jmp)
		Jmp(x, &pc);
	else if (temp == jmz)
		Jmz(x, &pc);
	else if (temp == jmn)
		Jmn(x, &pc);
	else if (temp == djn)
		Djn(x, &pc, i);
	else if (temp == cmp)
		Cmp(x, &pc);
	else if (temp == spl)
		Spl(x, &pc, exe[i]);
	else
	{
		printf("Instruction not recognized\n");
		printf("--- execute\n");
		printf("opcode: %d", temp);
		printf(" executing no: %d", pc);
		printf(" process: %d\n", i);
		exit(1);
	}

	if (temp != dat)
		exe[i] -> pc = pc;

	if (exe[i] && (temp != dat))
		exe[i] = exe[i] -> next;
}
