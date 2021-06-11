/* this module contains the main function, which settles open/close
   file i/o as well as little trivial details like adding an extension
   and stuff like that.  It has been debuged (except for BUG NOTES)
   so it's safe to trust. There is a little bit of inefficiency, but
   that's justified since I want a more easily readable program */

#include "assem.h"
#include <stdio.h>
#include <malloc.h>
#include <strings.h>

char	*outname(str)
char	*str;
/* accepts an input string and outputs a proper output file with ".e"
   extension.  If already has .e, as an extension, produce an error. 
   BUG NOTE: even if it's as innoculous as .eex, etc (as long as the
             extension starts with .e) it will still produce an error
   Otherwise, remove current extension
   and add .e extension.
   returns pointer to new output name                                       */
{
	char	*newstr;
	char	*dot;		/* position of '.' */

	if (!(newstr =(char *) malloc( (unsigned) strlen(str) + 3)))
	{
		printf("not enough memory --- outname\n");
		exit(1);
	}

	strcpy(newstr,str);

	if (!(dot = rindex(newstr,'.')))
		strcat(newstr,".e");	/* no extenstion */
	else if (*(dot + 1) == 'e')	/* same extension as output? */
	{
		printf("wrong input file name: %s\n", newstr);
		printf("try moving to non .e extension --- outname\n");
		exit(1);
	}
	else				/* perform surgery */
	{
		(*(dot + 1)) = 'e';
		(*(dot + 2)) = '\0';
	}

	return newstr;
}


/* main -- Open input and output files, giving default names if
           necessary.  Detects errors like not being able to open files
	   etc.
*/
main(argc, argv)
int	argc;
char	*argv[];
{
	FILE	*f1,*f2;		/* input file, output file */
	char	*outfile = "NONAME",	/* default output file */
		flag = 0;		/* standard input */

	if (argc == 1)		/* no arguments */
	{
		flag = 1;	/* read from standard input */
		argv[1] = outfile;
		argc = 2;	/* one file */
	}

	for (;argc > 1; argc--)
	{
		if (flag)
			f1 = stdin;	/* set file to standard input */
		else
			/* open input file */
			if (!(f1 = fopen(argv[argc - 1],"r")))
			{
				printf("oops cannot open file %s",
					argv[argc - 1]);
				printf("\n-- in main\n");
				exit(1);	/* error status 1 */
			}

			/* open output file */
			if (!(f2 = fopen(outname(argv[argc - 1]), "w")))
			{

				printf("cannot open write file %s",
					outname(argv[argc - 1]));
				printf("\n --- in main\n");
				exit(1);
			}

		printf("%s:\n", argv[argc - 1]);
		assemble(f1,f2);	/* call assembler */

		if (!flag)	/* close file */
			fclose(f1);

		fclose(f2);
	}
}

/* debugging version of assemble */
/* commented out because this module is now fully debugged */
/* --- Na Choon Piaw, 11/14 */
/*
assemble(infile,outfile)
FILE	*infile, *outfile;
{
	int	c;

	while ((c = fgetc(infile)) != EOF)
		fputc(c, outfile);
}
*/
