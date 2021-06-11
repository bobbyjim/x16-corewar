/** Output function **/

#include <stdio.h>
#include <curses.h>

#include "interp.h"

extern	cell	a[];

#if DEBUG
output()
{
	int	i;

	for (i = 0; i < SIZE; i ++)
	{
		if (a[i].lastmod == 0)
			putchar('0');
		else if (a[i].lastmod == 1)
			putchar('1');
		else if (a[i].lastmod == 2)
			putchar('2');
		else
		{
			printf("\nerror lastmod == %d", a[i].lastmod);
			printf("i == %d", i);
			exit(1);
		}
	}

	putchar('\n');
}
#endif

#ifdef SMALL
/* assume large, 80 col screen */

output(cycles)
long	cycles;
{
	int	map[SIZE / 5];	/* map to one fifth the size */
	int	i, j, k = 0;

	move(0,0);		/* start from the top */

	domap(map);

	for (i = 0; i < SIZE/5;)
	{
		move(k, 0);
		for (j = 1; j < COLS; i++, j++)
		{
			if (SIZE / 5<= i)
				break;

			if (map[i] == 0)
				addch('0');
			else if (map[i] == 1)
				addch('1');
			else if (map[i] == 2)
				addch('2');
		}

		k++;
	}
	move(23, 0);
	printw("Cycles left: %10d", cycles);
	refresh();
}

domap(arr)
int	arr[];
{
	int	i, j, pid1, pid2, pid3;

	for (j = 0; j < SIZE/5; j++)
	{
		pid1 = pid2 = pid3 = 0;
		for (i = j * 5; i < (j+1) * 5; i++)
		{
			if (SIZE <= i)
				break;
			if (a[i].lastmod == 0)
				pid1++;
			else if (a[i].lastmod == 1)
				pid2++;
			else if (a[i].lastmod == 2)
				pid3++;
			else
			{
				printf("invalid modification detected\n");
				printf("--- domap\n");
				exit(1);
			}
		}
		arr[j] = max(pid1, pid2, pid3);
	}
}

max(i, j, k)
int	i, j, k;
{
	if (i > j && i > k)
		return 0;
	else if (j > i && j > k)
		return 1;
	else
		return 2;
}
#endif

#ifdef BIG
output(cycles)
int	cycles;
{
	int	i, j, k;

	/* perform detailed mapping */
	for (i = 0, k = 0; i < SIZE; k++)
	{
		move (k, 0);
		for (j = 0; j < COLS; i++, j++)
		{
			if (i >= (SIZE - 1))
				break;

			/* else */
			if (a[i].lastmod == 0)
				addch('0');
			if (a[i].lastmod == 1)
				addch('1');
			else
				addch('2');
		}
	}
	move (k + 2, 0);
	printw("Cycles left: %10d\n", cycles);
	refresh();
}
#endif
