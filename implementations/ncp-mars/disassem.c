/*	Copyrighted (C) 1989 by Na Choon Piaw.  All rights reserved       */



/*	This program and documentation is Public Domain, and may be	  */
/*	distributed and copied by everyone provided this header		  */
/*	remains intact							  */

#include <stdio.h>
#include "assem.h"

/* dissassembler for red code programs.
   NOTE:  I'm not bothering with writing code to REALLY disassemble
          it, just output starting position, number of instructions,
	  opcode, and operands.
   11/25 ---- NCP                           */
#define IO(i,j) if ((i) < (j)) {printf("error reading file\n");exit(1);}


main(argc, argv)
int	argc;
char	*argv[];
{
	FILE	*f;
	int	i,j,k;
	memory	elements[MAXINST];

	if (argc < 1)
	{
		printf("usage: disassem <file>\n");
		exit(1);
	}

	if (!(f = fopen(argv[1],"r")))
	{
		printf("cannot open file %s\n", argv[1]);
		printf("------ main\n");
		exit(1);
	}

	i = fread(&j, sizeof(int), 1, f);
	IO(i,1)
	printf("Number of instructions: %d\n", j);

	i = fread(&k, sizeof(int), 1, f);
	IO(i,1)
	printf("Starting instruction: %d\n", k);

	i = fread(elements, sizeof(memory), j, f);
	IO(i,j)

	printf("NO.\tOPCODE\tFIRSTPARA\tSECONDPARA\n");
	for (i = 0; i < j;i++)
	{
		printf("%d\t", i);
		printf("%d\t", (int) elements[i].inst);
		if (elements[i].m1 == immed)
			putchar('#');
		else if (elements[i].m1 == indirect)
			putchar('@');
		printf("%d\t\t", (int) elements[i].para1);
                if (elements[i].m2 == immed)
                        putchar('#');
                else if (elements[i].m2 == indirect)
                        putchar('@');
                printf("%d\n", (int) elements[i].para2);
	}

	fclose(f);
}
