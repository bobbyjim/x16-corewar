/* execution code for the machine */

/*	This is a replacement for the screwed up code in "execute.c"
	Each instruction is a function, and accepts just enough information
	to execute that instruction.
*/

#include "interp.h"
#include <malloc.h>
#include <stdio.h>

extern	cell	a[];		/* core */

correct(thing)
int	*thing;
{
	if (*thing >= 0)
		*thing %= SIZE;
	else if (*thing < 0)
	{
		*thing %= SIZE;
		*thing = SIZE + *thing;
	}
}

int	getdirect(ins, off)
/* get position of direct parameter */
int	ins;	/* current position */
int	off;	/* offset of instruction */
{
	int	temp;

	temp = ins + off;
	correct(&temp);
	return (temp);
}

int	getindirect(ins, off)
/* get position of indirect parameter */
/* Note :-
A.K. Dewdney's specifications say that the pointer points relative to its
current position, NOT the instruction's position. */

int	ins;
int	off;
{
	int	temp;	/* temporary variable */

	correct(&off);
	temp = getdirect(ins, off);		/* get direct variable */
	correct(&temp);				/* correct it */
	temp = getdirect(temp, a[temp].para2);	/* This time it's for real */
	correct(&temp);
	return (temp);
}

int	getpara(ins, m, p)
/* Note:- This routine is designed only to work with indirect and direct
   operations.  NOT with immediate mode operations. Returns the address
   of the parameter by calling getdirect or getindirect			*/
int	ins;	/* location of current instruction */
mode	m;	/* mode */
int	p;	/* parameter */
{
	if (m == direct)
		return(getdirect(ins, p));
	else if (m == indirect)
		return(getindirect(ins, p));

	/* else */
	printf("getpara passed wrong parameter --- getpara\n");
	exit(1);
}

Mov	(ins, pc, pid)
/* MOV instruction :
sets the lastmod flag to the process id					*/
int	ins;	/* position of instruction to execute */
int	*pc;	/* program counter */
int	pid;	/* process ID */
{
	int	realpos;
	cell	temp;		/* store item to be moved */

	if (a[ins].m1 == immed)
	{
		temp.inst = dat;
		temp.para1 = 0;
		temp.para2 = a[ins].para1;
		temp.m1 = temp.m2 = immed;
	}
	else
	{
		realpos = getpara(ins, a[ins].m1, a[ins].para1);
		temp = a[realpos];
	}

	if (a[ins].m2 == immed)
	{
		printf("Tried to mov to immediate parameter\n");
		printf("--- mov\n");
		exit(1);
	}
	else
		realpos = getpara(ins, a[ins].m2, a[ins].para2);

	a[realpos] = temp;
	a[realpos].lastmod = pid;
	(*pc)++;
	correct(pc);
}

Add(ins, pc, pid)
/* ADD instruction */
int	ins;
int	*pc;
int	pid;
{
	int	x;		/* parameter A */
	int	realpos;	/* real position of B */

	if (a[ins].m1 == immed)
		x = a[ins].para1;
	else
		x = a[getpara(ins, a[ins].m1, a[ins].para1)].para2;

	if (a[ins].m2 == immed)
	{
		printf("Trying to add to immediate address\n");
		printf("--- ADD\n");
		exit(1);
	}
	else
		realpos = getpara(ins, a[ins].m2, a[ins].para2);

	a[realpos].para2 += x;
	a[realpos].lastmod = pid;
	correct(&(a[realpos].para2));
	(*pc)++;
	correct(pc);
}


Sub(ins, pc, pid)
/* SUB instruction */
int	ins;
int	*pc;
{
	int	x;		/* parameter A */
	int	realpos;	/* real position of B */

	if (a[ins].m1 == immed)
		x = a[ins].para1;
	else
		x = a[getpara(ins, a[ins].m1, a[ins].para1)].para2;

	if (a[ins].m2 == immed)
	{
		printf("Trying to subtract from immediate address\n");
		printf("--- SUB\n");
		exit(1);
	}
	else
		realpos = getpara(ins, a[ins].m2, a[ins].para2);

	a[realpos].para2 -= x;
	a[realpos].lastmod = pid;
	correct(&(a[realpos].para2));
	(*pc)++;
	correct(pc);
}

Jmp(ins,pc)
int	ins;
int	*pc;
{
	if (a[ins].m2 == immed)
	{
		printf("attempt to jump to immediate address\n");
		printf("-- JMP\n");
		exit(1);
	}
	else
		*pc = getpara(ins, a[ins].m2, a[ins].para2);

	correct(pc);
}

Jmz(ins, pc)
int	ins;
int	*pc;
{
	int	value;	/* value of first parameter */

	if (a[ins].m2 == immed)
		value = a[ins].para2;
	else
	{
		value = getpara(ins, a[ins].m2, a[ins].para2);
		correct(&value);
		value = a[value].para2;
	}

	correct(&value);

	if (value)
		(*pc)++;
	else
		*pc = getpara(ins, a[ins].m1, a[ins].para1);

	correct(pc);
}

Jmn(ins,pc)
int	ins;
int	*pc;
{
	int	value;

	if (a[ins].m2 == immed)
		value = a[ins].para2;
	else
	{
		value = getpara(ins, a[ins].m2, a[ins].para2);
		correct(&value);
		value = a[value].para2;
	}

	if (!value)
		*pc = getpara(ins, a[ins].m1, a[ins].para1);
	else
		(*pc)++;
	correct(pc);
}

Djn(ins, pc, pid)
int	ins;
int	*pc;
{
	int	temp;	/* position to decrement */

	if (a[ins].m2 == immed)
	{
		printf("tried to decrement immediate address\n");
		printf("--- DJZ\n");
		exit(1);
	}
	else
		temp = getpara(ins, a[ins].m2, a[ins].para2);

	(a[temp].para2)--;
	correct(&(a[temp].para2));
	a[temp].lastmod = pid;

	if (!a[temp].para2)
		(*pc)++;
	else
		*pc = getpara(ins, a[ins].m1, a[ins].para1);

	correct(pc);
}

Cmp(ins, pc)
int	ins;
int	*pc;
{
	int	value1, value2;

	if (a[ins].m1 == immed)
		value1 = a[ins].para1;
	else
		value1 = a[getpara(ins, a[ins].m1, a[ins].para1)].para2;

	if (a[ins].m2 == immed)
		value2 = a[ins].para2;
	else
		value2 = a[getpara(ins, a[ins].m2, a[ins].para2)].para2;

	correct(&value1);
	correct(&value2);
	if (value1 == value2)
		(*pc) += 2;	
	else
		(*pc)++;

	correct(pc);
}

Spl(ins, pc, mem)
int	ins;
int	*pc;
stream	*mem;		/* pointer to structure of current pc */
{
	int	newpc;
	stream	*newmem;

	if (a[ins].m2 == immed)
	{
		printf("Tried to split into immediate address\n");
		printf("--- SPL\n");
		exit(1);
	}
	else
		newpc = getpara(ins, a[ins].m2, a[ins].para2);

	if (!(newmem = (stream *) malloc(sizeof(stream))))
	{
		printf("no more memory!!\n");
		printf("--- SPL\n");
		exit(1);
	}

	correct(&newpc);
	newmem -> pc = newpc;
	newmem -> next = mem -> next;
	mem -> next = newmem;
	newmem -> prev = mem;
	newmem -> next -> prev = newmem;

	(*pc)++;
	correct(pc);
}

extern	stream	*exe[];

Dat(i)
/* kill stream */
int	i;
{
	stream	*curr = exe[i];		/* current */

	if (curr -> next == curr)
	{
		exe[i] = NULL;
		free(curr);
		return;
	}

	exe[i] = curr -> next;
	curr -> next -> prev = curr -> prev;
	curr -> prev -> next = curr -> next;

	free(curr);
}
