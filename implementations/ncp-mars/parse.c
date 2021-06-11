/*	Copyrighted (C) 1989 by Na Choon Piaw.  All rights reserved       */



/*	This program and documentation is Public Domain, and may be	  */
/*	distributed and copied by everyone provided this header		  */
/*	remains intact							  */

/* parser portion of the assembler.
   11/15/88         - NCP        */

#include <strings.h>
#include <stdio.h>
#include "assem.h"

/* parser:
  Algorithm:
	1)	initialize symbol table
	2)	Scan first element in token list
	3)	if symbol declaraion (e.g. "start:") then lookup and enter into
		symbol table
	4)	if instruction, skip forward number of parameters
		that instruction accepts
	5)	otherwise error (not instruction or symbol)
	6)	repeat 3-6 until no more tokens.
*/
parse(tokens,table)
tokenlist	*tokens;	/* list of tokens */
tag1		table[];	/* symbol table */
{
	int	i = 0;		/* instructon counter */

	init(table);		/* initialize table */

	while (tokens)
	{
		if (symbol(tokens))
		{
			insert(tokens,table,i);
			tokens = tokens -> next;
		}
		else if (instruction(tokens))
		{
			int	j = 0, k = para(tokens);

			/* move up to next instruction:
			   i.e., move from instruction + number of parameters
			   if instruction is one parameter, move twice, etc */
			while (j <= k)
			{
				tokens = tokens -> next;
				j++;
			}

			i++;		/* next instruction */
		}
		else			/* not instruction or symbol */
		{
			printf("%s not symbol or instruction\n",
				tokens -> token);
			printf(" --- parse\n");
			exit(1);
		}
	}	/* while */

	/* test for too many instructions */
	if (i > MAXINST)
	{
		printf("too many instructions\n");
		printf("---- parse\n");
		exit(1);
	}
} /* parse */

init(table)		/* function to initialize symbol table (set to 0 */
tag1	table[];
{
	int	i;

	for (i = 0; i < SYMBOLS; i++)
	{
		table[i].symbol = NULL;
		table[i].position = 0;
	}
}

int symbol(tok)
/* identifies a symbol:
	A symbol is just a token that ends with a ':'	*/
tokenlist	*tok;
{
	char	*t;		/* token string */

	t = tok -> token;
	if ((t[strlen(t) - 1]) == ':')	/* address last character */
		return 1;		/* is symbol declaration */
	else
		return 0;
}

/* inserts a token into a symbol table (without stripping the ':') -
   Algorithm:
	1)	search until symbol table reads a NULL string or
		the new symbol is equal to and old one.
	2)	if the new symbol is not a NULL, then error
		otherwise, insert
	3)	if out of space, error
*/
int insert(tok, table, no)
tokenlist	*tok;			/* token */
tag1		table[];		/* symbol table */
int		no;			/* instruction number */
{
	char	*t;		/* token string */
	int	i = 0;		/* index on table */

	t = tok -> token;

	/* search for empty place in table */
	for (; i < SYMBOLS && table[i].symbol != NULL &&
	       (strcmp(table[i].symbol,t) != 0); i++)
		;

	if (table[i].symbol != NULL)
	{
		printf("symbol %s already declared\n", t);
		printf("--- insert\n");
		exit(1);
	}

	table[i].symbol = t;
	table[i].position = no;
}

/* tests whether instruction */
int instruction(tok)
tokenlist	*tok;
{
	char	*t;		/* token string */

	t = tok -> token;
	/* note that due to a quirk in strcmp (it returns a zero if the
	   strings are equal) this looks particularly convoluted, but
	   the logic is simple.                                        */
	return(! (strcmp (t,MOV) && strcmp (t,ADD) && strcmp (t,SUB) &&
		  strcmp (t,JMP) && strcmp (t,JMZ) && strcmp (t,JMN) &&
		  strcmp (t,DJN) && strcmp (t,CMP) && strcmp (t,SPL) &&
		  strcmp (t,DAT)));
}

/* return the number of parameters an instruction has */
int para(tok)
tokenlist	*tok;
{
	char	*t;		/* token string */
	int	i = 0;		/* return value */

	t = tok -> token;

	/* use a multiple if-elseif statement */
	if (!strcmp(t, MOV))
		i = 2;
	else if (!strcmp(t, ADD))
		i = 2;
	else if (!strcmp(t, SUB))
		i = 2;
	else if (!strcmp(t, JMP))
		i = 1;
	else if (!strcmp(t, JMZ))
		i = 2;
	else if (!strcmp(t, JMN))
		i = 2;
	else if (!strcmp(t, DJN))
		i = 2;
	else if (!strcmp(t, CMP))
		i = 2;
	else if (!strcmp(t, SPL))
		i = 1;
	else if (!strcmp(t, DAT))
		i = 1;
	else	/* unrecognized instruction */
	{
		printf("%s is not an instruction\n", t);
		printf("--- para\n");
		exit(1);
	}

	return(i);
}	/* para */




/* debugging section of parse.c */
/* code commented out --- given a clean bill of health by Dr. Na Choon Piaw
   --- 11/15/88                                                           */
/* declare tokenize */
/* extern tokenlist *tokenize(); */
/* declare symbol table */
/* tag1	table[SYMBOLS]; */

/* main(argc,argv)
int	argc;
char	*argv[];
{
	tokenlist	*head;
	FILE		*f;
	int		i;

	f = fopen(argv[1], "r");
	head = tokenize(f);
	parse(head, table);

	printf("%s\t%s\n", "SYMBOL", "POSITION");
	for (i = 0; table[i].symbol != NULL; i++)
		printf("%s\t%d\n", table[i].symbol, table[i].position);
}  */
