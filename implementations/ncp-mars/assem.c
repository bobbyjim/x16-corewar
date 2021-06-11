/*	Copyrighted (C) 1989 by Na Choon Piaw.  All rights reserved       */



/*	This program and documentation is Public Domain, and may be	  */
/*	distributed and copied by everyone provided this header		  */
/*	remains intact							  */

/* assembler portion of the program */
/* 11/14 '88
  Implementing a one-pass "text in memory" assembler that handle tokens
  as well.  Symbol table is a linear table.
   Algorithm:
	1)	Tokenize the input file (i.e. produce a list of symbols in mem)
	2)	Run through once, parsing instructions but not turning
		into machine code yet.  Insert all new symbols into symbol
		table.
	3)	Run through second time,  turning it into machine code
		and looking up symbols.  -- the real error phase.
*/
#include <stdio.h>
#include <malloc.h>
#include "assem.h"

/* symbol table */
static tag1	table[SYMBOLS];

/* instructions list */
static memory	elements[MAXINST];

/* declaring the functions */
tokenlist	*tokenize();

/* top level connections for assembler hooks up tokenize, parse, lookup
   and output so that they can be written independently (no global
   variables in this program                                         */
assemble(infile, outfile)
FILE	*infile,*outfile;
{
	tokenlist	*head;
	int		i;		/* number of instructions */

	head = tokenize(infile);
	parse(head, table);
	i = lookup(head, table, elements);
	output(outfile, table, elements, i);
}

