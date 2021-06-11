/*	Copyrighted (C) 1989 by Na Choon Piaw.  All rights reserved       */



/*	This program and documentation is Public Domain, and may be	  */
/*	distributed and copied by everyone provided this header		  */
/*	remains intact							  */

/* output.c --- output routine for the asssembler.
   11/25/88  --- NCP                             */

/* algorithm:
   1.	write number of instructions into file
   2.	write code number of start instruction
   3.	while there are still instructions do
   4.		write instruction n
   5.   end
*/
#include "assem.h"
#include <stdio.h>

#define IO(i,j) if ((i) < (j)) { printf("error in writing file  --- output\n" \
				 ); }

output(f, table, code, no)
FILE	*f;		/* file to output to */
tag1	table[];	/* table of symbols */
memory	code[];		/* code itself */
int	no;		/* number of instructions */
{
	int	check,	/* check on how many bytes have been written */
		start;	/* starting instruction */

	check = fwrite(&no, sizeof(int), 1, f);
	IO(check,1)

	start = getsym("START",table);
	check = fwrite(&start, sizeof(int), 1, f);
	IO(check,1);

	check = fwrite(code, sizeof(memory), no, f);
	IO(check,no);
}
