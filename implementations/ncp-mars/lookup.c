/*	Copyrighted (C) 1989 by Na Choon Piaw.  All rights reserved       */



/*	This program and documentation is Public Domain, and may be	  */
/*	distributed and copied by everyone provided this header		  */
/*	remains intact							  */

/* lookup   ---- this is the real core of the assembler.  It will
		 perform functions such as matching symbols to instructions,
		 matching symbolic references to absolute code (and performing
		 whatever calculations necessary).                         */

#include "assem.h"
#include <malloc.h>
#include <stdio.h>
#include <strings.h>
#include <ctype.h>

/* declare external functions */
int		symbol();
int		instruction();
instr		getin();
int		para();
int		getpara();
mode		getmode();

/* algorithm :-
	1)	start from beginning of the list.
	2)	process element.
	3)	go on to next element.
	4)	return number of instructions
*/

int lookup(head, table, elements)
tokenlist	*head;
tag1		table[];
memory		elements[];
{
	int	i = 0,			/* instruction pointer */
		j = 0,			/* number of parameters */
		k = 0;			/* current parameters */

	initialize(elements);		/* initialize table */
	while (head && symbol(head))	/* look for first instruction */
		head = head -> next;
	while (head)
	{
		/* expect instruction */
		if (!instruction(head))
		{
			printf("%s is not instruction\n", head -> token);
			printf(" --- lookup\n");
			exit(1);
		}

		elements[i].inst = getin(head);

		/* expect number of parameters */
		j = para(head);
		k = 0;
		while (k < j)
		{
			head = head -> next;
			if (k == 0 && j == 2)	/* if single para, put in B */
			{
				elements[i].para1 = getpara(head,table,i);
				elements[i].m1 = getmode(head);
			}
			else
			{
				elements[i].para2 = getpara(head,table,i);
				elements[i].m2 = getmode(head);
			}
			k++;
		}

		if (head)
			head = head -> next;	/* next instruction, if any */

		while (head && symbol(head))	/* skim symbols */
			head = head -> next;

		i++;
	}	/* while */
	return i;
}

/* set all instructions to init */
initialize(elements)
memory	elements[];
{
	memory	ele;		/* set all to this initialized variable */
	int	i;

	ele.inst = dat;
	ele.para1 = ele.para2 = 0;
	ele.m1 = ele.m2 = immed;

	for (i = 0; i < MAXINST; i++)
		elements[i] = ele;
}

/* get the instr part of an instruction */
instr	getin(ptr)
tokenlist	*ptr;
{
	char	*t;		/* string */
	instr	x;		/* return value */

	t = ptr -> token;

	if (!strcmp(t, DAT))
		x = dat;
	else if (!strcmp(t, MOV))
		x = mov;
	else if (!strcmp(t, ADD))
		x = add;
	else if (!strcmp(t, SUB))
		x = sub;
	else if (!strcmp(t, JMP))
		x = jmp;
	else if (!strcmp(t, JMZ))
		x = jmz;
	else if (!strcmp(t, JMN))
		x = jmn;
	else if (!strcmp(t, DJN))
		x = djn;
	else if (!strcmp(t, CMP))
		x = cmp;
	else if (!strcmp(t, SPL))
		x = spl;
	else
	{
		printf("%s is not an instruction\n", t);
		printf("--- getin\n");	
		exit(1);
	}

	return x;
}	/* getin */

/* get the actual parameter (not the symbolic one)
Algorithm:
	1)	check if symbol
	2)	if symbol then look up, compare with current instruction,
		and calculate what the actual parameter should be.
	3)	if not symbol, then check that it is a numeric. if not, error.
	4)	otherwise, return atoi
*/
int	getpara(ptr, table, curr)
tokenlist	*ptr;		/* parameter instruction */
tag1		table[];	/* symbol table */
int		curr;		/* current instruction */
{
	char	*t;		/* token string */

	t = ptr -> token;

	if (*t == '@' || *t == '#')	/* ignore these */
		t++;

	if (number(t))
		return(atoi(t));
	else				/* must be symbol */
		return(getsym(t, table) - curr);
}

/* return the absolute location (from the beginning of the program) of
   a symbol                                                         */
int	getsym(str, table)
char	*str;
tag1	table[];
{
	int	i = 0;
	char	s[MAXBUFFER];

	/* add colon for strcmp */
	strcpy(s,str);
	strcat(s,":");

	for (; (i < SYMBOLS) && (table[i].symbol != NULL); i++)
		if (!strcmp(table[i].symbol, s))
			return(table[i].position);

	/* out here, not symbol */
	printf("symbol %s undefined\n", str);
	printf("--- getsym\n");
	exit(1);
}

/* checks that every element in string is a digit */
int	number(str)
char	*str;
{
	int	i = 1;

	if (*str == '+' || *str == '-')		/* positive or negative */
		str++;

	while (*str && i)
	{
		if (!isdigit(*str))
			i = 0;
		str++;
	}

	return i;
}

mode	getmode(ptr)
tokenlist	*ptr;
{
	char	*t;		/* token string */

	t = ptr -> token;

	if (*t == '@')
		return(indirect);
	else if (*t == '#')
		return(immed);
	else
		return(direct);
}

/* special debugging portion */

/* declared debugged (phew! this one was tough) on 11/23/88 by CP */
/*
tokenlist	*tokenize();
tag1 		table[SYMBOLS];
memory		elements[MAXINST];
extern tokenlist *tokenize();

main(argc, argv)
int	argc;
char	*argv[];
{
	tokenlist 	*head;
	FILE		*f;
	int		i;

	printf("loaded\n");
	f = fopen(argv[1],"r");
	printf("%s file opened\n", argv[1]);
	head = tokenize(f);
	printf("tokenized\n");
	parse(head, table);
	printf("parsed\n");
	i = lookup(head, table,elements);
	printf("%d instructions\n", i);

	printf("OPCODE\tFIRSTPARA\tSECONDPARA\n");
	for (i = 0; i < 20; i++)
	{
		printf("%d\t",(int) elements[i].inst);
		if (elements[i].m1 == indirect)
			printf("@");
		else if (elements[i].m1 == immed)
			printf("#");
		printf("%d\t\t", elements[i].para1);
                if (elements[i].m2 == indirect)
                        printf("@");
                else if (elements[i].m2 == immed)
                        printf("#");
		printf("%d\n", elements[i].para2);
	}
	fclose(f);
}

printsymbols(head)
tokenlist	*head;
{
	while (head != NULL)
	{
		printf("%s\n", head -> token);
		head = head -> next;
	}
}

printable(t)
tag1	t[];
{
	int	i;
	printf("%s\t%s\n","SYMBOL","POSITION");
	for (i = 0; table[i].symbol != NULL; i++)
		printf("%s\t%d\n", table[i].symbol,table[i].position);
}

*/
