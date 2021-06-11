/* main program for MARS interpreter
   As usual, this does not do anything important.
   Just calls all the proper subroutines in the right order and
   opens files.
   -- 12/16/88         --- NCP        */

#include "interp.h"
#include <ctype.h>
#include <stdio.h>
#include <memory.h>
#include <curses.h>

long	atol();

/* memory array -- called "a" for easy typing */
cell	a[SIZE];

/* next execution for each player */
stream	*exe[MAXPLAY];

/* returns which player won */
int	play();

usage()
{
	printf("usage: interp cycles file1 file2\n");
	printf("--- main\n");
}

/* checks that every element in string is a digit */
int	number(str)
char	*str;
{
	int	i = 1;

	while (*str && i)
	{
		if (!isdigit(*str))
			i = 0;
		str++;
	}

	return i;
}

initialize()
{
	int	counter;	/* general-purpose counter */

	counter = (int) time(0);
#ifndef DEBUG
	initscr();		/* for "curses" library */
	scrollok(stdscr, 0);
	nl();
	clear();
	refresh();
#endif

#ifdef SUN
	xcore_init();
#endif
	srand(counter);
	for (counter = 0; counter++; counter < SIZE)
		memset(a[counter],0,sizeof(cell));
}

main(argc, argv)
int	argc;
char	*argv[];
{
	FILE	*f;
	int	errcode, result;

	initialize();		/* initialize all global variables */

	if (argc != 4)		/* too many or too few */
	{
		usage();
		exit(1);
	}

	if (!(number(argv[1])))
	{
		usage();
		exit(1);
	}

	if ((f = fopen(argv[2], "r")) == NULL)
	{
		printf("%s cannot be opened\n", argv[2]);
		exit(1);
	}

	errcode = load(f, 1);
	fclose(f);
	if (errcode == 1) {
		printf("main: Sorry, but %s is too large to load\n",argv[2]);
		exit(1);
	}


	if ((f = fopen(argv[3], "r")) == NULL)
	{
		printf("%s cannot be opened\n", argv[3]);
		exit(1);
	}

	errcode = load(f,2);
	fclose(f);
	if (errcode == 1) {
		printf("main: Sorry, but %s is too large to load\n",argv[3]);
		exit(1);
	}

#ifndef DEBUG
	clear();
#endif

	result = play(atol(argv[1]));

#ifndef DEBUG
#ifndef SUN
	output(0);
#endif
	move(21, 0);
	if (!result)
		printw("nobody won!");
	else
		printw("%s won!", argv[result + 1]);
	move(22, 0);
	printw("Hit any key to continue...");
	refresh();
	getch(errcode);
	endwin();
#endif
#ifdef SUN
	xcore_done();
#endif
}
