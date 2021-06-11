/*	Copyrighted (C) 1989 by Na Choon Piaw.  All rights reserved       */



/*	This program and documentation is Public Domain, and may be	  */
/*	distributed and copied by everyone provided this header		  */
/*	remains intact							  */

/* tokenize ---- tokenizer for the assembler.  It splits up all of the
   tokens in the given input file, generate a linked list of such tokens,
   and outputs the pointer to that linked list.
  11/14                 ----- NCP                                    */
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <malloc.h>
#include "assem.h"

/* tokenize function:
   separate input stream (infile) into tokens.
   algorithm:
	1)	call nextoken to get next token.
	2)	if "null" then return the first pointer
	3)	otherwise, add the newtoken to the token list
	4)	go back to 1.
*/
tokenlist *tokenize(infile)
FILE	*infile;
{
	tokenlist	*head, *tail, *newtail;
	char		*nextoken(), *newtoken;

	head = tail = NULL;
	while ((newtoken = nextoken(infile)) != NULL)
	{
		if (!(newtail = (tokenlist *) 
		                 malloc((unsigned) sizeof(tokenlist))))
		{
			printf("ran out of space for tokens: %s\n", newtoken);
			printf("------  tokenize\n");
			exit(1);
		}

		/* otherwise , set old stuff to this and move tail one up*/
		newtail -> token = newtoken;
		newtail -> next = NULL;
		if (tail)			/* tail already defined */
		{
			tail -> next = newtail;		/* set previous ptr */
			tail = tail -> next;		/* move up list */
		}
		else
			head = tail = newtail;
	}	/* end while */

	return (head);	/* return function value */
}

/* function next token:
   return next token in the file.
   Algorithm:
	1)	read until start of next token or EOF
	2)	if EOF then return NULL pointer
	3)	read new token into buffer
	4)	malloc new string
	5)	put next token into new string
	6)	change all characters into upper case
	7)	return pointer to string created in (4)      */
/* BUG NOTE:
   Due to the way it is written, the assembler will not process things like
   "mov0 1" correctly, and neither will "mov 0 1;imp" work.
   this is because the tokenize breaks everything down that isn't a
   delimiter, and a ";" is not a delimiter, even though it's not in
   the instruction set.                                   NCP - 11/15/88 */
char	*nextoken(infile)
FILE	*infile;
{
	char	buffer[MAXBUFFER],	/* string buffer */
		*newtoken;		/* new token pointer */

	int	c,			/* character we read in one by one */
		i = 0;			/* integer counter */

	while ((c = fgetc(infile)) != EOF)
	{
		if (!isspace(c))
			break;		/* not space, so process */
	}

	if (c == EOF)
		return(NULL);		/* no more!! */

	if (c == COMMENT)		/* handle comments */
	{
		while (((c = fgetc(infile)) != '\n') && c != EOF)
			;		/* read until end of the line */

		if (c == EOF)
			return(NULL);

		/* process the rest of the file:
		   actually, we could have done this by using a goto
		   the beginning of this function, but I think this
		   is a lot more elegant                     --- CP */
		return(nextoken(infile));
	}

	while ((!isspace(c)) && (c != EOF))	/* read until next space */
	{
		if (i >= MAXBUFFER)	/* buffer out */
		{
			printf("buffer over extended\n");	
			printf("--- nextoken\n");
			exit(1);
		}

		buffer[i++] = c;

		c = fgetc(infile);
	}	/* end while */
	buffer[i] = '\0';		/* terminate with a null */

	if (!(newtoken = (char *) malloc( (unsigned) strlen(buffer) + 1)))
	{
		printf("not enough memory for token %s\n", buffer);
		printf(" ------- nextoken\n");
		exit(1);
	}

	strcpy(newtoken, buffer);

	upcase(newtoken);

	return(newtoken);
}	/* end of nextoken */

/* upcase function -- translate string to upper case (don't trust toupper) */
upcase(str)
char	*str;
{
	while (*str)
	{
		if ((*str <= 'z') && (*str >= 'a'))
			*str -= 'a' - 'A';
		str++;
	}
}

/* special main to test the above */
/* section commented out because code has been fully debugged
   and given a clean bill of health			CP - 11/14/88 
main(argc,argv)
int	argc;
char	*argv[];
{
	FILE	*f;
	tokenlist	*head;

	f = fopen(argv[1],"r");
	head = tokenize(f);
	while (head != NULL)
	{
		printf("%s\n", head -> token);
		head = head -> next;
	}
} */

